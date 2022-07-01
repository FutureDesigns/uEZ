/*-------------------------------------------------------------------------*
 * File:  LPC43xx_EMAC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC43xx EMAC (Ethernet) Driver.
 * Implementation:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_EMAC.h>
#include <uEZBSP.h>
#include <uEZGPIO.h>
#include <uEZErrors.h>
#include <Source/Library/Network/PHY/MDIOBitBang/MDIOBitBang.h>

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef DEBUG_LPC43xx_EMAC_STARTUP
#define DEBUG_LPC43xx_EMAC_STARTUP  0
#endif

#ifndef DEBUG_LPC43xx_EMAC_PACKETS
#define DEBUG_LPC43xx_EMAC_PACKETS               0
#endif

#define DEBUG_LPC43xx_EMAC_PACKETS_IN       DEBUG_LPC43xx_EMAC_PACKETS
#define DEBUG_LPC43xx_EMAC_PACKETS_OUT      DEBUG_LPC43xx_EMAC_PACKETS

#ifndef EMAC_USE_BITBANG_MDIO_DETECT
/* Bit banging to the EMAC can be used to avoid the dread LPC43xx lock up
 * that occurs when a PHY is attempting to be communicated with and there
 * is NO oscillation.  The LPC43xx state machine locks up hard and only
 * a power cycle recovers.
 */
#define EMAC_USE_BITBANG_MDIO_DETECT       1
#endif

#define EMAC_BITBANG_MDIO_DELAY_US  10 /* uSecs */

/*---------------------------------------------------------------------------*
 * Regions:
 *---------------------------------------------------------------------------*/
#if ECLIPSE_EDITOR
    #define EMAC_MEMORY // no mods (avoid messing up the syntax highlighting)
#elif ((COMPILER_TYPE==RowleyARM) || (COMPILER_TYPE==Keil4))
    #define EMAC_MEMORY __attribute__((section(".emacmem")));
#elif (COMPILER_TYPE==IAR)
    #define EMAC_MEMORY @ ".emacmem"
#else
    #define EMAC_MEMORY // no mods
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
/* EMAC Memory Buffer configuration for 16K Ethernet RAM. */
#define NUM_RX_FRAG         8           /* Num.of RX Fragments 4*1536= 6.0kB */
#define NUM_TX_FRAG         2           /* Num.of TX Fragments 2*1536= 3.0kB */
#define ETH_FRAG_SIZE       1536        /* Packet Fragment size 1536 Bytes   */
#define ETH_MAX_FLEN        1536        /* Max. Ethernet Frame Size          */

/* MAC Configuration Register 1 */
#define MAC1_REC_EN         0x00000001  /* Receive Enable                    */
#define MAC1_PASS_ALL       0x00000002  /* Pass All Receive Frames           */
#define MAC1_RX_FLOWC       0x00000004  /* RX Flow Control                   */
#define MAC1_TX_FLOWC       0x00000008  /* TX Flow Control                   */
#define MAC1_LOOPB          0x00000010  /* Loop Back Mode                    */
#define MAC1_RES_TX         0x00000100  /* Reset TX Logic                    */
#define MAC1_RES_MCS_TX     0x00000200  /* Reset MAC TX Control Sublayer     */
#define MAC1_RES_RX         0x00000400  /* Reset RX Logic                    */
#define MAC1_RES_MCS_RX     0x00000800  /* Reset MAC RX Control Sublayer     */
#define MAC1_SIM_RES        0x00004000  /* Simulation Reset                  */
#define MAC1_SOFT_RES       0x00008000  /* Soft Reset MAC                    */

/* MAC Configuration Register 2 */
#define MAC2_FULL_DUP       0x00000001  /* Full Duplex Mode                  */
#define MAC2_FRM_LEN_CHK    0x00000002  /* Frame Length Checking             */
#define MAC2_HUGE_FRM_EN    0x00000004  /* Huge Frame Enable                 */
#define MAC2_DLY_CRC        0x00000008  /* Delayed CRC Mode                  */
#define MAC2_CRC_EN         0x00000010  /* Append CRC to every Frame         */
#define MAC2_PAD_EN         0x00000020  /* Pad all Short Frames              */
#define MAC2_VLAN_PAD_EN    0x00000040  /* VLAN Pad Enable                   */
#define MAC2_ADET_PAD_EN    0x00000080  /* Auto Detect Pad Enable            */
#define MAC2_PPREAM_ENF     0x00000100  /* Pure Preamble Enforcement         */
#define MAC2_LPREAM_ENF     0x00000200  /* Long Preamble Enforcement         */
#undef  MAC2_NO_BACKOFF /* Remove compiler warning. */
#define MAC2_NO_BACKOFF     0x00001000  /* No Backoff Algorithm              */
#define MAC2_BACK_PRESSURE  0x00002000  /* Backoff Presurre / No Backoff     */
#define MAC2_EXCESS_DEF     0x00004000  /* Excess Defer                      */

/* Back-to-Back Inter-Packet-Gap Register */
#define IPGT_FULL_DUP       0x00000015  /* Recommended value for Full Duplex */
#define IPGT_HALF_DUP       0x00000012  /* Recommended value for Half Duplex */

/* Non Back-to-Back Inter-Packet-Gap Register */
#define IPGR_DEF            0x00000012  /* Recommended value                 */

/* Collision Window/Retry Register */
#define CLRT_DEF            0x0000370F  /* Default value                     */

/* PHY Support Register */
#undef SUPP_SPEED   /* Remove compiler warning. */
#define SUPP_SPEED          0x00000100  /* Reduced MII Logic Current Speed   */
#define SUPP_RES_RMII       0x00000800  /* Reset Reduced MII Logic           */

/* Test Register */
#define TEST_SHCUT_PQUANTA  0x00000001  /* Shortcut Pause Quanta             */
#define TEST_TST_PAUSE      0x00000002  /* Test Pause                        */
#define TEST_TST_BACKP      0x00000004  /* Test Back Pressure                */

/* MII Management Configuration Register */
#define MCFG_SCAN_INC       0x00000001  /* Scan Increment PHY Address        */
#define MCFG_SUPP_PREAM     0x00000002  /* Suppress Preamble                 */
#define MCFG_CLK_SEL        0x0000001C  /* Clock Select Mask                 */
#define MCFG_RES_MII        0x00008000  /* Reset MII Management Hardware     */

/* MII Management Command Register */
#undef MCMD_READ   /* Remove compiler warning. */
#define MCMD_READ           0x00000001  /* MII Read                          */
#undef MCMD_SCAN /* Remove compiler warning. */
#define MCMD_SCAN           0x00000002  /* MII Scan continuously             */

#define MII_WR_TOUT         0x00050000  /* MII Write timeout count           */
#define MII_RD_TOUT         0x00050000  /* MII Read timeout count            */

/* MII Management Address Register */
#define MADR_REG_ADR        0x0000001F  /* MII Register Address Mask         */
#define MADR_PHY_ADR        0x00001F00  /* PHY Address Mask                  */

/* MII Management Indicators Register */
#undef MIND_BUSY   /* Remove compiler warning. */
#define MIND_BUSY           0x00000001  /* MII is Busy                       */
#define MIND_SCAN           0x00000002  /* MII Scanning in Progress          */
#define MIND_NOT_VAL        0x00000004  /* MII Read Data not valid           */
#define MIND_MII_LINK_FAIL  0x00000008  /* MII Link Failed                   */

/* Command Register */
#define CR_RX_EN            0x00000001  /* Enable Receive                    */
#define CR_TX_EN            0x00000002  /* Enable Transmit                   */
#define CR_REG_RES          0x00000008  /* Reset Host Registers              */
#define CR_TX_RES           0x00000010  /* Reset Transmit Datapath           */
#define CR_RX_RES           0x00000020  /* Reset Receive Datapath            */
#define CR_PASS_RUNT_FRM    0x00000040  /* Pass Runt Frames                  */
#define CR_PASS_RX_FILT     0x00000080  /* Pass RX Filter                    */
#define CR_TX_FLOW_CTRL     0x00000100  /* TX Flow Control                   */
#define CR_RMII             0x00000200  /* Reduced MII Interface             */
#define CR_FULL_DUP         0x00000400  /* Full Duplex                       */

/* Status Register */
#define SR_RX_EN            0x00000001  /* Enable Receive                    */
#define SR_TX_EN            0x00000002  /* Enable Transmit                   */

