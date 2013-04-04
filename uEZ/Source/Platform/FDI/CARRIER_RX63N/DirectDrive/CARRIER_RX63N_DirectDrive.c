//---------------------------------------------------------------------------
// LCD low level work
//---------------------------------------------------------------------------
#include <uEZProcessor.h>
#include "CARRIER_RX63N_DirectDrive.h"

//#define __evenaccess // for Eclipse
//---------------------------------------------------------------------------
// CPU timing:
//---------------------------------------------------------------------------
//#define     PROCESSOR_OSCILLATOR_FREQUENCY      96000000L    // MHz
//#define     XTAL_FREQUENCY  (12000000L)                      // MHz
//#define     ICLK_MUL        (PROCESSOR_OSCILLATOR_FREQUENCY/XTAL_FREQUENCY) // 8 -- 96 MHz
//#define     PCLK_MUL        (ICLK_MUL/2) // 4 -- 48 MHz
//#define     BCLK_MUL        (ICLK_MUL/2) // 4 -- 48 MHz
//#define     ICLK_FREQUENCY  (XTAL_FREQUENCY * ICLK_MUL)
//#define     PCLK_FREQUENCY  (XTAL_FREQUENCY * PCLK_MUL)
//#define     BCLK_FREQUENCY  (XTAL_FREQUENCY * BCLK_MUL)

#define NUM_BCLOCKS_PER_DOT  (BCLK_FREQUENCY/DOT_CLOCK_FREQUENCY)
#define NUM_PCLOCKS_PER_DOT  (PCLK_FREQUENCY/DOT_CLOCK_FREQUENCY)

//---------------------------------------------------------------------------
// Frame Memory:
//---------------------------------------------------------------------------
#pragma section LCD_Frames // This really gets put into LCD_Frames_2 for some bizarre reason
DD_frame DD_Frames[DD_NUM_FRAMES];
#pragma section

//---------------------------------------------------------------------------
// Macros:
//---------------------------------------------------------------------------
#define DOT_CLOCKS_PER_FRAME              (H_DOTS_PER_LINE * V_LINES_PER_FRAME)
#define EXTRA_DOT_CLOCKS_PER_SECOND(rate) (DOT_CLOCK_FREQUENCY - (DOT_CLOCKS_PER_FRAME * (rate)))
#define EXTRA_LINES_PER_SECOND(rate)      (EXTRA_DOT_CLOCKS_PER_SECOND(rate) / H_DOTS_PER_LINE)
#define EXTRA_LINES_NEEDED(rate)          (EXTRA_LINES_PER_SECOND(rate)/(rate))

//---------------------------------------------------------------------------
// Globals:
//---------------------------------------------------------------------------
static unsigned short DD_Frame;
static short DD_CurrentLine;
static short DD_LinesPerPeriod = V_LINES_PER_FRAME
        + EXTRA_LINES_NEEDED(DESIRED_FRAME_RATE);
//static short DD_FrameRate = DESIRED_FRAME_RATE;
unsigned short DD_FrameCount;
unsigned short DD_IsBusActive;

// allocate BMP wrappers for frames
//BMP_t BMP_frames[DD_NUM_FRAMES];

//---------------------------------------------------------------------------
// Regions:
//---------------------------------------------------------------------------
typedef struct {
    pixel *pLine;
    short LineNext;
    short Height;
} DD_region_lines;
static DD_region_lines DD_RegionLines[MAX_REGIONS_PER_FRAME + 1];
static DD_region_lines *DD_CurrentRegionLines;
static DD_region_lines DD_ActiveRegionLines;

//---------------------------------------------------------------------------
// Functions:
//---------------------------------------------------------------------------
void LCDOn(void)
{
   // Set LCD_PWR inactive
   PORT8.PODR.BIT.B7 = 1;
   PORT8.PDR.BIT.B7 = 1;
}

