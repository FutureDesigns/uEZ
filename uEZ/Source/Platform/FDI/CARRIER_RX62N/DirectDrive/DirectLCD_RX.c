/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Technology Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Technology Corp. and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* TECHNOLOGY CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THE THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/**********************************************************************************/
/*                                                                                */
/*  FILE        :DirectLCD_XBCFT.c                                                */
/*  DATE        :November 12, 2008                                               */
/*  DESCRIPTION :LCD Direct Drive H8SX LCD Direct Driver
            Supports SRAM TMR Dotclock
                     SRAM TMR/EDACK DotClock
                     SDRAM TMR/EDAC DotClock
                     SDRAM Cluster Bclk/TMR DotClock   */
/*                                                                                */
/**********************************************************************************/
/************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
01.22.2008 RTA-JMB Driver Rewrite to Parameterization
11.12.2008 RTA-JMB Port driver to SDRAM
12.03.2008 RTA-JMB Unify H8SX Drivers
************************************************************************/
#include <Config.h>
#include "DirectLCD_Types.h"
#include "DirectLCD.h" 
#include "DirectLCD_CNF.h" 
#include "OSLayer.h"
#include <uEZProcessor.h>
#include "HAL/Interrupt.h"

#ifdef __RX600
#define __inline inline
#endif

void LCD_DD_BLANK_isr(void);
void LCD_DD_DATA_isr(void);
void TASK_DD_isr(void);

/* convert 'a' to a string */
#define TO_STR1(a) #a
#define TO_STR(a) TO_STR1(a)

/* Define Pin Macros to ease implementation (secondary macros _1, _2 used for expansion ONLY */
#define CAT(a,b) a ## b
#define MERGE2(a,b) CAT(a,b)

/* Set signal DDR to output */
#ifdef __RX600
#define DDRo2(port,pin) MERGE2(PORT,port) ## MERGE2(.DDR.BIT.B,pin) = 1;
#define DDRi2(port,pin) MERGE2(PORT,port) ## MERGE2(.DDR.BIT.B,pin) = 0;
#define PINo2(port,pin) MERGE2(PORT,port) ## MERGE2(.DR.BIT.B,pin)
#define PINi2(port,pin) MERGE2(PORT,port) ## MERGE2(.PORT.BIT.B,pin)
#define ICRi2(port,pin) MERGE2(PORT,port) ## MERGE2(.ICR.BIT.B,pin)
#else
/* memory for DDR shadowing...used by DDR_xxxx Macros */
EXTERN unsigned char P1DDRS, P2DDRS, P3DDRS, P4DDRS, P5DDRS, P6DDRS, P8DDRS, PADDRS, PBDDRS, PCDDRS,PDDDRS, PEDDRS, PFDDRS,  PGDDRS, PHDDRS, PIDDRS, PJDDRS, PKDDRS, PLDDRS, PMDDRS;

#define DDRo2(port,pin) ( ## MERGE2(P,port) ## .DDR = (unsigned char)( ## MERGE2(P,MERGE2(port,DDRS)) |= (1 << pin)))
#define DDRi2(port,pin) ( ## MERGE2(P,port) ## .DDR = (unsigned char)( ## MERGE2(P,MERGE2(port,DDRS)) &= ~(1 << pin)))
#define PINo2(port,pin) MERGE2(P,port) ## MERGE2(.DR.BIT.B,pin)
#define PINi2(port,pin) MERGE2(P,port) ## MERGE2(.PORT.BIT.B,pin)
#define ICRi2(port,pin) MERGE2(P,port) ## MERGE2(.ICR.BIT.B,pin)
#endif

#define DDRo(signal) DDRo2(signal##_PORT, signal##_PIN)

/* Set signal DDR to input */
#define DDRi(signal) DDRi2(signal##_PORT, signal##_PIN)

/* Write signal level (for output) */
#define PINo(signal) PINo2(signal##_PORT, signal##_PIN)

/* Read signal level (for input) */
#define PINi(signal) PINi2(signal##_PORT, signal##_PIN)

/* Signals buffer control (for inputs) */
#define ICRi(signal) ICRi2(signal##_PORT, signal##_PIN)

#define size_ra(ra) (sizeof(ra)/sizeof(ra[0]))

/* macros to allow easy access to new style ICU bits and IODEFINE macros */
#define _ICU2_(func,unit_channel) _##func(unit_channel)
#define _ICU_(func,irq) _ICU2_(func, MERGE2(irq ## _UNIT, MERGE2(_,irq ## _CHANNEL)))

extern xTaskHandle ExDMACHnd; /* RTOS_USAGE */
  
#if !defined SDRAM_DD && !defined SDRAM_CLUSTER_DD && !defined SRAM_DD && !defined SRAM_NOMUX_DD
#define SRAM_DD
#endif

//************************************************
// INTERNAL MACROS and CHECKING
#ifdef DD_TMR_MTU
#define DD_TMR_UNIT MTUA
#define DD_TMR MTU
#define DD_TMR_IRQ _MTU
#endif
#ifdef DD_TMR_TPU
#define DD_TMR_UNIT TPU
#define DD_TMR TPU
#define DD_TMR_IRQ _TPU
#endif

// Ensure that the resolution of the panel matches the resolution of the linked resources.
// To change the linked resources, copy the appropriate resolution sub-directory to 
// the "resources" directory.
// For example... $(Workspace)\Resources\QVGA -> $(Workspace)\Resources
#ifndef PANEL_ROTATE
  #if ((V_LINES_DISPLAY > FRAME_HEIGHT) || (H_DOT_DISPLAY > FRAME_WIDTH))
  #error Resource Resolution does not match Panel Resolution.
  #endif
#else  
  #if ((V_LINES_DISPLAY > FRAME_WIDTH) || (H_DOT_DISPLAY > FRAME_HEIGHT))
  #error Resource Resolution does not match Panel Resolution.
  #endif
#endif

// number of bus cycles in the dot clock
#define DOT_BUS_CYCLES (BCLK_FREQUENCY/DOT_CLOCK_FREQUENCY_DATA)
#if (DOT_BUS_CYCLES*DOT_CLOCK_FREQUENCY_DATA < (BCLK_FREQUENCY-2)) || (DOT_BUS_CYCLES*DOT_CLOCK_FREQUENCY_DATA > (BCLK_FREQUENCY+2))
  #error BCLK_FREQUENCY must be integer multiple of DOT_CLOCK_FREQUENCY_DATA
