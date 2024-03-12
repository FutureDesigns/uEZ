/*-------------------------------------------------------------------------*
* File:  uEZPacked.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/
#ifndef UEZPACKED_H_
#define UEZPACKED_H_

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
/**
 *  @file   uEZPacked.h
 *  @brief  uEZ Packed
 *
 *  uEZ Packed
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

#if (COMPILER_TYPE==GCC_ARM)
#define PACK_STRUCT_BEGIN
#define ATTR_IAR_PACKED	 
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#endif
  
#if (COMPILER_TYPE==IAR)
#define PACK_STRUCT_USE_INCLUDES 1
#define ATTR_IAR_PACKED	   __packed // IAR wants "typedef __packed struct"
#define PACK_STRUCT_BEGIN //__packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#endif

#if (COMPILER_TYPE==KEIL_UV)
#define PACK_STRUCT_BEGIN
#define ATTR_IAR_PACKED	 
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#endif


#if (COMPILER_TYPE==RENESASRX)
#define PACK_STRUCT_USE_INCLUDES 1
#define PACK_STRUCT_BEGIN
#define ATTR_IAR_PACKED	 
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#endif

#if (COMPILER_TYPE==VisualC)
/** All fields are packed by default with /Zp1 compile option */
#define PACK_STRUCT_BEGIN
#define ATTR_IAR_PACKED	 
#define PACK_STRUCT_STRUCT 
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#endif


/**
 * Example PACK_STRUCT usage to define packed MyRectangle:
 *
 *   PACK_STRUCT_BEGIN
 *   typedef ATTR_IAR_PACKED struct
 *   {
 *        PACK_STRUCT_FIELD(signed short xMin);
 *        PACK_STRUCT_FIELD(signed short yMin);
 *        PACK_STRUCT_FIELD(signed short xMax);
 *        PACK_STRUCT_FIELD(signed short yMax);
 *   } PACK_STRUCT_STRUCT MyRectangle;
 *   PACK_STRUCT_END
 */

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // UEZPACKED_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZPacked.h
 *-------------------------------------------------------------------------*/