void LCDOff(void)
{
   // Set LCD_PWR inactive
   PORT8.PODR.BIT.B7 = 0;
   PORT8.PDR.BIT.B7 = 1;
}

int LCDInit(void)
{
	MPC.P55PFS.BIT.PSEL = 0x18; // Setup P55 for EDREQ0
	PORT5.PMR.BIT.B5 = 1;		// P55 to peripheral
	
	MPC.P54PFS.BIT.PSEL = 0x18; // Setup P54 for EDACK0
	PORT5.PMR.BIT.B4 = 1;		// P54 to peripheral

    // Connect MTIOC3C-B to P56	
	MPC.P56PFS.BIT.PSEL = 0x01; // Setup P56 for MTIOC3C
	PORT5.PMR.BIT.B6 = 1;		// P56 to peripheral

	// Connect MTIOC0C-A to P34
	MPC.P34PFS.BIT.PSEL = 0x01;
	PORT3.PMR.BIT.B4 = 1;

    // Enable EXDMA
    SYSTEM.MSTPCRA.BIT.MSTPA29 /* MSTP_EXDMAC */= 0;

    // enable MTU 0-5
	MSTP(MTU) = 0;

    // turn on all OE based outputs
    MTU.TOER.BYTE = 0xFF;

	// HSYNC
    //PORT2.PDR.BIT.B0 = 1;		// Hsync active low , so set high.
    //PORT2.PODR.BIT.B0 = 1;		// set Hsync to output
	MPC.P20PFS.BIT.PSEL = 0x01; // Setup P20 for MTIOC1A
	PORT2.PMR.BIT.B0 = 1;		// P20 to peripheral

	// VSYNC
    //PORT8.PDR.BIT.B3 = 1;		// Vsync active low , so set high
    //PORT8.PODR.BIT.B3 = 1;		// set Vsync to output
	MPC.P83PFS.BIT.PSEL = 0x01; // Setup P83 for MTIOC4C
	PORT8.PMR.BIT.B3 = 1;		// P83 to peripheral

    // Set to idle state of DOTCLK
    PORT5.PDR.BIT.B6 = 1;
    // Set EDACK2 to output DOTCLK when we're bit banging
    PORT5.PODR.BIT.B6 = 1;

    // configure ExDMA
	MPC.P55PFS.BIT.PSEL = 0x18; // Setup P55 for EDREQ0
	PORT5.PMR.BIT.B5 = 1;		// P55 to peripheral
	
    // stops DMA transfer
    EXDMAC0.EDMCNT.BIT.DTE = 0;
    // transfer source: 0=software, 2=external, 3=peripheral
    EXDMAC0.EDMTMD.BIT.DCTG = 2;

    // dma size:  0=8bit, 1=16 bit, 2=32bit
    EXDMAC0.EDMTMD.BIT.SZ = 1;
    // 0=rising edge, 1=falling edge, 2=low level
    EXDMAC0.EDMRMD.BIT.DREQS = 2;

    // transfer mode: 0= normal, 1= repeat, 2= block, 3=cluster
    EXDMAC0.EDMTMD.BIT.MD = 2;

    // 1=Allow EDACK to output, 0=Don't output EDACK
    EXDMAC0.EDMOMD.BIT.DACKE = 1;

    // sets register to reset state
    EXDMAC0.EDMAMD.LONG = 0;
    // single address mode selected
    EXDMAC0.EDMAMD.BIT.AMS = 1;
    // single address direction read from EDMSAR
    EXDMAC0.EDMAMD.BIT.DIR = 0;
    // 0=block repeat source, 1=block repeat destination, 2=no block repeat
    EXDMAC0.EDMTMD.BIT.DTS = 2;
    // start EXDMAC unit
    EXDMAC.EDMAST.BIT.DMST = 1;

    // 2=increment source address after each xfer, 3=decrement source address after each xfer
#if LCD_FLIP_X
    EXDMAC0.EDMAMD.BIT.SM = 0x3; // NOTE: Change this to ROTATE panel
#else
	EXDMAC0.EDMAMD.BIT.SM = 0x2; // NOTE: Change this to ROTATE panel
#endif
    {
        // initialize line list with default values
        short region;
        for (region = 0; region < (sizeof(DD_RegionLines)
                / sizeof(DD_RegionLines[0])); region++)
            LCDSetLineSource(region, DD_FRAME_HEIGHT,
                    DD_Frames[DD_Frame].raster, DD_FRAME_WIDTH);
    }

    //----
    // SRAM read timing initial configuration
    // FRAME_BUS_CYCLES
	/*
    BSC.CS7WCR1.BIT.CSRWAIT = (((12000000L) * ((PROCESSOR_OSCILLATOR_FREQUENCY
            / 12000000L) / 2)) / 12000000L) - 1;
    BSC.CS7WCR2.BIT.RDON = 1; // required to work properly (At least one wait state for the read assert (RD#))
	*/
    // Stop all LCD_DD channels
    MTU.TSTR.BYTE &= ~(0xC1);
	
    //----
    // Configure Vertical SYNC (VSYNC) on MTU4 channel C
    // VSYNC uses PWM mode 1
    MTU4.TMDR.BYTE = 0x02;
    // Synchronize this channel to HPER!
    // Configure it so that a clear in another channel
    // (HPER Sync Clear) will also clear this channel
    // (TSYR.SYNC bit will be set further below to 1 to make this happen)
    MTU4.TCR.BYTE = 0x60;
    // VSYNC starts HIGH, and stays HIGH on match (does not change yet)
    MTU4.TIORL.BIT.IOC = 0x06;
    // Set the period timeout... VSYNC pulse is multiples of Hperiod
    // We'll reset at least after this many clocks if HPER is not
    // resetting us.
    // ??? This will occur during the vertical blank periods.
    MTU4.TGRC = H_DOTS_PER_LINE * NUM_PCLOCKS_PER_DOT - 1;

    //----
    // Configure Vertical Period (VPER) on MTU4 channel D
    // VPER uses PWM mode 1
    MTU4.TMDR.BYTE = 0x02;
    // Synchronize this channel to HPER!
    // Configure it so that a clear in another channel
    // (HPER Sync Clear) will also clear this channel
    // (TSYR.SYNC bit will be set further below to 1 to make this happen)
    MTU4.TCR.BYTE = 0x60;
    // VPER will initially be HIGH, and HIGH on match (nothing happening)
    // (Later it will be initially LOW and trigger to go HIGH)
    MTU4.TIORL.BIT.IOD = 0x06;
    // Vsynch pulse is multiples of Hperiod
    // ??? What is this value supposed to be?
    MTU4.TGRD = V_LINES_PULSE; // was 10

    //----
    // Configure Horizontal Sync (HSYNC) on MTU1 channel A
    // use PWM mode 1
    MTU1.TMDR.BYTE = 0x02;
    // Synchronize this channel to HPER!
    // Configure it so that a clear in another channel
    // (HPER Sync Clear) will also clear this channel
    // (TSYR.SYNC bit will be set further below to 1 to make this happen)
    MTU1.TCR.BYTE = 0x60;
    // HSYNC will initially be LOW, and then go HIGH on match.
    MTU1.TIOR.BIT.IOA = 0x2;
    // The HYSNC will wait H_DOT_PULSE pixels/dots before transitioning
    MTU1.TGRA = H_DOT_PULSE * NUM_PCLOCKS_PER_DOT - 1;

    //----
    // Configure Data Enable on MTU0 channel A which triggers/enables 
	// the ExDMA Request.
    // Use PWM mode 1
    MTU0.TMDR.BYTE = 0x02;
    // Synchronize this channel to HPER!
    // Configure it so that a clear in another channel
    // (HPER Sync Clear) will also clear this channel
    // (TSYR.SYNC bit will be set further below to 1 to make this happen)
    MTU0.TCR.BYTE = 0x60;
    // Since we'll be starting in the vertical blank area, we
    // won't want any transitions.
    // Keep the Data Enable line initially high and transition to high
    // on matches.
    MTU0.TIORH.BIT.IOA = 0x6;
    // NOTE: extra -1 is necessary because in controlling the external
    // MUX we don't want to switch the CLK as it is changing to avoid
    // causing a potential "glitch" -- Output of MUX needs to be same
    // before and after.
    MTU0.TGRA = ((H_DOT_PULSE + H_DOT_BACK_PORCH) * NUM_PCLOCKS_PER_DOT)
            - 1 - 1;
    // Keep the Data Enable line high all the time while we are in
    // the display.
	// MTU0 channel B
    MTU0.TIORH.BIT.IOB = 0x6;
    // Setup the end of the data enable to drop after all the dots
    // of the display have been processed.
    MTU0.TGRB = ((H_DOT_PULSE + H_DOT_BACK_PORCH + H_DOT_DISPLAY)
            * NUM_PCLOCKS_PER_DOT) - 1;

    // because we're using PWM 1 mode for DOTCLOCK and HSYNC, we need
    // to phase HDEN 1 cycle earlier...as state change is at END of
    // DOTCLOCK and HSYNC (not 0 cycle)
    MTU0.TGRA -= 1;
    MTU0.TGRB -= 1;

    //----
    // Configure Horizontal Period (HPER) for MTU1 channel B
    // NOTE: All the above PWM's depend on this one
    // When it resets, they all reset.
    // HPER uses PWM mode 1
    MTU1.TMDR.BYTE = 0x02;
    // Set TCR to clear TCNT on match on TGRB (HPER)
    MTU1.TCR.BYTE = 0x40;
    // HPER will be HIGH, and then transition to LOW on match
    MTU1.TIOR.BIT.IOB = 0x5;
    // Period time is one complete line
    MTU0.TGRB = H_DOTS_PER_LINE * NUM_PCLOCKS_PER_DOT - 1;

    //----
    // Configure Dot Clock (DOTCLK) & Dot Period (DOTPER)
    // NOTE: Must be on same channel!
    // The DOTCLK and DOTPER are used for outputting clocks
    // when the DMA is not generating the clocks.
    // They both use PWM mode 1
    MTU3.TMDR.BYTE = 0x02;
    // Set TCR to clear TCNT on TGRD (DOTPER)
    MTU3.TCR.BYTE = 0xC0;
#ifdef DOT_INVERT
    // DOTCLK will be initially HIGH, then go LOW on transitions
    MTU3.TIORL.BIT.IOC = 0x5;
    // DOTPER marks the reset point.
    // Just go from HIGH back to LOW.
    MTU3.TIORL.BIT.IOD = 0x6;
#else
	// DOTCLK will be initially LOW, then go HIGH on transitions
    MTU3.TIORL.BIT.IOC = 0x2;
    // DOTPER marks the reset point.
    // Just go from LOW back to HIGH.
    MTU3.TIORL.BIT.IOD = 0x1;
#endif
    // Set falling edge time to about half of the PCLK clock
    // cycles of a full dot clock.
    MTU3.TGRC = ((0.5) * NUM_PCLOCKS_PER_DOT) - 1;
    // The dot clock period is, well, the number of PCLK's
    // per dot.  Again, minus one because the transition is at
    // the END.
    MTU3.TGRD = ((1) * NUM_PCLOCKS_PER_DOT) - 1;

    //----
    // Configure Interrupts.
    // DD_PixelsLineISR() is called at the end of each data enable period
    MTU0.TIER.BIT.TGIEB = 1;

    // Run DD_BlankLineISR() at the end of each Data Enable section
    MTU0.TIER.BIT.TGIEA = 1;
    ICU.IPR[IPR_MTU0_TGIA0].BIT.IPR = 14;
	//ICU.IER[IPR_MTU0_TGIA0].BIT.IEN0 = 1;
    //ICU.IER[IER_MTU0_TGIA0].BIT.IEN0 = 1;
	IEN(MTU0, TGIA0) = 1;

    // Run DD_FrameISR() at start of Vertical Active period (via DOTCLK IRQ)
    ICU.IPR[IPR_MTU3_TGIC3].BIT.IPR = 14;
    MTU3.TIER.BIT.TGIEC = 1;

    //----
    // Synchronize MTU4 and MTU0 together so whenever
    // MTU0.TGRD (HPER) hits, they both reset their counters (TCNT)
    MTU.TSYR.BYTE = 0x83;

    //----
    // Start all MTU's we're using (MTU4, MTU3, MTU1, and MTU0)
    MTU.TSTR.BYTE |= 0xC3;

    // ensure first IRQ starts new frame
    DD_CurrentLine = 0;
    // Setup the total number of lines to get 60 Hz.
    // Found through testing.

    // To get the desired panel frequency, we have to insert
    // extra lines into the complete period, effectively creating
    // a larger time VSYNC period when no data is sent to the display.
    // This allows the CPU to catch up between frames and do other
    // processing while keeping a steady frame rate experience.
    // The formula is as follows:
    DD_LinesPerPeriod = (V_LINES_PER_FRAME)
            + EXTRA_LINES_NEEDED(DESIRED_FRAME_RATE);

	// Do we need to do this here?
	//IEN(MTU0, TGIB0);
	//IEN(MTU0, TGIA0);
	//IEN(MTU3, TGIC3);
	
	//ICU.IPR[IPR_MTU0_TGIB0].BIT.IPR = 14;

    //LCD_Frames_Init();

	return 0;
}

