/*-------------------------------------------------------------------------*
 * File:  RX62N_EMAC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX62N EMAC (Ethernet) Driver.
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
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/EMAC.h>
#include <uEZBSP.h>
#include <stdio.h>
#include "RX62N_UtilityFuncs.h"
#include "RX62N_EMAC.h"

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef DEBUG_RX62N_EMAC_STARTUP
#define DEBUG_RX62N_EMAC_STARTUP  0
#endif

#ifndef DEBUG_RX62N_EMAC_RX_DESC
#define DEBUG_RX62N_EMAC_RX_DESC  0
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
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

#define BASIC_MODE_CONTROL_REG      0
#define BASIC_MODE_STATUS_REG       1
#define PHY_IDENTIFIER1_REG         2
#define PHY_IDENTIFIER2_REG         3
#define AN_ADVERTISEMENT_REG        4
#define AN_LINK_PARTNER_ABILITY_REG 5
#define AN_EXPANSION_REG            6

/* Media Independent Interface */
#define  PHY_ST    1
#define  PHY_READ  2
#define  PHY_WRITE 1
//#define  PHY_ADDR  0x1F

#define  MDC_WAIT  2

/* PHY return definitions */
#define R_PHY_OK     0
#define R_PHY_ERROR -1

/* Auto-Negotiation Link Partner Status */
#define PHY_AN_LINK_PARTNER_100BASE 0x0180
#define PHY_AN_LINK_PARTNER_FULL    0x0140

/*
 *  Wait counter definitions of PHY-LSI initialization
 *  ICLK = 96MHz
 */
#define PHY_RESET_WAIT              0x00020000L
#define PHY_AUTO_NEGOTIATON_WAIT    0x00800000L

/* RX62N Descriptor and buffer configuration */
#define BUFSIZE   256                   /* Must be 32-bit aligned */
#define ENTRY     8                     /* Number of RX and TX buffers */

#define  ACT      0x80000000
#define  DL       0x40000000
#define  FP1      0x20000000
#define  FP0      0x10000000
#define  FE       0x08000000

#define  RFOVER   0x00000200
#define  RAD      0x00000100
#define  RMAF     0x00000080
#define  RRF      0x00000010
#define  RTLF     0x00000008
#define  RTSF     0x00000004
#define  PRE      0x00000002
#define  CERF     0x00000001

#define  TAD      0x00000100
#define  CND      0x00000008
#define  DLC      0x00000004
#define  CD       0x00000002
#define  TRO      0x00000001

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
struct Descriptor {
    __evenaccess TUInt32 status;
#if __LIT
    /* Little endian */
    __evenaccess TUInt16 size;
    __evenaccess TUInt16 bufsize;
#else
    /* Big endian */
    __evenaccess TUInt16 bufsize;
    __evenaccess TUInt16 size;

#endif
    TUInt8 *buf_p;
    struct Descriptor *next;
};

typedef struct Descriptor ethfifo;

struct enet_stats {
    TUInt32 rx_packets; /* total packets received    */
    TUInt32 tx_packets; /* total packets transmitted  */
    TUInt32 rx_errors; /* bad packets received      */
    TUInt32 tx_errors; /* packet transmit problems    */
    TUInt32 rx_dropped; /* no space in buffers      */
    TUInt32 tx_dropped; /* no space available      */
    TUInt32 multicast; /* multicast packets received  */
    TUInt32 collisions;

    /* detailed rx_errors: */
    TUInt32 rx_length_errors;
    TUInt32 rx_over_errors; /* receiver ring buffer overflow  */
    TUInt32 rx_crc_errors; /* recved pkt with crc error  */
    TUInt32 rx_frame_errors; /* recv'd frame alignment error  */
    TUInt32 rx_fifo_errors; /* recv'r fifo overrun      */
    TUInt32 rx_missed_errors; /* receiver missed packet    */

    /* detailed tx_errors */
    TUInt32 tx_aborted_errors;
    TUInt32 tx_carrier_errors;
    TUInt32 tx_fifo_errors;
    TUInt32 tx_heartbeat_errors;
    TUInt32 tx_window_errors;
};

struct ei_device {
    const TUInt8 *name;
    TUInt8 open;
    TUInt8 Tx_act;
    TUInt8 Rx_act;
    TUInt8 txing; /* Transmit Active */
    TUInt8 irqlock; /* EDMAC's interrupt disabled when '1'. */
    TUInt8 dmaing; /* EDMAC Active */
    ethfifo *rxcurrent; /* current receive discripter */
    ethfifo *txcurrent; /* current transmit discripter */
    TUInt8 save_irq; /* Original dev->irq value. */
    struct enet_stats stat;
    TUInt8 mac_addr[6];
};

typedef struct {
    const HAL_EMAC *iHAL;
    T_EMACSettings iSettings;
    TUInt16 *rptr;
    TUInt16 *tptr;
    struct ei_device le0;
    ethfifo *iCurrent;
    TUInt16 iFrameByteSize;
    TUInt16 iFrameByteIndex;
    TUInt16 iCurrentByteIndex;
    TUInt16 iCurrentByteSize;
} T_RX62N_EMAC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
/**
 * Pragma definitions for receive and transmit packet
 * descriptors
 */
#pragma section  _RX_DESC
static ethfifo rxdesc[ENTRY]; /* receive packet descriptor */
#pragma section  _TX_DESC
static ethfifo txdesc[ENTRY]; /* transmit packet descriptor */
#pragma section

// TODO: The BUFSIZE must match what?  lwIP's variable?

/**
 * Pragma definitions for receive and transmitbuffers managed by
 * the descriptors
 */
#pragma section  _RX_BUFF
static TUInt8 rxbuf[ENTRY][BUFSIZE]; /* receive data buffer */
#pragma section  _TX_BUFF
static TUInt8 txbuf[ENTRY][BUFSIZE]; /* transmit data buffer */
#pragma section

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/**
 * Internal functions
 */
#if DEBUG_RX62N_EMAC_RX_DESC
static void IOutputDescriptors(T_RX62N_EMAC_Workspace *p);
#endif