#endif

#if defined SRAM_NOMUX_DD || defined SRAM_DD
  #if (DOT_BUS_CYCLES < FRAME_BUS_CYCLES)
    #error ATTEMPTING TO RUN DOT CLOCK FASTER THAN RAM ALLOWS (DOT_CLOCK_FREQUENCY_DATA)
  #endif
#elif defined SDRAM_DD
  #ifndef SDRAM_CAS_LATENCY
    #error "SDRAM_CAS_LATENCY must be defined! (usually 2 or 3)"
  #endif
  #ifndef SDRAM_CAS_LATENCY
    #error "SDRAM_ROW_PRECHARGE must be defined! (usually 1)"
  #endif
  #if (DOT_CLOCK_FREQUENCY_DATA != (BCLK_FREQUENCY/(2+SDRAM_ROW_PRECHARGE)))
    #error DOT_CLOCK_FREQUENCY_DATA MUST BE BCLK_FREQUENCY/(2+SDRAM_ROW_PRECHARGE) WITH SDRAM_DD MODE
  #endif
#elif defined SDRAM_CLUSTER_DD
  #if (DOT_CLOCK_FREQUENCY_DATA != BCLK_FREQUENCY)
    #error DOT_CLOCK_FREQUENCY_DATA MUST BE BCLK_FREQUENCY WITH SDRAM_CLUSTER_DD MODE
  #endif  
#endif

// number of TMR cycles in the dot clock
#define DOT_TMR_CYCLES (PCLK_FREQUENCY/DOT_CLOCK_FREQUENCY_BLANK)
#if (DOT_TMR_CYCLES*DOT_CLOCK_FREQUENCY_BLANK < (PCLK_FREQUENCY-2)) || (DOT_TMR_CYCLES*DOT_CLOCK_FREQUENCY_BLANK > (PCLK_FREQUENCY+2))
  #error PCLK_FREQUENCY must be integer multiple of DOT_CLOCK_FREQUENCY_BLANK
#endif

#if ((PCLK_FREQUENCY/DOT_CLOCK_FREQUENCY_BLANK) < 2)
  #error NEED AT LEAST 2 PCLK STATES PER DOT CLOCK (MODIFY DOT_CLOCK_FREQUENCY_BLANK)
#endif

/* Define ratio of bus clock to peripheral clock 1x=1, 2x=2, 4x=4 */
// #define BCLK_PCLK_RATIO (BCLK_MUL/PCLK_MUL)
#define DOT_TO_PCLK(DOT_CLOCKS) ((DOT_CLOCKS) * (PCLK_FREQUENCY/DOT_CLOCK_FREQUENCY_BLANK))
#define BCLK_TO_DOTB(BCLK_CLOCKS) ((BCLK_CLOCKS) / (BCLK_FREQUENCY/DOT_CLOCK_FREQUENCY_BLANK))
#define DOTD_TO_DOTB(DOTD_CLOCKS) ((DOTD_CLOCKS) * (DOT_CLOCK_FREQUENCY_BLANK/1000L) / (DOT_CLOCK_FREQUENCY_DATA/1000L)) // Div by 1000 required to keep from exceeding 32 bit size

#if defined SRAM_NOMUX_DD
  #if ( DOT_CLOCK_FREQUENCY_DATA != DOT_CLOCK_FREQUENCY_BLANK)
    #error DOT_CLOCK_FREQUENCY_DATA MUST MATCH DOT_CLOCK_FREQUENCY_BLANK WITH SRAM_NOMUX_DD
  #endif
#endif

#ifndef H_DOT_FRONT_PORCH_BLANK
#define H_DOT_FRONT_PORCH_BLANK 0
#endif

#define FRAME_W(cs) MERGE3(BSC.CS, cs,WCR1.BIT.CSRWAIT)
#define FRAME_BSC_W FRAME_W(FRAME_CS)

// ExDMA Constants and BUS cycle additions
#define DRAM_REFRESH_BCLK (6)       // BClock cycles to refresh DRAM (includes RAS)
#define DRAM_RAS_BCLK (2)           // Additional BClock cyles for RAS (on page change)
#define ExDMA_REQ_TO_LCD_LATCH (6)  // BClock Cycles from request to data valid if nothing else on bus 
#define EXDMA_NEGOTIATE (3)         // BClock Cycles to renogitiate for the bus
#if (BCLK_FREQUENCY == ICLK_FREQUENCY)
#define EXDMA_CLUSTER_NEGOTIATE (5) // BClock cycles added to each cluster (more synchronization when equal?)
#else
#define EXDMA_CLUSTER_NEGOTIATE (4) // BClock cycles added to each cluster
#endif
#define ExDMA_SEGMENTS (4)          // Number of segments to break up transfer (H8S 256 transfer/request block limit)

#if defined SRAM_DD
  // Number of extra dot clocks required during block transfer
  #define ExDMA_BLK_EXTRA (ExDMA_REQ_TO_LCD_LATCH)  // Number of extra dot clocks required during block transfer
  // Factor in all extras and bus rates to determine number of dot clocks in ExDMA transfer (round up to integer dot clock value).
  #define H_DOT_DISPLAY_DATA (DOTD_TO_DOTB(H_DOT_DISPLAY) + BCLK_TO_DOTB(ExDMA_BLK_EXTRA) + 1)
#elif defined SRAM_NOMUX_DD
  // Number of extra bus clocks required during block transfer
  #define ExDMA_BLK_EXTRA 0
  // Number of Dot Clocks during data portion of line
  #define H_DOT_DISPLAY_DATA (H_DOT_DISPLAY)