//---------------------------------------------------------------------------
// TPU6/MTU0 TGIA6/TGIA0
#pragma interrupt (DD_BlankLineISR(vect=142))
//#pragma interrupt (DD_BlankLineISR(vect=VECT_MTU0_TGIA0, enable))
void DD_BlankLineISR(void)
{
    // Have we rolled to the next frame?
    if (DD_CurrentLine >= DD_LinesPerPeriod) {
        // If we have rolled over to a new frame (going out
        // of bounds), VSYNC and VPER will be set to go LOW, and will
        // stay LOW on a match.
        MTU4.TIORL.BIT.IOC = 0x1;
        MTU4.TIORL.BIT.IOD = 0x1;

        // Reset our counter.  We are now at the top of a new
        // frame in the vertical sync/pulse area.
        DD_CurrentLine = 0;
    } else {
        // If we have not rolled over, then we might have
        // transitioned from one line to the next.
        switch (DD_CurrentLine) {
            case (V_LINES_PULSE): {
                // The Vertical Sync (VSYNC) is complete.
                // The vertical back porch will now start.
                // VSYNC and VPER will now go to HIGH, and will stay HIGH
                // on match (does not change yet) while in the the
                // vertical pulse period
                MTU4.TIORL.BIT.IOC = 0x6;
                MTU4.TIORL.BIT.IOD = 0x6;
                break;
            }
            case (V_LINES_PULSE + V_LINES_BACK_PORCH): {
                // The Vertical Back Porch is now complete.
                // Get HPER ready to drop low on a match
                // (It'll start HIGH, then go LOW)
                MTU0.TIORH.BIT.IOA = 0x5;

                // The bus is now in use (or about to be)
                DD_IsBusActive = 1;

                // Setup the first region of lines by copying it from the
                // list and calling it "active"
                DD_CurrentRegionLines = DD_RegionLines;
                DD_ActiveRegionLines = *DD_CurrentRegionLines++;

                // Shift the current height to be base don the number of
                // lines we have already processed.
                DD_ActiveRegionLines.Height += V_LINES_PULSE
                        + V_LINES_BACK_PORCH - 1;

                // Clear the interrupt for DD_PixelsLineISR()
                // Enable DD_PixelsLineISR() to be called to start the ExDMA processing.
                ICU.IR[IR_MTU0_TGIB0].BIT.IR = 0;
                //ICU.IER[IER_MTU0_TGIB0].BIT.IEN1 = 1;
				IEN(MTU0, TGIB0) = 1;

                // Enable the DD_FrameISR() interrupt
                // ??? Should this be enabled here?
                //ICU.IER[IER_MTU3_TGIC3].BIT.IEN3 = 1;
                break;
            }
            case (V_LINES_PULSE + V_LINES_BACK_PORCH + 1): {
                // Just started the data portion of the frame.

                // Is this line needed since it is turned on in DD_PixelsLineISR()?
                //                ICU.IER[IER_MTU4_TGIC4].BIT.IEN0 = 1;
                // While processing data, adjust the Horizontal Period (HPER) to have
                // the total time plus 6 clocks (Cycles from request to data if nothing else
                // on bus).
                MTU1.TGRB 
                        = (((H_DOT_PULSE + H_DOT_BACK_PORCH + (DD_FRAME_WIDTH
                                + (6 / NUM_PCLOCKS_PER_DOT) + 1) + H_DOT_FRONT_PORCH))
                                * NUM_PCLOCKS_PER_DOT) - 1;
								
				// Add 20 counts to this to allow time for a line to finish while
				// SDRAM auto-refresh is turned on -MK
                MTU0.TGRB = 20 + ((H_DOT_PULSE + H_DOT_BACK_PORCH
                        + (((DD_FRAME_WIDTH) * 1) + (6
                                / NUM_PCLOCKS_PER_DOT) + 1))
                        * NUM_PCLOCKS_PER_DOT) - 1;
                break;
            }

            case (V_LINES_PULSE + V_LINES_BACK_PORCH + DD_FRAME_HEIGHT + 1): {
                // Finished the data portion of the frame and now in the
                // Vertical Front Porch.

                // Put the timings back to standard horizontal timings
                MTU1.TGRB = H_DOTS_PER_LINE * NUM_PCLOCKS_PER_DOT - 1;
                MTU0.TGRB = ((H_DOT_PULSE + H_DOT_BACK_PORCH + DD_FRAME_WIDTH)
                        * NUM_PCLOCKS_PER_DOT) - 1;
                break;
            }
        }
    }

    // If we are processing a non-data line, increment the current line
    // and wait for the next line interrupt.
    if ((DD_CurrentLine < (V_LINES_PULSE + V_LINES_BACK_PORCH))
            || (DD_CurrentLine > (V_LINES_PULSE + V_LINES_BACK_PORCH
                    + DD_FRAME_HEIGHT)))
        DD_CurrentLine++;
}