/* Transmit Status Vector 0 Register */
#define TSV0_CRC_ERR        0x00000001  /* CRC error                         */
#define TSV0_LEN_CHKERR     0x00000002  /* Length Check Error                */
#define TSV0_LEN_OUTRNG     0x00000004  /* Length Out of Range               */
#define TSV0_DONE           0x00000008  /* Tramsmission Completed            */
#define TSV0_MCAST          0x00000010  /* Multicast Destination             */
#define TSV0_BCAST          0x00000020  /* Broadcast Destination             */
#define TSV0_PKT_DEFER      0x00000040  /* Packet Deferred                   */
#define TSV0_EXC_DEFER      0x00000080  /* Excessive Packet Deferral         */
#define TSV0_EXC_COLL       0x00000100  /* Excessive Collision               */
#define TSV0_LATE_COLL      0x00000200  /* Late Collision Occured            */
#define TSV0_GIANT          0x00000400  /* Giant Frame                       */
#define TSV0_UNDERRUN       0x00000800  /* Buffer Underrun                   */
#define TSV0_BYTES          0x0FFFF000  /* Total Bytes Transferred           */
#define TSV0_CTRL_FRAME     0x10000000  /* Control Frame                     */
#define TSV0_PAUSE          0x20000000  /* Pause Frame                       */
#define TSV0_BACK_PRESS     0x40000000  /* Backpressure Method Applied       */
#define TSV0_VLAN           0x80000000  /* VLAN Frame                        */

/* Transmit Status Vector 1 Register */
#define TSV1_BYTE_CNT       0x0000FFFF  /* Transmit Byte Count               */
#define TSV1_COLL_CNT       0x000F0000  /* Transmit Collision Count          */

/* Receive Status Vector Register */
#define RSV_BYTE_CNT        0x0000FFFF  /* Receive Byte Count                */
#define RSV_PKT_IGNORED     0x00010000  /* Packet Previously Ignored         */
#define RSV_RXDV_SEEN       0x00020000  /* RXDV Event Previously Seen        */
#define RSV_CARR_SEEN       0x00040000  /* Carrier Event Previously Seen     */
#define RSV_REC_CODEV       0x00080000  /* Receive Code Violation            */
#define RSV_CRC_ERR         0x00100000  /* CRC Error                         */
#define RSV_LEN_CHKERR      0x00200000  /* Length Check Error                */
#define RSV_LEN_OUTRNG      0x00400000  /* Length Out of Range               */
#define RSV_REC_OK          0x00800000  /* Frame Received OK                 */
#define RSV_MCAST           0x01000000  /* Multicast Frame                   */
#define RSV_BCAST           0x02000000  /* Broadcast Frame                   */
#define RSV_DRIB_NIBB       0x04000000  /* Dribble Nibble                    */
#define RSV_CTRL_FRAME      0x08000000  /* Control Frame                     */
#define RSV_PAUSE           0x10000000  /* Pause Frame                       */
#define RSV_UNSUPP_OPC      0x20000000  /* Unsupported Opcode                */
#define RSV_VLAN            0x40000000  /* VLAN Frame                        */

/* Flow Control Counter Register */
#define FCC_MIRR_CNT        0x0000FFFF  /* Mirror Counter                    */
#define FCC_PAUSE_TIM       0xFFFF0000  /* Pause Timer                       */

/* Flow Control Status Register */
#define FCS_MIRR_CNT        0x0000FFFF  /* Mirror Counter Current            */

/* Receive Filter Control Register */
#define RFC_UCAST_EN        0x00000001  /* Accept Unicast Frames Enable      */
#define RFC_BCAST_EN        0x00000002  /* Accept Broadcast Frames Enable    */
#define RFC_MCAST_EN        0x00000004  /* Accept Multicast Frames Enable    */
#define RFC_UCAST_HASH_EN   0x00000008  /* Accept Unicast Hash Filter Frames */
#define RFC_MCAST_HASH_EN   0x00000010  /* Accept Multicast Hash Filter Fram.*/
#define RFC_PERFECT_EN      0x00000020  /* Accept Perfect Match Enable       */
#define RFC_MAGP_WOL_EN     0x00001000  /* Magic Packet Filter WoL Enable    */
#define RFC_PFILT_WOL_EN    0x00002000  /* Perfect Filter WoL Enable         */

/* Receive Filter WoL Status/Clear Registers */
#define WOL_UCAST           0x00000001  /* Unicast Frame caused WoL          */
#define WOL_BCAST           0x00000002  /* Broadcast Frame caused WoL        */
#define WOL_MCAST           0x00000004  /* Multicast Frame caused WoL        */
#define WOL_UCAST_HASH      0x00000008  /* Unicast Hash Filter Frame WoL     */
#define WOL_MCAST_HASH      0x00000010  /* Multicast Hash Filter Frame WoL   */
#define WOL_PERFECT         0x00000020  /* Perfect Filter WoL                */
#define WOL_RX_FILTER       0x00000080  /* RX Filter caused WoL              */
#define WOL_MAG_PACKET      0x00000100  /* Magic Packet Filter caused WoL    */

/* Interrupt Status/Enable/Clear/Set Registers */
#define INT_RX_OVERRUN      0x00000001  /* Overrun Error in RX Queue         */
#define INT_RX_ERR          0x00000002  /* Receive Error                     */
#define INT_RX_FIN          0x00000004  /* RX Finished Process Descriptors   */
#define INT_RX_DONE         0x00000008  /* Receive Done                      */
#define INT_TX_UNDERRUN     0x00000010  /* Transmit Underrun                 */
#define INT_TX_ERR          0x00000020  /* Transmit Error                    */
#define INT_TX_FIN          0x00000040  /* TX Finished Process Descriptors   */
#define INT_TX_DONE         0x00000080  /* Transmit Done                     */
#define INT_SOFT_INT        0x00001000  /* Software Triggered Interrupt      */
#define INT_WAKEUP          0x00002000  /* Wakeup Event Interrupt            */

/* Power Down Register */
#define PD_POWER_DOWN       0x80000000  /* Power Down MAC                    */

/* RX Descriptor Control Word */
#define RCTRL_SIZE          0x000007FF  /* Buffer size mask                  */
#define RCTRL_INT           0x80000000  /* Generate RxDone Interrupt         */

/* RX Status Hash CRC Word */
#define RHASH_SA            0x000001FF  /* Hash CRC for Source Address       */
#define RHASH_DA            0x001FF000  /* Hash CRC for Destination Address  */

/* RX Status Information Word */
#define RINFO_SIZE          0x000007FF  /* Data size in bytes                */
#define RINFO_CTRL_FRAME    0x00040000  /* Control Frame                     */
#define RINFO_VLAN          0x00080000  /* VLAN Frame                        */
#define RINFO_FAIL_FILT     0x00100000  /* RX Filter Failed                  */
#define RINFO_MCAST         0x00200000  /* Multicast Frame                   */
#define RINFO_BCAST         0x00400000  /* Broadcast Frame                   */
#define RINFO_CRC_ERR       0x00800000  /* CRC Error in Frame                */
#define RINFO_SYM_ERR       0x01000000  /* Symbol Error from PHY             */
#define RINFO_LEN_ERR       0x02000000  /* Length Error                      */
#define RINFO_RANGE_ERR     0x04000000  /* Range Error (exceeded max. size)  */
#define RINFO_ALIGN_ERR     0x08000000  /* Alignment Error                   */
#define RINFO_OVERRUN       0x10000000  /* Receive overrun                   */
#define RINFO_NO_DESCR      0x20000000  /* No new Descriptor available       */
#define RINFO_LAST_FLAG     0x40000000  /* Last Fragment in Frame            */
#define RINFO_ERR           0x80000000  /* Error Occured (OR of all errors)  */

#define RINFO_ERR_MASK     (RINFO_FAIL_FILT | RINFO_CRC_ERR   | RINFO_SYM_ERR | \
                            RINFO_LEN_ERR   | RINFO_ALIGN_ERR | RINFO_OVERRUN)

/* TX Descriptor Control Word */
#define TCTRL_SIZE          0x000007FF  /* Size of data buffer in bytes      */
#define TCTRL_OVERRIDE      0x04000000  /* Override Default MAC Registers    */
#define TCTRL_HUGE          0x08000000  /* Enable Huge Frame                 */
#define TCTRL_PAD           0x10000000  /* Pad short Frames to 64 bytes      */
#define TCTRL_CRC           0x20000000  /* Append a hardware CRC to Frame    */
#define TCTRL_LAST          0x40000000  /* Last Descriptor for TX Frame      */
#define TCTRL_INT           0x80000000  /* Generate TxDone Interrupt         */

/* TX Status Information Word */
#define TINFO_COL_CNT       0x01E00000  /* Collision Count                   */
#define TINFO_DEFER         0x02000000  /* Packet Deferred (not an error)    */
#define TINFO_EXCESS_DEF    0x04000000  /* Excessive Deferral                */
#define TINFO_EXCESS_COL    0x08000000  /* Excessive Collision               */
#define TINFO_LATE_COL      0x10000000  /* Late Collision Occured            */
#define TINFO_UNDERRUN      0x20000000  /* Transmit Underrun                 */
#define TINFO_NO_DESCR      0x40000000  /* No new Descriptor available       */
#define TINFO_ERR           0x80000000  /* Error Occured (OR of all errors)  */