#elif defined SDRAM_CLUSTER_DD
  // Number of extra bus clocks required during block transfer
  #define ExDMA_BLK_EXTRA (ExDMA_REQ_TO_LCD_LATCH + \
                          (DRAM_REFRESH_BCLK *(1+(H_DOT_DISPLAY)/(BCLK_FREQUENCY / 64000L))) +  \
                          ((EXDMA_CLUSTER_NEGOTIATE) *(1+((H_DOT_DISPLAY-1)/16))) +  \
                          (DRAM_RAS_BCLK *(2+(H_DOT_DISPLAY/SDRAM_PAGE_SIZE))))
                          // time from ExDMA request to start of transfer
                          // number of BCLK cycles consumed by refresh cycles (assumes refresh every 15.625us...64000L = 1000000L/15.625
                          // number of BCLK cycles consumed by cluster negotiation
                          // number of BCLK cycles consumed by Row Address Strobes (minimum 2...one at start ExDMA and one or more during transfer)
  
  // Factor in all extras and bus rates to determine number of dot clocks in ExDMA transfer (round up to integer dot clock value).
  #define H_DOT_DISPLAY_DATA (BCLK_TO_DOTB(H_DOT_DISPLAY + ExDMA_BLK_EXTRA) + 1)
#elif defined SDRAM_DD
  // Number of extra bus clocks required during block transfer
  #define ExDMA_BLK_EXTRA (ExDMA_REQ_TO_LCD_LATCH + \
                          (DRAM_REFRESH_BCLK *(1+(H_DOT_DISPLAY * (2+SDRAM_ROW_PRECHARGE))/(BCLK_FREQUENCY / 64000L))) +  \
                          (DRAM_RAS_BCLK *(2+(H_DOT_DISPLAY/SDRAM_PAGE_SIZE))))
                          // time from ExDMA request to start of transfer
                          // number of BCLK cycles consumed by refresh cycles (assumes refresh every 15.625us...64000L = 1000000L/15.625
                          // number of BCLK cycles consumed by Row Address Strobes (minimum 2...one at start ExDMA and one or more during transfer)
  
  // Factor in all extras and bus rates to determine number of dot clocks in ExDMA transfer (round up to integer dot clock value).
  #define H_DOT_DISPLAY_DATA (BCLK_TO_DOTB((H_DOT_DISPLAY * (2+SDRAM_ROW_PRECHARGE)) + ExDMA_BLK_EXTRA) + 1)
#else
  #error NO DD MEMORY TYPE SPECIFIED
#endif

/***************************************************************************************************/
/* Ensure we can meet desired behavior */
#define V_LINES_PANEL (V_LINES_PULSE+V_LINES_BACK_PORCH+V_LINES_DISPLAY+V_LINES_FRONT_PORCH)
#define H_DOT_PERIOD_DATA (H_DOT_PULSE+H_DOT_BACK_PORCH+H_DOT_DISPLAY_DATA+H_DOT_FRONT_PORCH)
#define H_DOT_PERIOD_BLANK (H_DOT_PULSE+H_DOT_BACK_PORCH+H_DOT_DISPLAY+H_DOT_FRONT_PORCH+H_DOT_FRONT_PORCH_BLANK)

#define H_DOT_PERIOD_PANEL ((V_LINES_DISPLAY*H_DOT_PERIOD_DATA)+((V_LINES_PANEL-V_LINES_DISPLAY)*H_DOT_PERIOD_BLANK))
#define VLINES_EXTEND ((DOT_CLOCK_FREQUENCY_BLANK-(H_DOT_PERIOD_PANEL*DESIRED_FRAME_RATE))/(H_DOT_PERIOD_BLANK*DESIRED_FRAME_RATE))
#if (VLINES_EXTEND < 0)
//  #error Cannot achieve DESIRED_FRAME_RATE
#endif

#define V_LINES_PERIOD (V_LINES_PANEL+VLINES_EXTEND)

#define H_DOTS_BLANK ((V_LINES_PERIOD - V_LINES_DISPLAY) * (H_DOT_PERIOD_BLANK))
#define H_DOTS_DATA (V_LINES_DISPLAY * H_DOT_PERIOD_DATA)
#define MCU_ACCESS_PCT ((H_DOTS_BLANK * 100)/(H_DOTS_BLANK+H_DOTS_DATA))
#if (MINIMUM_MCU_ACCESS_PCT > MCU_ACCESS_PCT)
//  #error Cannot achieve MINIMUM_MCU_ACCESS
#endif
/***************************************************************************************************/

#define TMR_CHANNEL(channel) MERGE2(DD_TMR,channel)
#define TMR_DD(signal) TMR_CHANNEL(signal##_CHANNEL)

#define TMR_IO_A .TIORH
#define TMR_IO_B .TIORH
#define TMR_IO_C .TIORL
#define TMR_IO_D .TIORL

#ifdef uEZGUI_RX_35_DD
	// For use with MTU 1, 2, 7, and 8
	#define TMR_IO2_A .TIOR
	#define TMR_IO2_B .TIOR
#endif

#define TMR_IO(channel,pin) TMR_CHANNEL(channel) ## MERGE2(MERGE2(TMR_IO_,pin), MERGE2(.BIT.IO,pin))
#define TMR_DD_IO(signal) TMR_IO(signal##_CHANNEL,signal##_TIOC)

#ifdef uEZGUI_RX_35_DD
	#define TMR_IO2(channel,pin) TMR_CHANNEL(channel) ## MERGE2(MERGE2(TMR_IO2_,pin), MERGE2(.BIT.IO,pin))
	#define TMR_DD_IO2(signal) TMR_IO2(signal##_CHANNEL,signal##_TIOC)
#endif

#define TMR_TGR(channel,pin) TMR_CHANNEL(channel) ## MERGE2(.TGR,pin)
#define TMR_DD_TGR(signal) TMR_TGR(signal##_CHANNEL,signal##_TIOC)

#define TMR_TGF(channel,pin)TMR_CHANNEL(channel) ## MERGE2(.TSR.BIT.TGF,pin)
#define TMR_DD_TGF(signal) TMR_TGF(signal##_CHANNEL,signal##_TIOC)

#define TMR_TGIE(channel,pin) TMR_CHANNEL(channel) ## MERGE2(.TIER.BIT.TGIE,pin)
#define TMR_DD_TGIE(signal) TMR_TGIE(signal##_CHANNEL,signal##_TIOC)

#define TMR_TCRA 0x20 /* clear on channel A */
#define TMR_TCRB 0x40 /* clear on channel B */
#define TMR_TCRC 0xA0 /* clear on channel C */
#define TMR_TCRD 0xC0 /* clear on channel D */
#define TMR_TCR(pin) MERGE2(TMR_TCR,pin)
#define TMR_DD_TCR(signal) TMR_TCR(signal##_TIOC)