//---------------------------------------------------------------------------
// TPU6/MTU0 TGIB6/TGIB0
#pragma interrupt (DD_PixelsLineISR(vect=143))
//#pragma interrupt (DD_PixelsLineISR(vect=VECT_MTU0_TGIB0, enable))
void DD_PixelsLineISR(void)
{
    // We have just got an interrupt to start sending out data.
    // Depending on which line we are processing changes what we do.
    if (DD_CurrentLine < (V_LINES_PULSE + V_LINES_BACK_PORCH + DD_FRAME_HEIGHT)) {
        // We are a standard line of data about to go out.
        // Determine the address of the line of pixels to send.
        // And send a single line of pixels.
        EXDMAC0.EDMSAR = (TUInt32)(DD_ActiveRegionLines.pLine);
        EXDMAC0.EDMCRB = 1;
        EXDMAC0.EDMCRA = /*(DD_FRAME_WIDTH << 16) + */DD_FRAME_WIDTH;
        EXDMAC0.EDMCNT.BIT.DTE = 1;

        // Setup for the next line to go out by moving to the next address
        DD_ActiveRegionLines.pLine += DD_ActiveRegionLines.LineNext;

        // Have we reached the last line of the frame data?
        if (DD_CurrentLine >= (V_LINES_PULSE + V_LINES_BACK_PORCH
                + DD_FRAME_HEIGHT - 1)) {
            // Yes, end of frame.  Turn back on the blank period interrupt
            // calls to DD_BlankLineISR() after making sure its interrupt flag
            // is cleared.  We'll still get one more interrupt in this
            // routine (see below)
            ICU.IR[IR_MTU0_TGIA0].BIT.IR = 0;
            //ICU.IER[IER_MTU0_TGIA0].BIT.IEN0 = 1;
			IEN(MTU0, TGIA0) = 1;
        }
    } else {
        // We have just reached the end of the frame.

        // Now keep the Data Enable line initially high and transiting to
        // high on matches.
        MTU4.TIORL.BIT.IOC = 0x6;

        // We are no longer using the bus to get data
        DD_IsBusActive = 0;

        // Turn off this interrupt (DD_PixelsLineISR) now.
        //ICU.IER[IER_MTU0_TGIB0].BIT.IEN1 = 0;
		IEN(MTU0, TGIB0) = 0;

        // Let the frame interrupt occur
        //ICU.IER[IER_MTU3_TGIC3].BIT.IEN3 = 1;
		IEN(MTU3, TGIC3) = 1;
    }

    // Next line please
    DD_CurrentLine++;
}