/******************************************************************************
 * Function Name: _phy_mii_write_1
 * Description  : Outputs 1 to the MII interface
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void _phy_mii_write_1(void)
{
    TUInt32 j;

    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000006;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000007;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000007;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000006;
    }
}

/******************************************************************************
 * Function Name: _phy_mii_write_0
 * Description  : Outputs 0 to the MII interface
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void _phy_mii_write_0(void)
{
    TUInt32 j;

    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000002;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000003;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000003;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000002;
    }
}

/******************************************************************************
 * Function Name: _phy_preamble
 * Description  : As preliminary preparation for access to the PHY module register,
 *                "1" is output via the MII management interface.
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void _phy_preamble(void)
{
    TUInt16 i;

    for (i = 32; i; i--)
        _phy_mii_write_1();
}

/******************************************************************************
 * Function Name: _phy_ta_10
 * Description  : Switches data bus so MII interface can drive data
 *              : for write operation
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void _phy_ta_10(void)
{
    _phy_mii_write_1();
    _phy_mii_write_0();
}

/******************************************************************************
 * Function Name: _phy_ta_z0
 * Description  : Performs bus release so that PHY can drive data
 *              : for read operation
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
static void _phy_ta_z0(void)
{
    TUInt32 j;

    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000000;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000001;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000001;
    }
    for (j = MDC_WAIT; j > 0; j--) {
        ETHERC.PIR.LONG = 0x00000000;
    }
}

/******************************************************************************
 * Function Name: _phy_reg_set
 * Description  : Sets a PHY device to read or write mode
 * Arguments    : reg_addr - address of the PHY register
 *              : option - mode
 * Return Value : none
 ******************************************************************************/
static void _phy_reg_set(TUInt16 phy_addr, TUInt16 reg_addr, TUInt32 option)
{
    TUInt32 i;
    TUInt16 data;

    // Setup a data register of 14 bits (we've already don the turn around
    // bits) in the upper 14 bits of this 16 bit field.
    data = 0;

    // Setup ST code
    data = (PHY_ST << 14);

    if (option == PHY_READ) {
        // Add in OP Code (RD=Read)
        data |= (PHY_READ << 12);
    } else {
        // Add in OP Code (WT=Write)
        data |= (PHY_WRITE << 12);
    }

    // Put in the PHY address
    data |= (phy_addr << 7);

    // And the Register address
    data |= (reg_addr << 2);

    // Now send out the 14-bits of data
    i = 14;
    while (i) {
        if ((data & 0x8000) == 0) {
            _phy_mii_write_0();
        } else {
            _phy_mii_write_1();
        }
        data <<= 1;
        i--;
    }
}

/******************************************************************************
 * Function Name: _phy_reg_read
 * Description  : Reads PHY register through MII interface
 * Arguments    : data - pointer to store the data read
 * Return Value : none
 ******************************************************************************/
static void _phy_reg_read(TUInt16 *data)
{
    TUInt32 i, j;
    TUInt16 reg_data;

    reg_data = 0;
    i = 16;
    while (i > 0) {
        for (j = MDC_WAIT; j > 0; j--) {
            ETHERC.PIR.LONG = 0x00000000;
        }
        for (j = MDC_WAIT; j > 0; j--) {
            ETHERC.PIR.LONG = 0x00000001;
        }

        reg_data <<= 1;
        // Store MDI bit
        reg_data |= (TUInt16)((ETHERC.PIR.LONG & 0x00000008) >> 3);

        for (j = MDC_WAIT; j > 0; j--) {
            ETHERC.PIR.LONG = 0x00000001;
        }
        for (j = MDC_WAIT; j > 0; j--) {
            ETHERC.PIR.LONG = 0x00000000;
        }
        i--;
    }
    *data = reg_data;
}

/******************************************************************************
 * Function Name: _phy_reg_write
 * Description  : Writes to PHY register through RMII/MII interface
 * Arguments    : data - value to write
 * Return Value : none
 ******************************************************************************/
void _phy_reg_write(TUInt16 data)
{
    TUInt32 i;

    i = 16;
    while (i > 0) {
        if ((data & 0x8000) == 0) {
            _phy_mii_write_0();
        } else {
            _phy_mii_write_1();
        }
        i--;
        data <<= 1;
    }
}

/******************************************************************************
 * Function Name: _phy_write
 * Description  : Writes to a PHY register
 * Arguments    : reg_addr - address of the PHY register
 *              : data - value
 * Return Value : none
 ******************************************************************************/
void _phy_write(TUInt16 phy_addr, TUInt16 reg_addr, TUInt16 data)
{
    _phy_preamble();
    _phy_reg_set(phy_addr, reg_addr, PHY_WRITE);
    _phy_ta_10();
    _phy_reg_write(data);
    _phy_ta_z0();
}

/******************************************************************************
 * Function Name: _phy_read
 * Description  : Reads a PHY register
 * Arguments    : reg_addr - address of the PHY register
 * Return Value : read value
 ******************************************************************************/
TUInt16 _phy_read(TUInt16 phy_addr, TUInt16 reg_addr)
{
    TUInt16 data;

    _phy_preamble();
    _phy_reg_set(phy_addr, reg_addr, PHY_READ);
    _phy_ta_z0();
 //   _phy_ta_z0();  (read off bits)
    _phy_reg_read(&data);
    _phy_ta_z0();

    return (data);
}

#if 0
/******************************************************************************
 * Function Name: phy_set_100full
 * Description  : Set Ethernet PHY device to 100 Mbps full duplex
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
void phy_set_100full(void)
{
    _phy_write(BASIC_MODE_CONTROL_REG, 0x2100);
}

/******************************************************************************
 * Function Name: phy_set_10half
 * Description  : Sets Ethernet PHY device to 10 Mbps half duplexR
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
void phy_set_10half(void)
{
    _phy_write(BASIC_MODE_CONTROL_REG, 0x0000);
}

/******************************************************************************
 * Function Name: phy_set_autonegotiate
 * Description  : Starts autonegotiate and reports the other side's
 *              : physical capability
 * Arguments    : none
 * Return Value : bit 8 - Full duplex 100 mbps
 *              : bit 7 - Half duplex 100 mbps
 *              : bit 6 - Full duplex 10 mbps
 *              : bit 5 - Half duplex 10 mbps
 *              : bit 4:0 - Always set to 00001 (IEEE 802.3)
 *              : -1 if error
 ******************************************************************************/
