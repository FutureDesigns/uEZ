/*-------------------------------------------------------------------------*
 * File:  uEZRandom.c
 *-------------------------------------------------------------------------*
 * Description:
 *      <DESCRIPTION>
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
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
/**
 *    @addtogroup uEZRandom
 *  @{
 *  @brief     uEZ Random Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ Random interface.
 *
 *  @par Example code:
 *  Example task to generate random numbers
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZRandom.h>
 *
 *  T_uezRandomStream randomstream;
 *  UEZRandomStreamCreate(&randomstream,UEZTickCounterGet(), UEZ_RANDOM_PSUEDO);
 *  TInt32 randomNumber = UEZRandomSigned32Bit(&randomstream);
 *  @endcode
 */
#include <uEZ.h>
#include <uEZRandom.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define MODULUS         2147483647
#define MULTIPLIER      48271
#define A256            22925

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Routine:  UEZRandomStreamCreate
 *---------------------------------------------------------------------------*/
/**
 *  Start a new random number stream with the given seed value using
 *      the given algorithm.
 *
 *  @param [in]    *aStream 	Stream to initialize
 *
 *  @param [in]    aPlant		Plant value to start random number. Recommend
 *	              				passing in UEZTickCounterGet() or some other 
 *								source.
 *  @param [in]    aStreamType 	algorithm/source to use for generating random
 * 								numbers.
 *
 *  @return        T_uezError   Error code. Returns UEZ_ERROR_NOT_SUPPORTED if
 *  							algorithm is unknown or unavailable.  Returns 
 *                              UEZ_ERROR_NONE on success.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZRandom.h>
 *
 *  T_uezRandomStream randomstream;
 *  UEZRandomStreamCreate(&randomstream,UEZTickCounterGet(), UEZ_RANDOM_PSUEDO);
 *  TInt32 randomNumber = UEZRandomSigned32Bit(&randomstream);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZRandomStreamCreate(
    T_uezRandomStream *aStream,
    TInt32 aPlant,
    T_uezRandomStreamType aStreamType)
{
    const TInt32 Q = MODULUS / A256;
    const TInt32 R = MODULUS % A256;
    TInt32 seed;
    TInt32 x;

    if (aStreamType == UEZ_RANDOM_PSUEDO) {
        seed = aPlant % MODULUS;
        x = A256 * (seed % Q) - R * (seed / Q);
        aStream->iType = aStreamType;
        aStream->iSeed = x + ((x > 0) ? 0 : MODULUS);
    } else {
        return UEZ_ERROR_NOT_SUPPORTED;
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZRandomSigned32Bit
 *---------------------------------------------------------------------------*/
/**
 *  Generates a random signed 32-bit number from the given stream.
 *
 *  @param [in]    *aStream 	Stream to get random number from
 *
 *  @return        TInt32 		Signed random number generated.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZRandom.h>
 *
 *  T_uezRandomStream randomstream;
 *  UEZRandomStreamCreate(&randomstream,UEZTickCounterGet(), UEZ_RANDOM_PSUEDO);
 *  TInt32 randomNumber = UEZRandomSigned32Bit(&randomstream);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TInt32 UEZRandomSigned32Bit(T_uezRandomStream *aStream)
{
    const long Q = MODULUS / MULTIPLIER;
    const long R = MODULUS % MULTIPLIER;
    TInt32 x;
    TInt32 seed = aStream->iSeed;

    x = MULTIPLIER * (seed % Q) - R * (seed / Q);
    aStream->iSeed = x + ((x > 0) ? 0 : MODULUS);

    return aStream->iSeed;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZRandom.c
 *-------------------------------------------------------------------------*/
