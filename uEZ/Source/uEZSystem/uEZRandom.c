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
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
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
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
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
#include <Source/Library/Random/simplerandom/simplerandom.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define MODULUS         2147483647
#define MULTIPLIER      48271
#define A256            22925

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static SimpleRandomCong_t G_rng_cong;

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
#if 0
    /* Original implementation didn't seem to work */
    const TInt32 Q = MODULUS / A256;
    const TInt32 R = MODULUS % A256;
    TInt32 seed;
    TInt32 x;
#endif
    if (aStreamType == UEZ_RANDOM_PSUEDO) {
#if 0
        /* Original implementation didn't seem to work */
        seed = aPlant % MODULUS;
        x = A256 * (seed % Q) - R * (seed / Q);
        aStream->iType = aStreamType;
        aStream->iSeed = x + ((x > 0) ? 0 : MODULUS);
#else
        aStream->iSeed = simplerandom_cong_next(&G_rng_cong);
#endif
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
#if 0
    /* Original implementation didn't seem to work */
    const long Q = MODULUS / MULTIPLIER;
    const long R = MODULUS % MULTIPLIER;
    TInt32 x;
    TInt32 seed = aStream->iSeed;

    x = MULTIPLIER * (seed % Q) - R * (seed / Q);
    aStream->iSeed = x + ((x > 0) ? 0 : MODULUS);
#else
    aStream->iSeed = simplerandom_cong_next(&G_rng_cong);
#endif
    return aStream->iSeed;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZRandom.c
 *-------------------------------------------------------------------------*/
