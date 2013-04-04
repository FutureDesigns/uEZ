/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Regions.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Code for splitting and cutting regions of the screen.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
#ifndef _SIMPLEUI_REGIONS_H_
#define _SIMPLEUI_REGIONS_H_

#include <uEZ.h>

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

#endif // _SIMPLEUI_REGIONS_H_
/*-------------------------------------------------------------------------*
 * End of File:  SimpleUI_Regions.h
 *-------------------------------------------------------------------------*/