#define DD_TMR_ICU2(func,unit,channeln,pinn) _ICU2_(func, MERGE2(MERGE2(unit,channeln), MERGE2(MERGE2(_TGI,pinn),channeln)))
#define  DD_TMR_ICU(func,irq) DD_TMR_ICU2(func, DD_TMR_IRQ, irq ## _CHANNEL, irq ## _TIOC)

//************************************************
// INTERNAL Memory Allocation

typedef struct 
{
  Raster_type raster[FRAME_HEIGHT * FRAME_WIDTH];
} frame_type;

/* Section definition for SRAM used as LCD Frame(s) */
#pragma section LCD_Frames // This really gets put into LCD_Frames_2 for some bizarre reason
frame_type frames[LCD_FRAMES];  // SRAM allocated for LCD frames
#pragma section
#pragma section LCD_Frames_2 // not sure why we need both LCD_Frames and LCD_Frames_2!
    int G_frames2;
#pragma section

static uI16 Frame;        // index to frame buffer
static sI16 Hcount;       // line counter
static sI16 VLinesPeriod; // number or lines in the frame...changes dynamically to set desired frame rate.
static sI16 FrameRate=DESIRED_FRAME_RATE;    // Frame rate corresponding to the VLinesPeriod

typedef struct
{
  Raster_type *pLine;     // pointer to starting line of region
  sI16 LineNext;          // value to change pLine by to get to start of next line
  sI16 Height;            // number of lines in this region
} line_list_type;

static line_list_type LineList[MAX_FRAME_REGIONS+1];  // information about raster data to display (allocate spare block to ensure proper list termination)
static line_list_type *pLineList;                   // pointer to current LineList record
static line_list_type ActiveLineList;               // Copy of LineList record that Driver modifies and uses for ExDMA transfer

uI16 LCD_Vcount;          // frame counter
uI16 LCD_BusActive;       // flag indicating if we're running ExDMA on this line

/*****************************************************************************
Name: LCDinit()    
Parameters: none                     
Returns:  Error Code as necesary     
Description:sequences the power to the LCD display, initalizes the exDMA,
and timer output TOICA0 to generate the exDMA request signal
*****************************************************************************/
#ifdef DD_TMR_MTU
static const uI08 TMR_mask[]={0x01,0x02,0x04,0x40,0x80,0x00};
#else
static const uI08 TMR_mask[]={0x01,0x02,0x04,0x08,0x10,0x20};
#endif

