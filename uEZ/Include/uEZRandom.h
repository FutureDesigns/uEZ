/*-------------------------------------------------------------------------*
* File:  uEZRandom.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ Interface to uEZ random number generators.
*-------------------------------------------------------------------------*/
/**
 *  @file   uEZRandom.h
 *  @brief  uEZ Random Number Generator
 *
 *  The uEZ Interface to uEZ random number generators.
 */
#ifndef UEZRANDOM_H_
#define UEZRANDOM_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZErrors.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
/**
 * Types of random number generators and their source.
 *  
 */
typedef enum {
    /** DEVICE_Random of name "Random0" used (not implemented yet) */
    UEZ_RANDOM_DEVICE, /**< enum value UEZ_RANDOM_DEVICE. */ 

    /** Lehmer Psuedo random number (0-0x7FFFFFFF values) */
    UEZ_RANDOM_PSUEDO,
} T_uezRandomStreamType;

/**
 * Random stream structure that tracks the current state of a random 
 * number stream.
 *  
 */
typedef struct {
    TInt32 iSeed;
    T_uezRandomStreamType iType;
} T_uezRandomStream;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/**
 *  Start a new random number stream with the given seed value using
 *      the given algorithm.
 *
 *  @param [in]     *aStream    Stream to initialize
 *  @param [in]     aPlant      Plant value to start random number.  Recommend
 *                              passing in UEZTickCounterGet() or some other
 *                              source.
 *  @param [in]     aStreamType Algorithm/source to use for generating random
 *                              numbers.
 *
 *  @return     T_uezError      Returns UEZ_ERROR_NOT_SUPPORTED if algorithm
 *                              is unknown or unavailable.  Returns UEZ_ERROR_NONE
 *                              on success.
 */
T_uezError UEZRandomStreamCreate(
    T_uezRandomStream *aStream,
    TInt32 aPlant,
    T_uezRandomStreamType aStreamType);

/**
 *  Generates a random signed 32-bit number from the given stream.
 *
 *  @param [in] *aStream        Stream to get random number from
 *
 *  @return     TInt32          Signed random number generated.
 */
TInt32 UEZRandomSigned32Bit(T_uezRandomStream *aStream);

#ifdef __cplusplus
}
#endif

#endif // UEZRANDOM_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZRandom.h
 *-------------------------------------------------------------------------*/
