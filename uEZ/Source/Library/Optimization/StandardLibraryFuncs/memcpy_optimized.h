#ifndef MEMCPY_OPTIMIZED_H_
#define MEMCPY_OPTIMIZED_H_

//#include <_ansi.h>
//#include <newlib.h>
//#include <sys/config.h>
//#include <string.h>

// Get everything from CMSIS_5 (or newer) DSP Include folder to have matching versions.
#include <arm_math.h> // will include arm_math_types.h and arm_mve.h automatically

// Note that almost no software uses the return value of memcpy, so use the versions without it for a perf boost.

/********************************************************************
 * No return value, allow unaligned
 ********************************************************************/
// the multipurpose function from newlib performance build
void memcpy_4lw_unaligned_no_ret (void *__RESTRICT dst0,
                            const void *__RESTRICT src0, size_t len0);

#if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE // CM55/CM85 only
// TODO
#endif

 /********************************************************************
 * No return value, do not allow unaligned
 ********************************************************************/

#if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE // CM55/CM85 only
void memcpy_4x_8x16_only_no_ret (uint8x16_t *__RESTRICT aligned_dst,
                           const uint8x16_t *__RESTRICT aligned_src, size_t len0);
#endif

// optimal unless MVE is available or you can get hand assembly for non-helium

void memcpy_4x_ull_only_no_ret (unsigned long long *__RESTRICT aligned_dst,
                          const unsigned long long *__RESTRICT aligned_src, size_t len0);

void memcpy_4x_longlongword_only_no_ret (long long *__RESTRICT aligned_dst,
                                   const long long *__RESTRICT aligned_src,  size_t len0);

void memcpy_4x_longword_only_no_ret (long *__RESTRICT aligned_dst,
                              const long *__RESTRICT aligned_src, size_t len0);

 /********************************************************************
 * No return value, do not allow unaligned, combination sizes
 ********************************************************************/

void memcpy_4x_longword_no_ret (void *__RESTRICT dst0,
                          const void *__RESTRICT src0, size_t len0);


#endif
