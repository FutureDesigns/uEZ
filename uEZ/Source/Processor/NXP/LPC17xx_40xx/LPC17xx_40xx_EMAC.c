/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_EMAC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC17xx_40xx EMAC (Ethernet) Driver.
 * Implementation:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/******************************************************************
 *****  Original credits:                                     *****
 *****  Name: cs8900.c                                        *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****  Func: ethernet packet-driver for use with LAN-        *****
 *****        controller CS8900 from Crystal/Cirrus Logic     *****
 *****                                                        *****
 *****  Keil: Module modified for use with Philips            *****
 *****        LPC2378 EMAC Ethernet controller                *****
 *****                                                        *****
 ******************************************************************/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <stdio.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_EMAC.h>
#include <uEZBSP.h>
#include <uEZGPIO.h>
#include <Source/Library/Network/PHY/MDIOBitBang/MDIOBitBang.h>

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef DEBUG_LPC17xx_40xx_EMAC_STARTUP
#define DEBUG_LPC17xx_40xx_EMAC_STARTUP  0
#endif

#ifndef EMAC_USE_BITBANG_MDIO_DETECT
/* Bit banging to the EMAC can be used to avoid the dread LPC17xx_40xx lock up
 * that occurs when a PHY is attempting to be communicated with and there
 * is NO oscillation.  The LPC17xx_40xx state machine locks up hard and only
 * a power cycle recovers.
 */
#define EMAC_USE_BITBANG_MDIO_DETECT       1
#endif

#define EMAC_BITBANG_MDIO_DELAY_US  10 /* uSecs */

/*---------------------------------------------------------------------------*
 * Regions:
 *---------------------------------------------------------------------------*/
#if ((COMPILER_TYPE==RowleyARM) || (COMPILER_TYPE==Keil4))
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

/* EMAC variables located in 16K Ethernet SRAM */
#define RX_DESC_BASE        ((TUInt32)G_emacMemory) //AHBSRAM0_BASE
#define RX_STAT_BASE        (RX_DESC_BASE + NUM_RX_FRAG*8)
#define TX_DESC_BASE        (RX_STAT_BASE + NUM_RX_FRAG*8)
#define TX_STAT_BASE        (TX_DESC_BASE + NUM_TX_FRAG*8)
#define RX_BUF_BASE         (TX_STAT_BASE + NUM_TX_FRAG*4)
#define TX_BUF_BASE         (RX_BUF_BASE  + NUM_RX_FRAG*ETH_FRAG_SIZE)
#define EMAC_MEMORY_SIZE    (NUM_RX_FRAG*8 + NUM_RX_FRAG*8 + NUM_TX_FRAG*8 + \
                              NUM_TX_FRAG*4 + NUM_RX_FRAG*ETH_FRAG_SIZE)

/* RX and TX descriptor and status definitions. */
#define RX_DESC_PACKET(i)   (*(unsigned int *)(RX_DESC_BASE   + 8*i))
#define RX_DESC_CTRL(i)     (*(unsigned int *)(RX_DESC_BASE+4 + 8*i))
#define RX_STAT_INFO(i)     (*(unsigned int *)(RX_STAT_BASE   + 8*i))
#define RX_STAT_HASHCRC(i)  (*(unsigned int *)(RX_STAT_BASE+4 + 8*i))
#define TX_DESC_PACKET(i)   (*(unsigned int *)(TX_DESC_BASE   + 8*i))
#define TX_DESC_CTRL(i)     (*(unsigned int *)(TX_DESC_BASE+4 + 8*i))
#define TX_STAT_INFO(i)     (*(unsigned int *)(TX_STAT_BASE   + 4*i))
#define RX_BUF(i)           (RX_BUF_BASE + ETH_FRAG_SIZE*i)
#define TX_BUF(i)           (TX_BUF_BASE + ETH_FRAG_SIZE*i)

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
#define BMSR_100BE_T4		0x8000
#define BMSR_100TX_FULL		0x4000
#define BMSR_100TX_HALF		0x2000
#define BMSR_10BE_FULL		0x1000
#define BMSR_10BE_HALF		0x0800
#define BMSR_AUTO_DONE		0x0020
#define BMSR_REMOTE_FAULT	0x0010
#define BMSR_NO_AUTO		0x0008
#define BMSR_LINK_ESTABLISHED	0x0004

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
    TUInt16 *rptr;
    TUInt16 *tptr;
    T_uezEMACReceiveInterruptCallback iReceiveCallback;
    void *iReceiveCallbackWorkspace;
    T_LPC17xx_40xx_EMAC_Settings iPinSettings;
    T_mdioBitBangSettings iMDIOSettings;
    TBool iIsDetected;
} T_LPC17xx_40xx_EMAC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_EMAC EMAC_LPC17xx_40xx_Interface;
static T_LPC17xx_40xx_EMAC_Workspace *G_LPC17xx_40xx_EMAC;