int16_t phy_set_autonegotiate(void)
{
    TUInt16 reg;
    TUInt32 count;

    _phy_write(AN_ADVERTISEMENT_REG, 0x01E1);
    _phy_write(BASIC_MODE_CONTROL_REG, 0x1200);

    count = 0;
    do {
        reg = _phy_read(BASIC_MODE_STATUS_REG);
        count++;
    }while (!(reg & 0x0020) && count < PHY_AUTO_NEGOTIATON_WAIT);

    if (count >= PHY_AUTO_NEGOTIATON_WAIT) {
        return R_PHY_ERROR;
    } else {
        return ((int16_t)_phy_read(AN_LINK_PARTNER_ABILITY_REG));
    }
}
#endif

#if 0
/******************************************************************************
 * Function Name: phy_init
 * Description  : Resets Ethernet PHY device
 * Arguments    : none
 * Return Value : none
 ******************************************************************************/
int16_t phy_init(void)
{
    TUInt16 reg;
    TUInt32 count;

    /* Reset PHY */
    _phy_write(BASIC_MODE_CONTROL_REG, 0x8000);
    count = 0;
    do {
        reg = _phy_read(BASIC_MODE_CONTROL_REG);
        count++;
    }while ((reg & 0x8000) && (count < PHY_RESET_WAIT));

    if (count >= PHY_RESET_WAIT) {
        return R_PHY_ERROR;
    } else {
        return R_PHY_OK;
    }
}
#endif

/******************************************************************************
 * Function Name: _eth_fifoInit
 * Description  : Initialize E-DMAC descriptors
 * Arguments    : p - pointer to descriptor
 *              : status - initial status of descriptor
 * Return Value : none
 ******************************************************************************/
void _eth_fifoInit(ethfifo p[], TUInt32 status)
{
    ethfifo *current = 0;
    TUInt32 i, j;

    for (i = 0; i < ENTRY; i++) {
        current = &p[i];

        if (status == 0) {
            current->buf_p = &txbuf[i][0];
        } else {
            current->buf_p = &rxbuf[i][0];
        }

        /* Clear buffer */
        for (j = 0; j < BUFSIZE; j++) {
            current->buf_p[j] = 0;
        }
        current->bufsize = BUFSIZE;
        current->size = 0;
        current->status = status;
        current->next = &p[i + 1];
    }

    /* Set the last fifo entry to complete the loop */
    current->status |= DL;
    current->next = &p[0];
}

/******************************************************************************
 * Function Name: _eth_fifoWrite
 * Description  : Transmits data and updates E-DMAC descriptor
 * Arguments    : p - pointer to descriptor
 *              : buf - pointer to data to transmit
 *              : size - number of data bytes
 * Return Value : Number of bytes written to transmit buffer for this descriptor
 *              : -1 Descriptor busy
 ******************************************************************************/
TInt32 _eth_fifoWrite(ethfifo *p, TUInt8 buf[], TUInt32 size)
{
    TInt32 i;
    ethfifo *current = p;

    if ((current->status & ACT) != 0) {
        /**
         * Current descriptor is active and ready to transmit or transmitting.
         **/
        return (-1);
    }

    for (i = 0; i < size; i++) {
        if (i >= BUFSIZE) {
            break;
        } else {
            /* transfer packet data */
            current->buf_p[i] = buf[i];
        }
    }

    current->bufsize = (TUInt16)i;

    return (i);

}

#if 0
/******************************************************************************
 * Function Name: _eth_fifoRead
 * Description  : Receives data and updates E-DMAC descriptor
 * Arguments    : p - pointer to descriptor
 *              : buf - pointer to data to receive
 * Return Value : Number of bytes received for this descriptor
 *              : -1 Descriptor busy
 *              : -2 Frame error
 ******************************************************************************/