//---------------------------------------------------------------------------
// TPU9/MTU3 TGIC9/TGIC3
#pragma interrupt (DD_FrameISR(vect=154))
//#pragma interrupt (DD_FrameISR(vect=VECT_MTU3_TGIC3, enable))
void DD_FrameISR(void)
{
    //extern void DirectDriveHook_FrameEnd();

    // Disable this interrupt (DD_FrameISR)
    //ICU.IER[IER_MTU3_TGIC3].BIT.IEN3 = 0;
	IEN(MTU3, TGIC3) = 0;

    // One more frame counted
    DD_FrameCount++;

    // Call a hook for when the end of the screen has occurred
    //DirectDriveHook_FrameEnd();
}


//---------------------------------------------------------------------------
short LCDSetLineSource(
        short Region,
        short LineCount,
        pixel *pSource,
        short LineStep)
{
    long PixelStart;
    if (Region >= 1)
        return (-1);

    if ((pSource < (pixel *)__sectop("BLCD_Frames_2")) || (pSource
            > (pixel *)__secend("BLCD_Frames_2")))
        return (-1);

    if ((&pSource[(long)LineCount * (long)LineStep] < (pixel *)__sectop(
            "BLCD_Frames_2")) || (&pSource[(long)LineCount * (long)LineStep]
            > (pixel *)__secend("BLCD_Frames_2")))

        if (LineCount > V_LINES_PER_FRAME)
            return (-1);

    PixelStart = 0;
#if LCD_FLIP_Y
    DD_RegionLines[Region].LineNext = LineStep;
    DD_RegionLines[Region].pLine = pSource + PixelStart;
    DD_RegionLines[Region].Height = LineCount;
#else
    DD_RegionLines[Region].LineNext = -LineStep;
    DD_RegionLines[Region].pLine = pSource + PixelStart + ((LineCount-1) * LineStep);
    DD_RegionLines[Region].Height = LineCount;
#endif
    return (0);
}

/*****************************************************************************
Name: LCDSetActiveRaster()    
Parameters: frame_request - selection of which frame to have ExDMA display.                     
Returns:  Address of raster that is being used for frame.     
Description: setActiveFrame defines which frame buffer to display
*****************************************************************************/
/*
TUInt16 * LCDSetActiveRaster(TUInt32 frame_request)
{
  if (frame_request < 5) {
      //Frame = frame_request;
      LCDSetLineSource(0, V_LINES_DISPLAY, DD_Frames[frame_request].raster, H_DOT_DISPLAY);
  }
  LCDSetLineSource(0, V_LINES_DISPLAY, (void *)frame_request, H_DOT_DISPLAY);

  return 0;
}
*/