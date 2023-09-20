/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Regions.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Code for splitting and cutting regions of the screen.
 *-------------------------------------------------------------------------*/
#ifndef _SIMPLEUI_REGIONS_H_
#define _SIMPLEUI_REGIONS_H_

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
#include <uEZ.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TUInt16 iLeft;
    TUInt16 iTop;
    TUInt16 iRight;
    TUInt16 iBottom;
} T_region;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void RegionShrink(
        T_region *aRegion, 
        TUInt16 aNumPixels);
void RegionShrinkTopBottom(
        T_region *aRegion, 
        TUInt16 aNumPixels);
void RegionShrinkLeftRight(
        T_region *aRegion, 
        TUInt16 aNumPixels);
void RegionSplitFromTop(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aNumPixels, 
        TUInt16 aNumPadding);
void RegionSplitFromLeft(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aNumPixels, 
        TUInt16 aNumPadding);
void RegionSplitFromBottom(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aNumPixels, 
        TUInt16 aNumPadding);
void RegionSplitFromRight(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aNumPixels, 
        TUInt16 aNumPadding);
void RegionCenterTopBottom(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aInnerHeight);
void RegionCenterLeftRight(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aInnerWidth);
void RegionCenter(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aInnerHeight, 
        TUInt16 aInnerWidth);

#ifdef __cplusplus
}
#endif

#endif // _SIMPLEUI_REGIONS_H_
/*-------------------------------------------------------------------------*
 * End of File:  SimpleUI_Regions.h
 *-------------------------------------------------------------------------*/
