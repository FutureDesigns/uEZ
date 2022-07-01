/*-------------------------------------------------------------------------*
 * File:  LPC1768_EMAC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1768 EMAC (Ethernet) Driver.
 * Implementation:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
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
#include <Source/Processor/NXP/LPC1768/LPC1768_EMAC.h>
#include <uEZBSP.h>

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef DEBUG_LPC1768_EMAC_STARTUP
#define DEBUG_LPC1768_EMAC_STARTUP  0
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
/* EMAC Memory Buffer configuration for 16K Ethernet RAM. */
#define NUM_RX_FRAG         3           /* Num.of RX Fragments 3*1536= 4.5kB */
#define NUM_TX_FRAG         3           /* Num.of TX Fragments 3*1536= 4.5kB */
#define ETH_FRAG_SIZE       1536        /* Packet Fragment size 1536 Bytes   */

#define ETH_MAX_FLEN        1536        /* Max. Ethernet Frame Size          */
// This sets up the EMAC to be about 9.4k leaving room for USB area
// or about 10 * 0x0400 leaving 0x06 * 0x0400 for USB.

// Memory map:
// --------------
// 0x2007C000 - 0x2007E7FF: Ethernet driver buffers
// 0x2007E800 - 0x2007FFFF: USB
// 0x20080000 - 0x20083FFF: lwIP PBUFs

/* EMAC variables located in 16K Ethernet SRAM */
#define RX_DESC_BASE        0x2007C000
#define RX_STAT_BASE        (RX_DESC_BASE + NUM_RX_FRAG*8)
#define TX_DESC_BASE        (RX_STAT_BASE + NUM_RX_FRAG*8)
#define TX_STAT_BASE        (TX_DESC_BASE + NUM_TX_FRAG*8)
#define RX_BUF_BASE         (TX_STAT_BASE + NUM_TX_FRAG*4)
#define TX_BUF_BASE         (RX_BUF_BASE  + NUM_RX_FRAG*ETH_FRAG_SIZE)

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
} T_LPC1768_EMAC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_EMAC EMAC_LPC1768_Interface;
static T_LPC1768_EMAC_Workspace *G_LPC1768_EMAC;

/*---------------------------------------------------------------------------*
 * Routine:  IPHYWrite
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the register in the given PHY to the given value.
 * Inputs:
 *      T_LPC1768_EMAC_Workspace *p -- EMAC's Workspace
 *      TUInt32 aPhyReg             -- PHY register to write
 *      TUInt32 aValue              -- Value to write to register
 *---------------------------------------------------------------------------*/