LCDErrorType LCDInit(T_LCDControllerSettings *aSettings)
{
    InterruptRegister(DD_TMR_ICU(VECT,HDEN), LCD_DD_BLANK_isr, INTERRUPT_PRIORITY_HIGH, "DD_HDEN");
    InterruptRegister(DD_TMR_ICU(VECT,HDEN2), LCD_DD_DATA_isr, INTERRUPT_PRIORITY_HIGH, "DD_DATA");
    InterruptRegister(DD_TMR_ICU(VECT,DOTCLK), TASK_DD_isr, INTERRUPT_PRIORITY_HIGH, "DD_DOT");

  PINo(HSYNC) = 1;              // Hsync active low , so set high.
  DDRo(HSYNC);                  // set Hsync to output
  PINo(VSYNC) = 1;              // Vsync active low , so set high
  DDRo(VSYNC);                  // set Vsync to output

  PINo(DOTCLK) = 1;  // Set to idle state of DOTCLK
  DDRo(DOTCLK);      // Set EDACK2 to output DOTCLK when we're bit banging

  // configure ExDMA
  ICRi(EXDMAC_DD_REQ) = 1;          // configure ExDMA pin to connect to peripheral
  EXDMAC_DD.EDMCNT.BIT.DTE = 0;     // stops DMA transfer
  EXDMAC_DD.EDMTMD.BIT.DCTG = 2;    // transfer source: 0=software, 2=external, 3=peripheral
#ifdef RAM_32WIDE  
  EXDMAC_DD.EDMTMD.BIT.SZ = 2;       // dma size:  0=8bit, 1=16 bit, 2=32bit
#else
  EXDMAC_DD.EDMTMD.BIT.SZ = 1;       // dma size:  0=8bit, 1=16 bit, 2=32bit
#endif  
  EXDMAC_DD.EDMRMD.BIT.DREQS = 2;   // 0=rising edge, 1=falling edge, 2=low level

#ifdef SDRAM_CLUSTER_DD
  EXDMAC_DD.EDMTMD.BIT.MD = 3;      // transfer mode: 0= normal, 1= repeat, 2= block, 3=cluster
#else
  EXDMAC_DD.EDMTMD.BIT.MD = 2;      // transfer mode: 0= normal, 1= repeat, 2= block, 3=cluster
#endif

#if defined SDRAM_DD || defined SRAM_DD
  EXDMAC_DD.EDMOMD.BIT.DACKE = 1;   // Allow EDACK to output
#else
  EXDMAC_DD.EDMOMD.BIT.DACKE = 0;   // Don't output EDACK
#endif
                                    
  EXDMAC_DD.EDMAMD.LONG = 0;        // sets register to reset state
  EXDMAC_DD.EDMAMD.BIT.AMS = 1;     // single address mode selected 
  EXDMAC_DD.EDMAMD.BIT.DIR = 0;     // single address direction read from EDMSAR
  EXDMAC_DD.EDMTMD.BIT.DTS = 2;     // 0=block repeat source, 1=block repeat destination, 2=no block repeat
  EXDMAC.EDMAST.BIT.DMST = 1;       // start EXDMAC unit

#ifndef PANEL_ROTATE
  #ifndef H_DOT_INVERT
    EXDMAC_DD.EDMAMD.BIT.SM = 0x2;    // increment source address after each xfer
  #else                               
    EXDMAC_DD.EDMAMD.BIT.SM = 0x3;    // decrement source address after each xfer
  #endif
#else
  EXDMAC_DD.EDMAMD.BIT.SM = 0x1;      // use offset register to control DMA transfers
  #ifndef V_LINES_INVERT
    EXDMAC_DD.EDMOFR = -V_LINES_RESOURCE*2;  // Two bytes per transfer (decrement through "Rows")
  #else
    EXDMAC_DD.EDMOFR = V_LINES_RESOURCE*2;  // Two bytes per transfer (increment through "Rows")
  #endif
#endif

  {
    /* initialize line list with default values */
    sI16 region;
    for (region=0; region < size_ra(LineList); region++)
      LCDSetLineSource(region, FRAME_HEIGHT, frames[Frame].raster, FRAME_WIDTH);
  }

#if defined SRAM_DD || defined SRAM_NOMUX_DD
  /* initial chip select configuration */
  FRAME_BSC_W = FRAME_BUS_CYCLES;             // wait states
#endif
    
  // Stop all LCD_DD channels
  DD_TMR_UNIT.TSTR.BYTE &= ~((TMR_mask[HPER_CHANNEL])|(TMR_mask[HSYNC_CHANNEL])|(TMR_mask[VSYNC_CHANNEL])|(TMR_mask[HDEN_CHANNEL]) | (TMR_mask[DOTCLK_CHANNEL]));

  // Configure Vsynch
//  TMR_DD(VSYNC).TMDR.BYTE = 0xC3;               // use PWM mode 2
  TMR_DD(VSYNC).TMDR.BYTE = 0x02;               // use PWM mode 1
  TMR_DD(VSYNC).TCR.BYTE = 0x60;                // Configure for HPER Synch clear (may be overwritten by HPER)
  TMR_DD_IO(VSYNC) = 0x6;                       // Vsynch init high, high on match.
//  TMR_DD_TGR(VSYNC) = 0;                        // Vsynch pulse is multiples of Hperiod
  TMR_DD_TGR(VSYNC) = DOT_TO_PCLK(H_DOT_PERIOD_BLANK)-1; // period timeout...Vsynch pulse is multiples of Hperiod
 
  TMR_DD(VPER).TMDR.BYTE = 0x02;               // use PWM mode 1
  TMR_DD(VPER).TCR.BYTE = 0x60;                // Configure for HPER Synch clear (may be overwritten by HPER)
  TMR_DD_IO(VPER) = 0x6;                       // Vsynch init high, high on match.
  TMR_DD_TGR(VPER) = 10;                        // Vsynch pulse is multiples of Hperiod

  // Configure Horizontal Synch
//  TMR_DD(HSYNC).TMDR.BYTE = 0xC3;               // use PWM mode 2
  TMR_DD(HSYNC).TMDR.BYTE = 0x02;               // use PWM mode 1
  TMR_DD(HSYNC).TCR.BYTE = 0x60;                // Configure for HPER Synch clear (may be overwritten by HPER)
#ifdef uEZGUI_RX_35_DD
  TMR_DD_IO2(HSYNC) = 0x2;                       // Hsynch init low, high on match.
#else
  TMR_DD_IO(HSYNC) = 0x2;                       // Hsynch init low, high on match.
#endif
  TMR_DD_TGR(HSYNC) = DOT_TO_PCLK(H_DOT_PULSE)-1; // Hynch Pulse Width

  // Configure Data Enable (ExDMA Request)
//  TMR_DD(HDEN).TMDR.BYTE = 0xC2;                // PWM Mode 1 operation
  TMR_DD(HDEN).TMDR.BYTE = 0x02;                // PWM Mode 1 operation
  TMR_DD(HDEN).TCR.BYTE = 0x60;                 // Clear from the HPER Synchronous Input
  TMR_DD_IO(HDEN) = 0x6;                        // Active Data Mask init high, High on Match
                                                // 
#if defined SRAM_NOMUX_DD
  TMR_DD_TGR(HDEN) = DOT_TO_PCLK(H_DOT_PULSE+H_DOT_BACK_PORCH + 0.5) - ExDMA_REQ_TO_LCD_LATCH + (DOT_BUS_CYCLES-2) -1;
#elif defined SDRAM_CLUSTER_DD  
  TMR_DD_TGR(HDEN) = DOT_TO_PCLK(H_DOT_PULSE+H_DOT_BACK_PORCH)-1; 
#else
  TMR_DD_TGR(HDEN) = DOT_TO_PCLK(H_DOT_PULSE+H_DOT_BACK_PORCH)-1-1; // extra -1 because in controlling the external mux we don't want to switch CLK as it is changing to avoid potential "glitch". Output of MUX should be same before/after.
#endif

  TMR_DD_IO(HDEN2) = 0x6;                       // Active Data Mask init high, High on Match
  TMR_DD_TGR(HDEN2) = DOT_TO_PCLK(H_DOT_PULSE+H_DOT_BACK_PORCH+H_DOT_DISPLAY)-1;

  /* because we're useing PWM 1 mode for DOTCLOCK and HSYNC, we need to phase HDEN 1 cycle earlier...as state change is at END of DOTCLOCK and HSYNC (not 0 cycle) */
  TMR_DD_TGR(HDEN) -= 1;
  TMR_DD_TGR(HDEN2) -= 1;
  
  // Configure Horizontal Period
//  TMR_DD(HPER).TMDR.BYTE = 0xC3;                // use PWM mode 2
  TMR_DD(HPER).TMDR.BYTE = 0x02;                // use PWM mode 1
  TMR_DD(HPER).TCR.BYTE = TMR_DD_TCR(HPER);     // Set TCR to clear TCNT on HPER
#ifdef uEZGUI_RX_35_DD
  TMR_DD_IO2(HPER) = 0x5;                  		// Init high, 0 on match (for PWM mode 1)
#else  
  TMR_DD_IO(HPER) = 0x5;                        // Init high, 0 on match (for PWM mode 1)
#endif  
  TMR_DD_TGR(HPER) = DOT_TO_PCLK(H_DOT_PERIOD_BLANK)-1; // period timeout

  // Configure Dot Clock & Dot Period (must be same channel)
//  TMR_DD(DOTCLK).TMDR.BYTE = 0xC3;              // use PWM mode 2
  TMR_DD(DOTCLK).TMDR.BYTE = 0x02;              // use PWM mode 1
  TMR_DD(DOTCLK).TCR.BYTE = TMR_DD_TCR(DOTPER); // Set TCR to clear TCNT on DOTPER
#ifdef DOT_INVERT
  TMR_DD_IO(DOTCLK) = 0x5;                      // Dot Clock init hi, Low on match
  TMR_DD_IO(DOTPER) = 0x6;                      // Used for timeout
#else
  TMR_DD_IO(DOTCLK) = 0x2;                      // Dot Clock init low, High on match
  TMR_DD_IO(DOTPER) = 0x1;                      // Used for timeout
#endif
  TMR_DD_TGR(DOTCLK) = DOT_TO_PCLK(0.5)-1;      // set falling edge time
  TMR_DD_TGR(DOTPER) = DOT_TO_PCLK(1)-1;        // Dot clock period

  // Allow timers to access ICU
  TMR_DD_TGIE(HDEN) = 1;
  TMR_DD_TGIE(HDEN2) = 1;
  TMR_DD_TGIE(DOTCLK) = 1;
  DD_TMR_ICU(IPR,HDEN) = configDIRECT_DRIVE_INTERRUPT_PRIORITY;
  
  // Run ISR at end of Active Data Mask
  DD_TMR_ICU(IEN,HDEN) = 1;
  
  // Run Task Control ISR at start of Vertical Active period (via DOTCLK IRQ)
  DD_TMR_ICU(IPR,DOTCLK) = configDIRECT_DRIVE_INTERRUPT_PRIORITY; // configKERNEL_INTERRUPT_PRIORITY;

  // Synch horizontal channels
  DD_TMR_UNIT.TSYR.BYTE = ((TMR_mask[HPER_CHANNEL])|(TMR_mask[HSYNC_CHANNEL])|(TMR_mask[VSYNC_CHANNEL])|(TMR_mask[HDEN_CHANNEL]));
  // Start all LCD_DD channels
  DD_TMR_UNIT.TSTR.BYTE |= ((TMR_mask[HPER_CHANNEL])|(TMR_mask[HSYNC_CHANNEL])|(TMR_mask[VSYNC_CHANNEL])|(TMR_mask[HDEN_CHANNEL]) | (TMR_mask[DOTCLK_CHANNEL]));

  Hcount = 0;              // ensure first IRQ starts new frame
  VLinesPeriod = V_LINES_PERIOD;

  return(LCDAPI_SUCCESS);
}
 
