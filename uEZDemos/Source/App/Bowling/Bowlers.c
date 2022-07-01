/*-------------------------------------------------------------------------*
 * File:  Bowlers.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Bowlers.h"

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_Bowler G_Bowlers[MAX_NUM_BOWLERS];
static T_uezSemaphore G_DataSem = 0;

static void IGrab()
{
    UEZSemaphoreGrab(G_DataSem, UEZ_TIMEOUT_INFINITE);
}

static void IRelease()
{
    UEZSemaphoreRelease(G_DataSem);
}

void GetBowlers(T_Bowler *aBowlerList)
{
    IGrab();
    memcpy((void*)aBowlerList, (void*)&G_Bowlers, sizeof(T_Bowler) * MAX_NUM_BOWLERS);
    IRelease();
}

void AddBowler(T_Bowler *aBowler, TUInt8 aPos)
{
    if(aPos > MAX_NUM_BOWLERS){
        return;
    }
    IGrab();
    memcpy((void*)&G_Bowlers[aPos], (void*)aBowler, sizeof(T_Bowler));
    IRelease();
}

void QuickStartBowlers(TUInt8 aNumBowlers)
{
    TUInt8 i = 0;
    T_Bowler bowler;

    if (aNumBowlers == 0){
        return;
    }

    for ( i = 0; i < aNumBowlers; i ++){
        sprintf(bowler.iName, "Bowler %d", i+1);
        bowler.iBumpers = EFalse;
        bowler.iScore = 0;
        AddBowler(&bowler, i);
    }
}

void ClearBowlers()
{
    IGrab();
    memset((void*)&G_Bowlers, 0, sizeof(T_Bowler) * MAX_NUM_BOWLERS);
    IRelease();
}

void DeleteBowler(TUInt8 aPos)
{
    TUInt8 i;
    //Remove the bowler
    memset((void*)&G_Bowlers[aPos], 0, sizeof(T_Bowler));

    IGrab();
    for ( i = aPos; i < MAX_NUM_BOWLERS - 1; i ++){
        //Move bowler below up the list
        memcpy((void*)&G_Bowlers[i], (void*)&G_Bowlers[i+1], sizeof(T_Bowler));
    }
    if ( i < MAX_NUM_BOWLERS){
        memset((void*)&G_Bowlers[i], 0, sizeof(T_Bowler));
    }
    IRelease();
}

void SetBowlerScore(TUInt8 aPos, TUInt32 aScore)
{

    IGrab();
    G_Bowlers[aPos].iScore = aScore;
    IRelease();
}

void BowlersInit()
{
    if( G_DataSem == 0){
        UEZSemaphoreCreateBinary(&G_DataSem);
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  Bowlers.c
 *-------------------------------------------------------------------------*/