TUInt32 _eth_fifoRead(ethfifo *p, TUInt8 buf[])
{
    TUInt32 i, temp_size; /* byte counter */
    ethfifo *current = p;

    if ((current->status & ACT) != 0) {
        /**
         * Current descriptor is active and ready to receive or receiving.
         * This is not an error.
         **/
        return (-1);
    }

    /**
     * Current descriptor is not active.
     * Process the descriptor and data received.
     **/
    else if ((current->status & FE) != 0) {
        /**
         * Frame error.
         * Must move to new descriptor as E-DMAC now points to next one.
         **/
        return (-2);
    } else
    /* We have a new data w/o errors to process. */
    {
        if ((current->status & FP0) == FP0) {
            /* This is the last descriptor.  Complete frame is received.   */
            temp_size = current->size;

            while (temp_size > BUFSIZE) {
                temp_size -= BUFSIZE;
            }
        } else {
            /**
             * This is either a start or continuos descriptor.
             * Complete frame is NOT received.
             **/
            temp_size = BUFSIZE;
        }

        /* Copy received data from receive buffer to user buffer */
        for (i = 0; i < temp_size; i++) {
            buf[i] = current->buf_p[i];
        }

        /* Return data size received */
        return (temp_size);
    }
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  IPHYWrite
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the register in the given PHY to the given value.
 * Inputs:
 *      T_RX62N_EMAC_Workspace *p -- EMAC's Workspace
 *      TUInt32 aPhyReg             -- PHY register to write
 *      TUInt32 aValue              -- Value to write to register
 *---------------------------------------------------------------------------*/
static void IPHYWrite(T_RX62N_EMAC_Workspace *p, TUInt8 PhyReg, TUInt32 Value)
{
#if 0
    TUInt32 timeout;

    // Write the PHY's register with the value
    MADR = p->iSettings.iPHYAddr | PhyReg;
    MWTD = Value;

    // Wait utill operation completed
    for (timeout = 0; timeout < MII_WR_TOUT; timeout++) {
        if ((MIND & MIND_BUSY) == 0) {
            break;
        }
    }
#endif
    _phy_write(p->iSettings.iPHYAddr, PhyReg, Value);
}

/*---------------------------------------------------------------------------*
 * Routine:  IPHYRead
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the register from the given PHY.
 * Inputs:
 *      T_RX62N_EMAC_Workspace *p -- EMAC's Workspace
 *      TUInt32 aPhyReg             -- PHY register to write
 *      TUInt32 aValue              -- Value to write to register
 *---------------------------------------------------------------------------*/
static TUInt16 IPHYRead(T_RX62N_EMAC_Workspace *p, TUInt8 PhyReg)
{
    return _phy_read(p->iSettings.iPHYAddr, PhyReg);
}

/*---------------------------------------------------------------------------*
 * Routine:  IEMACConfigPHY_National
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for the National PHY
 * Inputs:
 *      T_RX62N_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_National(T_RX62N_EMAC_Workspace *p)
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
            ETHERC.ECMR.BIT.DM = 1;
        } else {
            // Half duplex mode.
            ETHERC.ECMR.BIT.DM = 0;
        }

        // RMII: Configure 100MBit/10MBit mode.
        if (regv & 0x0002) {
            // 10MBit mode.
            ETHERC.ECMR.BIT.RTM = 0;
        } else {
            // 100MBit mode.
            ETHERC.ECMR.BIT.RTM = 1;
        }

        /* Enable receive and transmit */
        ETHERC.ECMR.BIT.RE = 1;
        ETHERC.ECMR.BIT.TE = 1;

        /* Enable EDMAC receive */
        EDMAC.EDRRR.LONG = 0x00000001;
        UEZBSPDelayMS(100);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IEMACConfigPHY_NationalDSP83640
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for the National PHY
 * Inputs:
 *      T_RX62N_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_NationalDSP83640(T_RX62N_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 timeout, regv;

    // Force ON Auto-MDIX
    //IPHYWrite(p, PHY_REG_PHYCR, IPHYRead(p, PHY_REG_PHYCR) | (1 << 15));
    IPHYWrite(p, AN_ADVERTISEMENT_REG, 0x01E1);
    IPHYWrite(p, BASIC_MODE_CONTROL_REG, 0x1200);


    // auto negotiation, restart AN
    // RE_AN should be self cleared
    IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);

    // Wait to complete Auto_Negotiation.
    for (timeout = 0; timeout < 1000; timeout++) {
        UEZBSPDelayMS(1);
        regv = IPHYRead(p, PHY_REG_BMSR);
        if (regv & 0x0020) {
            // Autonegotiation Complete.
            break;
        }
    }

    /* National DP83640 fix */
    IPHYWrite(p, 0x13, 0x0006);
    IPHYWrite(p, 0x14, (IPHYRead(p, 0x14)&0x7FFF));
    IPHYWrite(p, 0x13, 0x0000);

    /* Get the link partner response */
    //reg = (int16_t)_phy_read(AN_LINK_PARTNER_ABILITY_REG);

    // Check the link status.
    if (error == UEZ_ERROR_NONE) {
        error = UEZ_ERROR_TIMEOUT;
        for (timeout = 0; timeout < 1000; timeout++) {
            UEZBSPDelayMS(10);
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
            ETHERC.ECMR.BIT.DM = 1;
        } else {
            // Half duplex mode.
            ETHERC.ECMR.BIT.DM = 0;
        }

        // RMII: Configure 100MBit/10MBit mode.
        if (regv & 0x0002) {
            // 10MBit mode.
            ETHERC.ECMR.BIT.RTM = 0;
        } else {
            // 100MBit mode.
            ETHERC.ECMR.BIT.RTM = 1;
        }

        /* Enable receive and transmit */
        ETHERC.ECMR.BIT.RE = 1;
        ETHERC.ECMR.BIT.TE = 1;

        /* Enable EDMAC receive */
        EDMAC.EDRRR.LONG = 0x00000001;
        UEZBSPDelayMS(100);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IEMACConfigPHY_Micrel
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for the Micrel PHY
 * Inputs:
 *      T_RX62N_EMAC_Workspace *p -- EMAC's Workspace
 *---------------------------------------------------------------------------*/
static T_uezError IEMACConfigPHY_Micrel(T_RX62N_EMAC_Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 timeout, regv;
#if DEBUG_RX62N_EMAC_STARTUP
    TUInt32 reg;
#endif
    // auto negotiation, restart AN
    // RE_AN should be self cleared
    IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);

    // Wait to complete Auto_Negotiation.
    for (timeout = 0; timeout < 30; timeout++) {
#if DEBUG_RX62N_EMAC_STARTUP
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
#if DEBUG_RX62N_EMAC_STARTUP
    printf("nego regv %04X\n", regv);
#endif
    // successful negotiations; update link info
    regv &= 0x001C;
    switch (regv) {
        case 0x0004:
            // Half duplex mode.
            ETHERC.ECMR.BIT.DM = 0;
            // 10MBit mode.
            ETHERC.ECMR.BIT.RTM = 0;
            break;
        case 0x0008:
            // Half duplex mode.
            ETHERC.ECMR.BIT.DM = 0;
            // 100MBit mode.
            ETHERC.ECMR.BIT.RTM = 1;
            break;
        case 0x0014:
            // Full duplex is enabled.
            ETHERC.ECMR.BIT.DM = 1;
            // 10MBit mode.
            ETHERC.ECMR.BIT.RTM = 0;
            break;
        case 0x0018:
            // Full duplex is enabled.
            ETHERC.ECMR.BIT.DM = 1;
            // 100MBit mode.
            ETHERC.ECMR.BIT.RTM = 1;
            break;
        default: // Should not come here, force to set default, 100 FULL_DUPLEX
            // Full duplex is enabled.
            ETHERC.ECMR.BIT.DM = 1;
            // 100MBit mode.
            ETHERC.ECMR.BIT.RTM = 1;
            break;
    }

#if DEBUG_RX62N_EMAC_STARTUP
    for (reg = 0; reg < 32; reg++) {
        regv = IPHYRead(p, reg);
        printf("phyreg %d = %04X\n", reg, regv);
    }
#endif

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this EMAC Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_EMAC_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_EMAC_Workspace *p = (T_RX62N_EMAC_Workspace *)aWorkspace;

    // Setup a brand new le0 ethdriver
    memset(&p->le0, 0, sizeof(p->le0));
    p->le0.name = "eth0";

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure and setup the EMAC screen with the given settings
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *      T_EMACSettings *aSettings -- Pointer to settings to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_EMAC_Configure(void *aWorkspace, T_EMACSettings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_RX62N_EMAC_Workspace *p = (T_RX62N_EMAC_Workspace *)aWorkspace;
    TUInt32 regv, timeout;
    TUInt32 retry;
    //TUInt32 addr = 1;
    TUInt32 uniqueID;
    TUInt32 i;
    TUInt32 mac_h, mac_l;
    // TUInt16 phydata;
	TUInt16 addr;
    TUInt16 v;

    // Turn on the EMAC
    SYSTEM.MSTPCRB.BIT.MSTPB15 = 0;

// TODO: Only supporting RMII Mode currently
    //#if ETH_MODE_SEL == ETH_RMII_MODE
    // Pin configuration (and enable Ethernet pins)
    /*  EE=1, PHYMODE=0, ENETE3=0, ENETE2=0, ENETE1=1, ENETE0=0 (Ethernet)  */
    IOPORT.PFENET.BYTE = 0x82;

    // Copy over the settings
    p->iSettings = *aSettings;

    /* Initialize driver */
    p->le0.open = 1;
    _eth_fifoInit(rxdesc, (TUInt32)ACT);
    _eth_fifoInit(txdesc, (TUInt32)0);
    p->le0.rxcurrent = &rxdesc[0];
    p->le0.txcurrent = &txdesc[0];
    p->le0.mac_addr[0] = p->iSettings.iMACAddress[0];
    p->le0.mac_addr[1] = p->iSettings.iMACAddress[1];
    p->le0.mac_addr[2] = p->iSettings.iMACAddress[2];
    p->le0.mac_addr[3] = p->iSettings.iMACAddress[3];
    p->le0.mac_addr[4] = p->iSettings.iMACAddress[4];
    p->le0.mac_addr[5] = p->iSettings.iMACAddress[5];

    /* Initialize EDMAC and ETHERC  */
    EDMAC.EDMR.BIT.SWR = 1;

    /* Reset EDMAC */
    for (i = 0; i < 0x00000100; i++) {
    }

    /* ETHERC */
    /* TODO:  Check bit 5 */
    ETHERC.ECSR.LONG = 0x00000037; /* clear all ETHERC status BFR, PSRTO, LCHNG, MPD, ICD */
    /* TODO:  Check bit 5 */
    ETHERC.ECSIPR.LONG = 0x00000020; /* disable ETHERC status change interrupt */
    ETHERC.RFLR.LONG = 1518; /* ether payload is 1500+ CRC              */
    ETHERC.IPGR.LONG = 0x00000014; /* Intergap is 96-bit time */

    mac_h = ((TUInt32)p->iSettings.iMACAddress[0] << 24)
            | ((TUInt32)p->iSettings.iMACAddress[1] << 16)
            | ((TUInt32)p->iSettings.iMACAddress[2] << 8)
            | (TUInt32)p->iSettings.iMACAddress[3];

    mac_l = ((TUInt32)p->iSettings.iMACAddress[4] << 8)
            | (TUInt32)p->iSettings.iMACAddress[5];

    if (mac_h == 0 && mac_l == 0) {
        /*
         * If 2nd parameter is 0, the MAC address should be acquired from the system,
         * depending on user implementation. (e.g.: EEPROM)
         */
    } else {
        ETHERC.MAHR = mac_h;
        ETHERC.MALR.LONG = mac_l;
    }

    /* EDMAC */
    EDMAC.EESR.LONG = 0x47FF0F9F; /* clear all ETHERC and EDMAC status bits */
    EDMAC.RDLAR = p->le0.rxcurrent; /* initialize Rx Descriptor List Address */
    EDMAC.TDLAR = p->le0.txcurrent; /* initialize Tx Descriptor List Address */
    EDMAC.TRSCER.LONG = 0x00000000; /* copy-back status is RFE & TFE only */
    EDMAC.TFTR.LONG = 0x00000000; /* threshold of Tx_FIFO */
    EDMAC.FDR.LONG = 0x00000707; /* transmit fifo & receive fifo is 2048 bytes */
    EDMAC.RMCR.LONG = 0x00000001; /* RR in EDRRR is under driver control */
#if __LIT
    EDMAC.EDMR.BIT.DE = 1;
#endif

	// Find the PHY address
    for (addr = 0; addr < 32; addr++) {
        p->iSettings.iPHYAddr = addr;
        v = IPHYRead(p, PHY_REG_IDR1);
#if DEBUG_RX62N_EMAC_STARTUP
        printf("ADDR: %d, v=%04X\n", addr, v);
#endif
        if (v != 0xFFFF) {
            break;
        }
    }
//p->iSettings.iPHYAddr = 1; //UEZ_EMAC_PHY_ADDR;
#if DEBUG_RX62N_EMAC_STARTUP
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
        for (timeout = 0; timeout < 1000; timeout++) {
            UEZBSPDelayMS(1);
            regv = IPHYRead(p, PHY_REG_BMCR);
            if (!(regv & 0x8000)) {
                // Reset complete
                break;
            }
        }
#if DEBUG_RX62N_EMAC_STARTUP
        printf("regv=%04X\n", regv);
#endif
    } while ((timeout == 1000) && (--retry));

    if (!retry) {
        // report a timeout on the initialization
        return UEZ_ERROR_TIMEOUT;
    }

    // Setup PAUSE control (PAUSE and ASM_DIR in ANAR register)
    regv = IPHYRead(p, PHY_REG_ANAR);
#if DEBUG_RX62N_EMAC_STARTUP
    printf("pause regv=%04X\n", regv);
#endif
    IPHYWrite(p, PHY_REG_ANAR, regv | (1 << 10) | (1 << 11));

    uniqueID = (IPHYRead(p, PHY_REG_IDR1) << 16) | IPHYRead(p, PHY_REG_IDR2);

    switch (uniqueID) {
        case 0x20005C90: // National PHY
            error = IEMACConfigPHY_National(p);
            break;            
        case 0x20005CE1:    // National DP83640
            error = IEMACConfigPHY_NationalDSP83640(p);
            break;
        case 0x00221610: // Micrel 
        case 0x00221512: // Micrel KSZ8041TL-FTL
        case 0x0007C0F1: // SMSC LAN8720
#if DEBUG_RX62N_EMAC_STARTUP
            printf("Micrel init\n");
#endif
            error = IEMACConfigPHY_Micrel(p);
            break;
        default:
            // auto negotiation, restart AN
            // RE_AN should be self cleared
            IPHYWrite(p, PHY_REG_BMCR, PHY_AUTO_NEG);
            return UEZ_ERROR_NOT_SUPPORTED;
    }

    /* Enable receive and transmit */
    ETHERC.ECMR.BIT.RE = 1;
    ETHERC.ECMR.BIT.TE = 1;

    /* Enable EDMAC receive */
    EDMAC.EDRRR.LONG = 0x00000001;
    for (i = 0; i < 0x00000100; i++) {
    }

    return error;
}

static void IFrameError(T_RX62N_EMAC_Workspace *p)
{
    // Point to next frame and clear this descriptor.
    p->le0.stat.rx_errors++;

    p->le0.rxcurrent->status &= ~(FP1 | FP0 | FE);
    p->le0.rxcurrent->status &= ~(RFOVER | RAD | RMAF | RRF | RTLF | RTSF | PRE
            | CERF);
    p->le0.rxcurrent->status |= ACT;
    p->le0.rxcurrent = p->le0.rxcurrent->next;

    if (EDMAC.EDRRR.LONG == 0x00000000L) {
        // Restart if stopped
        EDMAC.EDRRR.LONG = 0x00000001L;
    }
}

#if DEBUG_RX62N_EMAC_RX_DESC
static void IOutputDescriptors(T_RX62N_EMAC_Workspace *p)
{
    int i;
    ethfifo *p_entry;

    printf("\f======================\n");
    //ethfifo rxdesc[ENTRY];
    p_entry = rxdesc;
    for (i = 0; i < ENTRY; i++, p_entry++) {
        if (p_entry == p->le0.rxcurrent) {
            printf("--> ");
        } else {
            printf("    ");
        }
        printf("%08X: ", p_entry);
        if (p_entry->status & (1UL << 31))
            printf("RACT ");
        else
            printf("---- ");
        if (p_entry->status & (1 << 30))
            printf("RDLE ");
        else
            printf("---- ");
        if (p_entry->status & (1 << 29))
            printf("<ST> ");
        else
            printf("     ");
        if (p_entry->status & (1 << 28))
            printf("<ED> ");
        else
            printf("     ");
        if (p_entry->status & (1 << 27))
            printf("RFE (%03X) ", p_entry->status & 0x3FF);
        else
            printf("OK  (   ) ");

        printf("%4d/%4d\n", p_entry->size, p_entry->bufsize);
    }
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_CheckFrameReceived
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if a frame has been received.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 * Outputs:
 *      TBool                     -- ETrue if frame received, else EFalse.
 *---------------------------------------------------------------------------*/
TBool RX62N_EMAC_CheckFrameReceived(void *aWorkspace)
{
    T_RX62N_EMAC_Workspace *p = (T_RX62N_EMAC_Workspace *)aWorkspace;
    p->iCurrent = p->le0.rxcurrent;

    // more packets received ?
    // Is current descriptor active and ready to receive or receiving?
    // (in other words, not done)
    if ((p->iCurrent->status & ACT) != 0) {
        // Actively doing something, not ready
        return EFalse;
    }

    // The current descriptor is NOT active.
    // Did we have a frame error?
    if ((p->iCurrent->status & FE) != 0) {
        // Frame error
        IFrameError(p);
        return EFalse;
    }

#if DEBUG_RX62N_EMAC_RX_DESC
    IOutputDescriptors(p);
#endif

    // A descriptor is waiting to be processed
    return ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_StartReadFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Reads the length of the received ethernet frame.  Returns the
 *      frame length.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 * Outputs:
 *      TUInt16                   -- Size of frame
 *---------------------------------------------------------------------------*/
TUInt16 RX62N_EMAC_StartReadFrame(void *aWorkspace)
{
    T_RX62N_EMAC_Workspace *p = (T_RX62N_EMAC_Workspace *)aWorkspace;
    TUInt32 temp_size;
    ethfifo *p_current;
    TUInt8 timeout;

    p_current = p->iCurrent = p->le0.rxcurrent;
    p->iCurrentByteIndex = 0;

    // Find the last entry
    while ((p_current->status & FP0) != FP0) {
        p_current = p_current->next;
        // Is this block ready?  Timeout after 5 ms
        timeout = 5;
        while (((p_current->status & ACT) != 0) && (timeout--)) {
            // NOTE: This delay sucks!  We should remove this
            // with an interrupt driven way later.
            UEZBSPDelayMS(1);
        }
    }

    // Return the complete size (lwIP breaks into blocks)
    temp_size = p_current->size;
#if DEBUG_RX62N_EMAC_RX_DESC
    printf("\n\nRead Start: %d\n", temp_size);
#endif
    // Track the total frame size
    p->iFrameByteSize = temp_size;
    p->iFrameByteIndex = 0;

    // Is this the last block of the frame chunks?
    if ((p->iCurrent->status & FP0) == FP0) {
        // Yes, it is.  Use this size
        p->iCurrentByteSize = p->iCurrent->size;
    } else {
        // Otherwise, we'll be a complete block in size
        p->iCurrentByteSize = BUFSIZE;
    }

    // Return the length of the current frame
    return (TUInt16)temp_size;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_EndReadFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      End the last frame.
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *---------------------------------------------------------------------------*/
void RX62N_EMAC_EndReadFrame(void *aWorkspace)
{
#if DEBUG_RX62N_EMAC_RX_DESC
    IOutputDescriptors((T_RX62N_EMAC_Workspace *)aWorkspace);
#endif
    // Do nothing in this driver
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_RequestSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Requests space in EMAC memory for storing an outgoing frame
 * Inputs:
 *      void *aWorkspace          -- EMAC Controller's workspace
 *---------------------------------------------------------------------------*/
void RX62N_EMAC_RequestSend(void *aWorkspace)
{
    // Do nothing for now
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_DoSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Send the frame
 * Inputs:
 *      void *aWorkspace            -- EMAC Controller's workspace
 *      TUInt16 aFrameSize          -- Size of frame
 *---------------------------------------------------------------------------*/
void RX62N_EMAC_DoSend(void *aWorkspace, TUInt16 aFrameSize)
{
    // Do nothing for now (it really happens in CopyToFrame)
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_CopyToFrame
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
void RX62N_EMAC_CopyToFrame(void *aWorkspace, void *aSource, TUInt32 aSize)
{
    TUInt8 *p_data = (TUInt8 *)aSource;
    TUInt32 flag = FP1; // Start of frame
    TInt32 transmitted;
    T_RX62N_EMAC_Workspace *p = (T_RX62N_EMAC_Workspace *)aWorkspace;

#if DEBUG_RX62N_EMAC_RX_DESC
    printf("  CopyToFrame: %d\n", aSize);
#endif
    for (transmitted = 0; aSize > 0; aSize -= (TUInt32)transmitted) {
        while (1) {
            // Setup one block worth of data
            transmitted = _eth_fifoWrite(p->le0.txcurrent, p_data, aSize);
            if (transmitted >= 1)
                break;
        }

        // Is this the last of the data?  If so, mark it as last
        if (transmitted == aSize)
            flag |= FP0;

        // Clear previous setetings and make active
        p->le0.txcurrent->status &= ~(FP1 | FP0);
        p->le0.txcurrent->status |= (flag | ACT);

        // Setup next flag and next section moving to next
        // data to be sent
        flag = 0;
        p->le0.txcurrent = p->le0.txcurrent->next;
        p_data += transmitted;
    }

    // Sent another packet
    p->le0.stat.tx_packets++;

    if (EDMAC.EDTRR.LONG == 0x00000000)
        EDMAC.EDTRR.LONG = 0x00000001;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_EMAC_CopyFromFrame
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
void RX62N_EMAC_CopyFromFrame(
        void *aWorkspace,
        void *aDestination,
        TUInt32 aSize)
{
    T_RX62N_EMAC_Workspace *p = (T_RX62N_EMAC_Workspace *)aWorkspace;
    TUInt8 *p_dest = aDestination;
    TBool isCopying = ETrue;
    TUInt8 timeout;

#if DEBUG_RX62N_EMAC_RX_DESC
    IOutputDescriptors(p);
    printf("CopyFromFrame: size: %d\n", aSize);
    printf("  index: %d, size: %d\n", p->iCurrentByteIndex, p->iCurrentByteSize);
    printf("  findex: %d, fsize: %d\n", p->iFrameByteIndex, p->iFrameByteSize);
#endif
    while (isCopying) {
        // Is this block ready?  If not, wait for up to 5 ms
        timeout = 5;
        while (((p->iCurrent->status & ACT) != 0) && (timeout--)) {
            // NOTE: This delay sucks!  We should remove this
            // with an interrupt driven way later.
            UEZBSPDelayMS(1);
        }
        // Abort if the timeout failed
        if (timeout == 0)
            break;

        // Copy over a byte
        *(p_dest++) = p->iCurrent->buf_p[p->iCurrentByteIndex++];
        p->iFrameByteIndex++;
        if ((p->iCurrentByteIndex >= p->iCurrentByteSize)
                || (p->iFrameByteIndex >= p->iFrameByteSize)) {
            // If we have reached the end of this descriptor's block OR we have
            // just plain reached the end of the frame, move to the next
            // descriptor block.

            // Was this the end?
            if ((p->iCurrent->status & FP0) == FP0) {
                p->le0.stat.rx_packets++;
                isCopying = EFalse;
#if DEBUG_RX62N_EMAC_RX_DESC
                printf("--End frame\n");
#endif
                } else {
                // More data to go, setup the length again
                p->iCurrentByteIndex = 0;
                p->iCurrentByteSize = BUFSIZE;
#if DEBUG_RX62N_EMAC_RX_DESC
                printf("--End desc\n");
#endif
                }

            // Clear the flags for start/end and set to active again
            p->le0.rxcurrent->status &= ~(FP1 | FP0);
            p->le0.rxcurrent->status |= ACT;
            // Next block please
            p->iCurrent = p->le0.rxcurrent = p->le0.rxcurrent->next;

            // Restart EDMAC if stopped
            if (EDMAC.EDRRR.LONG == 0x00000000L) {
                /* Restart if stopped */
                EDMAC.EDRRR.LONG = 0x00000001L;
            }
        }
        // Decrement the number of bytes needed.
        // Regardless of the above, if we got all the bytes we
        // wanted, stop looping here.
        aSize--;
        if (aSize == 0)
            break;
    }
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_EMAC RX62N_EMAC_Interface = {
        "RX62N:EMAC",
        0x0100,
        RX62N_EMAC_InitializeWorkspace,
        sizeof(T_RX62N_EMAC_Workspace),

        RX62N_EMAC_Configure,
        RX62N_EMAC_CheckFrameReceived,
        RX62N_EMAC_StartReadFrame,
        RX62N_EMAC_EndReadFrame,
        RX62N_EMAC_RequestSend,
        RX62N_EMAC_DoSend,
        RX62N_EMAC_CopyToFrame,
        RX62N_EMAC_CopyFromFrame,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void RX62N_100_EMAC_RMII_Require()
{
    static const T_uezGPIOPortPin pins[] = {
        	GPIO_PB5,  	// RMII_TXD0
			GPIO_PB6,	// RMII_TXD1
			GPIO_PB4,  	// RMII_TXD_EN
			GPIO_PB7,	// RMII_CRS_DV
			GPIO_PB1,  	// RMII_RXD0
			GPIO_PB0,	// RMII_RXD1
			GPIO_PB3,	// RMII_RX_ER
			GPIO_PB2,  	// REF50CK
			GPIO_PA4,  	// ET_MDC
			GPIO_PA3,  	// ET_MDIO
			GPIO_PA5,  	// ET_LINKSTA
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("EMAC", (T_halInterface *)&RX62N_EMAC_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
    // Turn on the EMAC
    SYSTEM.MSTPCRB.BIT.MSTPB15 = 0;

    #if 0
	/*  PA5=1 Set ET_LINKSTA input  */
    PORTA.ICR.BIT.B5 = 1;
//    PORTA.DDR.BIT.B5 = 1; // output
    /*  PA3=1 Set ET_MDIO input */
    PORTA.ICR.BIT.B3 = 1;
//    PORTA.DDR.BIT.B3 = 1; // output
    
    /* PB0=1 Set RMII_RXD1 input    */
    PORTB.ICR.BIT.B0 = 1;
    /* PB1=1 Set RMII_RXD0 input    */
    PORTB.ICR.BIT.B1 = 1;
    /* PB2=1 Set REF50CLK input */
    PORTB.ICR.BIT.B2 = 1;
    /* PB3=1 Set RMII_RX_ER input   */
    PORTB.ICR.BIT.B3 = 1;
    /* PB7=1 Set RMII_CRS_DV input  */
    PORTB.ICR.BIT.B7 = 1;

//PORTA.DDR.BYTE = 0xFF;
//PORTA.DR.BYTE = 0x00;
//PORTA.PORT.BYTE = 0x38;
//PORTB.DDR.BYTE = 0x70;
//PORTB.DR.BYTE = 0x00;
//PORTB.PORT.BYTE = 0x00;
//    PORTB.DDR.BYTE = 0x70;
//    PORTB.ICR.BYTE = 0x8F; // Turn on the input buffers on these pins
#else
    PORT0.DDR.BYTE = 0x00;                                      /* P0 Ether IRQ inputs.                                             */

    PORTA.DR.BYTE  = 0x00;                                      /* PA outputs all LOW to start.                                     */
    PORTA.DDR.BYTE = 0xFF;                                      /* PA: Expansion (PA0-PA2), Ether (PA3-PA5), Audio (PA6-PA7).       */

//PORTB.DR.BYTE  = 0x01;  // Force RMII_RXD1 to high to change address of PHY
//PORTB.DR.BYTE  = 0x00;  // Force RMII_RXD1 (PB0) and RMII_RXD0 (PB1) to zero
//PORTB.DDR.BYTE = 0x73;                                      // PB: Ether.                                                       */

    PORTC.DR.BYTE  = 0xF7;                                      // PC: CS, clk = high; IO reset = low (not reset, Ether PHY).       */
    PORTC.DDR.BYTE = 0x7F;                                      // PC: SPI (PC0-2, PC4-7), IO reset (PC3).                          */

//UEZBSPDelayMS(50);
    PORTB.DR.BYTE  = 0x00;
    PORTB.DDR.BYTE = 0x70;                                      // PB: Ether.                                                       */

    // RMII
    IOPORT.PFENET.BYTE = 0x82;                         /* EE=1, PHYMODE=0, ENETE3=0, ENETE2=0, ENETE1=1, ENETE0=0 (Ether)  */

    PORTA.ICR.BIT.B5 = 1;                              /* PA5=1 Set ET_LINKSTA input                                       */
    PORTA.ICR.BIT.B3 = 1;                              /* PA3=1 Set ET_MDIO input                                          */
    PORTB.ICR.BIT.B0 = 1;                              /* PB0=1 Set RMII_RXD1 input                                        */
    PORTB.ICR.BIT.B1 = 1;                              /* PB1=1 Set RMII_RXD0 input                                        */
    PORTB.ICR.BIT.B2 = 1;                              /* PB2=1 Set REF50CLK input                                         */
    PORTB.ICR.BIT.B3 = 1;                              /* PB3=1 Set RMII_RX_ER input                                       */
    PORTB.ICR.BIT.B7 = 1;                              /* PB7=1 Set RMII_CRS_DV input                                      */
#endif
}

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void RX62N_176_EMAC_RMII_Require()
{
    static const T_uezGPIOPortPin pins[] = {
        	GPIO_P81,  	// RMII_TXD0
			GPIO_P82,	// RMII_TXD1
			GPIO_P80,  	// RMII_TXD_EN
			GPIO_P83,	// RMII_CRS_DV
			GPIO_P75,  	// RMII_RXD0
			GPIO_P74,	// RMII_RXD1
			GPIO_P77,	// RMII_RX_ER
			GPIO_P76,  	// REF50CK
			GPIO_P72,  	// ET_MDC
			GPIO_P71,  	// ET_MDIO
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("EMAC", (T_halInterface *)&RX62N_EMAC_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	/*  P54=1 Set ET_LINKSTA input  */
    PORT5.ICR.BIT.B4 = 1;
    /*  P71=1 Set ET_MDIO input */
    PORT7.ICR.BIT.B1 = 1;
    /* P74=1 Set RMII_RXD1 input    */
    PORT7.ICR.BIT.B4 = 1;
    /* P75=1 Set RMII_RXD0 input    */
    PORT7.ICR.BIT.B5 = 1;
    /* P76=1 Set REF50CLK input */
    PORT7.ICR.BIT.B6 = 1;
    /* P77=1 Set RMII_RX_ER input   */
    PORT7.ICR.BIT.B7 = 1;
    /* P83=1 Set RMII_CRS_DV input  */
    PORT8.ICR.BIT.B3 = 1;
}

/*-------------------------------------------------------------------------*
 * End of File:  EMAC.c
 *-------------------------------------------------------------------------*/