/*****************************************************************************
Name:  LCD_DD_BLANK_isr   
Parameters: none                     
Returns:  nothing     
Description: Direct Drive interrupt service routine during the vertical blanking period.
  During the blanking period, we interrupt at the BEGINNING of the data enable period.
  This allows us maximum time to set up the TMR units prior to the start of the next line..
*****************************************************************************/
/* #pragma interrupt (LCD_DD_BLANK_isr(vect=DD_TMR_ICU(VECT,HDEN),enable)) */
void LCD_DD_BLANK_isr(void)
{
  // Configure behavior for the NEXT line
  if (Hcount >= VLinesPeriod)
  {
    TMR_DD_IO(VSYNC) = 0x1;                     // Vsynch init low, Low on match
    TMR_DD_IO(VPER) = 0x1;                     // Vsynch init low, Low on match
    Hcount = 0;
  }
  else
  switch (Hcount)
  {
    case (V_LINES_PULSE):
    {
      TMR_DD_IO(VSYNC) = 0x6;                     // Vsynch init High, High on match
      TMR_DD_IO(VPER) = 0x6;                     // Vsynch init High, High on match
      break;
    }

    case (V_LINES_PULSE+V_LINES_BACK_PORCH):
    {
      TMR_DD_IO(HDEN) = 0x5;                      // Enable Active Data Mask init high, Low on Match
      LCD_BusActive = 1;

      /* Configure RAM for ExDMA access timing */
#if defined SRAM_DD || defined SRAM_NOMUX_DD
  #if DOT_BUS_CYCLES != FRAME_BUS_CYCLES
    #if DOT_BUS_CYCLES <= 2   
          FRAME_BSC_AST = 0x0;                        // 0 =2 cycle external access and disables waits, 1=use wait states
    #else
          FRAME_BSC_AST = 0x1;                        // 0 =2 cycle external access and disables waits, 1=use wait states
          FRAME_BSC_W = DOT_BUS_CYCLES-3;             // wait states
    #endif
  #endif
#endif
      pLineList = LineList;                       // reset line list
      ActiveLineList = *pLineList++;              // copy initial line list for the driver and increment pointer
      ActiveLineList.Height += V_LINES_PULSE+V_LINES_BACK_PORCH-1; // offset Height so I can compare against "Hcount"
      
      DD_TMR_ICU(IR,HDEN2) = 0;                   // clear pending interrupt request
      DD_TMR_ICU(IEN,HDEN2) = 1;                  // start using HDEN2
      DD_TMR_ICU(IEN,DOTCLK) = 1;                 // Signal scheduler to suspend RAM access tasks
      break;
    }
    
    case (V_LINES_PULSE+V_LINES_BACK_PORCH+1):
    {
      DD_TMR_ICU(IEN,HDEN) = 1;                   // stop using HDEN (re-enabeld by HDEN2 ISR
      TMR_DD_TGR(HPER) = DOT_TO_PCLK(H_DOT_PERIOD_DATA)-1; // period timeout during data transfer
      TMR_DD_TGR(HDEN2) = DOT_TO_PCLK(H_DOT_PULSE+H_DOT_BACK_PORCH+H_DOT_DISPLAY_DATA)-1;  // end of enable for data transfer
      break;
    }
      
    case (V_LINES_PULSE+V_LINES_BACK_PORCH+V_LINES_DISPLAY+1):
    {
      TMR_DD_TGR(HPER) = DOT_TO_PCLK(H_DOT_PERIOD_BLANK)-1; // period timeout during blanking
      TMR_DD_TGR(HDEN2) = DOT_TO_PCLK(H_DOT_PULSE+H_DOT_BACK_PORCH+H_DOT_DISPLAY)-1; // end of enable during blanking (generates interrupts, so must be accurate even during blanking)
      break;
    }
  }

  if ((Hcount < (V_LINES_PULSE+V_LINES_BACK_PORCH)) || (Hcount > (V_LINES_PULSE+V_LINES_BACK_PORCH+V_LINES_DISPLAY)))
    Hcount++;                                       // increment the horizontal line counter
}