/* DP83848C PHY Registers */
#define PHY_REG_BMCR        0x00        /* Basic Mode Control Register       */
#define PHY_REG_BMSR        0x01        /* Basic Mode Status Register        */
#define PHY_REG_IDR1        0x02        /* PHY Identifier 1                  */
#define PHY_REG_IDR2        0x03        /* PHY Identifier 2                  */
#define PHY_REG_ANAR        0x04        /* Auto-Negotiation Advertisement    */
#define PHY_REG_ANLPAR      0x05        /* Auto-Neg. Link Partner Abitily    */
#define PHY_REG_ANER        0x06        /* Auto-Neg. Expansion Register      */
#define PHY_REG_ANNPTR      0x07        /* Auto-Neg. Next Page TX            */

/* PHY Extended Registers */
#define PHY_REG_STS         0x10        /* Status Register                   */
#define PHY_REG_MICR        0x11        /* MII Interrupt Control Register    */
#define PHY_REG_MISR        0x12        /* MII Interrupt Status Register     */
#define PHY_REG_FCSCR       0x14        /* False Carrier Sense Counter       */
#define PHY_REG_RECR        0x15        /* Receive Error Counter             */
#define PHY_REG_PCSR        0x16        /* PCS Sublayer Config. and Status   */
#define PHY_REG_RBR         0x17        /* RMII and Bypass Register          */
#define PHY_REG_LEDCR       0x18        /* LED Direct Control Register       */
#define PHY_REG_PHYCR       0x19        /* PHY Control Register              */
#define PHY_REG_10BTSCR     0x1A        /* 10Base-T Status/Control Register  */
#define PHY_REG_CDCTRL1     0x1B        /* CD Test Control and BIST Extens.  */
#define PHY_REG_EDCR        0x1D        /* Energy Detect Control Register    */

#define PHY_FULLD_100M      0x2100      /* Full Duplex 100Mbit               */
#define PHY_HALFD_100M      0x2000      /* Half Duplex 100Mbit               */
#define PHY_FULLD_10M       0x0100      /* Full Duplex 10Mbit                */
#define PHY_HALFD_10M       0x0000      /* Half Duplex 10MBit                */
#define PHY_AUTO_NEG        0x3000      /* Select Auto Negotiation           */

/* BMSR setting */
#define BMSR_100BE_T4       0x8000
#define BMSR_100TX_FULL     0x4000
#define BMSR_100TX_HALF     0x2000
#define BMSR_10BE_FULL      0x1000
#define BMSR_10BE_HALF      0x0800
#define BMSR_AUTO_DONE      0x0020
#define BMSR_REMOTE_FAULT   0x0010
#define BMSR_NO_AUTO        0x0008
#define BMSR_LINK_ESTABLISHED   0x0004

#define EMAC_INT_RXDONE     0x01 << 3

/* Below is the Micrel PHY definition used for IAR LPC23xx and
 Embedded Artists LPC24xx board. */
#define MIC_PHY_RXER_CNT			0x0015
#define MIC_PHY_INT_CTRL			0x001B
#define MIC_PHY_LINKMD_CTRL			0x001D
#define MIC_PHY_PHY_CTRL			0x001E
#define MIC_PHY_100BASE_PHY_CTRL	0x001F

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_EMAC *iHAL;
    T_EMACSettings iSettings;
    TUInt8 *rptr;
    TUInt8 *tptr;
    TUInt16 tsize;
    T_uezEMACReceiveInterruptCallback iReceiveCallback;
    void *iReceiveCallbackWorkspace;
    T_LPC43xx_EMAC_Settings iPinSettings;
    T_mdioBitBangSettings iMDIOSettings;
    TBool iIsDetected;
    TUInt16 iReceiveIndex;
    TUInt16 iTransmitIndex;
} T_LPC43xx_EMAC_Workspace;

typedef struct {
    __IO uint32_t CTRLSTAT;     /*!< TDES control and status word */
    __IO uint32_t BSIZE;        /*!< Buffer 1/2 byte counts */
    __IO uint32_t B1ADD;        /*!< Buffer 1 address */
    __IO uint32_t B2ADD;        /*!< Buffer 2 or next descriptor address */
} ENET_TXDESC_T;

/**
 * @brief Structure of a enhanced transmit descriptor (with timestamp)
 */
typedef struct {
    __IO uint32_t CTRLSTAT;     /*!< TDES control and status word */
    __IO uint32_t BSIZE;        /*!< Buffer 1/2 byte counts */
    __IO uint32_t B1ADD;        /*!< Buffer 1 address */
    __IO uint32_t B2ADD;        /*!< Buffer 2 or next descriptor address */
    __IO uint32_t TDES4;        /*!< Reserved */
    __IO uint32_t TDES5;        /*!< Reserved */
    __IO uint32_t TTSL;         /*!< Timestamp value low */
    __IO uint32_t TTSH;         /*!< Timestamp value high */
} ENET_ENHTXDESC_T;

/**
 * @brief Structure of a receive descriptor (without timestamp)
 */
typedef struct {
    __IO uint32_t STATUS;       /*!< RDES status word */
    __IO uint32_t CTRL;         /*!< Buffer 1/2 byte counts and control */
    __IO uint32_t B1ADD;        /*!< Buffer 1 address */
    __IO uint32_t B2ADD;        /*!< Buffer 2 or next descriptor address */
} ENET_RXDESC_T;

#define RXDESC_STATUS_OWN       (1UL<<31) //!< Own Bit
#define RXDESC_STATUS_AFM       (1UL<<30) //!< Destination Address Filter Fail
#define RXDESC_STATUS_FLEN(x)   (((x)>>16)&0x3FFF) //!< Frame Length
#define RXDESC_STATUS_ES        (1UL<<15) //!< Error Summary
#define RXDESC_STATUS_DE        (1UL<<14) //!< Descriptor Error
#define RXDESC_STATUS_SAF       (1UL<<13) //!< Source Address Filter Fail
#define RXDESC_STATUS_LE        (1UL<<12) //!< Length Error
#define RXDESC_STATUS_OE        (1UL<<11) //!< Overflow Error
#define RXDESC_STATUS_VLAN      (1UL<<10) //!< VLAN Tag
#define RXDESC_STATUS_FS        (1UL<< 9) //!< First Descriptor
#define RXDESC_STATUS_LS        (1UL<< 8) //!< Last Descriptor
#define RXDESC_STATUS_TSA       (1UL<< 7) //!< Timestamp available
#define RXDESC_STATUS_LC        (1UL<< 6) //!< Late Collision
#define RXDESC_STATUS_FT        (1UL<< 5) //!< Frame Type
#define RXDESC_STATUS_RWT       (1UL<< 4) //!< Receive Watchdog Timeout
#define RXDESC_STATUS_RE        (1UL<< 3) //!< Receive Error
#define RXDESC_STATUS_DRIBBLE   (1UL<< 2) //!< Dribble Bit Error
#define RXDESC_STATUS_CE        (1UL<< 1) //!< CRC Error
#define RXDESC_STATUS_ESA       (1UL<< 0) //!< Extended Status Available

#define TXDESC_STATUS_OWN       (1UL<<31) //!< Own Bit
#define TXDESC_STATUS_IC        (1UL<<30) //!< Interrupt on Completion
#define TXDESC_STATUS_LS        (1UL<<29) //!< Last Segment
#define TXDESC_STATUS_FS        (1UL<<28) //!< First Segment
#define TXDESC_STATUS_CRC       (1UL<<27) //!< Disable CRC
#define TXDESC_STATUS_DP        (1UL<<26) //!< Disable Pad
#define TXDESC_STATUS_TTSE      (1UL<<25) //!< Transmit Timestamp Enable
#define TXDESC_STATUS_TER       (1UL<<21) //!< Transmit End of Ring
#define TXDESC_STATUS_TCH       (1UL<<20) //!< Second Address Chained
#define TXDESC_STATUS_TTSS      (1UL<<17) //!< Transmit Timestamp Status
#define TXDESC_STATUS_IHE       (1UL<<16) //!< IP Header Error
#define TXDESC_STATUS_ES        (1UL<<15) //!< Error Summary
#define TXDESC_STATUS_JT        (1UL<<14) //!< Jabber Timeout
#define TXDESC_STATUS_FF        (1UL<<13) //!< Frame Flushed
#define TXDESC_STATUS_IPE       (1UL<<12) //!< IP Payload Error
#define TXDESC_STATUS_LC        (1UL<<11) //!< Loss of Carrier
#define TXDESC_STATUS_NC        (1UL<<10) //!< No Carrier
#define TXDESC_STATUS_LCOLI     (1UL<< 9) //!< Late Collision
#define TXDESC_STATUS_EC        (1UL<< 8) //!< Excessive collision
#define TXDESC_STATUS_VF        (1UL<< 7) //!< VLAN Frame
#define TXDESC_STATUS_CC        (1UL<< 3) //!< 6:3 Collision count
#define TXDESC_STATUS_ED        (1UL<< 2) //!< Excessive Deferral
#define TXDESC_STATUS_UF        (1UL<< 1) //!< Underflow Error
#define TXDESC_STATUS_DB        (1UL<< 0) //!< Deferred Bit

