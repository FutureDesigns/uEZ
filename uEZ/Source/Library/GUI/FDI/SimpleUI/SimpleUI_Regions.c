/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Choices.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Core code for handling the selection of choices (aka buttons) on
 *      the screen.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include "SimpleUI.h"

/*---------------------------------------------------------------------------*
 * Routine:  RegionShrinkTopBottom
 *---------------------------------------------------------------------------*
 * Description:
 *      Shrink a region by a number of pixels from the top and the bottom
 * Inputs:
 *      T_region *aRegion           -- Region to modify
 *      TUInt16 aNumPixels          -- Number of pixels to shrink
 *---------------------------------------------------------------------------*/
void RegionShrinkTopBottom(T_region *aRegion, TUInt16 aNumPixels)
{
    aRegion->iTop += aNumPixels;
    if (aRegion->iBottom >= aNumPixels)
        aRegion->iBottom -= aNumPixels;
    if (aRegion->iBottom < aRegion->iTop)
        aRegion->iBottom = aRegion->iTop;
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionShrinkLeftRight
 *---------------------------------------------------------------------------*
 * Description:
 *      Shrink a region by a number of pixels from the left and right.
 * Inputs:
 *      T_region *aRegion           -- Region to modify
 *      TUInt16 aNumPixels          -- Number of pixels to shrink
 *---------------------------------------------------------------------------*/
void RegionShrinkLeftRight(T_region *aRegion, TUInt16 aNumPixels)
{
    aRegion->iLeft += aNumPixels;
    if (aRegion->iRight >= aNumPixels)
        aRegion->iRight -= aNumPixels;
    if (aRegion->iRight < aRegion->iLeft)
        aRegion->iRight = aRegion->iLeft;
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionShrink
 *---------------------------------------------------------------------------*
 * Description:
 *      Shrink a region by a number of pixels from all 4 edges.
 * Inputs:
 *      T_region *aRegion           -- Region to modify
 *      TUInt16 aNumPixels          -- Number of pixels to shrink
 *---------------------------------------------------------------------------*/
void RegionShrink(T_region *aRegion, TUInt16 aNumPixels)
{
    RegionShrinkLeftRight(aRegion, aNumPixels);
    RegionShrinkTopBottom(aRegion, aNumPixels);
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionSplitFromTop
 *---------------------------------------------------------------------------*
 * Description:
 *      Shrink a region from the top and (optinally) create a new region
 *      out of that top.  A padding value tells how much to leave in
 *      between the two regions.
 * Inputs:
 *      T_region *aRegion           -- Region to modify
 *      T_region *aNewRegion        -- Region of new top area, 0 to ignore
 *      TUInt16 aNumPixels          -- Number of pixels off top
 *      TUInt16 aNumPadding         -- Number of additional pixels to
 *                                     strip from the top.
 *---------------------------------------------------------------------------*/
void RegionSplitFromTop(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aNumPixels, 
        TUInt16 aNumPadding)
{
    if (aNewRegion) {
        aNewRegion->iLeft = aRegion->iLeft;
        aNewRegion->iRight = aRegion->iRight;
        aNewRegion->iTop = aRegion->iTop;
        aNewRegion->iBottom = aRegion->iTop+aNumPixels;
        if (aNewRegion->iBottom > aRegion->iBottom)
            aNewRegion->iBottom = aRegion->iBottom;
    }
    aRegion->iTop += aNumPixels;
    aRegion->iTop += aNumPadding;
    if (aRegion->iTop > aRegion->iBottom)
        aRegion->iTop = aRegion->iBottom;
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionSplitFromBottom
 *---------------------------------------------------------------------------*
 * Description:
 *      Shrink a region from the bottom and (optinally) create a new region
 *      out of that bottom.  A padding value tells how much to leave in
 *      between the two regions.
 * Inputs:
 *      T_region *aRegion           -- Region to modify
 *      T_region *aNewRegion        -- Region of new bottom area, 0 to ignore
 *      TUInt16 aNumPixels          -- Number of pixels off bottom
 *      TUInt16 aNumPadding         -- Number of additional pixels to
 *                                     strip from the bottom.
 *---------------------------------------------------------------------------*/
void RegionSplitFromBottom(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aNumPixels, 
        TUInt16 aNumPadding)
{
    if (aNewRegion)  {
        aNewRegion->iLeft = aRegion->iLeft;
        aNewRegion->iRight = aRegion->iRight;
        aNewRegion->iBottom = aRegion->iBottom;
        aNewRegion->iTop = aRegion->iBottom-aNumPixels;
        if (aNewRegion->iTop < aRegion->iTop)
            aNewRegion->iTop = aRegion->iTop;
    }
    aRegion->iBottom -= aNumPixels;
    aRegion->iBottom -= aNumPadding;
    if (aRegion->iBottom < aRegion->iTop)
        aRegion->iBottom = aRegion->iTop;
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionSplitFromLeft
 *---------------------------------------------------------------------------*
 * Description:
 *      Shrink a region from the left and (optinally) create a new region
 *      out of that left.  A padding value tells how much to leave in
 *      between the two regions.
 * Inputs:
 *      T_region *aRegion           -- Region to modify
 *      T_region *aNewRegion        -- Region of new left area, 0 to ignore
 *      TUInt16 aNumPixels          -- Number of pixels off left
 *      TUInt16 aNumPadding         -- Number of additional pixels to
 *                                     strip from the left.
 *---------------------------------------------------------------------------*/
void RegionSplitFromLeft(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aNumPixels, 
        TUInt16 aNumPadding)
{
    if (aNewRegion)  {
          aNewRegion->iLeft = aRegion->iLeft;
          aNewRegion->iRight = aRegion->iLeft+aNumPixels;
          aNewRegion->iTop = aRegion->iTop;
          aNewRegion->iBottom = aRegion->iBottom;
          if (aNewRegion->iRight > aRegion->iRight)
              aNewRegion->iRight = aRegion->iRight;
    }
    aRegion->iLeft += aNumPixels;
    aRegion->iLeft += aNumPadding;
    if (aRegion->iLeft > aRegion->iRight)
        aRegion->iLeft = aRegion->iRight;
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionSplitFromRight
 *---------------------------------------------------------------------------*
 * Description:
 *      Shrink a region from the right and (optinally) create a new region
 *      out of that right.  A padding value tells how much to leave in
 *      between the two regions.
 * Inputs:
 *      T_region *aRegion           -- Region to modify
 *      T_region *aNewRegion        -- Region of new right area, 0 to ignore
 *      TUInt16 aNumPixels          -- Number of pixels off right
 *      TUInt16 aNumPadding         -- Number of additional pixels to
 *                                     strip from the right.
 *---------------------------------------------------------------------------*/
void RegionSplitFromRight(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aNumPixels, 
        TUInt16 aNumPadding)
{
    if (aNewRegion) {
        aNewRegion->iLeft = aRegion->iRight-aNumPixels;
        aNewRegion->iRight = aRegion->iRight;
        aNewRegion->iTop = aRegion->iTop;
        aNewRegion->iBottom = aRegion->iBottom;
        if (aNewRegion->iLeft < aRegion->iLeft)
            aNewRegion->iLeft = aRegion->iLeft;
    }
    aRegion->iRight -= aNumPixels;
    aRegion->iRight -= aNumPadding;
    if (aRegion->iRight < aRegion->iLeft)
        aRegion->iRight = aRegion->iLeft;
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionCenterTopBottom
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a new region in the middle of the given region centered
 *      top-bottom based on a given pixel height.
 * Inputs:
 *      T_region *aRegion           -- Region to center within
 *      T_region *aNewRegion        -- Newly created centered region
 *      TUInt16 aInnerHeight        -- Pixel height of region to be created
 *---------------------------------------------------------------------------*/
void RegionCenterTopBottom(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aInnerHeight)
{
    TUInt16 height;

    aNewRegion->iLeft = aRegion->iLeft;
    aNewRegion->iRight = aRegion->iRight;

    height = 1+aRegion->iBottom-aRegion->iTop;
    if (aInnerHeight >= height) {
        // No centering, just use parent
        aNewRegion->iTop = aRegion->iTop;
        aNewRegion->iBottom = aRegion->iBottom;
    } else {
        // Center by half
        aNewRegion->iTop = aRegion->iTop + 
              ((height-aInnerHeight)>>1);
        aNewRegion->iBottom = aNewRegion->iTop+aInnerHeight-1;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionCenterLeftRight
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a new region in the middle of the given region centered
 *      left-right based on a given pixel width.
 * Inputs:
 *      T_region *aRegion           -- Region to center within
 *      T_region *aNewRegion        -- Newly created centered region
 *      TUInt16 aInnerWidth         -- Pixel width of region to be created
 *---------------------------------------------------------------------------*/
void RegionCenterLeftRight(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aInnerWidth)
{
    TUInt16 width;

    aNewRegion->iTop = aRegion->iTop;
    aNewRegion->iBottom = aRegion->iBottom;

    width = 1+aRegion->iRight-aRegion->iLeft;
    if (aInnerWidth >= width) {
        // No centering, just use parent
        aNewRegion->iLeft = aRegion->iLeft;
        aNewRegion->iRight = aRegion->iRight;
    } else {
        // Center by half
        aNewRegion->iLeft = aRegion->iLeft + 
              ((width-aInnerWidth)>>1);
        aNewRegion->iRight = aNewRegion->iLeft+aInnerWidth-1;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  RegionCenter
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a new region in the middle of the given region centered
 *      left-right-top-bottom based on a given pixel height and width.
 * Inputs:
 *      T_region *aRegion           -- Region to center within
 *      T_region *aNewRegion        -- Newly created centered region
 *      TUInt16 aInnerHeight        -- Pixel width of region to be created
 *      TUInt16 aInnerWidth         -- Pixel height of region to be created
 *---------------------------------------------------------------------------*/
void RegionCenter(
        T_region *aRegion, 
        T_region *aNewRegion, 
        TUInt16 aInnerHeight, 
        TUInt16 aInnerWidth)
{
    T_region r;
    
    RegionCenterTopBottom(aRegion, &r, aInnerHeight);
    RegionCenterLeftRight(&r, aNewRegion, aInnerWidth);
}

/*-------------------------------------------------------------------------*
 * End of File:  SimpleUI_Choices.c
 *-------------------------------------------------------------------------*/
