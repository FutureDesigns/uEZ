/*-------------------------------------------------------------------------*
 * File:  Config_Processor.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Processor configuration
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#ifndef _CONFIG_PROCESSOR_H_
#define _CONFIG_PROCESSOR_H_

#define UEZ_PROCESSOR_CORE_TYPE                 CORE_TYPE_ARM7
#define UEZ_PROCESSOR_CORE_MODE                 CORE_MODE_ARM
#define UEZ_PROCESSOR_CORE_SUBTYPE              CORE_SUBTYPE_ARM7TDMI_S

// External processor speed
#ifndef OSCILLATOR_CLOCK_FREQUENCY
#define OSCILLATOR_CLOCK_FREQUENCY          12000000    // Hz
#endif

#ifndef PROCESSOR_OSCILLATOR_FREQUENCY
#define PROCESSOR_OSCILLATOR_FREQUENCY      72000000    // Hz
#endif

// Option to put the name of the interrupts in the interrupt
// table for internal debugging
#ifndef LPC2478_INTERRUPT_TRACK_NAMES
#define LPC2478_INTERRUPT_TRACK_NAMES       0
#endif



#endif // _CONFIG_PROCESSOR_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_Processor.h
 *-------------------------------------------------------------------------*/