/**
 * @brief Structure of a enhanced receive descriptor (with timestamp)
 */
typedef struct {
    __IO uint32_t STATUS;       /*!< RDES status word */
    __IO uint32_t CTRL;         /*!< Buffer 1/2 byte counts */
    __IO uint32_t B1ADD;        /*!< Buffer 1 address */
    __IO uint32_t B2ADD;        /*!< Buffer 2 or next descriptor address */
    __IO uint32_t EXTSTAT;      /*!< Extended Status */
    __IO uint32_t RDES5;        /*!< Reserved */
    __IO uint32_t RTSL;         /*!< Timestamp value low */
    __IO uint32_t RTSH;         /*!< Timestamp value high */
} ENET_ENHRXDESC_T;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_EMAC EMAC_LPC43xx_Interface;
static T_LPC43xx_EMAC_Workspace *G_LPC43xx_EMAC;

// Memory EMAC packet memory
#define ENET_NUM_TX_DESC 4
#define ENET_NUM_RX_DESC 4

#define EMAC_ETH_MAX_FLEN   1536

static ENET_ENHTXDESC_T TXDescs[ENET_NUM_TX_DESC]EMAC_MEMORY;
static ENET_ENHRXDESC_T RXDescs[ENET_NUM_RX_DESC]EMAC_MEMORY;

/* Transmit/receive buffers and indices */
static TUInt8 TXBuffer[ENET_NUM_TX_DESC][EMAC_ETH_MAX_FLEN]EMAC_MEMORY;
static TUInt8 RXBuffer[ENET_NUM_RX_DESC][EMAC_ETH_MAX_FLEN]EMAC_MEMORY;
//static int32_t rxFill, rxGet, rxAvail, rxNumDescs;
//static int32_t txFill, txGet, txUsed, txNumDescs;

/*---------------------------------------------------------------------------*
 * Routine:  IPHYWrite
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the register in the given PHY to the given value.
 * Inputs:
 *      T_LPC43xx_EMAC_Workspace *p -- EMAC's Workspace
 *      TUInt32 aPhyReg             -- PHY register to write
 *      TUInt32 aValue              -- Value to write to register
 *---------------------------------------------------------------------------*/