/*****************************************************************************
Name:  LCD_DD_DATA_isr   
Parameters: none                     
Returns:  nothing     
Description: Direct Drive interrupt service routine during the active data piece of the frame.
  During the data portion, we interrupt at the END of the data enable period.
  We use this configure the ExDMA for the next line of the transfer.
*****************************************************************************/
/* #pragma interrupt (LCD_DD_DATA_isr(vect=DD_TMR_ICU(VECT,HDEN2),enable)) */
void LCD_DD_DATA_isr(void)
{
  if (Hcount < (V_LINES_PULSE+V_LINES_BACK_PORCH+V_LINES_DISPLAY))
  {
    EXDMAC_DD.EDMSAR = ActiveLineList.pLine;    // start of next line transfer
    EXDMAC_DD.EDMCRB = 1;                       // output one line (block) of pixels 

#ifdef SDRAM_CLUSTER_DD
    EXDMAC_DD.EDMCRA = (16L << 8)+8L;              // transfer one 16 word cluster at a time.
#else
    EXDMAC_DD.EDMCRA = (H_DOT_DISPLAY << 16)+H_DOT_DISPLAY;  // transfer one panel line
#endif

    EXDMAC_DD.EDMCNT.BIT.DTE = 1;                     // re-start DMA transfer  
    ActiveLineList.pLine += ActiveLineList.LineNext;  // Active portion, need to update line pointer 
    
    #if (MAX_FRAME_REGIONS > 1)
      /* check if ready for next region...conditionally compiled based on need */
      if (Hcount >= ActiveLineList.Height)
      {
        ActiveLineList = *pLineList++;
        ActiveLineList.Height += Hcount;
      }
    #endif

    if (Hcount >= (V_LINES_PULSE+V_LINES_BACK_PORCH+V_LINES_DISPLAY-1))
    {
      /* this is last line to display...turn on HDEN */
      DD_TMR_ICU(IR,HDEN) = 0;                  // clear pending interrupt request
      DD_TMR_ICU(IEN,HDEN) = 1;                 // start using HDEN
    }
  }
  else
  {
    TMR_DD_IO(HDEN) = 0x6;                        // Disable Active Data Mask init high, High on Match
    LCD_BusActive = 0;
    /* Configure RAM for MCU access timing */
#if defined SRAM_DD || defined SRAM_NOMUX_DD
  #if DOT_BUS_CYCLES != FRAME_BUS_CYCLES
    #if FRAME_BUS_CYCLES <= 2   
        FRAME_BSC_AST = 0x0;                          // 0 =2 cycle external access and disables waits, 1=use wait states
    #else                                             
        FRAME_BSC_AST = 0x1;                          // 0 =2 cycle external access and disables waits, 1=use wait states
        FRAME_BSC_W = FRAME_BUS_CYCLES-3;             // wait states
    #endif
  #endif
#endif
    DD_TMR_ICU(IEN,HDEN2) = 0;                    // stop using HDEN2
    DD_TMR_ICU(IEN,DOTCLK) = 1;                   // Signal scheduler to resume RAM access tasks
    LCD_Vcount++;                                 //  increment v count here to give maximum time while bus idle
  }

  Hcount++;                                       // increment the horizontal line counter
}

/*****************************************************************************/
//
//  FUNCTION    TASK_DD_isr()
//  PARAMETERS  : None (ISR)
// 
// RETURNS     : None
//        
// DESCRIPTION :This task calls the scheduler to wake the task that blocks other
//              tasks that are accessing RAM. If RAM access tasks are not blocked,
//              the CPU will stall during ExDMA block transfers.
//              The IRQ source is a TMR channel that is configured to interrupt once/frame
//              at the end of the vertical blanking time.
//
/*****************************************************************************/ 
/* This is implemented as a called function to ensure a proper stack for the context switch ISR */
static portBASE_TYPE task_dd_helper(void)
{
  // Resume the ExMemory control task.
  //(void)RLCD_TaskResumeFromISR( ExDMACHnd );
  return(pdTRUE);
}
//#pragma interrupt (TASK_DD_isr(vect=DD_TMR_ICU(VECT,DOTCLK),enable))
void TASK_DD_isr(void)
{
  // Clear interupt and request to interupt
  DD_TMR_ICU(IEN,DOTCLK) = 0;
//  (void)xTaskResumeFromISR( ExDMACHnd );  /* RTOS_USAGE */
//  portEXIT_SWITCHING_ISR( pdTRUE );  /* RTOS_USAGE */
  task_dd_helper();
}     

/*****************************************************************************
Name: LCDSetLineSource()    
Parameters:
  Region: Region of display (horizontal strip). Ranging from 0 (bottom) to MAX_FRAME_REGIONS (user defined MACRO top).
  LineCount: Number of lines for this region.
  pSource: Address of first pixel of first line of region.
  LineStep: Distance (in pixels/uI16’s) from first pixel of first line to first pixel of second line (source regions can be wider than the panel).
Returns:  0 = ok, -1 = NG.
Description: setActiveFrame defines which frame buffer to display
*****************************************************************************/
sI16 LCDSetLineSource(sI16 Region, sI16 LineCount, Raster_type *pSource, sI16 LineStep)
{
  sI32 PixelStart;

  if (Region >= MAX_FRAME_REGIONS)
    return(-1);

  /* pSource must be within LCD_Frames section */
#ifdef RAM_32WIDE
  if ((pSource < (Raster_type *)__sectop("BLCD_Frames_2")) ||
      (pSource > (Raster_type *)__secend("BLCD_Frames_2")))
    return(-1);

  if ((&pSource[(sI32)LineCount*(sI32)LineStep] < (Raster_type *)__sectop("BLCD_Frames_2")) ||
      (&pSource[(sI32)LineCount*(sI32)LineStep] > (Raster_type *)__secend("BLCD_Frames_2")))
    return(-1);
#else
  if ((pSource < (Raster_type *)__sectop("BLCD_Frames_2")) ||
      (pSource > (Raster_type *)__secend("BLCD_Frames_2")))
    return(-1);

  if ((&pSource[(sI32)LineCount*(sI32)LineStep] < (Raster_type *)__sectop("BLCD_Frames_2")) ||
      (&pSource[(sI32)LineCount*(sI32)LineStep] > (Raster_type *)__secend("BLCD_Frames_2")))
    return(-1);
#endif
  /* only allow region to be up to panel size */
  if (LineCount > V_LINES_PANEL)
    return(-1);

  /* set line step and determine pixel start offset */
  #ifndef PANEL_ROTATE
    #ifndef H_DOT_INVERT
      PixelStart = 0;                       // move to start of line
    #else                               
      PixelStart = H_DOT_DISPLAY-1;           // move to end of line
    #endif
    #ifndef V_LINES_INVERT
      LineList[Region].LineNext = LineStep;   // Increment through lines
    #else
      LineList[Region].LineNext = -LineStep;  // Decrement through lines
      PixelStart += ((LineCount-1) * LineStep);  // move to end of frame
    #endif
  #else
    #ifndef H_DOT_INVERT
      PixelStart = 0;                       // move to start of line
      LineList[Region].LineNext = 1;          // Increment through "Columns"
    #else
      PixelStart = H_DOT_DISPLAY-1;           // move to end of line
      LineList[Region].LineNext = -1;         // Decrement through "Columns"
    #endif
    #ifndef V_LINES_INVERT
      PixelStart += ((sI32)LineCount * ((sI32)LineStep-1));  // move to end of frame
    #else
    #endif
  #endif

  LineList[Region].pLine = pSource + PixelStart;
  LineList[Region].Height = LineCount;

  return(0);
}

