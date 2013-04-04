/*-------------------------------------------------------------------------*
 * File:  Types/GPDMA.h
 *-------------------------------------------------------------------------*
 * Description:
 *     GPDMA HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _GPDMA_TYPES_H_
#define _GPDMA_TYPES_H_

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
/**
 *  @file   /Include/Types/GPDMA.h
 *  @brief  uEZ GPDMA Types
 *
 *  The uEZ GPDMA Types
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>

/*-------------------------------------------------------------------------*
* Defines:
*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef TUInt32 T_gpdmaControl;
    #define GPDMAControl_TransferSize_Mask    0x00000FFF
    #define GPDMAControl_TransferSize_Bit     0
    #define GPDMAControl_SBSize_Mask          0x00007000
    #define GPDMAControl_SBSize_Bit           12
    #define GPDMAControl_DBSize_Mask          0x00038000
    #define GPDMAControl_DBSize_Bit           15
    #define GPDMAControl_SWidth_Mask          0x001C0000
    #define GPDMAControl_SWidth_Bit           18
    #define GPDMAControl_DWidth_Mask          0x00E00000
    #define GPDMAControl_DWidth_Bit           21
    #define GPDMAControl_SI_Mask              0x04000000
    #define GPDMAControl_SI_Bit               26
    #define GPDMAControl_DI_Mask              0x08000000
    #define GPDMAControl_DI_Bit               27
    #define GPDMAControl_I_Mask               0x80000000
    #define GPDMAControl_I_Bit                31

    /** Number of transfers total */
    #define GPDMA_TRANSFER_SIZE(size)          \
          (((size)<< GPDMAControl_TransferSize_Bit) & GPDMAControl_TransferSize_Mask)

    /** Destination transfer width (8, 16, or 32 bits) */
    #define GPDMA_DBWIDTH_8          (0 << GPDMAControl_DWidth_Bit)
    #define GPDMA_DBWIDTH_16         (1 << GPDMAControl_DWidth_Bit)
    #define GPDMA_DBWIDTH_32         (2 << GPDMAControl_DWidth_Bit)

    /** Source transfer width (8, 16, or 32 bits) */
    #define GPDMA_SBWIDTH_8          (0 << GPDMAControl_SWidth_Bit)
    #define GPDMA_SBWIDTH_16         (1 << GPDMAControl_SWidth_Bit)
    #define GPDMA_SBWIDTH_32         (2 << GPDMAControl_SWidth_Bit)

    /** Source burst size (number of transfers per burst) */
    #define GPDMA_SBSIZE_1            (1 << GPDMAControl_SBSize_Bit)
    #define GPDMA_SBSIZE_4            (2 << GPDMAControl_SBSize_Bit)
    #define GPDMA_SBSIZE_8            (3 << GPDMAControl_SBSize_Bit)
    #define GPDMA_SBSIZE_16           (4 << GPDMAControl_SBSize_Bit)
    #define GPDMA_SBSIZE_64           (5 << GPDMAControl_SBSize_Bit)
    #define GPDMA_SBSIZE_128          (6 << GPDMAControl_SBSize_Bit)
    #define GPDMA_SBSIZE_256          (7 << GPDMAControl_SBSize_Bit)

    /** Destination burst size (number of transfers per burst) */
    #define GPDMA_DBSIZE_1            (1 << GPDMAControl_DBSize_Bit)
    #define GPDMA_DBSIZE_4            (2 << GPDMAControl_DBSize_Bit)
    #define GPDMA_DBSIZE_8            (3 << GPDMAControl_DBSize_Bit)
    #define GPDMA_DBSIZE_16           (4 << GPDMAControl_DBSize_Bit)
    #define GPDMA_DBSIZE_64           (5 << GPDMAControl_DBSize_Bit)
    #define GPDMA_DBSIZE_128          (6 << GPDMAControl_DBSize_Bit)
    #define GPDMA_DBSIZE_256          (7 << GPDMAControl_DBSize_Bit)

    /** Source increments (1) or uses same location (0) per transfer */
    #define GPDMA_SI                  (1 << GPDMAControl_SI_Bit)

    /** Destination increments (1) or uses same location (0) per transfer */
    #define GPDMA_DI                  (1 << GPDMAControl_DI_Bit)

    /** End of transfer interrupt */ 
    #define GPDMA_INT_ENABLE          (GPDMAControl_I_Mask)

typedef TUInt32 T_gpdmaState;
#define GPDMA_SUCCESS             0
#define GPDMA_FAIL                1
#define GPDMA_UNKNOWN             2

typedef TUInt32 T_gpdmaPeripheral;
#define GPDMA_PERIPHERAL_MEMORY   0xFFFFFFFF
#define GPDMA_PERIPHERAL_SSP0_TX  0
#define GPDMA_PERIPHERAL_SSP0_RX  1
#define GPDMA_PERIPHERAL_SSP1_TX  2
#define GPDMA_PERIPHERAL_SSP1_RX  3
#define GPDMA_PERIPHERAL_SD_MMC   4
#define GPDMA_PERIPHERAL_I2S0     5
#define GPDMA_PERIPHERAL_I2S1     6

typedef TUInt8 T_gpdmaFlowControl;
#define GPDMA_FLOW_CONTROL_DMA                       0
#define GPDMA_FLOW_CONTROL_DESTINATION_PERIPHERAL    1
#define GPDMA_FLOW_CONTROL_SOURCE_PERIPHERAL         2

typedef struct {
    TUInt32 iSourceAddress;
    TUInt32 iDestinationAddress;
    TUInt32 iNextLinkedList; /** T_gpdmaLinkedList */
    TUInt32 iControl;
} T_gpdmaLinkedList;

typedef void (*T_gpdmaCallback)(void *aCallbackWorkspace, T_gpdmaState aState);

typedef struct {
    T_gpdmaControl iControl;
    T_gpdmaPeripheral iSourcePeripheral;
    TUInt32 iSourceAddr;
    T_gpdmaPeripheral iDestinationPeripheral;
    TUInt32 iDestinationAddr;
    T_gpdmaFlowControl iFlowControl;
    TUInt32 iNumTransferItems;
    T_gpdmaLinkedList *iLinkedList;
} T_gpdmaRequest;

#endif // _GPDMA_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/GPDMA.h
 *-------------------------------------------------------------------------*/
