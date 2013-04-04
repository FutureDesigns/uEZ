/*-------------------------------------------------------------------------*
 * File:  uEZRandom.h
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
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZErrors.h>

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

#endif // UEZRANDOM_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZRandom.h
 *-------------------------------------------------------------------------*/
