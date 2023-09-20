/*----------------------------------------------------------------------------
 *      Ethernet Definitions
 *----------------------------------------------------------------------------
 *      Name:    EMAC.H
 *      Purpose: NXP EMAC hardware definitions
 *----------------------------------------------------------------------------
 *      Copyright (c) 2006 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#ifndef __EMAC_H
#define __EMAC_H

// For Ethernet Fragment size and MTU/Frame Size see LPCXXXX_EMAC.h to set per MCU.
//#define ETH_FRAG_SIZE       1536        /* Packet Fragment size 1536 Bytes   */
//#define ETH_MTU             1500        /* Max. Ethernet Frame Size          */

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

/* Below is the Micrel PHY definition used for IAR LPC23xx and 
Embedded Artists LPC24xx board. */
#define MIC_PHY_RXER_CNT			0x0015
#define MIC_PHY_INT_CTRL			0x001B
#define MIC_PHY_LINKMD_CTRL			0x001D
#define MIC_PHY_PHY_CTRL			0x001E
#define MIC_PHY_100BASE_PHY_CTRL	0x001F

//#define DP83848C_DEF_ADR    0x0100      /* Default PHY device address        */
#define DP83848C_DEF_ADR   	0     /* Default PHY device address        */
//#define DP83848C_ID         0x20005C90  /* PHY Identifier                    */

// prototypes
portBASE_TYPE           Init_EMAC(unsigned short type);
unsigned short ReadFrameBE_EMAC(void);
void           CopyToFrame_EMAC(void *Source, uint32_t Size);
void           CopyFromFrame_EMAC(void *Dest, unsigned short Size);
void           DummyReadFrame_EMAC(unsigned short Size);
unsigned short StartReadFrame(void);
void           EndReadFrame(void);
uint32_t   CheckFrameReceived(void);
void           RequestSend(void);
uint32_t   Rdy4Tx(void);
void           DoSend_EMAC(unsigned short FrameSize);
void vEMACWaitForInput( void );
uint32_t uiGetEMACRxData( unsigned char *ucBuffer );


#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

