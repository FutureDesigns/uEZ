#ifndef _DIRECT_LCD_TYPES_H_
#define _DIRECT_LCD_TYPES_H_

#ifdef MEM_DECL
#define EXTERN
#else
#define EXTERN extern
#endif

/* Typedef fundmental data types that may change between compilers/families */
typedef unsigned char uI08;
typedef unsigned short uI16;
typedef unsigned long uI32;
typedef signed char sI08;
typedef signed short sI16;
typedef signed long sI32;
typedef uI08 boolean;

#endif // _DIRECT_LCD_TYPES_H_
