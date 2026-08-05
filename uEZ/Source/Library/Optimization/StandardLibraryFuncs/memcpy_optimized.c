
// newlib 4.5 based functions

#include "memcpy_optimized.h"
//#include <inttypes.h>
#include "uEZMemory.h" // get the placement macros

#if (defined __GNUC__)
  // should check this both on and off to see what we get, GCC 14 CM4 build didn't change for the combination function.
  #if defined(__clang__)
    #define __inhibit_loop_to_libcall \
    __attribute__ ((__optimize__ ("-fno-tree-loop-distribute-patterns")))
  #else
    #define __inhibit_loop_to_libcall \
    __attribute__ ((__optimize__ ("-fno-tree-loop-distribute-patterns")))
  #endif
#elif (defined __ICCARM__) || (defined __ICCRX__) // IAR can't place flags in optimize, and can't use __opti... in non-gcc mode
    #define __inhibit_loop_to_libcall
#else
  #error "Flag not setup for this compiler."
#endif

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED(X, Y) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))

#define UNALIGNED_LL(X, Y) \
  (((long long)X & (sizeof (long long) - 1)) | ((long long)Y & (sizeof (long long) - 1)))

#define UNALIGNED_128(X, Y) \
  (((uint8x16_t)X & (sizeof (uint8x16_t) - 1)) | ((uint8x16_t)Y & (sizeof (uint8x16_t) - 1)))

/* How many bytes are copied each iteration of the word copy loop.  */
#define LITTLEBLOCKSIZE (sizeof (long))

/* How many bytes are copied each iteration of the 4X unrolled loop.  */
#define BIGBLOCKSIZE    (sizeof (long) << 2) // 16

/* How many bytes are copied each iteration of the 8X unrolled loop.  */
#define BLOCKSIZE_DW    (sizeof (long long) << 2) // 32
#define BLOCKSIZE_ULL    (sizeof (unsigned long long) << 2) // 32 bytes total, (64 bit type each)
#define BLOCKSIZE_128    (sizeof (uint8x16_t) << 2) // 64

/* Threshhold for punting to the byte copier.  */
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)
#define TOO_SMALL_LL(LEN)  ((LEN) < BLOCKSIZE_DW)
#define TOO_SMALL_128(LEN)  ((LEN) < BLOCKSIZE_128)

/********************************************************************
 * No return value, allow unaligned
 ********************************************************************/

 // multiple size unaligned
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_HIGH, void
__inhibit_loop_to_libcall
memcpy_4lw_unaligned_no_ret (void *__RESTRICT dst0,
                       const void *__RESTRICT src0, size_t len0)
{
    char *dst = dst0;
    const char *src = src0;
    long *aligned_dst;
    const long *aligned_src;

    /* If the size is small, or either SRC or DST is unaligned,
       then punt into the byte copy loop.  This should be rare.  */
    if (!TOO_SMALL(len0) && !UNALIGNED (src, dst))
      {
        aligned_dst = (long*)dst;
        aligned_src = (long*)src;

        /* Copy 4X long words at a time if possible.  */
        while (len0 >= BIGBLOCKSIZE)
          {
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            len0 -= BIGBLOCKSIZE;
          }

        /* Copy one long word at a time if possible.  */
        while (len0 >= LITTLEBLOCKSIZE)
          {
            *aligned_dst++ = *aligned_src++;
            len0 -= LITTLEBLOCKSIZE;
          }

         /* Pick up any residual with a byte copier.  */
        dst = (char*)aligned_dst;
        src = (char*)aligned_src;
      }

    while (len0--)
      *dst++ = *src++;
})

#if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE // CM55/CM85 only
// TODO
#endif

 /********************************************************************
 * No return value, do not allow unaligned, single size only
 ********************************************************************/
// In size order largest to smallest

#if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE // CM55/CM85 only

// must be aligned, mve 128 bit version that generates vldrb,u8, vstrb.8 instructions
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_HIGH, void
__inhibit_loop_to_libcall
memcpy_4x_8x16_only_no_ret (uint8x16_t *__RESTRICT aligned_dst,
                      const uint8x16_t *__RESTRICT aligned_src, size_t len0)
{
    // Copy 4X 8x16 words at a time if possible.
    while (len0 >= BLOCKSIZE_128)
      {
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        len0 -= BLOCKSIZE_128;
      }
})

#endif

// must be aligned, unsigned long long only
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_HIGH, void
__inhibit_loop_to_libcall
memcpy_4x_ull_only_no_ret (unsigned long long *__RESTRICT aligned_dst,
                     const unsigned long long *__RESTRICT aligned_src, size_t len0)
{
    // Copy 4X ULL words at a time if possible.
    while (len0 >= BLOCKSIZE_ULL)
      {
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        len0 -= BLOCKSIZE_ULL;
      }
})

// must be aligned, long long only
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_HIGH, void
__inhibit_loop_to_libcall
memcpy_4x_longlongword_only_no_ret (long long *__RESTRICT aligned_dst,
                              const long long *__RESTRICT aligned_src, size_t len0)
{
    // Copy 4X long long words at a time if possible.
    while (len0 >= BLOCKSIZE_DW)
      {
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        len0 -= BLOCKSIZE_DW;
      }
})

// must be aligned, long only
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_HIGH, void
__inhibit_loop_to_libcall
memcpy_4x_longword_only_no_ret (long *__RESTRICT aligned_dst,
                          const long *__RESTRICT aligned_src, size_t len0)
{
    // Copy 4X long words at a time if possible.
    while (len0 >= BIGBLOCKSIZE)
      {
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        len0 -= BIGBLOCKSIZE;
      }
})

 /********************************************************************
 * No return value, do not allow unaligned, combination sizes
 ********************************************************************/
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_HIGH, void
__inhibit_loop_to_libcall
memcpy_4x_longword_no_ret (void *__RESTRICT dst0,
                     const void *__RESTRICT src0, size_t len0)
{
    char *dst = dst0;
    const char *src = src0;
    long *aligned_dst;
    const long *aligned_src;

    aligned_dst = (long*)dst;
    aligned_src = (long*)src;

    /* Copy 4X long words at a time if possible.  */
    while (len0 >= BIGBLOCKSIZE)
      {
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        *aligned_dst++ = *aligned_src++;
        len0 -= BIGBLOCKSIZE;
      }

    /* Copy one long word at a time if possible.  */
    while (len0 >= LITTLEBLOCKSIZE)
      {
        *aligned_dst++ = *aligned_src++;
        len0 -= LITTLEBLOCKSIZE;
      }

     /* Pick up any residual with a byte copier.  */
    dst = (char*)aligned_dst;
    src = (char*)aligned_src;
})