// Memory EMAC packet memory
static TUInt8 G_emacMemory[EMAC_MEMORY_SIZE] EMAC_MEMORY;

/*---------------------------------------------------------------------------*
 * Routine:  IPHYWrite
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the register in the given PHY to the given value.
 * Inputs:
 *      T_LPC17xx_40xx_EMAC_Workspace *p -- EMAC's Workspace
 *      TUInt32 aPhyReg             -- PHY register to write
 *      TUInt32 aValue              -- Value to write to register
 *---------------------------------------------------------------------------*/
static void IPHYWrite(T_LPC17xx_40xx_EMAC_Workspace *p, TUInt8 PhyReg, TUInt32 Value)
{
    TUInt32 timeout;

    // Write the PHY's register with the value
    LPC_EMAC->MADR = p->iSettings.iPHYAddr | PhyReg;
    LPC_EMAC->MWTD = Value;

    // Wait utill operation completed
    for (timeout = 0; timeout < MII_WR_TOUT; timeout++) {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
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
 *      T_LPC17xx_40xx_EMAC_Workspace *p -- EMAC's Workspace
 *      TUInt32 aPhyReg             -- PHY register to write
 *      TUInt32 aValue              -- Value to write to register
 *---------------------------------------------------------------------------*/
static TUInt16 IPHYRead(T_LPC17xx_40xx_EMAC_Workspace *p, TUInt8 PhyReg)
{
    TUInt32 timeout;

    // Read the PHY's register
    LPC_EMAC->MADR = p->iSettings.iPHYAddr | PhyReg;
    LPC_EMAC->MCMD = MCMD_READ;

    // Wait until operation completed
    for (timeout = 0; timeout < MII_RD_TOUT; timeout++) {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
            break;
        }
    }
    LPC_EMAC->MCMD = 0;

    return (LPC_EMAC->MRDD);
}

/*---------------------------------------------------------------------------*
 * Routine:  IEMACConfigPHY_National
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for the National PHY
 * Inputs:
 *      T_LPC17xx_40xx_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_National(T_LPC17xx_40xx_EMAC_Workspace *p)
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
            LPC_EMAC->MAC2 |= MAC2_FULL_DUP;
            LPC_EMAC->Command |= CR_FULL_DUP;
            LPC_EMAC->IPGT = IPGT_FULL_DUP;
        } else {
            // Half duplex mode.
            LPC_EMAC->IPGT = IPGT_HALF_DUP;
        }

        // Configure 100MBit/10MBit mode.
        if (regv & 0x0002) {
            // 10MBit mode.
            LPC_EMAC->SUPP = 0;
        } else {
            // 100MBit mode.
            LPC_EMAC->SUPP = SUPP_SPEED;
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
static T_uezError IEMACConfigPHY_Micrel(T_LPC17xx_40xx_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 timeout, regv;
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
    TUInt32 reg;
#endif
    // auto negotiation, restart AN
    // RE_AN should be self cleared
    IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);

    // Wait to complete Auto_Negotiation.
    for (timeout = 0; timeout < 30; timeout++) {
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
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
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
    printf("nego regv %04X\n", regv);
#endif
    // successful negotiations; update link info
    regv &= 0x001C;
    switch (regv) {
        case 0x0004:
            // Half duplex mode.
            LPC_EMAC->IPGT = IPGT_HALF_DUP;
            // 10MBit mode.
            LPC_EMAC->SUPP = 0;
            break;
        case 0x0008:
            // Half duplex mode.
            LPC_EMAC->IPGT = IPGT_HALF_DUP;
            // 100MBit mode.
            LPC_EMAC->SUPP = SUPP_SPEED;
            break;
        case 0x0014:
            // Full duplex is enabled.
            LPC_EMAC->MAC2 |= MAC2_FULL_DUP;
            LPC_EMAC->Command |= CR_FULL_DUP;
            LPC_EMAC->IPGT = IPGT_FULL_DUP;
            // 10MBit mode.
            LPC_EMAC->SUPP = 0;
            break;
        case 0x0018:
            // Full duplex is enabled.
            LPC_EMAC->MAC2 |= MAC2_FULL_DUP;
            LPC_EMAC->Command |= CR_FULL_DUP;
            LPC_EMAC->IPGT = IPGT_FULL_DUP;
            // 100MBit mode.
            LPC_EMAC->SUPP = SUPP_SPEED;
            break;
        default: // Should not come here, force to set default, 100 FULL_DUPLEX
            // Full duplex is enabled.
            LPC_EMAC->MAC2 |= MAC2_FULL_DUP;
            LPC_EMAC->Command |= CR_FULL_DUP;
            LPC_EMAC->IPGT = IPGT_FULL_DUP;
            // 100MBit mode.
            LPC_EMAC->SUPP = SUPP_SPEED;
            break;
    }

#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
    for (reg = 0; reg < 32; reg++) {
        regv = IPHYRead(p, reg);
        printf("phyreg %d = %04X\n", reg, regv);
    }
#endif

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_EMAC_InitRxDescriptors
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize Receive Descriptors
 *---------------------------------------------------------------------------*/
static void ILPC17xx_40xx_EMAC_InitRxDescriptors(void)
{
    TUInt32 i;

    for (i = 0; i < NUM_RX_FRAG; i++) {
        RX_DESC_PACKET(i) = RX_BUF(i);
        RX_DESC_CTRL(i) = RCTRL_INT | (ETH_FRAG_SIZE - 1);
        RX_STAT_INFO(i) = 0;
        RX_STAT_HASHCRC(i) = 0;
    }

    // Set EMAC Receive Descriptor Registers.
    LPC_EMAC->RxDescriptor = RX_DESC_BASE;
    LPC_EMAC->RxStatus = RX_STAT_BASE;
    LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG - 1;

    /* Rx Descriptors Point to 0 */
    LPC_EMAC->RxConsumeIndex = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_EMAC_InitTxDescriptors
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize Transmit Descriptors
 *---------------------------------------------------------------------------*/
void ILPC17xx_40xx_EMAC_InitTxDescriptors(void)
{
    TUInt32 i;

    for (i = 0; i < NUM_TX_FRAG; i++) {
        TX_DESC_PACKET(i) = TX_BUF(i);
        TX_DESC_CTRL(i) = 0;
        TX_STAT_INFO(i) = 0;
    }

    /* Set EMAC Transmit Descriptor Registers. */
    LPC_EMAC->TxDescriptor = TX_DESC_BASE;
    LPC_EMAC->TxStatus = TX_STAT_BASE;
    LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG - 1;

    /* Tx Descriptors Point to 0 */
    LPC_EMAC->TxProduceIndex = 0;
}
/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this EMAC Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_EMAC_InitializeWorkspace(void *aWorkspace)
{
    //    T_LPC17xx_40xx_EMAC_Workspace *p = (T_LPC17xx_40xx_EMAC_Workspace *)aWorkspace;

    // Nothing in this case
    G_LPC17xx_40xx_EMAC = aWorkspace;
    G_LPC17xx_40xx_EMAC->iIsDetected = EFalse;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_DetectEMAC
 *---------------------------------------------------------------------------*
 * Description:
 *      Detect if the EMAC can be found at all.
 * Inputs:
 *      T_LPC17xx_40xx_EMAC_Workspace *p -- Workspace
 * Outputs:
 *      T_uezError                 -- Error code of UEZ_ERROR_NOT_FOUND if
 *                                    not detected, else UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
#if EMAC_USE_BITBANG_MDIO_DETECT
static T_uezError LPC17xx_40xx_EMAC_DetectEMAC(T_LPC17xx_40xx_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NOT_FOUND;
    T_uezGPIOPortPin pinMDC = p->iPinSettings.iMDC;
    T_uezGPIOPortPin pinMDIO = p->iPinSettings.iMDIO;
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
        UEZGPIOSetMux(pinMDC, 0);
        UEZGPIOSetMux(pinMDIO, 0);

        mdio.iDelayUSPerHalfBit = 50;// 50 uS per half, kind of slow
        mdio.iIsClause45 = EFalse;
        mdio.iPinMDC = pinMDC;
        mdio.iPinMDIO = pinMDIO;

        for (n=0; n<32; n++) {
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
            printf("LPC17xx_40xx_EMAC_DetectEMAC: PHY %d, Reg 2 = ", n);
#endif
            v = MDIOBitBangRead(&mdio, n, 0x0002);
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
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
 * Routine:  LPC17xx_40xx_EMAC_Configure
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
T_uezError LPC17xx_40xx_EMAC_Configure(void *aWorkspace, T_EMACSettings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_LPC17xx_40xx_EMAC_Workspace *p = (T_LPC17xx_40xx_EMAC_Workspace *)aWorkspace;
    TUInt32 regv, timeout;
    TUInt32 retry;
    TUInt32 addr = 1;
    TUInt32 uniqueID;
#if 1
    TUInt16 v;
#endif

    // Because the LPC17xx_40xx has a problem with EMAC's that are missing,
    // we will detect the EMAC using bit bang I2C
    // Don't allow this if not detected
    if (!p->iIsDetected)
        return UEZ_ERROR_NOT_FOUND;

    // Copy over the settings
    p->iSettings = *aSettings;

    // Shift the phy address now to make the calculations faster
    p->iSettings.iPHYAddr <<= 8;

    // Initializes the EMAC ethernet controller
    // Ensure EMAC is powered up
    //LPC_SC->PCONP |= 0x40000000;
    LPC17xx_40xxPowerOn(0x40000000);

    // Delay a small amount
    UEZBSPDelayMS(1);

    // Reset all EMAC internal modules.
    // TBD: Should we use rx and tx flow control since we might be running too slow
    LPC_EMAC->MAC1 =
        MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX
            | MAC1_SIM_RES | MAC1_SOFT_RES /*| MAC1_RX_FLOWC | MAC1_TX_FLOWC*/;
    LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;

    // A short delay after reset.
    UEZBSPDelayMS(1);

    // Initialize MAC control registers.
    LPC_EMAC->MAC1 = MAC1_PASS_ALL;
    LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
    LPC_EMAC->MAXF = ETH_MAX_FLEN;
    LPC_EMAC->CLRT = CLRT_DEF;
    LPC_EMAC->IPGR = IPGR_DEF;

    // host clock divided by 28, no suppress preamble, no scan increment
    // TBD: Should this be calculated at run time?
    LPC_EMAC->MCFG = 0x801C;
    UEZBSPDelayMS(1);

    // Apply a reset
    LPC_EMAC->MCFG = 0x0018;
    LPC_EMAC->MCMD = 0;

    // RMII configuration
    LPC_EMAC->Command |= CR_RMII;

    // PHY support: [8]=0 ->10 Mbps mode, =1 -> 100 Mbps mode
    // RMII setting, at power-on, default set to 100.
    LPC_EMAC->SUPP = SUPP_SPEED;

    UEZBSPDelayMS(500);

    // Find the PHY address
    for (addr = 0; addr < 16; addr++) {
        p->iSettings.iPHYAddr = addr << 8;
        v = IPHYRead(p, PHY_REG_IDR1);
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
        printf("ADDR: %d, v=%04X\n", addr, v);
#endif
        if (v != 0xFFFF) {
            break;
        }
    }
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
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
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
        printf("regv=%04X\n", regv);
#endif
    } while ((timeout == 100) && (--retry));

    if (!retry) {
        // report a timeout on the initialization
        return UEZ_ERROR_TIMEOUT;
    }

    // Setup PAUSE control (PAUSE and ASM_DIR in ANAR register)
    regv = IPHYRead(p, PHY_REG_ANAR);
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
    printf("pause regv=%04X\n", regv);
#endif
    IPHYWrite(p, PHY_REG_ANAR, regv | (1 << 10) | (1 << 11));

    uniqueID = (IPHYRead(p, PHY_REG_IDR1) << 16) | IPHYRead(p, PHY_REG_IDR2);
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
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
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
            printf("Micrel init\n");
#endif
            error = IEMACConfigPHY_Micrel(p);
            break;
        case 0x0007C0F1: // SMSC LAN8720
        case 0x000740F1: // SMSC LAN8720 (optional other ID)
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
            printf("SMSC LAN8720 init\n");
#endif
            // Uses same initialization as Micrel
            error = IEMACConfigPHY_Micrel(p);
            break;
        default:
            // auto negotiation, restart AN
            // RE_AN should be self cleared
            IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);
#if DEBUG_LPC17xx_40xx_EMAC_STARTUP
            printf("Unknown PHY: %08X\n", uniqueID);
#endif
            return UEZ_ERROR_NOT_SUPPORTED;
    }

    LPC_EMAC->SA0 = (p->iSettings.iMACAddress[5] << 8)
        | p->iSettings.iMACAddress[4];
    LPC_EMAC->SA1 = (p->iSettings.iMACAddress[3] << 8)
        | p->iSettings.iMACAddress[2];
    LPC_EMAC->SA2 = (p->iSettings.iMACAddress[1] << 8)
        | p->iSettings.iMACAddress[0];

    // Initialize Tx and Rx DMA Descriptors
    ILPC17xx_40xx_EMAC_InitRxDescriptors();
    ILPC17xx_40xx_EMAC_InitTxDescriptors();

    // Receive Broadcast and Perfect Match Packets
    //Ake changed, RFC_UCAST_EN seems to be incorrect.
    //RxFilterCtrl = RFC_UCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;
    LPC_EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

    // Create the semaphore used ot wake the uIP task.
    //??    vSemaphoreCreateBinary( xEMACSemaphore );

    // Reset all interrupts
    LPC_EMAC->IntClear = 0xFFFF;

    /* Enable receive and transmit mode of MAC Ethernet core */
    LPC_EMAC->Command |= (CR_RX_EN | CR_TX_EN);
    LPC_EMAC->MAC1 |= MAC1_REC_EN;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_CheckFrameReceived
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if a frame has been received.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 * Outputs:
 *      TBool                     -- ETrue if frame received, else EFalse.
 *---------------------------------------------------------------------------*/
TBool LPC17xx_40xx_EMAC_CheckFrameReceived(void *aWorkspace)
{
    // more packets received ?
    if (LPC_EMAC->RxProduceIndex != LPC_EMAC->RxConsumeIndex)
        return ETrue;
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_StartReadFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Reads the length of the received ethernet frame.  Returns the
 *      frame length.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 * Outputs:
 *      TUInt16                   -- Size of frame
 *---------------------------------------------------------------------------*/
TUInt16 LPC17xx_40xx_EMAC_StartReadFrame(void *aWorkspace)
{
    TUInt16 RxLen;
    TUInt32 idx;
    T_LPC17xx_40xx_EMAC_Workspace *p = (T_LPC17xx_40xx_EMAC_Workspace *)aWorkspace;

    idx = LPC_EMAC->RxConsumeIndex;
    RxLen = (RX_STAT_INFO(idx) & RINFO_SIZE) - 3;
    p->rptr = (TUInt16 *)RX_DESC_PACKET(idx);

    return RxLen;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_EndReadFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      End the last frame.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_EMAC_EndReadFrame(void *aWorkspace)
{
    TUInt32 idx;

    // DMA free packet.
    idx = LPC_EMAC->RxConsumeIndex;
    if (++idx == NUM_RX_FRAG)
        idx = 0;
    LPC_EMAC->RxConsumeIndex = idx;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_RequestSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Requests space in EMAC memory for storing an outgoing frame
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_EMAC_RequestSend(void *aWorkspace)
{
    T_LPC17xx_40xx_EMAC_Workspace *p = (T_LPC17xx_40xx_EMAC_Workspace *)aWorkspace;
    TUInt32 idx;

    idx = LPC_EMAC->TxProduceIndex;
    p->tptr = (TUInt16 *)TX_DESC_PACKET(idx);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_DoSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Send the frame
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      TUInt16 aFrameSize          -- Size of frame
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_EMAC_DoSend(void *aWorkspace, TUInt16 aFrameSize)
{
    TUInt32 idx;

    idx = LPC_EMAC->TxProduceIndex;
    TX_DESC_CTRL(idx) = (aFrameSize - 1) | TCTRL_LAST; /*Corrected problem with the size, -1 was missing*/
    if (++idx == NUM_TX_FRAG)
        idx = 0;
    LPC_EMAC->TxProduceIndex = idx;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_WriteTxFrameData
 *---------------------------------------------------------------------------*
 * Description:
 *      Writes a word in little-endian byte order to TX_BUFFER
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      TUInt16 aData               -- Data to write
 *---------------------------------------------------------------------------*/
static void ILPC17xx_40xx_WriteTxFrameData(
    T_LPC17xx_40xx_EMAC_Workspace *p,
    TUInt16 aData)
{
    *(p->tptr++) = aData;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_CopyToFrame
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
void LPC17xx_40xx_EMAC_CopyToFrame(void *aWorkspace, void *aSource, TUInt32 aSize)
{
    T_LPC17xx_40xx_EMAC_Workspace *p = (T_LPC17xx_40xx_EMAC_Workspace *)aWorkspace;
    TUInt16 *piSource;

    piSource = aSource;
    aSize = (aSize + 1) & 0xFFFE; // round Size up to next even number
    while (aSize > 0) {
        ILPC17xx_40xx_WriteTxFrameData(p, *piSource++);
        aSize -= 2;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_ReadRxFrameData
 *---------------------------------------------------------------------------*
 * Description:
 *      Reads a word in little-endian byte order from RX_BUFFER
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 * Outputs:
 *      TUInt16 aData               -- Data read
 *---------------------------------------------------------------------------*/
TUInt16 ILPC17xx_40xx_ReadRxFrameData(T_LPC17xx_40xx_EMAC_Workspace *p)
{
    return (*(p->rptr++));
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_CopyFromFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Copies bytes from frame port to MCU-memory
 *      NOTES:
 *          - an odd number of byte may only be transfered
 *              if the frame is read to the end!
 *          - MCU-memory MUST start at word-boundary
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      void *aDestination          -- Location to store data
 *      TUInt32 aSize               -- Number of bytes to copy
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_EMAC_CopyFromFrame(
    void *aWorkspace,
    void *aDestination,
    TUInt32 aSize)
{
    T_LPC17xx_40xx_EMAC_Workspace *p = (T_LPC17xx_40xx_EMAC_Workspace *)aWorkspace;
    // Keil: Pointer added to correct expression
    TUInt16 *piDest;

    // Keil: Line added
    piDest = aDestination;
    while (aSize > 1) {
        *piDest++ = ILPC17xx_40xx_ReadRxFrameData(p);
        aSize -= 2;
    }

    // check for leftover byte...
    if (aSize) {
        // the LAN-Controller will return 0 for the highbyte
        *(TUInt8 *)piDest = (TUInt8)ILPC17xx_40xx_ReadRxFrameData(p);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      An interrupt on this EMAC has been received.  Catch it and
 *      possible pass it on to a callback routine.
 *---------------------------------------------------------------------------*/
static void LPC17xx_40xx_EMAC_ProcessInterrupt(void)
{
    T_LPC17xx_40xx_EMAC_Workspace *p = G_LPC17xx_40xx_EMAC;
    TUInt32 status;

    // Clear the interrupt.
    status = LPC_EMAC->IntStatus;
    if (status & EMAC_INT_RXDONE) {
        if (p->iReceiveCallback) {
            p->iReceiveCallback(p->iReceiveCallbackWorkspace);
        }
    }
    // Clear any interrupts that triggered this code
    LPC_EMAC->IntClear = status;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Stub to catch a EMAC interrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE( LPC17xx_40xx_EMAC_ISR)
{
    IRQ_START();
    LPC17xx_40xx_EMAC_ProcessInterrupt();
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_EnableReceiveInterrupt
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
void LPC17xx_40xx_EMAC_EnableReceiveInterrupt(
    void *aWorkspace,
    T_uezEMACReceiveInterruptCallback aCallback,
    void *aCallbackWorkspace)
{
    T_LPC17xx_40xx_EMAC_Workspace *p = (T_LPC17xx_40xx_EMAC_Workspace *)aWorkspace;

    p->iReceiveCallback = aCallback;
    p->iReceiveCallbackWorkspace = aCallbackWorkspace;
    // Register this interrupt (if not already)
    InterruptRegister(ENET_IRQn, (TISRFPtr)LPC17xx_40xx_EMAC_ISR,
        INTERRUPT_PRIORITY_HIGH, "EMAC");
    InterruptEnable(ENET_IRQn);
    // Turn on those types of interrupts
    LPC_EMAC->IntEnable = INT_RX_DONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_DisableReceiveInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Disables receive interrupts from the EMAC.
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_EMAC_DisableReceiveInterrupt(void *aWorkspace)
{
    // Turn off those types of interrupts
    LPC_EMAC->IntClear = INT_RX_DONE;
    // Turn off the whole interrutp since it is the only one
    InterruptEnable(ENET_IRQn);
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_EMAC G_LPC17xx_40xx_EMAC_Interface = {
    {
        "LPC17xx_40xx EMAC",
        0x0100,
        LPC17xx_40xx_EMAC_InitializeWorkspace,
        sizeof(T_LPC17xx_40xx_EMAC_Workspace), },

    LPC17xx_40xx_EMAC_Configure,
    LPC17xx_40xx_EMAC_CheckFrameReceived,
    LPC17xx_40xx_EMAC_StartReadFrame,
    LPC17xx_40xx_EMAC_EndReadFrame,
    LPC17xx_40xx_EMAC_RequestSend,
    LPC17xx_40xx_EMAC_DoSend,
    LPC17xx_40xx_EMAC_CopyToFrame,
    LPC17xx_40xx_EMAC_CopyFromFrame,

    // uEZ v1.10
    LPC17xx_40xx_EMAC_EnableReceiveInterrupt,
    LPC17xx_40xx_EMAC_DisableReceiveInterrupt,

};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_EMAC_RMII_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Creates the EMAC HAL driver for the LPC17xx_40xx device.  Also looks
 *      to see if it exists.  If found, sets the value in *aIsFound to ETrue.
 *      If NOT found, the value in *aIsFound IS NOT CHANGED!  Set *aIsFound
 *      to EFalse before calling this routine.
 * Inputs:
 *      T_LPC17xx_40xx_EMAC_Settings *aSettings -- Settings to configure EMAC as
 *      TBool *aIsFound -- REturned flag if found
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_EMAC_RMII_Require(const T_LPC17xx_40xx_EMAC_Settings *aSettings)
{
    T_LPC17xx_40xx_EMAC_Workspace *p;

    static const T_LPC17xx_40xx_IOCON_ConfigList pinsTX_EN[] = { {
        GPIO_P1_4,
        IOCON_D_DEFAULT(1) }, // ENET_TX_EN
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsTXD3[] = { {
        GPIO_P1_3,
        IOCON_D_DEFAULT(1) }, // ENET_TXD[3]
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsTXD2[] = { {
        GPIO_P1_2,
        IOCON_D_DEFAULT(1) }, // ENET_TXD[2]
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsTXD1[] = { {
        GPIO_P1_1,
        IOCON_D_DEFAULT(1) }, // ENET_TXD[1]
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsTXD0[] = { {
        GPIO_P1_0,
        IOCON_D_DEFAULT(1) }, // ENET_TXD[0]
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsTX_ER[] = { {
        GPIO_P1_5,
        IOCON_D_DEFAULT(1) }, // ENET_TX_ER
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsTX_CLK[] = { {
        GPIO_P1_6,
        IOCON_D_DEFAULT(1) }, // ENET_TX_CLK
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsRX_DV[] = { {
        GPIO_P1_13,
        IOCON_D_DEFAULT(1) }, // ENET_RX_DV
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsRXD3[] = { {
        GPIO_P1_12,
        IOCON_D_DEFAULT(1) }, // ENET_RXD[3]
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsRXD2[] = { {
        GPIO_P1_11,
        IOCON_D_DEFAULT(1) }, // ENET_RXD[2]
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsRXD1[] = { {
        GPIO_P1_10,
        IOCON_D_DEFAULT(1) }, // ENET_RXD[1]
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsRXD0[] = { {
        GPIO_P1_9,
        IOCON_D_DEFAULT(1) }, // ENET_RXD[0]
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsRX_ER[] = { {
        GPIO_P1_14,
#if (UEZ_PROCESSOR == NXP_LPC1788)
        IOCON_D_DEFAULT(1) }, // ENET_RX_ER
#elif (UEZ_PROCESSOR == NXP_LPC4088)
        IOCON_W_DEFAULT(1) }, // ENET_RX_ER
#endif
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsRX_CLK[] = { {
        GPIO_P1_15,
        IOCON_D_DEFAULT(1) }, // ENET_RX_CLK
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsCOL[] = { {
        GPIO_P1_7,
        IOCON_D_DEFAULT(1) }, // ENET_COL
        };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsCRS[] = { {
        GPIO_P1_8,
        IOCON_D_DEFAULT(1) }, // ENET_CRS
        };
    // Use the MDC and MDIO pins as EMAC pins
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsMDC[] = {
        {   GPIO_P1_16, 
#if (UEZ_PROCESSOR == NXP_LPC1788)
        IOCON_D_DEFAULT(1)}, // ENET_MDC
#elif (UEZ_PROCESSOR == NXP_LPC4088)
		IOCON_W_DEFAULT(1)}, // ENET_MDC
#endif		
        {   GPIO_P2_8, IOCON_D_DEFAULT(4)}, // ENET_MDC
    };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsMDIO[] = {
        {   GPIO_P1_17, 
#if (UEZ_PROCESSOR == NXP_LPC1788)
        IOCON_D_DEFAULT(1)}, // ENET_MDIO
#elif (UEZ_PROCESSOR == NXP_LPC4088)
		IOCON_W_DEFAULT(1)}, // ENET_MDIO
#endif
        {   GPIO_P2_9, IOCON_D_DEFAULT(4)}, // ENET_MDIO
    };
    HAL_DEVICE_REQUIRE_ONCE();

    // Register EMAC
    HALInterfaceRegister("EMAC", (T_halInterface *)&G_LPC17xx_40xx_EMAC_Interface, 0,
        (T_halWorkspace **)&p);
    p->iPinSettings = *aSettings;
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iTX_EN, pinsTX_EN,
        ARRAY_COUNT(pinsTX_EN));
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aSettings->iTXD3, pinsTXD3,
        ARRAY_COUNT(pinsTXD3));
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aSettings->iTXD2, pinsTXD2,
        ARRAY_COUNT(pinsTXD2));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iTXD1, pinsTXD1, ARRAY_COUNT(pinsTXD1));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iTXD0, pinsTXD0, ARRAY_COUNT(pinsTXD0));
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aSettings->iTX_ER, pinsTX_ER,
        ARRAY_COUNT(pinsTX_ER));
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aSettings->iTX_CLK, pinsTX_CLK,
        ARRAY_COUNT(pinsTX_CLK));
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aSettings->iRX_DV, pinsRX_DV,
        ARRAY_COUNT(pinsRX_DV));
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aSettings->iRXD3, pinsRXD3,
        ARRAY_COUNT(pinsRXD3));
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aSettings->iRXD2, pinsRXD2,
        ARRAY_COUNT(pinsRXD2));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iRXD1, pinsRXD1, ARRAY_COUNT(pinsRXD1));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iRXD0, pinsRXD0, ARRAY_COUNT(pinsRXD0));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iRX_ER, pinsRX_ER,
        ARRAY_COUNT(pinsRX_ER));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iRX_CLK, pinsRX_CLK,
        ARRAY_COUNT(pinsRX_CLK));
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aSettings->iCOL, pinsCOL,
        ARRAY_COUNT(pinsCOL));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iCRS, pinsCRS, ARRAY_COUNT(pinsCRS));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iMDC, pinsMDC, ARRAY_COUNT(pinsMDC));
    LPC17xx_40xx_IOCON_ConfigPin(aSettings->iMDIO, pinsMDIO, ARRAY_COUNT(pinsMDIO));

#if EMAC_USE_BITBANG_MDIO_DETECT
    p->iMDIOSettings.iDelayUSPerHalfBit = EMAC_BITBANG_MDIO_DELAY_US;
    p->iMDIOSettings.iIsClause45 = EFalse;
    p->iMDIOSettings.iPinMDC = aSettings->iMDC;
    p->iMDIOSettings.iPinMDIO = aSettings->iMDIO;
    if (LPC17xx_40xx_EMAC_DetectEMAC(p) == UEZ_ERROR_NONE)
        p->iIsDetected = ETrue;
#else
    p->iIsDetected = ETrue;
#endif
}

/*-------------------------------------------------------------------------*
 * End of File:  EMAC.c
 *-------------------------------------------------------------------------*/
