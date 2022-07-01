// Private stuff goes here
#define DEBUG_VNC_CODING            0
#if DEBUG_VNC_CODING
    #define dprintf printf
#else
    #define dprintf(...)
#endif

/*
 * Macros for endian swapping.
 */

#define Swap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))

#define Swap24(l) ((((l) & 0xff) << 16) | (((l) >> 16) & 0xff) | \
                   (((l) & 0x00ff00)))

#define Swap32(l) (((l) >> 24) | \
                   (((l) & 0x00ff0000) >> 8)  | \
                   (((l) & 0x0000ff00) << 8)  | \
                   ((l) << 24))

#if UEZVNC_BIG_ENDIAN
    #define Swap16IfLE(s) (s)
    #define Swap24IfLE(l) (l)
    #define Swap32IfLE(l) (l)
#else
    #define Swap16IfLE(s) Swap16(s)
    #define Swap24IfLE(l) Swap24(l)
    #define Swap32IfLE(l) Swap32(l)
#endif