/*****************************************************************************
Name: LCDSetActiveRaster()    
Parameters: frame_request - selection of which frame to have ExDMA display.                     
Returns:  Address of raster that is being used for frame.     
Description: setActiveFrame defines which frame buffer to display
*****************************************************************************/
Raster_type * LCDSetActiveRaster(uI32 frame_request)
{
#if 0
  if (frame_request < LCD_FRAMES)
  {
    Frame = frame_request;
    LCDSetLineSource(0, FRAME_HEIGHT, frames[Frame].raster, FRAME_WIDTH);
  }
#else
  if (frame_request < 5) {
      Frame = frame_request;
      LCDSetLineSource(0, FRAME_HEIGHT, frames[Frame].raster, FRAME_WIDTH);
  }
  LCDSetLineSource(0, FRAME_HEIGHT, (void *)frame_request, FRAME_WIDTH);
#endif
  return(frames[Frame].raster);
}

/*****************************************************************************
Name: LCDGetActiveFrame()    
Parameters: none.                     
Returns:  Index of raster that is being used for frame.     
Description: Returns information about the which frame is currently active
*****************************************************************************/
uI16 LCDGetActiveFrame(void)
{
  return(Frame);
}

/*****************************************************************************
Name: LCDSetRasterOffset()    
Parameters: x,y: pixels to offset raster by, 0,0 = no offset.                     
Returns:  0 = ok, -1 = NG.     
Description: allows setting the display window within frame. Bounds offset as
 necessary.
*****************************************************************************/
sI16 LCDSetRasterOffset(sI16 x, sI16 y)
{
  /* ensure requested window is within the resource memory */
  if ((x < 0) || (y < 0))
    return(-1);
    
  if ((x > (FRAME_WIDTH - H_DOT_DISPLAY)) || (y > (FRAME_HEIGHT - V_LINES_DISPLAY)))
    return(-1);

  return (LCDSetLineSource(0, V_LINES_DISPLAY, &frames[Frame].raster[(sI32)y*FRAME_WIDTH + (sI32)x], FRAME_WIDTH));
}

/*****************************************************************************
Name: LCDGetFrameRate()    
Parameters: none:
Returns:  Current frame Rate in Hz.
Description: Returns current frame rate.
*****************************************************************************/
sI16 LCDGetFrameRate(void)
{
  return(FrameRate);
}

/*****************************************************************************
Name: LCDSetFrameRate()    
Parameters: rate: Desired Frame Rate in Hz.
Returns:  Positive % MCU access if we can achieve frame rate, Negative if cannot achieve frame rate.
Description: Controls the display frame rate by controlling the vertical blanking time.
*****************************************************************************/
sI16 LCDSetFrameRate(sI16 rate)
{
  /* first calculate how many DOTCLK cycles remaining per second at desired rate */
  sI32 VLinesExtend = DOT_CLOCK_FREQUENCY_BLANK-(H_DOT_PERIOD_PANEL* (sI32)rate);
  /* now divide remaining DOTCLK into "rate" number of lines */
  VLinesExtend /= (H_DOT_PERIOD_BLANK* (sI32)rate);
  /* if postive, set the value for use */
  if ((VLinesExtend > 0) && (rate > 0))
  {
    sI32 HDotsBlank;
    VLinesPeriod = V_LINES_PANEL+(sI16)VLinesExtend;
    HDotsBlank = (VLinesPeriod - V_LINES_DISPLAY) * (H_DOT_PERIOD_BLANK);
    FrameRate = rate;
    return ((HDotsBlank * 100L)/(HDotsBlank + H_DOTS_DATA));
  }
  else
  {
    return((sI16)VLinesExtend);
  }
}

/*****************************************************************************
Name: LCDOff()    
Parameters: none                     
Returns:  nothing     
Description: Shuts down the LCD - powers down LCD module
*****************************************************************************/
void LCDOff(void)
{
  #ifdef USE_LCD_PANEL_POWER
    PINo(LCD_PANEL_POWER) = 0;            // turn off power to LCD module
  #endif
}

/*****************************************************************************
Name: LCDBacklight()    
Parameters: int state                     
Returns:  nothing     
Description: nonzero value turns backlight on...
*****************************************************************************/
void LCDBacklight(int state)
{
  if (state)
  {
     DDRo(LCD_BACKLIGHT);           // enable backlight by setting direction register
     PINo(LCD_BACKLIGHT) = 1;       // turn on power to backlight
  }
  else
  {
     PINo(LCD_BACKLIGHT) = 0;       // turn off power to backlight
     DDRo(LCD_BACKLIGHT);           // disable backlight by setting direction register
  }   
}

void LCDOn(void)
{
    LCD_Vcount = 0;
    LCDSetActiveRaster(0);

    while ( LCD_Vcount < 2)
        UEZTaskDelay(1);

#ifdef USE_LCD_PANEL_POWER
    PINo(LCD_PANEL_POWER) = 1;    // prepare to turn on power to display (when output enabled below)
    DDRo(LCD_PANEL_POWER);        // turn on power
#endif

}