static void IPHYWrite(T_LPC43xx_EMAC_Workspace *p, TUInt8 PhyReg, TUInt32 Value)
{
    TUInt32 timeout;

    LPC_ETHERNET->MAC_MII_ADDR = p->iSettings.iPHYAddr | (4 << 2) | (PhyReg << 6);
    LPC_ETHERNET->MAC_MII_DATA = Value;
    LPC_ETHERNET->MAC_MII_ADDR |= (1<<1);
    LPC_ETHERNET->MAC_MII_ADDR |= (1<<0);

    for (timeout = 0; timeout < MII_WR_TOUT; timeout++) {
        if ((LPC_ETHERNET->MAC_MII_ADDR & (1 << 0)) == 0) {
            break;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  IPHYRead
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the register from the given PHY.
 * Inputs:
 *      T_LPC43xx_EMAC_Workspace *p -- EMAC's Workspace
 *      TUInt32 aPhyReg             -- PHY register to write
 *      TUInt32 aValue              -- Value to write to register
 *---------------------------------------------------------------------------*/
static TUInt16 IPHYRead(T_LPC43xx_EMAC_Workspace *p, TUInt8 PhyReg)
{
    TUInt32 timeout;

    LPC_ETHERNET->MAC_MII_ADDR = p->iSettings.iPHYAddr | (4 << 2) | (PhyReg << 6);
    LPC_ETHERNET->MAC_MII_ADDR |= (1<<0);

    // Wait until operation completed
    for (timeout = 0; timeout < MII_RD_TOUT; timeout++) {
        if ((LPC_ETHERNET->MAC_MII_ADDR & (1 << 0)) == 0) {
            break;
        }
    }

    return (TUInt16)(LPC_ETHERNET->MAC_MII_DATA);
}

/*---------------------------------------------------------------------------*
 * Routine:  IEMACConfigPHY_National
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for the National PHY
 * Inputs:
 *      T_LPC17xx_40xx_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_National(T_LPC43xx_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 timeout, regv;

    // Force ON Auto-MDIX
    IPHYWrite(p, PHY_REG_PHYCR, IPHYRead(p, PHY_REG_PHYCR) | (1 << 15));

    // auto negotiation, restart AN
    // RE_AN should be self cleared
    IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);

    // Wait to complete Auto_Negotiation.
    for (timeout = 0; timeout < 10; timeout++) {
        UEZBSPDelayMS(100);
        regv = IPHYRead(p, PHY_REG_BMSR);
        if (regv & 0x0020) {
            // Autonegotiation Complete.
            break;
        }
    }

    // Check the link status.
    if (error == UEZ_ERROR_NONE) {
        error = UEZ_ERROR_TIMEOUT;
        for (timeout = 0; timeout < 10; timeout++) {
            UEZBSPDelayMS(100);
            regv = IPHYRead(p, PHY_REG_STS);
            if (regv & 0x0001) {
                // Link is on.
                error = UEZ_ERROR_NONE;
                break;
            }
        }
    }

    if (error == UEZ_ERROR_NONE) {
        // Configure Full/Half Duplex mode.
        if (regv & 0x0004) {
            // Full duplex is enabled.
            //LPC_EMAC->MAC2 |= MAC2_FULL_DUP;
            //LPC_EMAC->Command |= CR_FULL_DUP;
            //LPC_EMAC->IPGT = IPGT_FULL_DUP;
        } else {
            // Half duplex mode.
            //LPC_EMAC->IPGT = IPGT_HALF_DUP;
        }

        // Configure 100MBit/10MBit mode.
        if (regv & 0x0002) {
            // 10MBit mode.
            //LPC_EMAC->SUPP = 0;
        } else {
            // 100MBit mode.
            //LPC_EMAC->SUPP = SUPP_SPEED;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IEMACConfigPHY_Micrel
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for the Micrel PHY
 * Inputs:
 *      T_LPC17xx_40xx_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_Micrel(T_LPC43xx_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 timeout, regv;
#if DEBUG_LPC43xx_EMAC_STARTUP
    TUInt32 reg;
#endif
    // auto negotiation, restart AN
    // RE_AN should be self cleared
    IPHYWrite(p, PHY_REG_BMCR,(1 << 12));// PHY_AUTO_NEG);

    // Wait to complete Auto_Negotiation.
    for (timeout = 0; timeout < 30; timeout++) {
#if DEBUG_LPC43xx_EMAC_STARTUP
        printf(".");
#endif
        UEZBSPDelayMS(100);
        regv = IPHYRead(p, PHY_REG_BMSR);
        if (regv & (BMSR_AUTO_DONE | BMSR_LINK_ESTABLISHED)) {
            // Autonegotiation Complete.
            break;
        }
    }

    regv = IPHYRead(p, MIC_PHY_100BASE_PHY_CTRL);
#if DEBUG_LPC43xx_EMAC_STARTUP
    printf("nego regv %04X\n", regv);
#endif
    // successful negotiations; update link info
    regv &= 0x001C;
    switch (regv) {
        case 0x0004:
            // Half duplex mode.
            LPC_ETHERNET->MAC_CONFIG &= ~(1 << 11);
            // 10MBit mode.
            LPC_ETHERNET->MAC_CONFIG &= ~(1 << 14);
            break;
        case 0x0008:
            // Half duplex mode.
            LPC_ETHERNET->MAC_CONFIG &= ~(1 << 11);
            // 100MBit mode.
            LPC_ETHERNET->MAC_CONFIG |= (1 << 14);
            break;
        case 0x0014:
            // Full duplex is enabled.
            LPC_ETHERNET->MAC_CONFIG |= (1 << 11);
            // 10MBit mode.
            LPC_ETHERNET->MAC_CONFIG &= ~(1 << 14);
            break;
        case 0x0018:
            // Full duplex is enabled.
            LPC_ETHERNET->MAC_CONFIG |= (1 << 11);
            // 100MBit mode.
            LPC_ETHERNET->MAC_CONFIG |= (1 << 14);
            //LPC_EMAC->SUPP = SUPP_SPEED;
            break;
        default: // Should not come here, force to set default, 100 FULL_DUPLEX
            // Full duplex is enabled.
            LPC_ETHERNET->MAC_CONFIG |= (1 << 11);
            // 100MBit mode.
            LPC_ETHERNET->MAC_CONFIG |= (1 << 14);
            //LPC_EMAC->SUPP = SUPP_SPEED;
            break;
    }

    //IPHYWrite(p, PHY_REG_RBR, (1 << 5));

#if DEBUG_LPC43xx_EMAC_STARTUP
    for (reg = 0; reg < 32; reg++) {
        regv = IPHYRead(p, reg);
        printf("phyreg %d = %04X\n", reg, regv);
    }
#endif

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC43xx_EMAC_InitRxDescriptors
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize Receive Descriptors
 *
 *      Received Descriptors are setup in a ring formation.  B1ADD points
 *      to its own buffer.  B2ADD points to the next buffer in the ring
 *      telling the DMA where to go next.  All buffers start out with OWN
 *      set to allow the DMA to fill all blocks as they come in.
 *      As a safety precaution, the last descriptor has RER set to also
 *      indicate the end of the ring (thus, we probably don't even need
 *      the B2ADD link back to the top).
 *
 *      In this version of code, we are not using split blocks for the
 *      buffers require all buffers to be fully sized packets (1500+ bytes)
 *
 *      RXDesc[0]                                <-----+
 *        B1ADD              --> [= RXBuffer[0] =]     |
 *        B2ADD              -->    RXDesc[1] --+      |
 *      RXDesc[1]                             <-+      |
 *        B1ADD              --> [= RXBuffer[1] =]     |
 *        B2ADD              -->    RXDesc[2] --+      |
 *      RXDesc[2]                             <-+      |
 *        B1ADD              --> [= RXBuffer[2] =]     |
 *        B2ADD              -->    RXDesc[3] --+      |
 *      RXDesc[3]                             <-+      |
 *        B1ADD              --> [= RXBuffer[3] =]     |
 *        B2ADD              -->    RXDesc[0] ---------+
 *
 *---------------------------------------------------------------------------*/
static void ILPC43xx_EMAC_InitRxDescriptors(void)
{
    TUInt32 i;

    memset((void*)RXDescs, 0, sizeof(RXDescs));

    for(i = 0; i < ENET_NUM_RX_DESC; i++){
        RXDescs[i].CTRL = (1<<14);// | (((EMAC_ETH_MAX_FLEN) & 0xFFF) << 0);
        //RXDescs[i].B1ADD = (TUInt32) &RXBuffer[i + 1];
        RXDescs[i].B2ADD = (TUInt32) &RXDescs[i + 1];
    }

    RXDescs[ENET_NUM_RX_DESC - 1].CTRL = (1 << 14) | (1<<15) ;//| (((EMAC_ETH_MAX_FLEN) & 0xFFF) << 0);
    //RXDescs[ENET_NUM_RX_DESC - 1].B1ADD = (TUInt32) &RXBuffer[0];
    RXDescs[ENET_NUM_RX_DESC - 1].B2ADD = (TUInt32) &RXDescs[0];

     // Set EMAC Receive Descriptor Registers.
    LPC_ETHERNET->DMA_REC_DES_ADDR = (TUInt32)RXDescs;

    for(i = 0; i < ENET_NUM_RX_DESC; i++){
        RXDescs[i].B1ADD = (TUInt32) &RXBuffer[i];
        RXDescs[i].CTRL = (1<<14) | (((EMAC_ETH_MAX_FLEN) & 0xFFF) << 0);
        RXDescs[i].STATUS = (TUInt32)(1 << 31);
    }
//    RXDescs[ENET_NUM_RX_DESC - 1].CTRL = (1 << 14) | (1<<15) | (((EMAC_ETH_MAX_FLEN) & 0xFFF) << 0);
//    RXDescs[ENET_NUM_RX_DESC - 1].B1ADD = (TUInt32) &RXBuffer[0];
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC43xx_EMAC_InitTxDescriptors
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize Transmit Descriptors
 *
 *      Transmit Descriptors are setup in a ring formation.  B1ADD points
 *      to its own buffer.  B2ADD points to the next buffer in the ring
 *      telling the DMA where to go next.  All buffers start out with OWN
 *      clear since no data has been put into the buffer yet.
 *      As a safety precaution, the last descriptor has TER set to also
 *      indicate the end of the ring (thus, we probably don't even need
 *      the B2ADD link back to the top).
 *
 *      In this version of code, we are not using split blocks for the
 *      buffers require all buffers to be fully sized packets (1500+ bytes)
 *
 *      TXDesc[0]                                <-----+
 *        B1ADD              --> [= TXBuffer[0] =]     |
 *        B2ADD              -->    TXDesc[1] --+      |
 *      TXDesc[1]                             <-+      |
 *        B1ADD              --> [= TXBuffer[1] =]     |
 *        B2ADD              -->    TXDesc[2] --+      |
 *      TXDesc[2]                             <-+      |
 *        B1ADD              --> [= TXBuffer[2] =]     |
 *        B2ADD              -->    TXDesc[3] --+      |
 *      TXDesc[3]                             <-+      |
 *        B1ADD              --> [= TXBuffer[3] =]     |
 *        B2ADD              -->    TXDesc[0] ---------+
 *
 *---------------------------------------------------------------------------*/
void ILPC43xx_EMAC_InitTxDescriptors(void)
{
    TUInt32 i;

    memset((void*)TXDescs, 0, sizeof(TXDescs));

    // Setup desctriptors, 1 for 1, and we'll provide CRC and any data padding (to pad out to 64 byte minimum packets)
    for (i = 0; i < ENET_NUM_TX_DESC; i++) {
        TXDescs[i].CTRLSTAT = (1 << 20) | (3 << 22);
//        TXDescs[i].CTRLSTAT = (1 << 20) | (3 << 22) | TXDESC_STATUS_DP | TXDESC_STATUS_CRC;
        TXDescs[i].B2ADD = (TUInt32) &TXDescs[i + 1];
        TXDescs[i].B1ADD = (TUInt32) &TXBuffer[i];
    }
    TXDescs[ENET_NUM_TX_DESC - 1].CTRLSTAT = (1 << 20) | (1 << 21) | (3 << 23);
//    TXDescs[ENET_NUM_TX_DESC - 1].CTRLSTAT = TXDESC_STATUS_TCH | TXDESC_STATUS_TER | (3 << 22 /* reserved */) | TXDESC_STATUS_DP | TXDESC_STATUS_CRC;
    TXDescs[ENET_NUM_TX_DESC - 1].B2ADD = (TUInt32) &TXDescs[0];

    /* Set EMAC Transmit Descriptor Registers. */
    LPC_ETHERNET->DMA_TRANS_DES_ADDR = (TUInt32)TXDescs;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_DetectEMAC
 *---------------------------------------------------------------------------*
 * Description:
 *      Detect if the EMAC can be found at all.
 * Inputs:
 *      T_LPC43xx_EMAC_Workspace *p -- Workspace
 * Outputs:
 *      T_uezError                 -- Error code of UEZ_ERROR_NOT_FOUND if
 *                                    not detected, else UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
#if EMAC_USE_BITBANG_MDIO_DETECT
static T_uezError LPC43xx_EMAC_DetectEMAC(T_LPC43xx_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NOT_FOUND;
    T_uezGPIOPortPin pinMDC = p->iMDIOSettings.iPinMDC;
    T_uezGPIOPortPin pinMDIO = p->iMDIOSettings.iPinMDIO;
    T_gpioMux pinMDCMux;
    T_gpioMux pinMDIOMux;
    TUInt32 n;
    TUInt32 v;
    T_mdioBitBangSettings mdio;

    // Check that the MDIO lines are available
    if ((pinMDC != GPIO_NONE) && (pinMDIO != GPIO_NONE)) {
        // Remember what the settings of these pins were
        UEZGPIOGetMux(pinMDC, &pinMDCMux);
        UEZGPIOGetMux(pinMDIO, &pinMDIOMux);

        // Set the signals to high
        UEZGPIOSet(pinMDC);
        UEZGPIOSet(pinMDIO);
        if((pinMDC >> 8) <= 4){
            UEZGPIOSetMux(pinMDC, 0);
        } else {
            UEZGPIOSetMux(pinMDC, 4);
        }
        if((pinMDIO >> 8) <= 4){
            UEZGPIOSetMux(pinMDIO, 0);
        } else {
            UEZGPIOSetMux(pinMDIO, 4);
        }

        mdio.iDelayUSPerHalfBit = 50;// 50 uS per half, kind of slow
        mdio.iIsClause45 = EFalse;
        mdio.iPinMDC = pinMDC;
        mdio.iPinMDIO = pinMDIO;

        for (n=0; n<32; n++) {
#if DEBUG_LPC43xx_EMAC_STARTUP
            printf("LPC43xx_EMAC_DetectEMAC: PHY %d, Reg 2 = ", n);
#endif
            v = MDIOBitBangRead(&mdio, n, 0x0002);
#if DEBUG_LPC43xx_EMAC_STARTUP
            printf("%04X\n", v);
#endif
            if (v != 0xFFFF && v != 0) {
                error = UEZ_ERROR_NONE;
                break;
            }
        }

        // Put the settings back
        UEZGPIOSetMux(pinMDC, pinMDCMux);
        UEZGPIOSetMux(pinMDIO, pinMDIOMux);
    }

    return error;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure and setup the EMAC screen with the given settings
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *      T_EMACSettings *aSettings -- Pointer to settings to use
 * Outputs:
 *      T_uezError                -- Error code.  Returns UEZ_ERROR_NOT_FOUND
 *                                  if PHY not connected.
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_EMAC_Configure(void *aWorkspace, T_EMACSettings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;
    TUInt32 regv, timeout;
    TUInt32 retry;
    TUInt32 addr = 1;
    TUInt32 uniqueID;
#if 1
    TUInt16 v;
#endif

    // Because the LPC43xx has a problem with EMAC's that are missing,
    // we will detect the EMAC using bit bang I2C
    // Don't allow this if not detected
    if (!p->iIsDetected)
        return UEZ_ERROR_NOT_FOUND;

    // Copy over the settings
    p->iSettings = *aSettings;

    // Shift the phy address now to make the calculations faster
    p->iSettings.iPHYAddr = (p->iSettings.iPHYAddr << 11);

    // Initializes the EMAC Ethernet controller

    // Delay a small amount
    UEZBSPDelayMS(1);

    LPC_CREG->CREG6 |= 0x4;

    LPC_RGU->RESET_CTRL0 = (1 << 22);

    while((LPC_RGU->RESET_ACTIVE_STATUS0 & (1<<22)) == 1);

    LPC_ETHERNET->DMA_BUS_MODE |= (1<<0);

    while((LPC_ETHERNET->DMA_BUS_MODE & 0x01) == 1);

    LPC_ETHERNET->DMA_BUS_MODE = /*(1 << 1) |*/ (1 << 7) | (1 << 8) | (1 << 17);

    LPC_ETHERNET->MAC_CONFIG =  (3<<17) | (1<<15) | (1 << 14) | (1<<13) | (1 << 11) | (1 << 10);

    /* Setup default filter */
    LPC_ETHERNET->MAC_FRAME_FILTER = (1<<0) | (1UL<<31);

    /* Flush transmit FIFO */
    LPC_ETHERNET->DMA_OP_MODE = (1<<20);

    /* Setup DMA to flush receive FIFOs at 32 bytes, service TX FIFOs at
       64 bytes */
    LPC_ETHERNET->DMA_OP_MODE |= (1<<3) | (0<<14);

    /* Clear all MAC interrupts */
    LPC_ETHERNET->DMA_STAT = 0x1E7FF;

    LPC_ETHERNET->DMA_INT_EN = 0;

    LPC_ETHERNET->MAC_ADDR0_HIGH = (p->iSettings.iMACAddress[5] << 8) |
                                   (p->iSettings.iMACAddress[4]);
    LPC_ETHERNET->MAC_ADDR0_LOW =  (p->iSettings.iMACAddress[3] << 24) |
                                   (p->iSettings.iMACAddress[2] << 16) |
                                   (p->iSettings.iMACAddress[1] << 8) |
                                   (p->iSettings.iMACAddress[0]);

    LPC_ETHERNET->MAC_CONFIG =  (3<<17) | (1<<15) | (1 << 14) | (1<<13) | (1 << 11) | (1 << 10);

    /* Setup default filter */
    LPC_ETHERNET->MAC_FRAME_FILTER = (1<<0) | (1UL<<31);

    UEZBSPDelayMS(500);

    // Find the PHY address
    for (addr = 0; addr < 16; addr++) {
        p->iSettings.iPHYAddr = addr << 11;
        v = IPHYRead(p, PHY_REG_IDR1);
#if DEBUG_LPC43xx_EMAC_STARTUP
        printf("ADDR: %d, v=%04X\n", addr, v);
#endif
        if (v != 0xFFFF) {
            break;
        }
    }
#if DEBUG_LPC43xx_EMAC_STARTUP
    printf("Addr: %d, Settings: %08X\n", addr, p->iSettings.iPHYAddr);
#endif

    /* Put the Phy in reset mode */
    IPHYWrite(p, PHY_REG_BMCR, 0x8000);
    UEZBSPDelayMS(1);

    // Wait for hardware reset to end.
    // We try 100 times with a 10 ms delay for a complete
    // time of 1 second.
    retry = 3;
    do {
        for (timeout = 0; timeout < 100; timeout++) {
            UEZBSPDelayMS(10);
            regv = IPHYRead(p, PHY_REG_BMCR);
            if (!(regv & 0x8000)) {
                // Reset complete
                break;
            }
        }
#if DEBUG_LPC43xx_EMAC_STARTUP
        printf("regv=%04X\n", regv);
#endif
    } while ((timeout == 100) && (--retry));

    if (!retry) {
        // report a timeout on the initialization
        return UEZ_ERROR_TIMEOUT;
    }

    // Setup PAUSE control (PAUSE and ASM_DIR in ANAR register)
    regv = IPHYRead(p, PHY_REG_ANAR);
#if DEBUG_LPC43xx_EMAC_STARTUP
    printf("pause regv=%04X\n", regv);
#endif
    //IPHYWrite(p, PHY_REG_ANAR, regv | (1 << 10) | (1 << 11));

    uniqueID = (IPHYRead(p, PHY_REG_IDR1) << 16) | IPHYRead(p, PHY_REG_IDR2);
#if DEBUG_LPC43xx_EMAC_STARTUP
    printf("PHY ID: %08X\n", uniqueID);
#endif

    switch (uniqueID) {
        case 0x20005C90: // National PHY
            error = IEMACConfigPHY_National(p);
            break;
        case 0x00221610: // Micrel
        case 0x00221512: // Micrel KSZ8041TL-FTL
        case 0x00221513: // Micrel KSZ8041NL
        case 0x00221555: //Micrel KSZ8051
#if DEBUG_LPC43xx_EMAC_STARTUP
            printf("Micrel init\n");
#endif
            error = IEMACConfigPHY_Micrel(p);
            break;
        case 0x0007C0F1: // SMSC LAN8720
        case 0x000740F1: // SMSC LAN8720 (optional other ID)
#if DEBUG_LPC43xx_EMAC_STARTUP
            printf("SMSC LAN8720 init\n");
#endif
            // Uses same initialization as Micrel
            error = IEMACConfigPHY_Micrel(p);
            break;
        default:
            // auto negotiation, restart AN
            // RE_AN should be self cleared
            IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);
#if DEBUG_LPC43xx_EMAC_STARTUP
            printf("Unknown PHY: %08X\n", uniqueID);
#endif
            return UEZ_ERROR_NOT_SUPPORTED;
    }

    // Initialize Tx and Rx DMA Descriptors
    ILPC43xx_EMAC_InitTxDescriptors();
    ILPC43xx_EMAC_InitRxDescriptors();
    p->iReceiveIndex = 0;
    p->iTransmitIndex = 0;

    /* Flush transmit FIFO */
    LPC_ETHERNET->DMA_OP_MODE = (1<<20);

    /* Setup DMA to flush receive FIFOs at 32 bytes, service TX FIFOs at
       64 bytes */
    LPC_ETHERNET->DMA_OP_MODE |= (1<<3) | (0<<14);

    /* Clear all MAC interrupts */
    LPC_ETHERNET->DMA_STAT = 0x1E7FF;

    /* Enable MAC interrupts */
    LPC_ETHERNET->DMA_INT_EN = (1 << 0) | (1 << 4) | (1 << 5) | (1 << 6) |
                               (1 << 16) | (1 << 15) | (1 << 2) | (1 << 7);

    LPC_ETHERNET->DMA_OP_MODE |= /*(1 << 24) |*/ (1 << 13) | (1 << 1);
    
    /* Enable receive and transmit mode of MAC Ethernet core */
    LPC_ETHERNET->MAC_CONFIG |= (1 << 3) | (1 << 2);
    
    LPC_ETHERNET->DMA_REC_POLL_DEMAND = 1;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_CheckFrameReceived
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if a frame has been received.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 * Outputs:
 *      TBool                     -- ETrue if frame received, else EFalse.
 *---------------------------------------------------------------------------*/
TBool LPC43xx_EMAC_CheckFrameReceived(void *aWorkspace)
{
    ENET_RXDESC_T *p_rdesc;
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;
    p_rdesc = (ENET_RXDESC_T *)&RXDescs[p->iReceiveIndex];

    // Ensure we poll for demand
    LPC_ETHERNET->DMA_REC_POLL_DEMAND = 1;

    // Is the current descriptor waiting for us?
    if ((p_rdesc->STATUS & RXDESC_STATUS_OWN)==0) {
        // Not owned by DMA, so it is for us!
        return ETrue;
    }

    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_StartReadFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Reads the length of the received ethernet frame.  Returns the
 *      frame length.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 * Outputs:
 *      TUInt16                   -- Size of frame
 *---------------------------------------------------------------------------*/
TUInt16 LPC43xx_EMAC_StartReadFrame(void *aWorkspace)
{
    TUInt16 RxLen = 0;
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;
    ENET_ENHRXDESC_T *pd = &RXDescs[p->iReceiveIndex];

    // Just get hte current receive index's size
    RxLen = RXDESC_STATUS_FLEN(pd->STATUS);
    p->rptr = &RXBuffer[p->iReceiveIndex][0];

    return RxLen;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_EndReadFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      End the last frame.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *---------------------------------------------------------------------------*/
void LPC43xx_EMAC_EndReadFrame(void *aWorkspace)
{
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;
    ENET_ENHRXDESC_T *pd = &RXDescs[p->iReceiveIndex];

    // Mark owned by DMA now
    pd->STATUS |= RXDESC_STATUS_OWN;

    // Move to the next free slot
    p->iReceiveIndex++;
    if (p->iReceiveIndex >= ENET_NUM_RX_DESC)
        p->iReceiveIndex = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_RequestSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Requests space in EMAC memory for storing an outgoing frame
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *---------------------------------------------------------------------------*/
void LPC43xx_EMAC_RequestSend(void *aWorkspace)
{
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;
    ENET_ENHTXDESC_T *pd = &TXDescs[p->iTransmitIndex];

// TODO: It would have been nice to respond with a 'false' if there is
// no room to send data!
    if (pd->CTRLSTAT & TXDESC_STATUS_OWN) {
#if DEBUG_LPC43xx_EMAC_PACKETS_OUT
        printf("Error!  No room to send packets!\n");
#endif
        p->tptr = 0;
        p->tsize = 0;
    } else {
        // Start the pointer here
        p->tptr = &TXBuffer[p->iTransmitIndex][0];
        p->tsize = 0;
// TESTING:  Clear the packet before we put data into it so we can see
//           if we didn't write to the whole packet
        memset(p->tptr, 0xCC, sizeof(TXBuffer[p->iTransmitIndex]));
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_DoSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Send the frame
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      TUInt16 aFrameSize          -- Size of frame
 *---------------------------------------------------------------------------*/
void LPC43xx_EMAC_DoSend(void *aWorkspace, TUInt16 aFrameSize)
{
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;
    ENET_ENHTXDESC_T *pd = &TXDescs[p->iTransmitIndex];

    if (p->tptr) {
#if DEBUG_LPC43xx_EMAC_PACKETS_OUT
        printf("Sending %d\n", p->iTransmitIndex);
        if (aFrameSize != p->tsize) {
            printf("Error!  Transfer sizes do not match! (%d != %d)", aFrameSize, p->tsize);
        }
#endif
        // Write the final size to send
        pd->BSIZE = aFrameSize; // cut off CRC and let the system create it

        // Give the block to the DMA to send out the network.  It'll clear
        // it when done.
        pd->CTRLSTAT |= TXDESC_STATUS_OWN | TXDESC_STATUS_LS | TXDESC_STATUS_FS;

        // Move to next transmit block
        p->iTransmitIndex++;
        if (p->iTransmitIndex >= ENET_NUM_TX_DESC)
            p->iTransmitIndex = 0;

        // No pointer declared
        p->tptr = 0;
    } else {
#if DEBUG_LPC43xx_EMAC_PACKETS_OUT
        printf("Sending data to nowhere!\n");
#endif
    }

    // Ensure we tell the MAC to check the descriptors to send everything
    LPC_ETHERNET->DMA_TRANS_POLL_DEMAND = 1;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_CopyToFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Copies bytes from MCU-memory to frame port
 *      NOTES:
 *        - an odd number of byte may only be transfered
 *                  if the frame is written to the end!
 *        - MCU-memory MUST start at word-boundary
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      void *aSource               -- Location of data to send
 *      TUInt32 aSize               -- Number of bytes to send
 *---------------------------------------------------------------------------*/
void LPC43xx_EMAC_CopyToFrame(void *aWorkspace, void *aSource, TUInt32 aSize)
{
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;

    if (p->tptr) {
        // NOTE: We're not checking to see if there is room!
        memcpy(p->tptr, aSource, aSize);

#if DEBUG_LPC43xx_EMAC_PACKETS_OUT
        printf("OUT Packet %4d%s\n", aSize, (p->tptr != &TXBuffer[p->iTransmitIndex][0])?" (continuation)":"");
        int32_t len = aSize;
        for (int32_t i=0; i<len; i++) {
            printf("%02X ", ((TUInt8 *)aSource)[i]);
            if ((i%16) == 15)
                printf("\n");
        }
         printf("\n");
#endif

         p->tptr += aSize;
         p->tsize += aSize;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_CopyFromFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Copies bytes from frame port to MCU-memory
 *      This routine can be called multiple times to break up packets
 *      into smaller pbufs or sections.
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      void *aDestination          -- Location to store data
 *      TUInt32 aSize               -- Number of bytes to copy
 *---------------------------------------------------------------------------*/
void LPC43xx_EMAC_CopyFromFrame(
    void *aWorkspace,
    void *aDestination,
    TUInt32 aSize)
{
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;
    memcpy(aDestination, p->rptr, aSize);

#if DEBUG_LPC43xx_EMAC_PACKETS_IN
    printf("IN  Packet %4d%s\n", aSize, (p->rptr != &RXBuffer[p->iReceiveIndex][0])?" (continuation)":"");
    int32_t len = aSize;
    for (int32_t i=0; i<len; i++) {
        printf("%02X ", ((TUInt8 *)aDestination)[i]);
        if ((i%16) == 15)
            printf("\n");
    }
     printf("\n");
#endif

     // TESTING
//     memset(p->rptr, 0xCC, aSize));

     p->rptr += aSize;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      An interrupt on this EMAC has been received.  Catch it and
 *      possible pass it on to a callback routine.
 *---------------------------------------------------------------------------*/
static void LPC43xx_EMAC_ProcessInterrupt(void)
{
    T_LPC43xx_EMAC_Workspace *p = G_LPC43xx_EMAC;
    TUInt32 status;

    // Clear the interrupt.
    status = LPC_ETHERNET->DMA_STAT;
    if (status & (1<<6)) {
        if (p->iReceiveCallback) {
            p->iReceiveCallback(p->iReceiveCallbackWorkspace);
        }
    }
    // Clear any interrupts that triggered this code
    LPC_ETHERNET->DMA_STAT = status;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Stub to catch a EMAC interrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE( LPC43xx_EMAC_ISR)
{
    IRQ_START();
    LPC43xx_EMAC_ProcessInterrupt();
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_EnableReceiveInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable interrupts to occur when packets are received by the EMAC
 *      instead of just polling for the data.  The given callback routine
 *      will be called per interrupt.  The callback routine is called
 *      from withing the interrupt routine, so it should merely set
 *      a flag, release a semaphore, or any other minimal processing, and
 *      return quickly.
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      T_uezEMACReceiveInterruptCallback aCallback -- Routine to call on
 *          interrupts.  Can be set to 0 for no processing.
 *      void *aCallbackWorkspace -- Workspace pointer to pass to callback
 *          or 0 for none.
 *---------------------------------------------------------------------------*/
void LPC43xx_EMAC_EnableReceiveInterrupt(
    void *aWorkspace,
    T_uezEMACReceiveInterruptCallback aCallback,
    void *aCallbackWorkspace)
{
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;

    p->iReceiveCallback = aCallback;
    p->iReceiveCallbackWorkspace = aCallbackWorkspace;
    // Register this interrupt (if not already)
    InterruptRegister(ETHERNET_IRQn, (TISRFPtr)LPC43xx_EMAC_ISR,
        INTERRUPT_PRIORITY_HIGH, "EMAC");
    InterruptEnable(ETHERNET_IRQn);
    // Turn on those types of interrupts
    //LPC_ETHERNET->DMA_INT_EN = (1<<16) | (1<<6);//INT_RX_DONE;
    LPC_ETHERNET->DMA_INT_EN = (1 << 0) | (1 << 4) | (1 << 5) | (1 << 6) |
                               (1 << 16) | (1 << 15) | (1 << 2) | (1 << 7);
    LPC_ETHERNET->DMA_REC_POLL_DEMAND = 1;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_DisableReceiveInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Disables receive interrupts from the EMAC.
 *---------------------------------------------------------------------------*/
void LPC43xx_EMAC_DisableReceiveInterrupt(void *aWorkspace)
{
    // Turn off those types of interrupts
    LPC_ETHERNET->DMA_INT_EN = 0;//INT_RX_DONE;
    // Turn off the whole interrutp since it is the only one
    InterruptDisable(ETHERNET_IRQn);//?
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this EMAC Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_EMAC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_EMAC_Workspace *p = (T_LPC43xx_EMAC_Workspace *)aWorkspace;

    G_LPC43xx_EMAC = p;
    p->iIsDetected = EFalse;

    // Ensure our pointers and indexes are zero'd out
    p->tptr = 0;
    p->rptr = 0;
    p->iReceiveIndex = 0;
    p->iTransmitIndex = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_EMAC G_LPC43xx_EMAC_Interface = {
    {
        "LPC43xx EMAC",
        0x0100,
        LPC43xx_EMAC_InitializeWorkspace,
        sizeof(T_LPC43xx_EMAC_Workspace), },

    LPC43xx_EMAC_Configure,
    LPC43xx_EMAC_CheckFrameReceived,
    LPC43xx_EMAC_StartReadFrame,
    LPC43xx_EMAC_EndReadFrame,
    LPC43xx_EMAC_RequestSend,
    LPC43xx_EMAC_DoSend,
    LPC43xx_EMAC_CopyToFrame,
    LPC43xx_EMAC_CopyFromFrame,

    // uEZ v1.10
    LPC43xx_EMAC_EnableReceiveInterrupt,
    LPC43xx_EMAC_DisableReceiveInterrupt,

};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_EMAC_RMII_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Creates the EMAC HAL driver for the LPC43xx device.  Also looks
 *      to see if it exists.  If found, sets the value in *aIsFound to ETrue.
 *      If NOT found, the value in *aIsFound IS NOT CHANGED!  Set *aIsFound
 *      to EFalse before calling this routine.
 * Inputs:
 *      T_LPC43xx_EMAC_Settings *aSettings -- Settings to configure EMAC as
 *      TBool *aIsFound -- REturned flag if found
 *---------------------------------------------------------------------------*/
#define EMAC_DRIVE(mode)          SCU_NORMAL_DRIVE(mode, \
                                                   SCU_EPD_DISABLE, \
                                                   SCU_EPUN_DISABLE, \
                                                   SCU_EHS_FAST, \
                                                   SCU_EZI_ENABLE, \
                                                   SCU_ZIF_ENABLE)
void LPC43xx_EMAC_RMII_Require(const T_LPC43xx_EMAC_Settings *aSettings)
{
    T_LPC43xx_EMAC_Workspace *p;

    static const T_LPC43xx_SCU_ConfigList TX_EN[] = {
            {GPIO_P0_1       , SCU_NORMAL_DRIVE_DEFAULT(6)}, //TX Enable
            {GPIO_P6_3       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //TX Enable
    };
    static const T_LPC43xx_SCU_ConfigList TXD3[] = {
            {GPIO_P5_18      , SCU_NORMAL_DRIVE_DEFAULT(5)}, //TXD 3
            {GPIO_P6_2       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //TXD 3
    };
    static const T_LPC43xx_SCU_ConfigList TXD2[] = {
            {GPIO_P5_17      , SCU_NORMAL_DRIVE_DEFAULT(5)}, //TXD 2
            {GPIO_P6_1       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //TXD 2
    };
    static const T_LPC43xx_SCU_ConfigList TXD1[] = {
            {GPIO_P0_15      , SCU_NORMAL_DRIVE_DEFAULT(3)}, //TXD 1
    };
    static const T_LPC43xx_SCU_ConfigList TXD0[] = {
            {GPIO_P0_13      , SCU_NORMAL_DRIVE_DEFAULT(3)}, //TXD 0
    };
    static const T_LPC43xx_SCU_ConfigList TX_ER[] = {
            {GPIO_P6_4       , SCU_NORMAL_DRIVE_DEFAULT(4)}, //TX Error
            {GPIO_P6_13      , SCU_NORMAL_DRIVE_DEFAULT(6)}, //TX Error
    };
    static const T_LPC43xx_SCU_ConfigList TX_CLK[] = {
            {GPIO_PZ_0_P1_19 , SCU_NORMAL_DRIVE(0, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))}, //TX Clk
    };
    static const T_LPC43xx_SCU_ConfigList RX_DV[] = {
            {GPIO_P0_3       , SCU_NORMAL_DRIVE_DEFAULT(7)}, //RX DV
            {GPIO_P6_7       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //RX DV
    };
    static const T_LPC43xx_SCU_ConfigList RXD3[] = {
            {GPIO_P4_14      , SCU_NORMAL_DRIVE_DEFAULT(5)}, //RXD 3
            {GPIO_P6_6       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //RXD 3
    };
    static const T_LPC43xx_SCU_ConfigList RXD2[] = {
            {GPIO_P4_15      , SCU_NORMAL_DRIVE_DEFAULT(5)}, //RXD 2
            {GPIO_P6_5       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //RXD 2
    };
    static const T_LPC43xx_SCU_ConfigList RXD1[] = {
            {GPIO_P0_0       , SCU_NORMAL_DRIVE_DEFAULT(2)}, //RXD 1
    };
    static const T_LPC43xx_SCU_ConfigList RXD0[] = {
            {GPIO_P0_2       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //RXD 0
    };
    static const T_LPC43xx_SCU_ConfigList RX_ER[] = {
            {GPIO_P4_13      , SCU_NORMAL_DRIVE_DEFAULT(5)}, //RX Error
            {GPIO_P6_8       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //RX Error
    };
    static const T_LPC43xx_SCU_ConfigList RX_CLK[] = {
            {GPIO_PZ_Z_PC_0  , SCU_NORMAL_DRIVE(3, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))}, //RX Clk
    };
    static const T_LPC43xx_SCU_ConfigList COL[] = {
            {GPIO_P0_1       , SCU_NORMAL_DRIVE_DEFAULT(2)}, //COL
            {GPIO_P2_1       , SCU_NORMAL_DRIVE_DEFAULT(7)}, //COL
            {GPIO_P4_11      , SCU_NORMAL_DRIVE_DEFAULT(5)}, //COL
    };
    static const T_LPC43xx_SCU_ConfigList CRS[] = {
            {GPIO_P0_3       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //CRS
            {GPIO_P4_2       , SCU_NORMAL_DRIVE_DEFAULT(5)}, //CRS
    };
    static const T_LPC43xx_SCU_ConfigList MDC[] = {
            {GPIO_P5_0       , SCU_NORMAL_DRIVE_DEFAULT(7)}, //MDC
            {GPIO_P3_15      , SCU_NORMAL_DRIVE_DEFAULT(6)}, //MDC
            {GPIO_P6_0       , SCU_NORMAL_DRIVE_DEFAULT(3)}, //MDC
    };
    static const T_LPC43xx_SCU_ConfigList MDIO[] = {
            {GPIO_P0_12      , SCU_NORMAL_DRIVE_DEFAULT(3)}, //MDIO
    };

    HAL_DEVICE_REQUIRE_ONCE();

    // Register EMAC
    HALInterfaceRegister("EMAC", (T_halInterface *)&G_LPC43xx_EMAC_Interface, 0,
        (T_halWorkspace **)&p);

    LPC43xx_SCU_ConfigPinOrNone(aSettings->iTX_EN, TX_EN, ARRAY_COUNT(TX_EN));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iTXD3, TXD3, ARRAY_COUNT(TXD3));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iTXD2, TXD2, ARRAY_COUNT(TXD2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iTXD1, TXD1, ARRAY_COUNT(TXD1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iTXD0, TXD0, ARRAY_COUNT(TXD0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iTX_ER, TX_ER,
            ARRAY_COUNT(TX_ER));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iTX_CLK, TX_CLK,
            ARRAY_COUNT(TX_CLK));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iRX_DV, RX_DV,
            ARRAY_COUNT(RX_DV));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iRXD3, RXD3,
            ARRAY_COUNT(RXD3));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iRXD2, RXD2,
            ARRAY_COUNT(RXD2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iRXD1, RXD1, ARRAY_COUNT(RXD1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iRXD0, RXD0, ARRAY_COUNT(RXD0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iRX_ER, RX_ER, ARRAY_COUNT(RX_ER));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iRX_CLK, RX_CLK, ARRAY_COUNT(RX_CLK));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCOL, COL, ARRAY_COUNT(COL));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCRS, CRS, ARRAY_COUNT(CRS));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMDC, MDC, ARRAY_COUNT(MDC));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMDIO, MDIO, ARRAY_COUNT(MDIO));

#if EMAC_USE_BITBANG_MDIO_DETECT
    p->iMDIOSettings.iDelayUSPerHalfBit = EMAC_BITBANG_MDIO_DELAY_US;
    p->iMDIOSettings.iIsClause45 = EFalse;
    p->iMDIOSettings.iPinMDC = aSettings->iMDC;
    p->iMDIOSettings.iPinMDIO = aSettings->iMDIO;
    if (LPC43xx_EMAC_DetectEMAC(p) == UEZ_ERROR_NONE)
        p->iIsDetected = ETrue;
#else
    p->iIsDetected = ETrue;
#endif
}

/*-------------------------------------------------------------------------*
 * End of File:  EMAC.c
 *-------------------------------------------------------------------------*/
