/*-------------------------------------------------------------------------*
 * File:  Flash_S29GL064N90_16Bit_driver.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Flash for Spansion S29GL064N90
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    volatile TUInt16 *iBaseAddr;
    TUInt32 iNumOpen;
    TBool iIsProgramMode;
    TBool iIsChipInfoReady;
    T_FlashChipInfo iChipInfo;
} T_Flash_S29GL064N90_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
//void Flash_S29GL064N90_16Bit_Configure(
//        void *aWorkspace,
//        void *aBaseAddr);
T_uezError Flash_S29GL064N90_16Bit_GetBlockInfo(
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aOffset,
        T_FlashBlockInfo *aBlockInfo);
T_uezError Flash_S29GL064N90_16Bit_GetChipInfo(
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        T_FlashChipInfo *aInfo);
T_uezError Flash_S29GL064N90_16Bit_QueryReg(
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aReg,
        TUInt32 *aValue);
T_uezError Flash_S29GL064N90_16Bit_ChipErase(T_Flash_S29GL064N90_Workspace *aWorkspace);
T_uezError Flash_S29GL064N90_16Bit_BlockErase(
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aOffset,
        TUInt32 aNumBytes);
T_uezError Flash_S29GL064N90_16Bit_Write(
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes);
T_uezError Flash_S29GL064N90_16Bit_Read(
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes);
T_uezError Flash_S29GL064N90_16Bit_Open(T_Flash_S29GL064N90_Workspace *aWorkspace);
T_uezError Flash_S29GL064N90_16Bit_Close(T_Flash_S29GL064N90_Workspace *aWorkspace);
T_uezError Flash_S29GL064N90_16Bit_InitializeWorkspace(T_Flash_S29GL064N90_Workspace *aW);

/*-------------------------------------------------------------------------*
 * End of File:  Flash_S29GL064N90_16Bit_driver.h
 *-------------------------------------------------------------------------*/