static void IPHYWrite(T_LPC1768_EMAC_Workspace *p, TUInt8 PhyReg, TUInt32 Value)
{
    TUInt32 timeout;

    // Write the PHY's register with the value
    EMAC->MADR = p->iSettings.iPHYAddr | PhyReg;
    EMAC->MWTD = Value;

    // Wait utill operation completed
    for (timeout=0; timeout < MII_WR_TOUT; timeout++) {
        if ((EMAC->MIND & MIND_BUSY) == 0) {
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
 *      T_LPC1768_EMAC_Workspace *p -- EMAC's Workspace
 *      TUInt32 aPhyReg             -- PHY register to write
 *      TUInt32 aValue              -- Value to write to register
 *---------------------------------------------------------------------------*/
static TUInt16 IPHYRead(T_LPC1768_EMAC_Workspace *p, TUInt8 PhyReg)
{
    TUInt32 timeout;

    // Read the PHY's register
    EMAC->MADR = p->iSettings.iPHYAddr | PhyReg;
    EMAC->MCMD = MCMD_READ;

    // Wait until operation completed
    for (timeout=0; timeout < MII_RD_TOUT; timeout++) {
        if ((EMAC->MIND & MIND_BUSY) == 0) {
            break;
        }
    }
    EMAC->MCMD = 0;

    return (EMAC->MRDD);
}

/*---------------------------------------------------------------------------*
 * Routine:  IEMACConfigPHY_National
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for the National PHY
 * Inputs:
 *      T_LPC1768_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_National(T_LPC1768_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 timeout, regv;

    // Force ON Auto-MDIX
    IPHYWrite(p, PHY_REG_PHYCR, IPHYRead(p, PHY_REG_PHYCR)|(1<<15));

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
    if (error == UEZ_ERROR_NONE)  {
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
            EMAC->MAC2    |= MAC2_FULL_DUP;
            EMAC->Command |= CR_FULL_DUP;
            EMAC->IPGT    = IPGT_FULL_DUP;
        } else {
            // Half duplex mode.
            EMAC->IPGT = IPGT_HALF_DUP;
        }

        // Configure 100MBit/10MBit mode.
        if (regv & 0x0002) {
            // 10MBit mode.
            EMAC->SUPP = 0;
        } else {
            // 100MBit mode.
            EMAC->SUPP = SUPP_SPEED;
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
 *      T_LPC1768_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_Micrel(T_LPC1768_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 timeout, regv;
#if DEBUG_LPC1768_EMAC_STARTUP
    TUInt32 reg;
#endif
    // auto negotiation, restart AN
    // RE_AN should be self cleared
    IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);

    // Wait to complete Auto_Negotiation.
    for (timeout = 0; timeout < 30; timeout++) {
#if DEBUG_LPC1768_EMAC_STARTUP
    printf(".");
#endif
        UEZBSPDelayMS(100);
        regv = IPHYRead(p, PHY_REG_BMSR);
        if (regv & (BMSR_AUTO_DONE|BMSR_LINK_ESTABLISHED)) {
            // Autonegotiation Complete.
            break;
        }
    }

    regv = IPHYRead(p, MIC_PHY_100BASE_PHY_CTRL);
#if DEBUG_LPC1768_EMAC_STARTUP
    printf("nego regv %04X\n", regv);
#endif
    // successful negotiations; update link info
    regv &= 0x001C;
    switch (regv)  {
        case 0x0004:
            // Half duplex mode.
            // 10MBit mode.
            EMAC->SUPP    &= ~SUPP_SPEED;
            EMAC->MAC2    &= ~MAC2_FULL_DUP;
            EMAC->Command &= ~CR_FULL_DUP;
            EMAC->IPGT     = IPGT_HALF_DUP;
            break;
        case 0x0008:
            // Half duplex mode.
            // 100MBit mode.
            EMAC->SUPP    |= SUPP_SPEED;
            EMAC->MAC2    &= ~MAC2_FULL_DUP;
            EMAC->Command &= ~CR_FULL_DUP;
            EMAC->IPGT     = IPGT_HALF_DUP;
            break;
        case 0x0014:
            // Full duplex is enabled.
            // 10MBit mode.
            EMAC->SUPP    &= ~SUPP_SPEED;
            EMAC->MAC2    |= MAC2_FULL_DUP;
            EMAC->Command |= CR_FULL_DUP;
            EMAC->IPGT     = IPGT_FULL_DUP;
            break;
        case 0x0018:
            // Full duplex is enabled.
            // 100MBit mode.
            EMAC->SUPP    |= SUPP_SPEED;
            EMAC->MAC2    |= MAC2_FULL_DUP;
            EMAC->Command |= CR_FULL_DUP;
            EMAC->IPGT     = IPGT_FULL_DUP;
            break;
        default:	// Should not come here, force to set default, 100 FULL_DUPLEX
            // Full duplex is enabled.
            // 100MBit mode.
            EMAC->SUPP    |= SUPP_SPEED;
            EMAC->MAC2    |= MAC2_FULL_DUP;
            EMAC->Command |= CR_FULL_DUP;
            EMAC->IPGT     = IPGT_FULL_DUP;
            break;
    }

#if DEBUG_LPC1768_EMAC_STARTUP
    for (reg=0; reg<32; reg++) {
        regv = IPHYRead(p, reg);
        printf("phyreg %d = %04X\n", reg, regv);
    }
#endif

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IEMACConfigPHY_MicrelKSZ8001L
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for the Micrel PHY KSZ8001L
 * Inputs:
 *      T_LPC1768_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_MicrelKSZ8001L(T_LPC1768_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 timeout, regv;
    #define MICREL_KSZ8001L_TIMEOUT         10000 // 10 seconds
#if DEBUG_LPC1768_EMAC_STARTUP
    TUInt32 reg;
#endif
    // auto negotiation, restart AN
    // RE_AN should be self cleared
    IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);

    // Wait to complete Auto_Negotiation.
    for (timeout = 0; timeout < MICREL_KSZ8001L_TIMEOUT; timeout+=200) {
#if DEBUG_LPC1768_EMAC_STARTUP
    printf(".");
#endif
        UEZTaskDelay(200);
        regv = IPHYRead(p, PHY_REG_BMSR);
        if (regv & (BMSR_AUTO_DONE|BMSR_LINK_ESTABLISHED)) {
            // Autonegotiation Complete.
            break;
        }
    }
    if (timeout >= MICREL_KSZ8001L_TIMEOUT) {
        error = UEZ_ERROR_NOT_READY;
        return error;
    }

    regv = IPHYRead(p, MIC_PHY_100BASE_PHY_CTRL);
#if DEBUG_LPC1768_EMAC_STARTUP
    printf("nego regv %04X\n", regv);
#endif
    // successful negotiations; update link info
    regv &= 0x001C;
    switch (regv)  {
        case 0x0004:
            // Half duplex mode.
            // 10MBit mode.
            EMAC->SUPP    &= ~SUPP_SPEED;
            EMAC->MAC2    &= ~MAC2_FULL_DUP;
            EMAC->Command &= ~CR_FULL_DUP;
            EMAC->IPGT     = IPGT_HALF_DUP;
            break;
        case 0x0008:
            // Half duplex mode.
            // 100MBit mode.
            EMAC->SUPP    |= SUPP_SPEED;
            EMAC->MAC2    &= ~MAC2_FULL_DUP;
            EMAC->Command &= ~CR_FULL_DUP;
            EMAC->IPGT     = IPGT_HALF_DUP;
            break;
        case 0x0014:
            // Full duplex is enabled.
            // 10MBit mode.
            EMAC->SUPP    &= ~SUPP_SPEED;
            EMAC->MAC2    |= MAC2_FULL_DUP;
            EMAC->Command |= CR_FULL_DUP;
            EMAC->IPGT     = IPGT_FULL_DUP;
            break;
        case 0x0018:
            // Full duplex is enabled.
            // 100MBit mode.
            EMAC->SUPP    |= SUPP_SPEED;
            EMAC->MAC2    |= MAC2_FULL_DUP;
            EMAC->Command |= CR_FULL_DUP;
            EMAC->IPGT     = IPGT_FULL_DUP;
            break;
        default:	// Should not come here, force to set default, 100 FULL_DUPLEX
            // Full duplex is enabled.
            // 100MBit mode.
            EMAC->SUPP    |= SUPP_SPEED;
            EMAC->MAC2    |= MAC2_FULL_DUP;
            EMAC->Command |= CR_FULL_DUP;
            EMAC->IPGT     = IPGT_FULL_DUP;
            break;
    }

#if DEBUG_LPC1768_EMAC_STARTUP
    for (reg=0; reg<32; reg++) {
        regv = IPHYRead(p, reg);
        printf("phyreg %d = %04X\n", reg, regv);
    }
#endif

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1768_EMAC_InitRxDescriptors
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize Receive Descriptors
 *---------------------------------------------------------------------------*/
static void ILPC1768_EMAC_InitRxDescriptors(void)
{
    TUInt32 i;

    for (i = 0; i < NUM_RX_FRAG; i++) {
        RX_DESC_PACKET(i)  = RX_BUF(i);
        RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
        RX_STAT_INFO(i)    = 0;
        RX_STAT_HASHCRC(i) = 0;
    }

    // Set EMAC Receive Descriptor Registers.
    EMAC->RxDescriptor    = RX_DESC_BASE;
    EMAC->RxStatus        = RX_STAT_BASE;
    EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

    /* Rx Descriptors Point to 0 */
    EMAC->RxConsumeIndex  = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1768_EMAC_InitTxDescriptors
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize Transmit Descriptors
 *---------------------------------------------------------------------------*/
void ILPC1768_EMAC_InitTxDescriptors(void)
{
    TUInt32 i;

    for (i = 0; i < NUM_TX_FRAG; i++) {
        TX_DESC_PACKET(i) = TX_BUF(i);
        TX_DESC_CTRL(i)   = 0;
        TX_STAT_INFO(i)   = 0;
    }

    /* Set EMAC Transmit Descriptor Registers. */
    EMAC->TxDescriptor    = TX_DESC_BASE;
    EMAC->TxStatus        = TX_STAT_BASE;
    EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

    /* Tx Descriptors Point to 0 */
    EMAC->TxProduceIndex  = 0;
}
/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this EMAC Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_EMAC_InitializeWorkspace(void *aWorkspace)
{
//    T_LPC1768_EMAC_Workspace *p = (T_LPC1768_EMAC_Workspace *)aWorkspace;

    G_LPC1768_EMAC = aWorkspace;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure and setup the EMAC screen with the given settings
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *      T_EMACSettings *aSettings -- Pointer to settings to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_EMAC_Configure(
            void *aWorkspace,
            T_EMACSettings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_LPC1768_EMAC_Workspace *p = (T_LPC1768_EMAC_Workspace *)aWorkspace;
    TUInt32 regv, timeout;
    TUInt32 retry;
#if DEBUG_LPC1768_EMAC_STARTUP
    TUInt32 addr=1;
    TUInt16 v;
#endif
    // Copy over the settings
    p->iSettings = *aSettings;

    // Shift the phy address now to make the calculations faster
    p->iSettings.iPHYAddr <<= 8;

    // Initializes the EMAC ethernet controller
    // Ensure EMAC is powered up
    SC->PCONP |= (1<<30);

    // Delay a small amount
    UEZBSPDelayMS(100);

    // Reset all EMAC internal modules.
    // TBD: Should we use rx and tx flow control since we might be running too slow
    EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX |
        MAC1_SIM_RES | MAC1_SOFT_RES | MAC1_RX_FLOWC | MAC1_TX_FLOWC;
    EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;

    // A short delay after reset.
    UEZBSPDelayMS(1);

    // Initialize MAC control registers.
    EMAC->MAC1 = MAC1_PASS_ALL;
    EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
    EMAC->MAXF = ETH_MAX_FLEN;
    EMAC->CLRT = CLRT_DEF;
    EMAC->IPGR = IPGR_DEF;

    // host clock divided by 28, no suppress preamble, no scan increment
    // TBD: Should this be calculated at run time?
    EMAC->MCFG = 0x801C;	
    UEZBSPDelayMS(10);

    // Apply a reset
    EMAC->MCFG = 0x0018;	
    EMAC->MCMD = 0;

    // RMII configuration
    EMAC->Command |= CR_RMII;

    // PHY support: [8]=0 ->10 Mbps mode, =1 -> 100 Mbps mode
    // RMII setting, at power-on, default set to 100.
    EMAC->SUPP = SUPP_SPEED;	

    UEZBSPDelayMS(100);

#if DEBUG_LPC1768_EMAC_STARTUP
    for (addr=0; addr<16; addr++) {
        p->iSettings.iPHYAddr = addr<<8;
        v = IPHYRead(p, PHY_REG_IDR1);
        printf("ADDR: %d, v=%04X\n", addr, v);
        if (v != 0xFFFF) {
            break;
        }
    }
    printf("Addr: %d, Settings: %08X\n", addr, p->iSettings.iPHYAddr);
#endif

    /* Put the Phy in reset mode */
    IPHYWrite(p, PHY_REG_BMCR, 0x8000);
    UEZBSPDelayMS(1000);

    // Wait for hardware reset to end.
    // We try 500 times with a 10 ms delay for a complete
    // time of 1 second.
    retry = 3;
    do {
        for (timeout = 0; timeout < 500; timeout++) {
            UEZBSPDelayMS(10);
            regv = IPHYRead(p, PHY_REG_BMCR);
            if (!(regv & 0x8000)) {
                // Reset complete
                break;
            }
        }
#if DEBUG_LPC1768_EMAC_STARTUP
        printf("regv=%04X\n", regv);
#endif
    } while ((timeout == 100) && (--retry));

    if (!retry) {
        // report a timeout on the initialization
        return UEZ_ERROR_TIMEOUT;
    }

    // Setup PAUSE control (PAUSE and ASM_DIR in ANAR register)
    regv = IPHYRead(p, PHY_REG_ANAR);
#if DEBUG_LPC1768_EMAC_STARTUP
    printf("pause regv=%04X\n", regv);
#endif
    IPHYWrite(p, PHY_REG_ANAR, regv|(1<<10)|(1<<11));

    switch ((IPHYRead(p, PHY_REG_IDR1)<<16)|IPHYRead(p, PHY_REG_IDR2))  {
        case 0x20005C90:    // National PHY
            error = IEMACConfigPHY_National(p);
            break;
        case 0x00221610:    // Micrel
        case 0x00221512:    // Micrel KSZ8041TL-FTL
#if DEBUG_LPC1768_EMAC_STARTUP
            printf("Micrel init\n");
#endif
            error = IEMACConfigPHY_Micrel(p);
            break;
        default:
            // auto negotiation, restart AN
            // RE_AN should be self cleared
            IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);
            return UEZ_ERROR_NOT_SUPPORTED;
        case 0x00221613:    // Micrel KSZ8001L
#if DEBUG_LPC1768_EMAC_STARTUP
            printf("Micrel KSZ8001L init\n");
#endif
            error = IEMACConfigPHY_MicrelKSZ8001L(p);
            break;
    }

    EMAC->SA0 = (p->iSettings.iMACAddress[5] << 8) | p->iSettings.iMACAddress[4];
    EMAC->SA1 = (p->iSettings.iMACAddress[3] << 8) | p->iSettings.iMACAddress[2];
    EMAC->SA2 = (p->iSettings.iMACAddress[1] << 8) | p->iSettings.iMACAddress[0];

    // Initialize Tx and Rx DMA Descriptors
    ILPC1768_EMAC_InitRxDescriptors();
    ILPC1768_EMAC_InitTxDescriptors();

    // Receive Broadcast and Perfect Match Packets
    //Ake changed, RFC_UCAST_EN seems to be incorrect.
    //RxFilterCtrl = RFC_UCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;
    EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

    // Create the semaphore used ot wake the uIP task.
//??    vSemaphoreCreateBinary( xEMACSemaphore );

    // Reset all interrupts
    EMAC->IntClear  = 0xFFFF;

    /* Enable receive and transmit mode of MAC Ethernet core */
    EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
    EMAC->MAC1     |= MAC1_REC_EN;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_CheckFrameReceived
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if a frame has been received.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 * Outputs:
 *      TBool                     -- ETrue if frame received, else EFalse.
 *---------------------------------------------------------------------------*/
TBool LPC1768_EMAC_CheckFrameReceived(void *aWorkspace)
{
    // more packets received ?
    if (EMAC->RxProduceIndex != EMAC->RxConsumeIndex)
        return ETrue;
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_StartReadFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Reads the length of the received ethernet frame.  Returns the
 *      frame length.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 * Outputs:
 *      TUInt16                   -- Size of frame
 *---------------------------------------------------------------------------*/
TUInt16 LPC1768_EMAC_StartReadFrame(void *aWorkspace)
{
    TUInt16 RxLen;
    TUInt32 idx;
    T_LPC1768_EMAC_Workspace *p = (T_LPC1768_EMAC_Workspace *)aWorkspace;

    idx = EMAC->RxConsumeIndex;
    RxLen = (RX_STAT_INFO(idx) & RINFO_SIZE) - 3;
    p->rptr = (TUInt16 *)RX_DESC_PACKET(idx);

    return RxLen;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_EndReadFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      End the last frame.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *---------------------------------------------------------------------------*/
void LPC1768_EMAC_EndReadFrame(void *aWorkspace)
{
    TUInt32 idx;

    // DMA free packet.
    idx = EMAC->RxConsumeIndex;
    if (++idx == NUM_RX_FRAG)
        idx = 0;
    EMAC->RxConsumeIndex = idx;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_RequestSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Requests space in EMAC memory for storing an outgoing frame
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *---------------------------------------------------------------------------*/
void LPC1768_EMAC_RequestSend(void *aWorkspace)
{
    T_LPC1768_EMAC_Workspace *p = (T_LPC1768_EMAC_Workspace *)aWorkspace;
    TUInt32 idx;

    idx  = EMAC->TxProduceIndex;
    p->tptr = (TUInt16 *)TX_DESC_PACKET(idx);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_DoSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Send the frame
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      TUInt16 aFrameSize          -- Size of frame
 *---------------------------------------------------------------------------*/
void LPC1768_EMAC_DoSend(void *aWorkspace, TUInt16 aFrameSize)
{
    TUInt32 idx;

    idx = EMAC->TxProduceIndex;
    TX_DESC_CTRL(idx) = (aFrameSize-1) | TCTRL_LAST; /*Corrected problem with the size, -1 was missing*/
    if (++idx == NUM_TX_FRAG)
        idx = 0;
    EMAC->TxProduceIndex = idx;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1768_WriteTxFrameData
 *---------------------------------------------------------------------------*
 * Description:
 *      Writes a word in little-endian byte order to TX_BUFFER
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      TUInt16 aData               -- Data to write
 *---------------------------------------------------------------------------*/
static void ILPC1768_WriteTxFrameData(T_LPC1768_EMAC_Workspace *p, TUInt16 aData)
{
    *(p->tptr++) = aData;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_CopyToFrame
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
void LPC1768_EMAC_CopyToFrame(
            void *aWorkspace,
            void *aSource,
            TUInt32 aSize)
{
    T_LPC1768_EMAC_Workspace *p = (T_LPC1768_EMAC_Workspace *)aWorkspace;
    TUInt16 *piSource;

    piSource = aSource;
    aSize = (aSize + 1) & 0xFFFE;    // round Size up to next even number
    while (aSize > 0) {
        ILPC1768_WriteTxFrameData(p, *piSource++);
        aSize -= 2;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1768_ReadRxFrameData
 *---------------------------------------------------------------------------*
 * Description:
 *      Reads a word in little-endian byte order from RX_BUFFER
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 * Outputs:
 *      TUInt16 aData               -- Data read
 *---------------------------------------------------------------------------*/
TUInt16 ILPC1768_ReadRxFrameData(T_LPC1768_EMAC_Workspace *p)
{
    return (*(p->rptr++));
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_CopyFromFrame
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
void LPC1768_EMAC_CopyFromFrame(
                void *aWorkspace,
                void *aDestination,
                TUInt32 aSize)
{
    T_LPC1768_EMAC_Workspace *p = (T_LPC1768_EMAC_Workspace *)aWorkspace;
    // Keil: Pointer added to correct expression
    TUInt16 *piDest;

    // Keil: Line added
    piDest = aDestination;
    while (aSize > 1) {
        *piDest++ = ILPC1768_ReadRxFrameData(p);
        aSize -= 2;
    }

    // check for leftover byte...
    if (aSize) {
        // the LAN-Controller will return 0 for the highbyte
        *(TUInt8 *)piDest = (TUInt8)ILPC1768_ReadRxFrameData(p);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      An interrupt on this EMAC has been received.  Catch it and
 *      possible pass it on to a callback routine.
 *---------------------------------------------------------------------------*/
static void LPC1768_EMAC_ProcessInterrupt(void)
{
    T_LPC1768_EMAC_Workspace *p = G_LPC1768_EMAC;
    TUInt32 status;

    // Clear the interrupt.
    status = EMAC->IntStatus;
    if (status & EMAC_INT_RXDONE) {
        if (p->iReceiveCallback) {
            p->iReceiveCallback(p->iReceiveCallbackWorkspace);
        }
    }
    // Clear any interrupts that triggered this code
    EMAC->IntClear = status;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Stub to catch a EMAC interrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(LPC1768_EMAC_ISR)
{
    IRQ_START();
    LPC1768_EMAC_ProcessInterrupt();
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_EnableReceiveInterrupt
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
void LPC1768_EMAC_EnableReceiveInterrupt(
        void *aWorkspace,
        T_uezEMACReceiveInterruptCallback aCallback,
        void *aCallbackWorkspace)
{
    T_LPC1768_EMAC_Workspace *p = (T_LPC1768_EMAC_Workspace *)aWorkspace;

    p->iReceiveCallback = aCallback;
    p->iReceiveCallbackWorkspace = aCallbackWorkspace;
    // Register this interrupt (if not already)
    InterruptRegister(ENET_IRQn, (TISRFPtr)LPC1768_EMAC_ISR,
            INTERRUPT_PRIORITY_HIGH, "EMAC");
    InterruptEnable(ENET_IRQn);
    // Turn on those types of interrupts
    EMAC->IntEnable = INT_RX_DONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_EMAC_DisableReceiveInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Disables receive interrupts from the EMAC.
 *---------------------------------------------------------------------------*/
void LPC1768_EMAC_DisableReceiveInterrupt(void *aWorkspace)
{
    // Turn off those types of interrupts
    EMAC->IntClear = INT_RX_DONE;
    // Turn off the whole interrutp since it is the only one
    InterruptEnable(ENET_IRQn);
}

/*---------------------------------------------------------------------------*
* HAL Interface table:
*---------------------------------------------------------------------------*/
const HAL_EMAC G_LPC1768_EMAC_Interface = {
    {
	"LPC1768 EMAC",
        0x0100,
        LPC1768_EMAC_InitializeWorkspace,
        sizeof(T_LPC1768_EMAC_Workspace),
    },

    LPC1768_EMAC_Configure,
    LPC1768_EMAC_CheckFrameReceived,
    LPC1768_EMAC_StartReadFrame,
    LPC1768_EMAC_EndReadFrame,
    LPC1768_EMAC_RequestSend,
    LPC1768_EMAC_DoSend,
    LPC1768_EMAC_CopyToFrame,
    LPC1768_EMAC_CopyFromFrame,

    // uEZ v2.02
    LPC1768_EMAC_EnableReceiveInterrupt,
    LPC1768_EMAC_DisableReceiveInterrupt,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1768_EMAC_RMII_Require(const T_LPC1768_EMAC_Settings *aSettings)
{
    static const T_LPC1768_IOCON_ConfigList pinsTX_EN[] = {
            {GPIO_P1_4,  IOCON_D_DEFAULT(1)}, // ENET_TX_EN
    };

    static const T_LPC1768_IOCON_ConfigList pinsTXD1[] = {
            {GPIO_P1_1,  IOCON_D_DEFAULT(1)}, // ENET_TXD[1]
    };
    static const T_LPC1768_IOCON_ConfigList pinsTXD0[] = {
            {GPIO_P1_0,  IOCON_D_DEFAULT(1)}, // ENET_TXD[0]
    };

    static const T_LPC1768_IOCON_ConfigList pinsRXD1[] = {
            {GPIO_P1_10, IOCON_D_DEFAULT(1)}, // ENET_RXD[1]
    };
    static const T_LPC1768_IOCON_ConfigList pinsRXD0[] = {
            {GPIO_P1_9,  IOCON_D_DEFAULT(1)}, // ENET_RXD[0]
    };
    static const T_LPC1768_IOCON_ConfigList pinsRX_ER[] = {
            {GPIO_P1_14, IOCON_D_DEFAULT(1)}, // ENET_RX_ER
    };
    static const T_LPC1768_IOCON_ConfigList pinsRX_CLK[] = {
            {GPIO_P1_15, IOCON_D_DEFAULT(1)}, // ENET_RX_CLK
    };

    static const T_LPC1768_IOCON_ConfigList pinsCRS[] = {
            {GPIO_P1_8,  IOCON_D_DEFAULT(1)}, // ENET_CRS
    };
    static const T_LPC1768_IOCON_ConfigList pinsMDC[] = {
            {GPIO_P1_16, IOCON_D_DEFAULT(1)}, // ENET_MDC
            {GPIO_P2_8,  IOCON_D_DEFAULT(4)}, // ENET_MDC
    };
    static const T_LPC1768_IOCON_ConfigList pinsMDIO[] = {
            {GPIO_P1_17, IOCON_D_DEFAULT(1)}, // ENET_MDIO
            {GPIO_P2_9,  IOCON_D_DEFAULT(4)}, // ENET_MDIO
    };
    HAL_DEVICE_REQUIRE_ONCE();

    // Register EMAC
    HALInterfaceRegister("EMAC", (T_halInterface *)&G_LPC1768_EMAC_Interface,
            0, 0);
    LPC1768_IOCON_ConfigPin(aSettings->iTX_EN, pinsTX_EN,
            ARRAY_COUNT(pinsTX_EN));
    LPC1768_IOCON_ConfigPin(aSettings->iTXD1, pinsTXD1, ARRAY_COUNT(pinsTXD1));
    LPC1768_IOCON_ConfigPin(aSettings->iTXD0, pinsTXD0, ARRAY_COUNT(pinsTXD0));
    LPC1768_IOCON_ConfigPin(aSettings->iRXD1, pinsRXD1, ARRAY_COUNT(pinsRXD1));
    LPC1768_IOCON_ConfigPin(aSettings->iRXD0, pinsRXD0, ARRAY_COUNT(pinsRXD0));
    LPC1768_IOCON_ConfigPin(aSettings->iRX_ER, pinsRX_ER,
            ARRAY_COUNT(pinsRX_ER));
    LPC1768_IOCON_ConfigPin(aSettings->iRX_CLK, pinsRX_CLK,
            ARRAY_COUNT(pinsRX_CLK));
    LPC1768_IOCON_ConfigPin(aSettings->iCRS, pinsCRS, ARRAY_COUNT(pinsCRS));
    LPC1768_IOCON_ConfigPin(aSettings->iMDC, pinsMDC, ARRAY_COUNT(pinsMDC));
    LPC1768_IOCON_ConfigPin(aSettings->iMDIO, pinsMDIO, ARRAY_COUNT(pinsMDIO));
}

/*-------------------------------------------------------------------------*
 * End of File:  EMAC.c
 *-------------------------------------------------------------------------*/
