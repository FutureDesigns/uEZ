// Copyright (c) 2008 Rowley Associates Limited.
//
// This file may be distributed under the terms of the License Agreement
// provided with this software.
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#ifndef LPC2478_h
#define LPC2478_h

#include <Config.h>
#include <Types/GPIO.h>

#define USB_HOST_BASE 0xFFE0C000

#define HcRevision (*(volatile unsigned *)0xFFE0C000)
#define HcRevision_OFFSET 0x0
#define HcRevision_REV_MASK 0xFF
#define HcRevision_REV_BIT 0

#define HcControl (*(volatile unsigned *)0xFFE0C004)
#define HcControl_OFFSET 0x4
#define HcControl_CBSR_MASK 0x3
#define HcControl_CBSR_BIT 0
#define HcControl_PLE_MASK 0x4
#define HcControl_PLE 0x4
#define HcControl_PLE_BIT 2
#define HcControl_IE_MASK 0x8
#define HcControl_IE 0x8
#define HcControl_IE_BIT 3
#define HcControl_CLE_MASK 0x10
#define HcControl_CLE 0x10
#define HcControl_CLE_BIT 4
#define HcControl_BLE_MASK 0x20
#define HcControl_BLE 0x20
#define HcControl_BLE_BIT 5
#define HcControl_HCFS_MASK 0xC0
#define HcControl_HCFS_BIT 6
#define HcControl_IR_MASK 0x100
#define HcControl_IR 0x100
#define HcControl_IR_BIT 8
#define HcControl_RWC_MASK 0x200
#define HcControl_RWC 0x200
#define HcControl_RWC_BIT 9
#define HcControl_RWE_MASK 0x400
#define HcControl_RWE 0x400
#define HcControl_RWE_BIT 10

#define HcCommandStatus (*(volatile unsigned *)0xFFE0C008)
#define HcCommandStatus_OFFSET 0x8
#define HcCommandStatus_HCR_MASK 0x1
#define HcCommandStatus_HCR 0x1
#define HcCommandStatus_HCR_BIT 0
#define HcCommandStatus_CLR_MASK 0x2
#define HcCommandStatus_CLR 0x2
#define HcCommandStatus_CLR_BIT 1
#define HcCommandStatus_BLF_MASK 0x4
#define HcCommandStatus_BLF 0x4
#define HcCommandStatus_BLF_BIT 2
#define HcCommandStatus_OCR_MASK 0x8
#define HcCommandStatus_OCR 0x8
#define HcCommandStatus_OCR_BIT 3
#define HcCommandStatus_SOC_MASK 0xC0
#define HcCommandStatus_SOC_BIT 6

#define HcInterruptStatus (*(volatile unsigned *)0xFFE0C00C)
#define HcInterruptStatus_OFFSET 0xC
#define HcInterruptStatus_SO_MASK 0x1
#define HcInterruptStatus_SO 0x1
#define HcInterruptStatus_SO_BIT 0
#define HcInterruptStatus_WDH_MASK 0x2
#define HcInterruptStatus_WDH 0x2
#define HcInterruptStatus_WDH_BIT 1
#define HcInterruptStatus_SF_MASK 0x4
#define HcInterruptStatus_SF 0x4
#define HcInterruptStatus_SF_BIT 2
#define HcInterruptStatus_RD_MASK 0x8
#define HcInterruptStatus_RD 0x8
#define HcInterruptStatus_RD_BIT 3
#define HcInterruptStatus_UE_MASK 0x10
#define HcInterruptStatus_UE 0x10
#define HcInterruptStatus_UE_BIT 4
#define HcInterruptStatus_FNO_MASK 0x20
#define HcInterruptStatus_FNO 0x20
#define HcInterruptStatus_FNO_BIT 5
#define HcInterruptStatus_RHSC_MASK 0x40
#define HcInterruptStatus_RHSC 0x40
#define HcInterruptStatus_RHSC_BIT 6
#define HcInterruptStatus_OC_MASK 0x40000000
#define HcInterruptStatus_OC 0x40000000
#define HcInterruptStatus_OC_BIT 30

#define HcInterruptEnable (*(volatile unsigned *)0xFFE0C010)
#define HcInterruptEnable_OFFSET 0x10
#define HcInterruptEnable_SO_MASK 0x1
#define HcInterruptEnable_SO 0x1
#define HcInterruptEnable_SO_BIT 0
#define HcInterruptEnable_WDH_MASK 0x2
#define HcInterruptEnable_WDH 0x2
#define HcInterruptEnable_WDH_BIT 1
#define HcInterruptEnable_SF_MASK 0x4
#define HcInterruptEnable_SF 0x4
#define HcInterruptEnable_SF_BIT 2
#define HcInterruptEnable_RD_MASK 0x8
#define HcInterruptEnable_RD 0x8
#define HcInterruptEnable_RD_BIT 3
#define HcInterruptEnable_UE_MASK 0x10
#define HcInterruptEnable_UE 0x10
#define HcInterruptEnable_UE_BIT 4
#define HcInterruptEnable_FNO_MASK 0x20
#define HcInterruptEnable_FNO 0x20
#define HcInterruptEnable_FNO_BIT 5
#define HcInterruptEnable_RHSC_MASK 0x40
#define HcInterruptEnable_RHSC 0x40
#define HcInterruptEnable_RHSC_BIT 6
#define HcInterruptEnable_OC_MASK 0x40000000
#define HcInterruptEnable_OC 0x40000000
#define HcInterruptEnable_OC_BIT 30
#define HcInterruptEnable_MIE_MASK 0x80000000
#define HcInterruptEnable_MIE 0x80000000
#define HcInterruptEnable_MIE_BIT 31

#define HcInterruptDisable (*(volatile unsigned *)0xFFE0C014)
#define HcInterruptDisable_OFFSET 0x14
#define HcInterruptDisable_SO_MASK 0x1
#define HcInterruptDisable_SO 0x1
#define HcInterruptDisable_SO_BIT 0
#define HcInterruptDisable_WDH_MASK 0x2
#define HcInterruptDisable_WDH 0x2
#define HcInterruptDisable_WDH_BIT 1
#define HcInterruptDisable_SF_MASK 0x4
#define HcInterruptDisable_SF 0x4
#define HcInterruptDisable_SF_BIT 2
#define HcInterruptDisable_RD_MASK 0x8
#define HcInterruptDisable_RD 0x8
#define HcInterruptDisable_RD_BIT 3
#define HcInterruptDisable_UE_MASK 0x10
#define HcInterruptDisable_UE 0x10
#define HcInterruptDisable_UE_BIT 4
#define HcInterruptDisable_FNO_MASK 0x20
#define HcInterruptDisable_FNO 0x20
#define HcInterruptDisable_FNO_BIT 5
#define HcInterruptDisable_RHSC_MASK 0x40
#define HcInterruptDisable_RHSC 0x40
#define HcInterruptDisable_RHSC_BIT 6
#define HcInterruptDisable_OC_MASK 0x40000000
#define HcInterruptDisable_OC 0x40000000
#define HcInterruptDisable_OC_BIT 30
#define HcInterruptDisable_MIE_MASK 0x80000000
#define HcInterruptDisable_MIE 0x80000000
#define HcInterruptDisable_MIE_BIT 31

#define HcHCCA (*(volatile unsigned *)0xFFE0C018)
#define HcHCCA_OFFSET 0x18
#define HcHCCA_HCCA_MASK 0xFFFFFF00
#define HcHCCA_HCCA_BIT 8

#define HcPeriodCurrentED (*(volatile unsigned *)0xFFE0C01C)
#define HcPeriodCurrentED_OFFSET 0x1C
#define HcPeriodCurrentED_PCED_MASK 0xFFFFFFF0
#define HcPeriodCurrentED_PCED_BIT 4

#define HcControlHeadED (*(volatile unsigned *)0xFFE0C020)
#define HcControlHeadED_OFFSET 0x20
#define HcControlHeadED_CHED_MASK 0xFFFFFFF0
#define HcControlHeadED_CHED_BIT 4

#define HcControlCurrentED (*(volatile unsigned *)0xFFE0C024)
#define HcControlCurrentED_OFFSET 0x24
#define HcControlCurrentED_CCED_MASK 0xFFFFFFF0
#define HcControlCurrentED_CCED_BIT 4

#define HcBulkHeadED (*(volatile unsigned *)0xFFE0C028)
#define HcBulkHeadED_OFFSET 0x28
#define HcBulkHeadED_BHED_MASK 0xFFFFFFF0
#define HcBulkHeadED_BHED_BIT 4

#define HcBulkCurrentED (*(volatile unsigned *)0xFFE0C02C)
#define HcBulkCurrentED_OFFSET 0x2C
#define HcBulkCurrentED_BCED_MASK 0xFFFFFFF0
#define HcBulkCurrentED_BCED_BIT 4

#define HcDoneHead (*(volatile unsigned *)0xFFE0C030)
#define HcDoneHead_OFFSET 0x30
#define HcDoneHead_DH_MASK 0xFFFFFFF0
#define HcDoneHead_DH_BIT 4

#define HcFmInterval (*(volatile unsigned *)0xFFE0C034)
#define HcFmInterval_OFFSET 0x34
#define HcFmInterval_FI_MASK 0x3FFF
#define HcFmInterval_FI_BIT 0
#define HcFmInterval_FSMPS_MASK 0x7FFF0000
#define HcFmInterval_FSMPS_BIT 16
#define HcFmInterval_FIT_MASK 0x80000000
#define HcFmInterval_FIT 0x80000000
#define HcFmInterval_FIT_BIT 31

#define HcFmRemaining (*(volatile unsigned *)0xFFE0C038)
#define HcFmRemaining_OFFSET 0x38
#define HcFmRemaining_FR_MASK 0x3FFF
#define HcFmRemaining_FR_BIT 0
#define HcFmRemaining_FRT_MASK 0x80000000
#define HcFmRemaining_FRT 0x80000000
#define HcFmRemaining_FRT_BIT 31

#define HcFmNumber (*(volatile unsigned *)0xFFE0C03C)
#define HcFmNumber_OFFSET 0x3C
#define HcFmNumber_FN_MASK 0xFFFF
#define HcFmNumber_FN_BIT 0

#define HcPeriodicStart (*(volatile unsigned *)0xFFE0C040)
#define HcPeriodicStart_OFFSET 0x40
#define HcPeriodicStart_PS_MASK 0x3FFF
#define HcPeriodicStart_PS_BIT 0

#define HcLSThreshold (*(volatile unsigned *)0xFFE0C044)
#define HcLSThreshold_OFFSET 0x44
#define HcLSThreshold_LST_MASK 0xFFF
#define HcLSThreshold_LST_BIT 0

#define HcRhDescriptorA (*(volatile unsigned *)0xFFE0C048)
#define HcRhDescriptorA_OFFSET 0x48
#define HcRhDescriptorA_NDP_MASK 0xFF
#define HcRhDescriptorA_NDP_BIT 0
#define HcRhDescriptorA_PSM_MASK 0x100
#define HcRhDescriptorA_PSM 0x100
#define HcRhDescriptorA_PSM_BIT 8
#define HcRhDescriptorA_NPS_MASK 0x200
#define HcRhDescriptorA_NPS 0x200
#define HcRhDescriptorA_NPS_BIT 9
#define HcRhDescriptorA_DT_MASK 0x400
#define HcRhDescriptorA_DT 0x400
#define HcRhDescriptorA_DT_BIT 10
#define HcRhDescriptorA_OCPM_MASK 0x800
#define HcRhDescriptorA_OCPM 0x800
#define HcRhDescriptorA_OCPM_BIT 11
#define HcRhDescriptorA_NOCP_MASK 0x1000
#define HcRhDescriptorA_NOCP 0x1000
#define HcRhDescriptorA_NOCP_BIT 12
#define HcRhDescriptorA_POTPGT_MASK 0xFF000000
#define HcRhDescriptorA_POTPGT_BIT 24

#define HcRhDescriptorB (*(volatile unsigned *)0xFFE0C04C)
#define HcRhDescriptorB_OFFSET 0x4C
#define HcRhDescriptorB_DR_MASK 0xFFFF
#define HcRhDescriptorB_DR_BIT 0
#define HcRhDescriptorB_PPCM_MASK 0xFFFF0000
#define HcRhDescriptorB_PPCM_BIT 16

#define HcRhStatus (*(volatile unsigned *)0xFFE0C050)
#define HcRhStatus_OFFSET 0x50
#define HcRhStatus_LPS_MASK 0x1
#define HcRhStatus_LPS 0x1
#define HcRhStatus_LPS_BIT 0
#define HcRhStatus_OCI_MASK 0x2
#define HcRhStatus_OCI 0x2
#define HcRhStatus_OCI_BIT 1
#define HcRhStatus_DRWE_MASK 0x8000
#define HcRhStatus_DRWE 0x8000
#define HcRhStatus_DRWE_BIT 15
#define HcRhStatus_LPSC_MASK 0x10000
#define HcRhStatus_LPSC 0x10000
#define HcRhStatus_LPSC_BIT 16
#define HcRhStatus_OCIC_MASK 0x20000
#define HcRhStatus_OCIC 0x20000
#define HcRhStatus_OCIC_BIT 17
#define HcRhStatus_CRWE_MASK 0x80000000
#define HcRhStatus_CRWE 0x80000000
#define HcRhStatus_CRWE_BIT 31

#define HcRhPortStatus1 (*(volatile unsigned *)0xFFE0C054)
#define HcRhPortStatus1_OFFSET 0x54
#define HcRhPortStatus1_CCS_MASK 0x1
#define HcRhPortStatus1_CCS 0x1
#define HcRhPortStatus1_CCS_BIT 0
#define HcRhPortStatus1_PES_MASK 0x2
#define HcRhPortStatus1_PES 0x2
#define HcRhPortStatus1_PES_BIT 1
#define HcRhPortStatus1_PSS_MASK 0x4
#define HcRhPortStatus1_PSS 0x4
#define HcRhPortStatus1_PSS_BIT 2
#define HcRhPortStatus1_POCI_MASK 0x8
#define HcRhPortStatus1_POCI 0x8
#define HcRhPortStatus1_POCI_BIT 3
#define HcRhPortStatus1_PRS_MASK 0x10
#define HcRhPortStatus1_PRS 0x10
#define HcRhPortStatus1_PRS_BIT 4
#define HcRhPortStatus1_PPS_MASK 0x100
#define HcRhPortStatus1_PPS 0x100
#define HcRhPortStatus1_PPS_BIT 8
#define HcRhPortStatus1_LSDA_MASK 0x200
#define HcRhPortStatus1_LSDA 0x200
#define HcRhPortStatus1_LSDA_BIT 9
#define HcRhPortStatus1_CSC_MASK 0x10000
#define HcRhPortStatus1_CSC 0x10000
#define HcRhPortStatus1_CSC_BIT 16
#define HcRhPortStatus1_PESC_MASK 0x20000
#define HcRhPortStatus1_PESC 0x20000
#define HcRhPortStatus1_PESC_BIT 17
#define HcRhPortStatus1_PSSC_MASK 0x40000
#define HcRhPortStatus1_PSSC 0x40000
#define HcRhPortStatus1_PSSC_BIT 18
#define HcRhPortStatus1_OCIC_MASK 0x80000
#define HcRhPortStatus1_OCIC 0x80000
#define HcRhPortStatus1_OCIC_BIT 19
#define HcRhPortStatus1_PRSC_MASK 0x100000
#define HcRhPortStatus1_PRSC 0x100000
#define HcRhPortStatus1_PRSC_BIT 20

#define HcRhPortStatus2 (*(volatile unsigned *)0xFFE0C058)
#define HcRhPortStatus2_OFFSET 0x58
#define HcRhPortStatus2_CCS_MASK 0x1
#define HcRhPortStatus2_CCS 0x1
#define HcRhPortStatus2_CCS_BIT 0
#define HcRhPortStatus2_PES_MASK 0x2
#define HcRhPortStatus2_PES 0x2
#define HcRhPortStatus2_PES_BIT 1
#define HcRhPortStatus2_PSS_MASK 0x4
#define HcRhPortStatus2_PSS 0x4
#define HcRhPortStatus2_PSS_BIT 2
#define HcRhPortStatus2_POCI_MASK 0x8
#define HcRhPortStatus2_POCI 0x8
#define HcRhPortStatus2_POCI_BIT 3
#define HcRhPortStatus2_PRS_MASK 0x10
#define HcRhPortStatus2_PRS 0x10
#define HcRhPortStatus2_PRS_BIT 4
#define HcRhPortStatus2_PPS_MASK 0x100
#define HcRhPortStatus2_PPS 0x100
#define HcRhPortStatus2_PPS_BIT 8
#define HcRhPortStatus2_LSDA_MASK 0x200
#define HcRhPortStatus2_LSDA 0x200
#define HcRhPortStatus2_LSDA_BIT 9
#define HcRhPortStatus2_CSC_MASK 0x10000
#define HcRhPortStatus2_CSC 0x10000
#define HcRhPortStatus2_CSC_BIT 16
#define HcRhPortStatus2_PESC_MASK 0x20000
#define HcRhPortStatus2_PESC 0x20000
#define HcRhPortStatus2_PESC_BIT 17
#define HcRhPortStatus2_PSSC_MASK 0x40000
#define HcRhPortStatus2_PSSC 0x40000
#define HcRhPortStatus2_PSSC_BIT 18
#define HcRhPortStatus2_OCIC_MASK 0x80000
#define HcRhPortStatus2_OCIC 0x80000
#define HcRhPortStatus2_OCIC_BIT 19
#define HcRhPortStatus2_PRSC_MASK 0x100000
#define HcRhPortStatus2_PRSC 0x100000
#define HcRhPortStatus2_PRSC_BIT 20

#define FIO_BASE 0x3FFFC000

#define FIO0DIR (*(volatile unsigned long *)0x3FFFC000)
#define FIO0DIR_OFFSET 0x0

#define FIO0DIR0 (*(volatile unsigned char *)0x3FFFC000)
#define FIO0DIR0_OFFSET 0x0

#define FIO0DIR1 (*(volatile unsigned char *)0x3FFFC001)
#define FIO0DIR1_OFFSET 0x1

#define FIO0DIR2 (*(volatile unsigned char *)0x3FFFC002)
#define FIO0DIR2_OFFSET 0x2

#define FIO0DIR3 (*(volatile unsigned char *)0x3FFFC003)
#define FIO0DIR3_OFFSET 0x3

#define FIO0DIRL (*(volatile unsigned short *)0x3FFFC000)
#define FIO0DIRL_OFFSET 0x0

#define FIO0DIRH (*(volatile unsigned short *)0x3FFFC002)
#define FIO0DIRH_OFFSET 0x2

#define FIO0MASK (*(volatile unsigned long *)0x3FFFC010)
#define FIO0MASK_OFFSET 0x10

#define FIO0MASK0 (*(volatile unsigned char *)0x3FFFC010)
#define FIO0MASK0_OFFSET 0x10

#define FIO0MASK1 (*(volatile unsigned char *)0x3FFFC011)
#define FIO0MASK1_OFFSET 0x11

#define FIO0MASK2 (*(volatile unsigned char *)0x3FFFC012)
#define FIO0MASK2_OFFSET 0x12

#define FIO0MASK3 (*(volatile unsigned char *)0x3FFFC013)
#define FIO0MASK3_OFFSET 0x13

#define FIO0MASKL (*(volatile unsigned short *)0x3FFFC010)
#define FIO0MASKL_OFFSET 0x10

#define FIO0MASKH (*(volatile unsigned short *)0x3FFFC012)
#define FIO0MASKH_OFFSET 0x12

#define FIO0PIN (*(volatile unsigned long *)0x3FFFC014)
#define FIO0PIN_OFFSET 0x14

#define FIO0PIN0 (*(volatile unsigned char *)0x3FFFC014)
#define FIO0PIN0_OFFSET 0x14

#define FIO0PIN1 (*(volatile unsigned char *)0x3FFFC015)
#define FIO0PIN1_OFFSET 0x15

#define FIO0PIN2 (*(volatile unsigned char *)0x3FFFC016)
#define FIO0PIN2_OFFSET 0x16

#define FIO0PIN3 (*(volatile unsigned char *)0x3FFFC017)
#define FIO0PIN3_OFFSET 0x17

#define FIO0PINL (*(volatile unsigned short *)0x3FFFC014)
#define FIO0PINL_OFFSET 0x14

#define FIO0PINH (*(volatile unsigned short *)0x3FFFC016)
#define FIO0PINH_OFFSET 0x16

#define FIO0SET (*(volatile unsigned long *)0x3FFFC018)
#define FIO0SET_OFFSET 0x18

#define FIO0SET0 (*(volatile unsigned char *)0x3FFFC018)
#define FIO0SET0_OFFSET 0x18

#define FIO0SET1 (*(volatile unsigned char *)0x3FFFC019)
#define FIO0SET1_OFFSET 0x19

#define FIO0SET2 (*(volatile unsigned char *)0x3FFFC01A)
#define FIO0SET2_OFFSET 0x1A

#define FIO0SET3 (*(volatile unsigned char *)0x3FFFC01B)
#define FIO0SET3_OFFSET 0x1B

#define FIO0SETL (*(volatile unsigned short *)0x3FFFC018)
#define FIO0SETL_OFFSET 0x18

#define FIO0SETH (*(volatile unsigned short *)0x3FFFC01A)
#define FIO0SETH_OFFSET 0x1A

#define FIO0CLR (*(volatile unsigned long *)0x3FFFC01C)
#define FIO0CLR_OFFSET 0x1C

#define FIO0CLR0 (*(volatile unsigned char *)0x3FFFC01C)
#define FIO0CLR0_OFFSET 0x1C

#define FIO0CLR1 (*(volatile unsigned char *)0x3FFFC01D)
#define FIO0CLR1_OFFSET 0x1D

#define FIO0CLR2 (*(volatile unsigned char *)0x3FFFC01E)
#define FIO0CLR2_OFFSET 0x1E

#define FIO0CLR3 (*(volatile unsigned char *)0x3FFFC01F)
#define FIO0CLR3_OFFSET 0x1F

#define FIO0CLRL (*(volatile unsigned short *)0x3FFFC01C)
#define FIO0CLRL_OFFSET 0x1C

#define FIO0CLRH (*(volatile unsigned short *)0x3FFFC01E)
#define FIO0CLRH_OFFSET 0x1E

#define FIO1DIR (*(volatile unsigned long *)0x3FFFC020)
#define FIO1DIR_OFFSET 0x20

#define FIO1DIR0 (*(volatile unsigned char *)0x3FFFC020)
#define FIO1DIR0_OFFSET 0x20

#define FIO1DIR1 (*(volatile unsigned char *)0x3FFFC021)
#define FIO1DIR1_OFFSET 0x21

#define FIO1DIR2 (*(volatile unsigned char *)0x3FFFC022)
#define FIO1DIR2_OFFSET 0x22

#define FIO1DIR3 (*(volatile unsigned char *)0x3FFFC023)
#define FIO1DIR3_OFFSET 0x23

#define FIO1DIRL (*(volatile unsigned short *)0x3FFFC020)
#define FIO1DIRL_OFFSET 0x20

#define FIO1DIRH (*(volatile unsigned short *)0x3FFFC022)
#define FIO1DIRH_OFFSET 0x22

#define FIO1MASK (*(volatile unsigned long *)0x3FFFC030)
#define FIO1MASK_OFFSET 0x30

#define FIO1MASK0 (*(volatile unsigned char *)0x3FFFC030)
#define FIO1MASK0_OFFSET 0x30

#define FIO1MASK1 (*(volatile unsigned char *)0x3FFFC031)
#define FIO1MASK1_OFFSET 0x31

#define FIO1MASK2 (*(volatile unsigned char *)0x3FFFC032)
#define FIO1MASK2_OFFSET 0x32

#define FIO1MASK3 (*(volatile unsigned char *)0x3FFFC033)
#define FIO1MASK3_OFFSET 0x33

#define FIO1MASKL (*(volatile unsigned short *)0x3FFFC030)
#define FIO1MASKL_OFFSET 0x30

#define FIO1MASKH (*(volatile unsigned short *)0x3FFFC032)
#define FIO1MASKH_OFFSET 0x32

#define FIO1PIN (*(volatile unsigned long *)0x3FFFC034)
#define FIO1PIN_OFFSET 0x34

#define FIO1PIN0 (*(volatile unsigned char *)0x3FFFC034)
#define FIO1PIN0_OFFSET 0x34

#define FIO1PIN1 (*(volatile unsigned char *)0x3FFFC035)
#define FIO1PIN1_OFFSET 0x35

#define FIO1PIN2 (*(volatile unsigned char *)0x3FFFC036)
#define FIO1PIN2_OFFSET 0x36

#define FIO1PIN3 (*(volatile unsigned char *)0x3FFFC037)
#define FIO1PIN3_OFFSET 0x37

#define FIO1PINL (*(volatile unsigned short *)0x3FFFC034)
#define FIO1PINL_OFFSET 0x34

#define FIO1PINH (*(volatile unsigned short *)0x3FFFC036)
#define FIO1PINH_OFFSET 0x36

#define FIO1SET (*(volatile unsigned long *)0x3FFFC038)
#define FIO1SET_OFFSET 0x38

#define FIO1SET0 (*(volatile unsigned char *)0x3FFFC038)
#define FIO1SET0_OFFSET 0x38

#define FIO1SET1 (*(volatile unsigned char *)0x3FFFC039)
#define FIO1SET1_OFFSET 0x39

#define FIO1SET2 (*(volatile unsigned char *)0x3FFFC03A)
#define FIO1SET2_OFFSET 0x3A

#define FIO1SET3 (*(volatile unsigned char *)0x3FFFC03B)
#define FIO1SET3_OFFSET 0x3B

#define FIO1SETL (*(volatile unsigned short *)0x3FFFC038)
#define FIO1SETL_OFFSET 0x38

#define FIO1SETH (*(volatile unsigned short *)0x3FFFC03A)
#define FIO1SETH_OFFSET 0x3A

#define FIO1CLR (*(volatile unsigned long *)0x3FFFC03C)
#define FIO1CLR_OFFSET 0x3C

#define FIO1CLR0 (*(volatile unsigned char *)0x3FFFC03C)
#define FIO1CLR0_OFFSET 0x3C

#define FIO1CLR1 (*(volatile unsigned char *)0x3FFFC03D)
#define FIO1CLR1_OFFSET 0x3D

#define FIO1CLR2 (*(volatile unsigned char *)0x3FFFC03E)
#define FIO1CLR2_OFFSET 0x3E

#define FIO1CLR3 (*(volatile unsigned char *)0x3FFFC03F)
#define FIO1CLR3_OFFSET 0x3F

#define FIO1CLRL (*(volatile unsigned short *)0x3FFFC03C)
#define FIO1CLRL_OFFSET 0x3C

#define FIO1CLRH (*(volatile unsigned short *)0x3FFFC03E)
#define FIO1CLRH_OFFSET 0x3E

#define FIO2DIR (*(volatile unsigned long *)0x3FFFC040)
#define FIO2DIR_OFFSET 0x40

#define FIO2DIR0 (*(volatile unsigned char *)0x3FFFC040)
#define FIO2DIR0_OFFSET 0x40

#define FIO2DIR1 (*(volatile unsigned char *)0x3FFFC041)
#define FIO2DIR1_OFFSET 0x41

#define FIO2DIR2 (*(volatile unsigned char *)0x3FFFC042)
#define FIO2DIR2_OFFSET 0x42

#define FIO2DIR3 (*(volatile unsigned char *)0x3FFFC043)
#define FIO2DIR3_OFFSET 0x43

#define FIO2DIRL (*(volatile unsigned short *)0x3FFFC040)
#define FIO2DIRL_OFFSET 0x40

#define FIO2DIRH (*(volatile unsigned short *)0x3FFFC042)
#define FIO2DIRH_OFFSET 0x42

#define FIO2MASK (*(volatile unsigned long *)0x3FFFC050)
#define FIO2MASK_OFFSET 0x50

#define FIO2MASK0 (*(volatile unsigned char *)0x3FFFC050)
#define FIO2MASK0_OFFSET 0x50

#define FIO2MASK1 (*(volatile unsigned char *)0x3FFFC051)
#define FIO2MASK1_OFFSET 0x51

#define FIO2MASK2 (*(volatile unsigned char *)0x3FFFC052)
#define FIO2MASK2_OFFSET 0x52

#define FIO2MASK3 (*(volatile unsigned char *)0x3FFFC053)
#define FIO2MASK3_OFFSET 0x53

#define FIO2MASKL (*(volatile unsigned short *)0x3FFFC050)
#define FIO2MASKL_OFFSET 0x50

#define FIO2MASKH (*(volatile unsigned short *)0x3FFFC052)
#define FIO2MASKH_OFFSET 0x52

#define FIO2PIN (*(volatile unsigned long *)0x3FFFC054)
#define FIO2PIN_OFFSET 0x54

#define FIO2PIN0 (*(volatile unsigned char *)0x3FFFC054)
#define FIO2PIN0_OFFSET 0x54

#define FIO2PIN1 (*(volatile unsigned char *)0x3FFFC055)
#define FIO2PIN1_OFFSET 0x55

#define FIO2PIN2 (*(volatile unsigned char *)0x3FFFC056)
#define FIO2PIN2_OFFSET 0x56

#define FIO2PIN3 (*(volatile unsigned char *)0x3FFFC057)
#define FIO2PIN3_OFFSET 0x57

#define FIO2PINL (*(volatile unsigned short *)0x3FFFC054)
#define FIO2PINL_OFFSET 0x54

#define FIO2PINH (*(volatile unsigned short *)0x3FFFC056)
#define FIO2PINH_OFFSET 0x56

#define FIO2SET (*(volatile unsigned long *)0x3FFFC058)
#define FIO2SET_OFFSET 0x58

#define FIO2SET0 (*(volatile unsigned char *)0x3FFFC058)
#define FIO2SET0_OFFSET 0x58

#define FIO2SET1 (*(volatile unsigned char *)0x3FFFC059)
#define FIO2SET1_OFFSET 0x59

#define FIO2SET2 (*(volatile unsigned char *)0x3FFFC05A)
#define FIO2SET2_OFFSET 0x5A

#define FIO2SET3 (*(volatile unsigned char *)0x3FFFC05B)
#define FIO2SET3_OFFSET 0x5B

#define FIO2SETL (*(volatile unsigned short *)0x3FFFC058)
#define FIO2SETL_OFFSET 0x58

#define FIO2SETH (*(volatile unsigned short *)0x3FFFC05A)
#define FIO2SETH_OFFSET 0x5A

#define FIO2CLR (*(volatile unsigned long *)0x3FFFC05C)
#define FIO2CLR_OFFSET 0x5C

#define FIO2CLR0 (*(volatile unsigned char *)0x3FFFC05C)
#define FIO2CLR0_OFFSET 0x5C

#define FIO2CLR1 (*(volatile unsigned char *)0x3FFFC05D)
#define FIO2CLR1_OFFSET 0x5D

#define FIO2CLR2 (*(volatile unsigned char *)0x3FFFC05E)
#define FIO2CLR2_OFFSET 0x5E

#define FIO2CLR3 (*(volatile unsigned char *)0x3FFFC05F)
#define FIO2CLR3_OFFSET 0x5F

#define FIO2CLRL (*(volatile unsigned short *)0x3FFFC05C)
#define FIO2CLRL_OFFSET 0x5C

#define FIO2CLRH (*(volatile unsigned short *)0x3FFFC05E)
#define FIO2CLRH_OFFSET 0x5E

#define FIO3DIR (*(volatile unsigned long *)0x3FFFC060)
#define FIO3DIR_OFFSET 0x60

#define FIO3DIR0 (*(volatile unsigned char *)0x3FFFC060)
#define FIO3DIR0_OFFSET 0x60

#define FIO3DIR1 (*(volatile unsigned char *)0x3FFFC061)
#define FIO3DIR1_OFFSET 0x61

#define FIO3DIR2 (*(volatile unsigned char *)0x3FFFC062)
#define FIO3DIR2_OFFSET 0x62

#define FIO3DIR3 (*(volatile unsigned char *)0x3FFFC063)
#define FIO3DIR3_OFFSET 0x63

#define FIO3DIRL (*(volatile unsigned short *)0x3FFFC060)
#define FIO3DIRL_OFFSET 0x60

#define FIO3DIRH (*(volatile unsigned short *)0x3FFFC062)
#define FIO3DIRH_OFFSET 0x62

#define FIO3MASK (*(volatile unsigned long *)0x3FFFC070)
#define FIO3MASK_OFFSET 0x70

#define FIO3MASK0 (*(volatile unsigned char *)0x3FFFC070)
#define FIO3MASK0_OFFSET 0x70

#define FIO3MASK1 (*(volatile unsigned char *)0x3FFFC071)
#define FIO3MASK1_OFFSET 0x71

#define FIO3MASK2 (*(volatile unsigned char *)0x3FFFC072)
#define FIO3MASK2_OFFSET 0x72

#define FIO3MASK3 (*(volatile unsigned char *)0x3FFFC073)
#define FIO3MASK3_OFFSET 0x73

#define FIO3MASKL (*(volatile unsigned short *)0x3FFFC070)
#define FIO3MASKL_OFFSET 0x70

#define FIO3MASKH (*(volatile unsigned short *)0x3FFFC072)
#define FIO3MASKH_OFFSET 0x72

#define FIO3PIN (*(volatile unsigned long *)0x3FFFC074)
#define FIO3PIN_OFFSET 0x74

#define FIO3PIN0 (*(volatile unsigned char *)0x3FFFC074)
#define FIO3PIN0_OFFSET 0x74

#define FIO3PIN1 (*(volatile unsigned char *)0x3FFFC075)
#define FIO3PIN1_OFFSET 0x75

#define FIO3PIN2 (*(volatile unsigned char *)0x3FFFC076)
#define FIO3PIN2_OFFSET 0x76

#define FIO3PIN3 (*(volatile unsigned char *)0x3FFFC077)
#define FIO3PIN3_OFFSET 0x77

#define FIO3PINL (*(volatile unsigned short *)0x3FFFC074)
#define FIO3PINL_OFFSET 0x74

#define FIO3PINH (*(volatile unsigned short *)0x3FFFC076)
#define FIO3PINH_OFFSET 0x76

#define FIO3SET (*(volatile unsigned long *)0x3FFFC078)
#define FIO3SET_OFFSET 0x78

#define FIO3SET0 (*(volatile unsigned char *)0x3FFFC078)
#define FIO3SET0_OFFSET 0x78

#define FIO3SET1 (*(volatile unsigned char *)0x3FFFC079)
#define FIO3SET1_OFFSET 0x79

#define FIO3SET2 (*(volatile unsigned char *)0x3FFFC07A)
#define FIO3SET2_OFFSET 0x7A

#define FIO3SET3 (*(volatile unsigned char *)0x3FFFC07B)
#define FIO3SET3_OFFSET 0x7B

#define FIO3SETL (*(volatile unsigned short *)0x3FFFC078)
#define FIO3SETL_OFFSET 0x78

#define FIO3SETH (*(volatile unsigned short *)0x3FFFC07A)
#define FIO3SETH_OFFSET 0x7A

#define FIO3CLR (*(volatile unsigned long *)0x3FFFC07C)
#define FIO3CLR_OFFSET 0x7C

#define FIO3CLR0 (*(volatile unsigned char *)0x3FFFC07C)
#define FIO3CLR0_OFFSET 0x7C

#define FIO3CLR1 (*(volatile unsigned char *)0x3FFFC07D)
#define FIO3CLR1_OFFSET 0x7D

#define FIO3CLR2 (*(volatile unsigned char *)0x3FFFC07E)
#define FIO3CLR2_OFFSET 0x7E

#define FIO3CLR3 (*(volatile unsigned char *)0x3FFFC07F)
#define FIO3CLR3_OFFSET 0x7F

#define FIO3CLRL (*(volatile unsigned short *)0x3FFFC07C)
#define FIO3CLRL_OFFSET 0x7C

#define FIO3CLRH (*(volatile unsigned short *)0x3FFFC07E)
#define FIO3CLRH_OFFSET 0x7E

#define FIO4DIR (*(volatile unsigned long *)0x3FFFC080)
#define FIO4DIR_OFFSET 0x80

#define FIO4DIR0 (*(volatile unsigned char *)0x3FFFC080)
#define FIO4DIR0_OFFSET 0x80

#define FIO4DIR1 (*(volatile unsigned char *)0x3FFFC081)
#define FIO4DIR1_OFFSET 0x81

#define FIO4DIR2 (*(volatile unsigned char *)0x3FFFC082)
#define FIO4DIR2_OFFSET 0x82

#define FIO4DIR3 (*(volatile unsigned char *)0x3FFFC083)
#define FIO4DIR3_OFFSET 0x83

#define FIO4DIRL (*(volatile unsigned short *)0x3FFFC080)
#define FIO4DIRL_OFFSET 0x80

#define FIO4DIRH (*(volatile unsigned short *)0x3FFFC082)
#define FIO4DIRH_OFFSET 0x82

#define FIO4MASK (*(volatile unsigned long *)0x3FFFC090)
#define FIO4MASK_OFFSET 0x90

#define FIO4MASK0 (*(volatile unsigned char *)0x3FFFC090)
#define FIO4MASK0_OFFSET 0x90

#define FIO4MASK1 (*(volatile unsigned char *)0x3FFFC091)
#define FIO4MASK1_OFFSET 0x91

#define FIO4MASK2 (*(volatile unsigned char *)0x3FFFC092)
#define FIO4MASK2_OFFSET 0x92

#define FIO4MASK3 (*(volatile unsigned char *)0x3FFFC093)
#define FIO4MASK3_OFFSET 0x93

#define FIO4MASKL (*(volatile unsigned short *)0x3FFFC090)
#define FIO4MASKL_OFFSET 0x90

#define FIO4MASKH (*(volatile unsigned short *)0x3FFFC092)
#define FIO4MASKH_OFFSET 0x92

#define FIO4PIN (*(volatile unsigned long *)0x3FFFC094)
#define FIO4PIN_OFFSET 0x94

#define FIO4PIN0 (*(volatile unsigned char *)0x3FFFC094)
#define FIO4PIN0_OFFSET 0x94

#define FIO4PIN1 (*(volatile unsigned char *)0x3FFFC095)
#define FIO4PIN1_OFFSET 0x95

#define FIO4PIN2 (*(volatile unsigned char *)0x3FFFC096)
#define FIO4PIN2_OFFSET 0x96

#define FIO4PIN3 (*(volatile unsigned char *)0x3FFFC097)
#define FIO4PIN3_OFFSET 0x97

#define FIO4PINL (*(volatile unsigned short *)0x3FFFC094)
#define FIO4PINL_OFFSET 0x94

#define FIO4PINH (*(volatile unsigned short *)0x3FFFC096)
#define FIO4PINH_OFFSET 0x96

#define FIO4SET (*(volatile unsigned long *)0x3FFFC098)
#define FIO4SET_OFFSET 0x98

#define FIO4SET0 (*(volatile unsigned char *)0x3FFFC098)
#define FIO4SET0_OFFSET 0x98

#define FIO4SET1 (*(volatile unsigned char *)0x3FFFC099)
#define FIO4SET1_OFFSET 0x99

#define FIO4SET2 (*(volatile unsigned char *)0x3FFFC09A)
#define FIO4SET2_OFFSET 0x9A

#define FIO4SET3 (*(volatile unsigned char *)0x3FFFC09B)
#define FIO4SET3_OFFSET 0x9B

#define FIO4SETL (*(volatile unsigned short *)0x3FFFC098)
#define FIO4SETL_OFFSET 0x98

#define FIO4SETH (*(volatile unsigned short *)0x3FFFC09A)
#define FIO4SETH_OFFSET 0x9A

#define FIO4CLR (*(volatile unsigned long *)0x3FFFC09C)
#define FIO4CLR_OFFSET 0x9C

#define FIO4CLR0 (*(volatile unsigned char *)0x3FFFC09C)
#define FIO4CLR0_OFFSET 0x9C

#define FIO4CLR1 (*(volatile unsigned char *)0x3FFFC09D)
#define FIO4CLR1_OFFSET 0x9D

#define FIO4CLR2 (*(volatile unsigned char *)0x3FFFC09E)
#define FIO4CLR2_OFFSET 0x9E

#define FIO4CLR3 (*(volatile unsigned char *)0x3FFFC09F)
#define FIO4CLR3_OFFSET 0x9F

#define FIO4CLRL (*(volatile unsigned short *)0x3FFFC09C)
#define FIO4CLRL_OFFSET 0x9C

#define FIO4CLRH (*(volatile unsigned short *)0x3FFFC09E)
#define FIO4CLRH_OFFSET 0x9E

#define WDT_BASE 0xE0000000

#define WDMOD (*(volatile unsigned long *)0xE0000000)
#define WDMOD_OFFSET 0x0
#define WDMOD_WDEN_MASK 0x1
#define WDMOD_WDEN 0x1
#define WDMOD_WDEN_BIT 0
#define WDMOD_WDRESET_MASK 0x2
#define WDMOD_WDRESET 0x2
#define WDMOD_WDRESET_BIT 1
#define WDMOD_WDTOF_MASK 0x4
#define WDMOD_WDTOF 0x4
#define WDMOD_WDTOF_BIT 2
#define WDMOD_WDINT_MASK 0x8
#define WDMOD_WDINT 0x8
#define WDMOD_WDINT_BIT 3

#define WDTC (*(volatile unsigned long *)0xE0000004)
#define WDTC_OFFSET 0x4

#define WDFEED (*(volatile unsigned long *)0xE0000008)
#define WDFEED_OFFSET 0x8

#define WDTV (*(volatile unsigned long *)0xE000000C)
#define WDTV_OFFSET 0xC

#define WDCLKSEL (*(volatile unsigned long *)0xE0000010)
#define WDCLKSEL_OFFSET 0x10
#define WDCLKSEL_WDSEL_MASK 0x3
#define WDCLKSEL_WDSEL_BIT 0

#define TIMER0_BASE 0xE0004000

#define T0IR (*(volatile unsigned char *)0xE0004000)
#define T0IR_OFFSET 0x0
#define T0IR_MR0_MASK 0x1
#define T0IR_MR0 0x1
#define T0IR_MR0_BIT 0
#define T0IR_MR1_MASK 0x2
#define T0IR_MR1 0x2
#define T0IR_MR1_BIT 1
#define T0IR_MR2_MASK 0x4
#define T0IR_MR2 0x4
#define T0IR_MR2_BIT 2
#define T0IR_MR3_MASK 0x8
#define T0IR_MR3 0x8
#define T0IR_MR3_BIT 3
#define T0IR_CR0_MASK 0x10
#define T0IR_CR0 0x10
#define T0IR_CR0_BIT 4
#define T0IR_CR1_MASK 0x20
#define T0IR_CR1 0x20
#define T0IR_CR1_BIT 5
#define T0IR_CR2_MASK 0x40
#define T0IR_CR2 0x40
#define T0IR_CR2_BIT 6
#define T0IR_CR3_MASK 0x80
#define T0IR_CR3 0x80
#define T0IR_CR3_BIT 7

#define T0TCR (*(volatile unsigned char *)0xE0004004)
#define T0TCR_OFFSET 0x4
#define T0TCR_Counter_Enable_MASK 0x1
#define T0TCR_Counter_Enable 0x1
#define T0TCR_Counter_Enable_BIT 0
#define T0TCR_Counter_Reset_MASK 0x2
#define T0TCR_Counter_Reset 0x2
#define T0TCR_Counter_Reset_BIT 1

#define T0TC (*(volatile unsigned long *)0xE0004008)
#define T0TC_OFFSET 0x8

#define T0PR (*(volatile unsigned long *)0xE000400C)
#define T0PR_OFFSET 0xC

#define T0PC (*(volatile unsigned long *)0xE0004010)
#define T0PC_OFFSET 0x10

#define T0MCR (*(volatile unsigned short *)0xE0004014)
#define T0MCR_OFFSET 0x14
#define T0MCR_MR0I_MASK 0x1
#define T0MCR_MR0I 0x1
#define T0MCR_MR0I_BIT 0
#define T0MCR_MR0R_MASK 0x2
#define T0MCR_MR0R 0x2
#define T0MCR_MR0R_BIT 1
#define T0MCR_MR0S_MASK 0x4
#define T0MCR_MR0S 0x4
#define T0MCR_MR0S_BIT 2
#define T0MCR_MR1I_MASK 0x8
#define T0MCR_MR1I 0x8
#define T0MCR_MR1I_BIT 3
#define T0MCR_MR1R_MASK 0x10
#define T0MCR_MR1R 0x10
#define T0MCR_MR1R_BIT 4
#define T0MCR_MR1S_MASK 0x20
#define T0MCR_MR1S 0x20
#define T0MCR_MR1S_BIT 5
#define T0MCR_MR2I_MASK 0x40
#define T0MCR_MR2I 0x40
#define T0MCR_MR2I_BIT 6
#define T0MCR_MR2R_MASK 0x80
#define T0MCR_MR2R 0x80
#define T0MCR_MR2R_BIT 7
#define T0MCR_MR2S_MASK 0x100
#define T0MCR_MR2S 0x100
#define T0MCR_MR2S_BIT 8
#define T0MCR_MR3I_MASK 0x200
#define T0MCR_MR3I 0x200
#define T0MCR_MR3I_BIT 9
#define T0MCR_MR3R_MASK 0x400
#define T0MCR_MR3R 0x400
#define T0MCR_MR3R_BIT 10
#define T0MCR_MR3S_MASK 0x800
#define T0MCR_MR3S 0x800
#define T0MCR_MR3S_BIT 11

#define T0MR0 (*(volatile unsigned long *)0xE0004018)
#define T0MR0_OFFSET 0x18

#define T0MR1 (*(volatile unsigned long *)0xE000401C)
#define T0MR1_OFFSET 0x1C

#define T0MR2 (*(volatile unsigned long *)0xE0004020)
#define T0MR2_OFFSET 0x20

#define T0MR3 (*(volatile unsigned long *)0xE0004024)
#define T0MR3_OFFSET 0x24

#define T0CCR (*(volatile unsigned short *)0xE0004028)
#define T0CCR_OFFSET 0x28
#define T0CCR_CAP0RE_MASK 0x1
#define T0CCR_CAP0RE 0x1
#define T0CCR_CAP0RE_BIT 0
#define T0CCR_CAP0FE_MASK 0x2
#define T0CCR_CAP0FE 0x2
#define T0CCR_CAP0FE_BIT 1
#define T0CCR_CAP0I_MASK 0x4
#define T0CCR_CAP0I 0x4
#define T0CCR_CAP0I_BIT 2
#define T0CCR_CAP1RE_MASK 0x8
#define T0CCR_CAP1RE 0x8
#define T0CCR_CAP1RE_BIT 3
#define T0CCR_CAP1FE_MASK 0x10
#define T0CCR_CAP1FE 0x10
#define T0CCR_CAP1FE_BIT 4
#define T0CCR_CAP1I_MASK 0x20
#define T0CCR_CAP1I 0x20
#define T0CCR_CAP1I_BIT 5
#define T0CCR_CAP2RE_MASK 0x40
#define T0CCR_CAP2RE 0x40
#define T0CCR_CAP2RE_BIT 6
#define T0CCR_CAP2FE_MASK 0x80
#define T0CCR_CAP2FE 0x80
#define T0CCR_CAP2FE_BIT 7
#define T0CCR_CAP2I_MASK 0x100
#define T0CCR_CAP2I 0x100
#define T0CCR_CAP2I_BIT 8
#define T0CCR_CAP3RE_MASK 0x200
#define T0CCR_CAP3RE 0x200
#define T0CCR_CAP3RE_BIT 9
#define T0CCR_CAP3FE_MASK 0x400
#define T0CCR_CAP3FE 0x400
#define T0CCR_CAP3FE_BIT 10
#define T0CCR_CAP3I_MASK 0x800
#define T0CCR_CAP3I 0x800
#define T0CCR_CAP3I_BIT 11

#define T0CR0 (*(volatile unsigned long *)0xE000402C)
#define T0CR0_OFFSET 0x2C

#define T0CR1 (*(volatile unsigned long *)0xE0004030)
#define T0CR1_OFFSET 0x30

#define T0CR2 (*(volatile unsigned long *)0xE0004034)
#define T0CR2_OFFSET 0x34

#define T0CR3 (*(volatile unsigned long *)0xE0004038)
#define T0CR3_OFFSET 0x38

#define T0EMR (*(volatile unsigned short *)0xE000403C)
#define T0EMR_OFFSET 0x3C
#define T0EMR_EM0_MASK 0x1
#define T0EMR_EM0 0x1
#define T0EMR_EM0_BIT 0
#define T0EMR_EM1_MASK 0x2
#define T0EMR_EM1 0x2
#define T0EMR_EM1_BIT 1
#define T0EMR_EM2_MASK 0x4
#define T0EMR_EM2 0x4
#define T0EMR_EM2_BIT 2
#define T0EMR_EM3_MASK 0x8
#define T0EMR_EM3 0x8
#define T0EMR_EM3_BIT 3
#define T0EMR_EMC0_MASK 0x30
#define T0EMR_EMC0_BIT 4
#define T0EMR_EMC1_MASK 0xC0
#define T0EMR_EMC1_BIT 6
#define T0EMR_EMC2_MASK 0x300
#define T0EMR_EMC2_BIT 8
#define T0EMR_EMC3_MASK 0xC00
#define T0EMR_EMC3_BIT 10

#define T0CTCR (*(volatile unsigned long *)0xE0004070)
#define T0CTCR_OFFSET 0x70
#define T0CTCR_Counter_Timer_Mode_MASK 0x3
#define T0CTCR_Counter_Timer_Mode_BIT 0
#define T0CTCR_Count_Input_Select_MASK 0xC
#define T0CTCR_Count_Input_Select_BIT 2

#define TIMER1_BASE 0xE0008000

#define T1IR (*(volatile unsigned char *)0xE0008000)
#define T1IR_OFFSET 0x0
#define T1IR_MR0_MASK 0x1
#define T1IR_MR0 0x1
#define T1IR_MR0_BIT 0
#define T1IR_MR1_MASK 0x2
#define T1IR_MR1 0x2
#define T1IR_MR1_BIT 1
#define T1IR_MR2_MASK 0x4
#define T1IR_MR2 0x4
#define T1IR_MR2_BIT 2
#define T1IR_MR3_MASK 0x8
#define T1IR_MR3 0x8
#define T1IR_MR3_BIT 3
#define T1IR_CR0_MASK 0x10
#define T1IR_CR0 0x10
#define T1IR_CR0_BIT 4
#define T1IR_CR1_MASK 0x20
#define T1IR_CR1 0x20
#define T1IR_CR1_BIT 5
#define T1IR_CR2_MASK 0x40
#define T1IR_CR2 0x40
#define T1IR_CR2_BIT 6
#define T1IR_CR3_MASK 0x80
#define T1IR_CR3 0x80
#define T1IR_CR3_BIT 7

#define T1TCR (*(volatile unsigned char *)0xE0008004)
#define T1TCR_OFFSET 0x4
#define T1TCR_Counter_Enable_MASK 0x1
#define T1TCR_Counter_Enable 0x1
#define T1TCR_Counter_Enable_BIT 0
#define T1TCR_Counter_Reset_MASK 0x2
#define T1TCR_Counter_Reset 0x2
#define T1TCR_Counter_Reset_BIT 1

#define T1TC (*(volatile unsigned long *)0xE0008008)
#define T1TC_OFFSET 0x8

#define T1PR (*(volatile unsigned long *)0xE000800C)
#define T1PR_OFFSET 0xC

#define T1PC (*(volatile unsigned long *)0xE0008010)
#define T1PC_OFFSET 0x10

#define T1MCR (*(volatile unsigned short *)0xE0008014)
#define T1MCR_OFFSET 0x14
#define T1MCR_MR0I_MASK 0x1
#define T1MCR_MR0I 0x1
#define T1MCR_MR0I_BIT 0
#define T1MCR_MR0R_MASK 0x2
#define T1MCR_MR0R 0x2
#define T1MCR_MR0R_BIT 1
#define T1MCR_MR0S_MASK 0x4
#define T1MCR_MR0S 0x4
#define T1MCR_MR0S_BIT 2
#define T1MCR_MR1I_MASK 0x8
#define T1MCR_MR1I 0x8
#define T1MCR_MR1I_BIT 3
#define T1MCR_MR1R_MASK 0x10
#define T1MCR_MR1R 0x10
#define T1MCR_MR1R_BIT 4
#define T1MCR_MR1S_MASK 0x20
#define T1MCR_MR1S 0x20
#define T1MCR_MR1S_BIT 5
#define T1MCR_MR2I_MASK 0x40
#define T1MCR_MR2I 0x40
#define T1MCR_MR2I_BIT 6
#define T1MCR_MR2R_MASK 0x80
#define T1MCR_MR2R 0x80
#define T1MCR_MR2R_BIT 7
#define T1MCR_MR2S_MASK 0x100
#define T1MCR_MR2S 0x100
#define T1MCR_MR2S_BIT 8
#define T1MCR_MR3I_MASK 0x200
#define T1MCR_MR3I 0x200
#define T1MCR_MR3I_BIT 9
#define T1MCR_MR3R_MASK 0x400
#define T1MCR_MR3R 0x400
#define T1MCR_MR3R_BIT 10
#define T1MCR_MR3S_MASK 0x800
#define T1MCR_MR3S 0x800
#define T1MCR_MR3S_BIT 11

#define T1MR0 (*(volatile unsigned long *)0xE0008018)
#define T1MR0_OFFSET 0x18

#define T1MR1 (*(volatile unsigned long *)0xE000801C)
#define T1MR1_OFFSET 0x1C

#define T1MR2 (*(volatile unsigned long *)0xE0008020)
#define T1MR2_OFFSET 0x20

#define T1MR3 (*(volatile unsigned long *)0xE0008024)
#define T1MR3_OFFSET 0x24

#define T1CCR (*(volatile unsigned short *)0xE0008028)
#define T1CCR_OFFSET 0x28
#define T1CCR_CAP0RE_MASK 0x1
#define T1CCR_CAP0RE 0x1
#define T1CCR_CAP0RE_BIT 0
#define T1CCR_CAP0FE_MASK 0x2
#define T1CCR_CAP0FE 0x2
#define T1CCR_CAP0FE_BIT 1
#define T1CCR_CAP0I_MASK 0x4
#define T1CCR_CAP0I 0x4
#define T1CCR_CAP0I_BIT 2
#define T1CCR_CAP1RE_MASK 0x8
#define T1CCR_CAP1RE 0x8
#define T1CCR_CAP1RE_BIT 3
#define T1CCR_CAP1FE_MASK 0x10
#define T1CCR_CAP1FE 0x10
#define T1CCR_CAP1FE_BIT 4
#define T1CCR_CAP1I_MASK 0x20
#define T1CCR_CAP1I 0x20
#define T1CCR_CAP1I_BIT 5
#define T1CCR_CAP2RE_MASK 0x40
#define T1CCR_CAP2RE 0x40
#define T1CCR_CAP2RE_BIT 6
#define T1CCR_CAP2FE_MASK 0x80
#define T1CCR_CAP2FE 0x80
#define T1CCR_CAP2FE_BIT 7
#define T1CCR_CAP2I_MASK 0x100
#define T1CCR_CAP2I 0x100
#define T1CCR_CAP2I_BIT 8
#define T1CCR_CAP3RE_MASK 0x200
#define T1CCR_CAP3RE 0x200
#define T1CCR_CAP3RE_BIT 9
#define T1CCR_CAP3FE_MASK 0x400
#define T1CCR_CAP3FE 0x400
#define T1CCR_CAP3FE_BIT 10
#define T1CCR_CAP3I_MASK 0x800
#define T1CCR_CAP3I 0x800
#define T1CCR_CAP3I_BIT 11

#define T1CR0 (*(volatile unsigned long *)0xE000802C)
#define T1CR0_OFFSET 0x2C

#define T1CR1 (*(volatile unsigned long *)0xE0008030)
#define T1CR1_OFFSET 0x30

#define T1CR2 (*(volatile unsigned long *)0xE0008034)
#define T1CR2_OFFSET 0x34

#define T1CR3 (*(volatile unsigned long *)0xE0008038)
#define T1CR3_OFFSET 0x38

#define T1EMR (*(volatile unsigned short *)0xE000803C)
#define T1EMR_OFFSET 0x3C
#define T1EMR_EM0_MASK 0x1
#define T1EMR_EM0 0x1
#define T1EMR_EM0_BIT 0
#define T1EMR_EM1_MASK 0x2
#define T1EMR_EM1 0x2
#define T1EMR_EM1_BIT 1
#define T1EMR_EM2_MASK 0x4
#define T1EMR_EM2 0x4
#define T1EMR_EM2_BIT 2
#define T1EMR_EM3_MASK 0x8
#define T1EMR_EM3 0x8
#define T1EMR_EM3_BIT 3
#define T1EMR_EMC0_MASK 0x30
#define T1EMR_EMC0_BIT 4
#define T1EMR_EMC1_MASK 0xC0
#define T1EMR_EMC1_BIT 6
#define T1EMR_EMC2_MASK 0x300
#define T1EMR_EMC2_BIT 8
#define T1EMR_EMC3_MASK 0xC00
#define T1EMR_EMC3_BIT 10

#define T1CTCR (*(volatile unsigned long *)0xE0008070)
#define T1CTCR_OFFSET 0x70
#define T1CTCR_Counter_Timer_Mode_MASK 0x3
#define T1CTCR_Counter_Timer_Mode_BIT 0
#define T1CTCR_Count_Input_Select_MASK 0xC
#define T1CTCR_Count_Input_Select_BIT 2

#define UART0_BASE 0xE000C000

#define U0RBR (*(volatile unsigned char *)0xE000C000)
#define U0RBR_OFFSET 0x0

#define U0THR (*(volatile unsigned char *)0xE000C000)
#define U0THR_OFFSET 0x0

#define U0DLL (*(volatile unsigned char *)0xE000C000)
#define U0DLL_OFFSET 0x0

#define U0DLM (*(volatile unsigned char *)0xE000C004)
#define U0DLM_OFFSET 0x4

#define U0IER (*(volatile unsigned long *)0xE000C004)
#define U0IER_OFFSET 0x4
#define U0IER_RBR_Interrupt_Enable_MASK 0x1
#define U0IER_RBR_Interrupt_Enable 0x1
#define U0IER_RBR_Interrupt_Enable_BIT 0
#define U0IER_THRE_Interrupt_Enable_MASK 0x2
#define U0IER_THRE_Interrupt_Enable 0x2
#define U0IER_THRE_Interrupt_Enable_BIT 1
#define U0IER_Rx_Line_Status_Interrupt_Enable_MASK 0x4
#define U0IER_Rx_Line_Status_Interrupt_Enable 0x4
#define U0IER_Rx_Line_Status_Interrupt_Enable_BIT 2
#define U0IER_ABTOIntEn_MASK 0x100
#define U0IER_ABTOIntEn 0x100
#define U0IER_ABTOIntEn_BIT 8
#define U0IER_ABEOIntEn_MASK 0x200
#define U0IER_ABEOIntEn 0x200
#define U0IER_ABEOIntEn_BIT 9

#define U0IIR (*(volatile unsigned long *)0xE000C008)
#define U0IIR_OFFSET 0x8
#define U0IIR_IntStatus_MASK 0x1
#define U0IIR_IntStatus 0x1
#define U0IIR_IntStatus_BIT 0
#define U0IIR_IntId_MASK 0xE
#define U0IIR_IntId_BIT 1
#define U0IIR_FIFO_Enable_MASK 0xC0
#define U0IIR_FIFO_Enable_BIT 6
#define U0IIR_ABEOInt_MASK 0x100
#define U0IIR_ABEOInt 0x100
#define U0IIR_ABEOInt_BIT 8
#define U0IIR_ABTOInt_MASK 0x200
#define U0IIR_ABTOInt 0x200
#define U0IIR_ABTOInt_BIT 9

#define U0FCR (*(volatile unsigned char *)0xE000C008)
#define U0FCR_OFFSET 0x8
#define U0FCR_FIFO_Enable_MASK 0x1
#define U0FCR_FIFO_Enable 0x1
#define U0FCR_FIFO_Enable_BIT 0
#define U0FCR_Rx_FIFO_Reset_MASK 0x2
#define U0FCR_Rx_FIFO_Reset 0x2
#define U0FCR_Rx_FIFO_Reset_BIT 1
#define U0FCR_Tx_FIFO_Reset_MASK 0x4
#define U0FCR_Tx_FIFO_Reset 0x4
#define U0FCR_Tx_FIFO_Reset_BIT 2
#define U0FCR_Rx_Trigger_Level_Select_MASK 0xC0
#define U0FCR_Rx_Trigger_Level_Select_BIT 6

#define U0LCR (*(volatile unsigned char *)0xE000C00C)
#define U0LCR_OFFSET 0xC
#define U0LCR_Word_Length_Select_MASK 0x3
#define U0LCR_Word_Length_Select_BIT 0
#define U0LCR_Stop_Bit_Select_MASK 0x4
#define U0LCR_Stop_Bit_Select 0x4
#define U0LCR_Stop_Bit_Select_BIT 2
#define U0LCR_Parity_Enable_MASK 0x8
#define U0LCR_Parity_Enable 0x8
#define U0LCR_Parity_Enable_BIT 3
#define U0LCR_Parity_Select_MASK 0x30
#define U0LCR_Parity_Select_BIT 4
#define U0LCR_Break_Control_MASK 0x40
#define U0LCR_Break_Control 0x40
#define U0LCR_Break_Control_BIT 6
#define U0LCR_Divisor_Latch_Access_Bit_MASK 0x80
#define U0LCR_Divisor_Latch_Access_Bit 0x80
#define U0LCR_Divisor_Latch_Access_Bit_BIT 7

#define U0LSR (*(volatile unsigned char *)0xE000C014)
#define U0LSR_OFFSET 0x14
#define U0LSR_RDR_MASK 0x1
#define U0LSR_RDR 0x1
#define U0LSR_RDR_BIT 0
#define U0LSR_OE_MASK 0x2
#define U0LSR_OE 0x2
#define U0LSR_OE_BIT 1
#define U0LSR_PE_MASK 0x4
#define U0LSR_PE 0x4
#define U0LSR_PE_BIT 2
#define U0LSR_FE_MASK 0x8
#define U0LSR_FE 0x8
#define U0LSR_FE_BIT 3
#define U0LSR_BI_MASK 0x10
#define U0LSR_BI 0x10
#define U0LSR_BI_BIT 4
#define U0LSR_THRE_MASK 0x20
#define U0LSR_THRE 0x20
#define U0LSR_THRE_BIT 5
#define U0LSR_TEMT_MASK 0x40
#define U0LSR_TEMT 0x40
#define U0LSR_TEMT_BIT 6
#define U0LSR_RXFE_MASK 0x80
#define U0LSR_RXFE 0x80
#define U0LSR_RXFE_BIT 7

#define U0SCR (*(volatile unsigned char *)0xE000C01C)
#define U0SCR_OFFSET 0x1C

#define U0ACR (*(volatile unsigned long *)0xE000C020)
#define U0ACR_OFFSET 0x20
#define U0ACR_Start_MASK 0x1
#define U0ACR_Start 0x1
#define U0ACR_Start_BIT 0
#define U0ACR_Mode_MASK 0x2
#define U0ACR_Mode 0x2
#define U0ACR_Mode_BIT 1
#define U0ACR_AutoRestart_MASK 0x4
#define U0ACR_AutoRestart 0x4
#define U0ACR_AutoRestart_BIT 2
#define U0ACR_ABEOIntClr_MASK 0x100
#define U0ACR_ABEOIntClr 0x100
#define U0ACR_ABEOIntClr_BIT 8
#define U0ACR_ABTOIntClr_MASK 0x200
#define U0ACR_ABTOIntClr 0x200
#define U0ACR_ABTOIntClr_BIT 9

#define U0FDR (*(volatile unsigned long *)0xE000C028)
#define U0FDR_OFFSET 0x28
#define U0FDR_DIVADDVAL_MASK 0xF
#define U0FDR_DIVADDVAL_BIT 0
#define U0FDR_MULVAL_MASK 0xF0
#define U0FDR_MULVAL_BIT 4

#define U0TER (*(volatile unsigned char *)0xE000C030)
#define U0TER_OFFSET 0x30
#define U0TER_TXEN_MASK 0x80
#define U0TER_TXEN 0x80
#define U0TER_TXEN_BIT 7

#define UART1_BASE 0xE0010000

#define U1RBR (*(volatile unsigned char *)0xE0010000)
#define U1RBR_OFFSET 0x0

#define U1THR (*(volatile unsigned char *)0xE0010000)
#define U1THR_OFFSET 0x0

#define U1DLL (*(volatile unsigned char *)0xE0010000)
#define U1DLL_OFFSET 0x0

#define U1DLM (*(volatile unsigned char *)0xE0010004)
#define U1DLM_OFFSET 0x4

#define U1IER (*(volatile unsigned long *)0xE0010004)
#define U1IER_OFFSET 0x4
#define U1IER_RBR_Interrupt_Enable_MASK 0x1
#define U1IER_RBR_Interrupt_Enable 0x1
#define U1IER_RBR_Interrupt_Enable_BIT 0
#define U1IER_THRE_Interrupt_Enable_MASK 0x2
#define U1IER_THRE_Interrupt_Enable 0x2
#define U1IER_THRE_Interrupt_Enable_BIT 1
#define U1IER_Rx_Line_Status_Interrupt_Enable_MASK 0x4
#define U1IER_Rx_Line_Status_Interrupt_Enable 0x4
#define U1IER_Rx_Line_Status_Interrupt_Enable_BIT 2
#define U1IER_Modem_Status_Interrupt_Enable_MASK 0x8
#define U1IER_Modem_Status_Interrupt_Enable 0x8
#define U1IER_Modem_Status_Interrupt_Enable_BIT 3
#define U1IER_CTS_Interrupt_Enable_MASK 0x80
#define U1IER_CTS_Interrupt_Enable 0x80
#define U1IER_CTS_Interrupt_Enable_BIT 7
#define U1IER_ABTOIntEn_MASK 0x100
#define U1IER_ABTOIntEn 0x100
#define U1IER_ABTOIntEn_BIT 8
#define U1IER_ABEOIntEn_MASK 0x200
#define U1IER_ABEOIntEn 0x200
#define U1IER_ABEOIntEn_BIT 9

#define U1IIR (*(volatile unsigned long *)0xE0010008)
#define U1IIR_OFFSET 0x8
#define U1IIR_Interrupt_Pending_MASK 0x1
#define U1IIR_Interrupt_Pending 0x1
#define U1IIR_Interrupt_Pending_BIT 0
#define U1IIR_Interrupt_Identification_MASK 0xE
#define U1IIR_Interrupt_Identification_BIT 1
#define U1IIR_FIFO_Enable_MASK 0xC0
#define U1IIR_FIFO_Enable_BIT 6
#define U1IIR_ABEOInt_MASK 0x100
#define U1IIR_ABEOInt 0x100
#define U1IIR_ABEOInt_BIT 8
#define U1IIR_ABTOInt_MASK 0x200
#define U1IIR_ABTOInt 0x200
#define U1IIR_ABTOInt_BIT 9

#define U1FCR (*(volatile unsigned char *)0xE0010008)
#define U1FCR_OFFSET 0x8
#define U1FCR_FIFO_Enable_MASK 0x1
#define U1FCR_FIFO_Enable 0x1
#define U1FCR_FIFO_Enable_BIT 0
#define U1FCR_Rx_FIFO_Reset_MASK 0x2
#define U1FCR_Rx_FIFO_Reset 0x2
#define U1FCR_Rx_FIFO_Reset_BIT 1
#define U1FCR_Tx_FIFO_Reset_MASK 0x4
#define U1FCR_Tx_FIFO_Reset 0x4
#define U1FCR_Tx_FIFO_Reset_BIT 2
#define U1FCR_Rx_Trigger_Level_Select_MASK 0xC0
#define U1FCR_Rx_Trigger_Level_Select_BIT 6

#define U1LCR (*(volatile unsigned char *)0xE001000C)
#define U1LCR_OFFSET 0xC
#define U1LCR_Word_Length_Select_MASK 0x3
#define U1LCR_Word_Length_Select_BIT 0
#define U1LCR_Stop_Bit_Select_MASK 0x4
#define U1LCR_Stop_Bit_Select 0x4
#define U1LCR_Stop_Bit_Select_BIT 2
#define U1LCR_Parity_Enable_MASK 0x8
#define U1LCR_Parity_Enable 0x8
#define U1LCR_Parity_Enable_BIT 3
#define U1LCR_Parity_Select_MASK 0x30
#define U1LCR_Parity_Select_BIT 4
#define U1LCR_Break_Control_MASK 0x40
#define U1LCR_Break_Control 0x40
#define U1LCR_Break_Control_BIT 6
#define U1LCR_Divisor_Latch_Access_Bit_MASK 0x80
#define U1LCR_Divisor_Latch_Access_Bit 0x80
#define U1LCR_Divisor_Latch_Access_Bit_BIT 7

#define U1MCR (*(volatile unsigned char *)0xE0010010)
#define U1MCR_OFFSET 0x10
#define U1MCR_DTR_Control_MASK 0x1
#define U1MCR_DTR_Control 0x1
#define U1MCR_DTR_Control_BIT 0
#define U1MCR_RTS_Control_MASK 0x2
#define U1MCR_RTS_Control 0x2
#define U1MCR_RTS_Control_BIT 1
#define U1MCR_Loopback_Mode_Select_MASK 0x10
#define U1MCR_Loopback_Mode_Select 0x10
#define U1MCR_Loopback_Mode_Select_BIT 4
#define U1MCR_RTSen_MASK 0x40
#define U1MCR_RTSen 0x40
#define U1MCR_RTSen_BIT 6
#define U1MCR_CTSen_MASK 0x80
#define U1MCR_CTSen 0x80
#define U1MCR_CTSen_BIT 7

#define U1LSR (*(volatile unsigned char *)0xE0010014)
#define U1LSR_OFFSET 0x14
#define U1LSR_RDR_MASK 0x1
#define U1LSR_RDR 0x1
#define U1LSR_RDR_BIT 0
#define U1LSR_OE_MASK 0x2
#define U1LSR_OE 0x2
#define U1LSR_OE_BIT 1
#define U1LSR_PE_MASK 0x4
#define U1LSR_PE 0x4
#define U1LSR_PE_BIT 2
#define U1LSR_FE_MASK 0x8
#define U1LSR_FE 0x8
#define U1LSR_FE_BIT 3
#define U1LSR_BI_MASK 0x10
#define U1LSR_BI 0x10
#define U1LSR_BI_BIT 4
#define U1LSR_THRE_MASK 0x20
#define U1LSR_THRE 0x20
#define U1LSR_THRE_BIT 5
#define U1LSR_TEMT_MASK 0x40
#define U1LSR_TEMT 0x40
#define U1LSR_TEMT_BIT 6
#define U1LSR_RXFE_MASK 0x80
#define U1LSR_RXFE 0x80
#define U1LSR_RXFE_BIT 7

#define U1MSR (*(volatile unsigned char *)0xE0010018)
#define U1MSR_OFFSET 0x18
#define U1MSR_Delta_CTS_MASK 0x1
#define U1MSR_Delta_CTS 0x1
#define U1MSR_Delta_CTS_BIT 0
#define U1MSR_Delta_DSR_MASK 0x2
#define U1MSR_Delta_DSR 0x2
#define U1MSR_Delta_DSR_BIT 1
#define U1MSR_Trailing_Edge_RI_MASK 0x4
#define U1MSR_Trailing_Edge_RI 0x4
#define U1MSR_Trailing_Edge_RI_BIT 2
#define U1MSR_Delta_DCD_MASK 0x8
#define U1MSR_Delta_DCD 0x8
#define U1MSR_Delta_DCD_BIT 3
#define U1MSR_CTS_MASK 0x10
#define U1MSR_CTS 0x10
#define U1MSR_CTS_BIT 4
#define U1MSR_DSR_MASK 0x20
#define U1MSR_DSR 0x20
#define U1MSR_DSR_BIT 5
#define U1MSR_RI_MASK 0x40
#define U1MSR_RI 0x40
#define U1MSR_RI_BIT 6
#define U1MSR_DCD_MASK 0x80
#define U1MSR_DCD 0x80
#define U1MSR_DCD_BIT 7

#define U1SCR (*(volatile unsigned char *)0xE001001C)
#define U1SCR_OFFSET 0x1C

#define U1ACR (*(volatile unsigned long *)0xE0010020)
#define U1ACR_OFFSET 0x20
#define U1ACR_Start_MASK 0x1
#define U1ACR_Start 0x1
#define U1ACR_Start_BIT 0
#define U1ACR_Mode_MASK 0x2
#define U1ACR_Mode 0x2
#define U1ACR_Mode_BIT 1
#define U1ACR_AutoRestart_MASK 0x4
#define U1ACR_AutoRestart 0x4
#define U1ACR_AutoRestart_BIT 2
#define U1ACR_ABEOIntClr_MASK 0x100
#define U1ACR_ABEOIntClr 0x100
#define U1ACR_ABEOIntClr_BIT 8
#define U1ACR_ABTOIntClr_MASK 0x200
#define U1ACR_ABTOIntClr 0x200
#define U1ACR_ABTOIntClr_BIT 9

#define U1FDR (*(volatile unsigned long *)0xE0010028)
#define U1FDR_OFFSET 0x28
#define U1FDR_DIVADDVAL_MASK 0xF
#define U1FDR_DIVADDVAL_BIT 0
#define U1FDR_MULVAL_MASK 0xF0
#define U1FDR_MULVAL_BIT 4

#define U1TER (*(volatile unsigned char *)0xE0010030)
#define U1TER_OFFSET 0x30
#define U1TER_TXEN_MASK 0x80
#define U1TER_TXEN 0x80
#define U1TER_TXEN_BIT 7

#define PWM0_BASE 0xE0014000

#define PWM0IR (*(volatile unsigned long *)0xE0014000)
#define PWM0IR_OFFSET 0x0
#define PWM0IR_PWMMR0_Interrupt_MASK 0x1
#define PWM0IR_PWMMR0_Interrupt 0x1
#define PWM0IR_PWMMR0_Interrupt_BIT 0
#define PWM0IR_PWMMR1_Interrupt_MASK 0x2
#define PWM0IR_PWMMR1_Interrupt 0x2
#define PWM0IR_PWMMR1_Interrupt_BIT 1
#define PWM0IR_PWMMR2_Interrupt_MASK 0x4
#define PWM0IR_PWMMR2_Interrupt 0x4
#define PWM0IR_PWMMR2_Interrupt_BIT 2
#define PWM0IR_PWMMR3_Interrupt_MASK 0x8
#define PWM0IR_PWMMR3_Interrupt 0x8
#define PWM0IR_PWMMR3_Interrupt_BIT 3
#define PWM0IR_PWMMR4_Interrupt_MASK 0x100
#define PWM0IR_PWMMR4_Interrupt 0x100
#define PWM0IR_PWMMR4_Interrupt_BIT 8
#define PWM0IR_PWMMR5_Interrupt_MASK 0x200
#define PWM0IR_PWMMR5_Interrupt 0x200
#define PWM0IR_PWMMR5_Interrupt_BIT 9
#define PWM0IR_PWMMR6_Interrupt_MASK 0x400
#define PWM0IR_PWMMR6_Interrupt 0x400
#define PWM0IR_PWMMR6_Interrupt_BIT 10

#define PWM0TCR (*(volatile unsigned long *)0xE0014004)
#define PWM0TCR_OFFSET 0x4
#define PWM0TCR_Counter_Enable_MASK 0x1
#define PWM0TCR_Counter_Enable 0x1
#define PWM0TCR_Counter_Enable_BIT 0
#define PWM0TCR_Counter_Reset_MASK 0x2
#define PWM0TCR_Counter_Reset 0x2
#define PWM0TCR_Counter_Reset_BIT 1
#define PWM0TCR_PWM_Enable_MASK 0x8
#define PWM0TCR_PWM_Enable 0x8
#define PWM0TCR_PWM_Enable_BIT 3

#define PWM0TC (*(volatile unsigned long *)0xE0014008)
#define PWM0TC_OFFSET 0x8

#define PWM0PR (*(volatile unsigned long *)0xE001400C)
#define PWM0PR_OFFSET 0xC

#define PWM0PC (*(volatile unsigned long *)0xE0014010)
#define PWM0PC_OFFSET 0x10

#define PWM0MCR (*(volatile unsigned long *)0xE0014014)
#define PWM0MCR_OFFSET 0x14
#define PWM0MCR_PWMMR0I_MASK 0x1
#define PWM0MCR_PWMMR0I 0x1
#define PWM0MCR_PWMMR0I_BIT 0
#define PWM0MCR_PWMMR0R_MASK 0x2
#define PWM0MCR_PWMMR0R 0x2
#define PWM0MCR_PWMMR0R_BIT 1
#define PWM0MCR_PWMMR0S_MASK 0x4
#define PWM0MCR_PWMMR0S 0x4
#define PWM0MCR_PWMMR0S_BIT 2
#define PWM0MCR_PWMMR1I_MASK 0x8
#define PWM0MCR_PWMMR1I 0x8
#define PWM0MCR_PWMMR1I_BIT 3
#define PWM0MCR_PWMMR1R_MASK 0x10
#define PWM0MCR_PWMMR1R 0x10
#define PWM0MCR_PWMMR1R_BIT 4
#define PWM0MCR_PWMMR1S_MASK 0x20
#define PWM0MCR_PWMMR1S 0x20
#define PWM0MCR_PWMMR1S_BIT 5
#define PWM0MCR_PWMMR2I_MASK 0x40
#define PWM0MCR_PWMMR2I 0x40
#define PWM0MCR_PWMMR2I_BIT 6
#define PWM0MCR_PWMMR2R_MASK 0x80
#define PWM0MCR_PWMMR2R 0x80
#define PWM0MCR_PWMMR2R_BIT 7
#define PWM0MCR_PWMMR2S_MASK 0x100
#define PWM0MCR_PWMMR2S 0x100
#define PWM0MCR_PWMMR2S_BIT 8
#define PWM0MCR_PWMMR3I_MASK 0x200
#define PWM0MCR_PWMMR3I 0x200
#define PWM0MCR_PWMMR3I_BIT 9
#define PWM0MCR_PWMMR3R_MASK 0x400
#define PWM0MCR_PWMMR3R 0x400
#define PWM0MCR_PWMMR3R_BIT 10
#define PWM0MCR_PWMMR3S_MASK 0x800
#define PWM0MCR_PWMMR3S 0x800
#define PWM0MCR_PWMMR3S_BIT 11
#define PWM0MCR_PWMMR4I_MASK 0x1000
#define PWM0MCR_PWMMR4I 0x1000
#define PWM0MCR_PWMMR4I_BIT 12
#define PWM0MCR_PWMMR4R_MASK 0x2000
#define PWM0MCR_PWMMR4R 0x2000
#define PWM0MCR_PWMMR4R_BIT 13
#define PWM0MCR_PWMMR4S_MASK 0x4000
#define PWM0MCR_PWMMR4S 0x4000
#define PWM0MCR_PWMMR4S_BIT 14
#define PWM0MCR_PWMMR5I_MASK 0x8000
#define PWM0MCR_PWMMR5I 0x8000
#define PWM0MCR_PWMMR5I_BIT 15
#define PWM0MCR_PWMMR5R_MASK 0x10000
#define PWM0MCR_PWMMR5R 0x10000
#define PWM0MCR_PWMMR5R_BIT 16
#define PWM0MCR_PWMMR5S_MASK 0x20000
#define PWM0MCR_PWMMR5S 0x20000
#define PWM0MCR_PWMMR5S_BIT 17
#define PWM0MCR_PWMMR6I_MASK 0x40000
#define PWM0MCR_PWMMR6I 0x40000
#define PWM0MCR_PWMMR6I_BIT 18
#define PWM0MCR_PWMMR6R_MASK 0x80000
#define PWM0MCR_PWMMR6R 0x80000
#define PWM0MCR_PWMMR6R_BIT 19
#define PWM0MCR_PWMMR6S_MASK 0x100000
#define PWM0MCR_PWMMR6S 0x100000
#define PWM0MCR_PWMMR6S_BIT 20

#define PWM0MR0 (*(volatile unsigned long *)0xE0014018)
#define PWM0MR0_OFFSET 0x18

#define PWM0MR1 (*(volatile unsigned long *)0xE001401C)
#define PWM0MR1_OFFSET 0x1C

#define PWM0MR2 (*(volatile unsigned long *)0xE0014020)
#define PWM0MR2_OFFSET 0x20

#define PWM0MR3 (*(volatile unsigned long *)0xE0014024)
#define PWM0MR3_OFFSET 0x24

#define PWM0MR4 (*(volatile unsigned long *)0xE0014040)
#define PWM0MR4_OFFSET 0x40

#define PWM0MR5 (*(volatile unsigned long *)0xE0014044)
#define PWM0MR5_OFFSET 0x44

#define PWM0MR6 (*(volatile unsigned long *)0xE0014048)
#define PWM0MR6_OFFSET 0x48

#define PWM0PCR (*(volatile unsigned long *)0xE001404C)
#define PWM0PCR_OFFSET 0x4C
#define PWM0PCR_PWMSEL2_MASK 0x4
#define PWM0PCR_PWMSEL2 0x4
#define PWM0PCR_PWMSEL2_BIT 2
#define PWM0PCR_PWMSEL3_MASK 0x8
#define PWM0PCR_PWMSEL3 0x8
#define PWM0PCR_PWMSEL3_BIT 3
#define PWM0PCR_PWMSEL4_MASK 0x10
#define PWM0PCR_PWMSEL4 0x10
#define PWM0PCR_PWMSEL4_BIT 4
#define PWM0PCR_PWMSEL5_MASK 0x20
#define PWM0PCR_PWMSEL5 0x20
#define PWM0PCR_PWMSEL5_BIT 5
#define PWM0PCR_PWMSEL6_MASK 0x40
#define PWM0PCR_PWMSEL6 0x40
#define PWM0PCR_PWMSEL6_BIT 6
#define PWM0PCR_PWMENA1_MASK 0x200
#define PWM0PCR_PWMENA1 0x200
#define PWM0PCR_PWMENA1_BIT 9
#define PWM0PCR_PWMENA2_MASK 0x400
#define PWM0PCR_PWMENA2 0x400
#define PWM0PCR_PWMENA2_BIT 10
#define PWM0PCR_PWMENA3_MASK 0x800
#define PWM0PCR_PWMENA3 0x800
#define PWM0PCR_PWMENA3_BIT 11
#define PWM0PCR_PWMENA4_MASK 0x1000
#define PWM0PCR_PWMENA4 0x1000
#define PWM0PCR_PWMENA4_BIT 12
#define PWM0PCR_PWMENA5_MASK 0x2000
#define PWM0PCR_PWMENA5 0x2000
#define PWM0PCR_PWMENA5_BIT 13
#define PWM0PCR_PWMENA6_MASK 0x4000
#define PWM0PCR_PWMENA6 0x4000
#define PWM0PCR_PWMENA6_BIT 14

#define PWM0LER (*(volatile unsigned long *)0xE0014050)
#define PWM0LER_OFFSET 0x50
#define PWM0LER_Enable_PWM_Match_0_Latch_MASK 0x1
#define PWM0LER_Enable_PWM_Match_0_Latch 0x1
#define PWM0LER_Enable_PWM_Match_0_Latch_BIT 0
#define PWM0LER_Enable_PWM_Match_1_Latch_MASK 0x2
#define PWM0LER_Enable_PWM_Match_1_Latch 0x2
#define PWM0LER_Enable_PWM_Match_1_Latch_BIT 1
#define PWM0LER_Enable_PWM_Match_2_Latch_MASK 0x4
#define PWM0LER_Enable_PWM_Match_2_Latch 0x4
#define PWM0LER_Enable_PWM_Match_2_Latch_BIT 2
#define PWM0LER_Enable_PWM_Match_3_Latch_MASK 0x8
#define PWM0LER_Enable_PWM_Match_3_Latch 0x8
#define PWM0LER_Enable_PWM_Match_3_Latch_BIT 3
#define PWM0LER_Enable_PWM_Match_4_Latch_MASK 0x10
#define PWM0LER_Enable_PWM_Match_4_Latch 0x10
#define PWM0LER_Enable_PWM_Match_4_Latch_BIT 4
#define PWM0LER_Enable_PWM_Match_5_Latch_MASK 0x20
#define PWM0LER_Enable_PWM_Match_5_Latch 0x20
#define PWM0LER_Enable_PWM_Match_5_Latch_BIT 5
#define PWM0LER_Enable_PWM_Match_6_Latch_MASK 0x40
#define PWM0LER_Enable_PWM_Match_6_Latch 0x40
#define PWM0LER_Enable_PWM_Match_6_Latch_BIT 6

#define PWM1_BASE 0xE0018000

#define PWM1IR (*(volatile unsigned long *)0xE0018000)
#define PWM1IR_OFFSET 0x0
#define PWM1IR_PWMMR0_Interrupt_MASK 0x1
#define PWM1IR_PWMMR0_Interrupt 0x1
#define PWM1IR_PWMMR0_Interrupt_BIT 0
#define PWM1IR_PWMMR1_Interrupt_MASK 0x2
#define PWM1IR_PWMMR1_Interrupt 0x2
#define PWM1IR_PWMMR1_Interrupt_BIT 1
#define PWM1IR_PWMMR2_Interrupt_MASK 0x4
#define PWM1IR_PWMMR2_Interrupt 0x4
#define PWM1IR_PWMMR2_Interrupt_BIT 2
#define PWM1IR_PWMMR3_Interrupt_MASK 0x8
#define PWM1IR_PWMMR3_Interrupt 0x8
#define PWM1IR_PWMMR3_Interrupt_BIT 3
#define PWM1IR_PWMMR4_Interrupt_MASK 0x100
#define PWM1IR_PWMMR4_Interrupt 0x100
#define PWM1IR_PWMMR4_Interrupt_BIT 8
#define PWM1IR_PWMMR5_Interrupt_MASK 0x200
#define PWM1IR_PWMMR5_Interrupt 0x200
#define PWM1IR_PWMMR5_Interrupt_BIT 9
#define PWM1IR_PWMMR6_Interrupt_MASK 0x400
#define PWM1IR_PWMMR6_Interrupt 0x400
#define PWM1IR_PWMMR6_Interrupt_BIT 10

#define PWM1TCR (*(volatile unsigned long *)0xE0018004)
#define PWM1TCR_OFFSET 0x4
#define PWM1TCR_Counter_Enable_MASK 0x1
#define PWM1TCR_Counter_Enable 0x1
#define PWM1TCR_Counter_Enable_BIT 0
#define PWM1TCR_Counter_Reset_MASK 0x2
#define PWM1TCR_Counter_Reset 0x2
#define PWM1TCR_Counter_Reset_BIT 1
#define PWM1TCR_PWM_Enable_MASK 0x8
#define PWM1TCR_PWM_Enable 0x8
#define PWM1TCR_PWM_Enable_BIT 3

#define PWM1TC (*(volatile unsigned long *)0xE0018008)
#define PWM1TC_OFFSET 0x8

#define PWM1PR (*(volatile unsigned long *)0xE001800C)
#define PWM1PR_OFFSET 0xC

#define PWM1PC (*(volatile unsigned long *)0xE0018010)
#define PWM1PC_OFFSET 0x10

#define PWM1MCR (*(volatile unsigned long *)0xE0018014)
#define PWM1MCR_OFFSET 0x14
#define PWM1MCR_PWMMR0I_MASK 0x1
#define PWM1MCR_PWMMR0I 0x1
#define PWM1MCR_PWMMR0I_BIT 0
#define PWM1MCR_PWMMR0R_MASK 0x2
#define PWM1MCR_PWMMR0R 0x2
#define PWM1MCR_PWMMR0R_BIT 1
#define PWM1MCR_PWMMR0S_MASK 0x4
#define PWM1MCR_PWMMR0S 0x4
#define PWM1MCR_PWMMR0S_BIT 2
#define PWM1MCR_PWMMR1I_MASK 0x8
#define PWM1MCR_PWMMR1I 0x8
#define PWM1MCR_PWMMR1I_BIT 3
#define PWM1MCR_PWMMR1R_MASK 0x10
#define PWM1MCR_PWMMR1R 0x10
#define PWM1MCR_PWMMR1R_BIT 4
#define PWM1MCR_PWMMR1S_MASK 0x20
#define PWM1MCR_PWMMR1S 0x20
#define PWM1MCR_PWMMR1S_BIT 5
#define PWM1MCR_PWMMR2I_MASK 0x40
#define PWM1MCR_PWMMR2I 0x40
#define PWM1MCR_PWMMR2I_BIT 6
#define PWM1MCR_PWMMR2R_MASK 0x80
#define PWM1MCR_PWMMR2R 0x80
#define PWM1MCR_PWMMR2R_BIT 7
#define PWM1MCR_PWMMR2S_MASK 0x100
#define PWM1MCR_PWMMR2S 0x100
#define PWM1MCR_PWMMR2S_BIT 8
#define PWM1MCR_PWMMR3I_MASK 0x200
#define PWM1MCR_PWMMR3I 0x200
#define PWM1MCR_PWMMR3I_BIT 9
#define PWM1MCR_PWMMR3R_MASK 0x400
#define PWM1MCR_PWMMR3R 0x400
#define PWM1MCR_PWMMR3R_BIT 10
#define PWM1MCR_PWMMR3S_MASK 0x800
#define PWM1MCR_PWMMR3S 0x800
#define PWM1MCR_PWMMR3S_BIT 11
#define PWM1MCR_PWMMR4I_MASK 0x1000
#define PWM1MCR_PWMMR4I 0x1000
#define PWM1MCR_PWMMR4I_BIT 12
#define PWM1MCR_PWMMR4R_MASK 0x2000
#define PWM1MCR_PWMMR4R 0x2000
#define PWM1MCR_PWMMR4R_BIT 13
#define PWM1MCR_PWMMR4S_MASK 0x4000
#define PWM1MCR_PWMMR4S 0x4000
#define PWM1MCR_PWMMR4S_BIT 14
#define PWM1MCR_PWMMR5I_MASK 0x8000
#define PWM1MCR_PWMMR5I 0x8000
#define PWM1MCR_PWMMR5I_BIT 15
#define PWM1MCR_PWMMR5R_MASK 0x10000
#define PWM1MCR_PWMMR5R 0x10000
#define PWM1MCR_PWMMR5R_BIT 16
#define PWM1MCR_PWMMR5S_MASK 0x20000
#define PWM1MCR_PWMMR5S 0x20000
#define PWM1MCR_PWMMR5S_BIT 17
#define PWM1MCR_PWMMR6I_MASK 0x40000
#define PWM1MCR_PWMMR6I 0x40000
#define PWM1MCR_PWMMR6I_BIT 18
#define PWM1MCR_PWMMR6R_MASK 0x80000
#define PWM1MCR_PWMMR6R 0x80000
#define PWM1MCR_PWMMR6R_BIT 19
#define PWM1MCR_PWMMR6S_MASK 0x100000
#define PWM1MCR_PWMMR6S 0x100000
#define PWM1MCR_PWMMR6S_BIT 20

#define PWM1MR0 (*(volatile unsigned long *)0xE0018018)
#define PWM1MR0_OFFSET 0x18

#define PWM1MR1 (*(volatile unsigned long *)0xE001801C)
#define PWM1MR1_OFFSET 0x1C

#define PWM1MR2 (*(volatile unsigned long *)0xE0018020)
#define PWM1MR2_OFFSET 0x20

#define PWM1MR3 (*(volatile unsigned long *)0xE0018024)
#define PWM1MR3_OFFSET 0x24

#define PWM1CCR (*(volatile unsigned long *)0xE0018028)
#define PWM1CCR_OFFSET 0x28
#define PWM1CCR_CAP0RE_MASK 0x1
#define PWM1CCR_CAP0RE 0x1
#define PWM1CCR_CAP0RE_BIT 0
#define PWM1CCR_CAP0FE_MASK 0x2
#define PWM1CCR_CAP0FE 0x2
#define PWM1CCR_CAP0FE_BIT 1
#define PWM1CCR_CAP0I_MASK 0x4
#define PWM1CCR_CAP0I 0x4
#define PWM1CCR_CAP0I_BIT 2
#define PWM1CCR_CAP1RE_MASK 0x8
#define PWM1CCR_CAP1RE 0x8
#define PWM1CCR_CAP1RE_BIT 3
#define PWM1CCR_CAP1FE_MASK 0x10
#define PWM1CCR_CAP1FE 0x10
#define PWM1CCR_CAP1FE_BIT 4
#define PWM1CCR_CAP1I_MASK 0x20
#define PWM1CCR_CAP1I 0x20
#define PWM1CCR_CAP1I_BIT 5
#define PWM1CCR_CAP2RE_MASK 0x80
#define PWM1CCR_CAP2RE 0x80
#define PWM1CCR_CAP2RE_BIT 7
#define PWM1CCR_CAP2FE_MASK 0x80
#define PWM1CCR_CAP2FE 0x80
#define PWM1CCR_CAP2FE_BIT 7
#define PWM1CCR_CAP2I_MASK 0x100
#define PWM1CCR_CAP2I 0x100
#define PWM1CCR_CAP2I_BIT 8
#define PWM1CCR_CAP3RE_MASK 0x200
#define PWM1CCR_CAP3RE 0x200
#define PWM1CCR_CAP3RE_BIT 9
#define PWM1CCR_CAP3FE_MASK 0x400
#define PWM1CCR_CAP3FE 0x400
#define PWM1CCR_CAP3FE_BIT 10
#define PWM1CCR_CAP3I_MASK 0x800
#define PWM1CCR_CAP3I 0x800
#define PWM1CCR_CAP3I_BIT 11

#define PWM1CR0 (*(volatile unsigned long *)0xE001802C)
#define PWM1CR0_OFFSET 0x2C

#define PWM1CR1 (*(volatile unsigned long *)0xE0018030)
#define PWM1CR1_OFFSET 0x30

#define PWM1CR2 (*(volatile unsigned long *)0xE0018034)
#define PWM1CR2_OFFSET 0x34

#define PWM1CR3 (*(volatile unsigned long *)0xE0018038)
#define PWM1CR3_OFFSET 0x38

#define PWM1EMR (*(volatile unsigned long *)0xE001803C)
#define PWM1EMR_OFFSET 0x3C
#define PWM1EMR_EM0_MASK 0x1
#define PWM1EMR_EM0 0x1
#define PWM1EMR_EM0_BIT 0
#define PWM1EMR_EM1_MASK 0x2
#define PWM1EMR_EM1 0x2
#define PWM1EMR_EM1_BIT 1
#define PWM1EMR_EM2_MASK 0x4
#define PWM1EMR_EM2 0x4
#define PWM1EMR_EM2_BIT 2
#define PWM1EMR_EM3_MASK 0x8
#define PWM1EMR_EM3 0x8
#define PWM1EMR_EM3_BIT 3
#define PWM1EMR_EMC0_MASK 0x30
#define PWM1EMR_EMC0_BIT 4
#define PWM1EMR_EMC1_MASK 0xC0
#define PWM1EMR_EMC1_BIT 6
#define PWM1EMR_EMC2_MASK 0x300
#define PWM1EMR_EMC2_BIT 8
#define PWM1EMR_EMC3_MASK 0xC00
#define PWM1EMR_EMC3_BIT 10

#define PWM1MR4 (*(volatile unsigned long *)0xE0018040)
#define PWM1MR4_OFFSET 0x40

#define PWM1MR5 (*(volatile unsigned long *)0xE0018044)
#define PWM1MR5_OFFSET 0x44

#define PWM1MR6 (*(volatile unsigned long *)0xE0018048)
#define PWM1MR6_OFFSET 0x48

#define PWM1PCR (*(volatile unsigned long *)0xE001804C)
#define PWM1PCR_OFFSET 0x4C
#define PWM1PCR_PWMSEL2_MASK 0x4
#define PWM1PCR_PWMSEL2 0x4
#define PWM1PCR_PWMSEL2_BIT 2
#define PWM1PCR_PWMSEL3_MASK 0x8
#define PWM1PCR_PWMSEL3 0x8
#define PWM1PCR_PWMSEL3_BIT 3
#define PWM1PCR_PWMSEL4_MASK 0x10
#define PWM1PCR_PWMSEL4 0x10
#define PWM1PCR_PWMSEL4_BIT 4
#define PWM1PCR_PWMSEL5_MASK 0x20
#define PWM1PCR_PWMSEL5 0x20
#define PWM1PCR_PWMSEL5_BIT 5
#define PWM1PCR_PWMSEL6_MASK 0x40
#define PWM1PCR_PWMSEL6 0x40
#define PWM1PCR_PWMSEL6_BIT 6
#define PWM1PCR_PWMENA1_MASK 0x200
#define PWM1PCR_PWMENA1 0x200
#define PWM1PCR_PWMENA1_BIT 9
#define PWM1PCR_PWMENA2_MASK 0x400
#define PWM1PCR_PWMENA2 0x400
#define PWM1PCR_PWMENA2_BIT 10
#define PWM1PCR_PWMENA3_MASK 0x800
#define PWM1PCR_PWMENA3 0x800
#define PWM1PCR_PWMENA3_BIT 11
#define PWM1PCR_PWMENA4_MASK 0x1000
#define PWM1PCR_PWMENA4 0x1000
#define PWM1PCR_PWMENA4_BIT 12
#define PWM1PCR_PWMENA5_MASK 0x2000
#define PWM1PCR_PWMENA5 0x2000
#define PWM1PCR_PWMENA5_BIT 13
#define PWM1PCR_PWMENA6_MASK 0x4000
#define PWM1PCR_PWMENA6 0x4000
#define PWM1PCR_PWMENA6_BIT 14

#define PWM1LER (*(volatile unsigned long *)0xE0018050)
#define PWM1LER_OFFSET 0x50
#define PWM1LER_Enable_PWM_Match_0_Latch_MASK 0x1
#define PWM1LER_Enable_PWM_Match_0_Latch 0x1
#define PWM1LER_Enable_PWM_Match_0_Latch_BIT 0
#define PWM1LER_Enable_PWM_Match_1_Latch_MASK 0x2
#define PWM1LER_Enable_PWM_Match_1_Latch 0x2
#define PWM1LER_Enable_PWM_Match_1_Latch_BIT 1
#define PWM1LER_Enable_PWM_Match_2_Latch_MASK 0x4
#define PWM1LER_Enable_PWM_Match_2_Latch 0x4
#define PWM1LER_Enable_PWM_Match_2_Latch_BIT 2
#define PWM1LER_Enable_PWM_Match_3_Latch_MASK 0x8
#define PWM1LER_Enable_PWM_Match_3_Latch 0x8
#define PWM1LER_Enable_PWM_Match_3_Latch_BIT 3
#define PWM1LER_Enable_PWM_Match_4_Latch_MASK 0x10
#define PWM1LER_Enable_PWM_Match_4_Latch 0x10
#define PWM1LER_Enable_PWM_Match_4_Latch_BIT 4
#define PWM1LER_Enable_PWM_Match_5_Latch_MASK 0x20
#define PWM1LER_Enable_PWM_Match_5_Latch 0x20
#define PWM1LER_Enable_PWM_Match_5_Latch_BIT 5
#define PWM1LER_Enable_PWM_Match_6_Latch_MASK 0x40
#define PWM1LER_Enable_PWM_Match_6_Latch 0x40
#define PWM1LER_Enable_PWM_Match_6_Latch_BIT 6

#define PWM1CTCR (*(volatile unsigned long *)0xE0018070)
#define PWM1CTCR_OFFSET 0x70
#define PWM1CTCR_Counter_Timer_Mode_MASK 0x3
#define PWM1CTCR_Counter_Timer_Mode_BIT 0
#define PWM1CTCR_Count_Input_Select_MASK 0xC
#define PWM1CTCR_Count_Input_Select_BIT 2

#define I2C0_BASE 0xE001C000

#define I2C0CONSET (*(volatile unsigned char *)0xE001C000)
#define I2C0CONSET_OFFSET 0x0
#define I2C0CONSET_AA_MASK 0x4
#define I2C0CONSET_AA 0x4
#define I2C0CONSET_AA_BIT 2
#define I2C0CONSET_SI_MASK 0x8
#define I2C0CONSET_SI 0x8
#define I2C0CONSET_SI_BIT 3
#define I2C0CONSET_STO_MASK 0x10
#define I2C0CONSET_STO 0x10
#define I2C0CONSET_STO_BIT 4
#define I2C0CONSET_STA_MASK 0x20
#define I2C0CONSET_STA 0x20
#define I2C0CONSET_STA_BIT 5
#define I2C0CONSET_I2EN_MASK 0x40
#define I2C0CONSET_I2EN 0x40
#define I2C0CONSET_I2EN_BIT 6

#define I2C0STAT (*(volatile unsigned char *)0xE001C004)
#define I2C0STAT_OFFSET 0x4
#define I2C0STAT_Status_MASK 0x7C
#define I2C0STAT_Status_BIT 2

#define I2C0DAT (*(volatile unsigned char *)0xE001C008)
#define I2C0DAT_OFFSET 0x8

#define I2C0ADR (*(volatile unsigned char *)0xE001C00C)
#define I2C0ADR_OFFSET 0xC
#define I2C0ADR_GC_MASK 0x1
#define I2C0ADR_GC 0x1
#define I2C0ADR_GC_BIT 0
#define I2C0ADR_Address_MASK 0x7E
#define I2C0ADR_Address_BIT 1

#define I2C0SCLH (*(volatile unsigned short *)0xE001C010)
#define I2C0SCLH_OFFSET 0x10

#define I2C0SCLL (*(volatile unsigned short *)0xE001C014)
#define I2C0SCLL_OFFSET 0x14

#define I2C0CONCLR (*(volatile unsigned char *)0xE001C018)
#define I2C0CONCLR_OFFSET 0x18
#define I2C0CONCLR_AAC_MASK 0x4
#define I2C0CONCLR_AAC 0x4
#define I2C0CONCLR_AAC_BIT 2
#define I2C0CONCLR_SIC_MASK 0x8
#define I2C0CONCLR_SIC 0x8
#define I2C0CONCLR_SIC_BIT 3
#define I2C0CONCLR_STAC_MASK 0x20
#define I2C0CONCLR_STAC 0x20
#define I2C0CONCLR_STAC_BIT 5
#define I2C0CONCLR_I2ENC_MASK 0x40
#define I2C0CONCLR_I2ENC 0x40
#define I2C0CONCLR_I2ENC_BIT 6

#define SPI0_BASE 0xE0020000

#define S0SPCR (*(volatile unsigned short *)0xE0020000)
#define S0SPCR_OFFSET 0x0
#define S0SPCR_BitEnable_MASK 0x4
#define S0SPCR_BitEnable 0x4
#define S0SPCR_BitEnable_BIT 2
#define S0SPCR_CPHA_MASK 0x8
#define S0SPCR_CPHA 0x8
#define S0SPCR_CPHA_BIT 3
#define S0SPCR_CPOL_MASK 0x10
#define S0SPCR_CPOL 0x10
#define S0SPCR_CPOL_BIT 4
#define S0SPCR_MSTR_MASK 0x20
#define S0SPCR_MSTR 0x20
#define S0SPCR_MSTR_BIT 5
#define S0SPCR_LSBF_MASK 0x40
#define S0SPCR_LSBF 0x40
#define S0SPCR_LSBF_BIT 6
#define S0SPCR_SPIE_MASK 0x80
#define S0SPCR_SPIE 0x80
#define S0SPCR_SPIE_BIT 7
#define S0SPCR_BITS_MASK 0xF00
#define S0SPCR_BITS_BIT 8

#define S0SPSR (*(volatile unsigned char *)0xE0020004)
#define S0SPSR_OFFSET 0x4
#define S0SPSR_ABRT_MASK 0x8
#define S0SPSR_ABRT 0x8
#define S0SPSR_ABRT_BIT 3
#define S0SPSR_MODF_MASK 0x10
#define S0SPSR_MODF 0x10
#define S0SPSR_MODF_BIT 4
#define S0SPSR_ROVR_MASK 0x20
#define S0SPSR_ROVR 0x20
#define S0SPSR_ROVR_BIT 5
#define S0SPSR_WCOL_MASK 0x40
#define S0SPSR_WCOL 0x40
#define S0SPSR_WCOL_BIT 6
#define S0SPSR_SPIF_MASK 0x80
#define S0SPSR_SPIF 0x80
#define S0SPSR_SPIF_BIT 7

#define S0SPDR (*(volatile unsigned short *)0xE0020008)
#define S0SPDR_OFFSET 0x8

#define S0SPCCR (*(volatile unsigned char *)0xE002000C)
#define S0SPCCR_OFFSET 0xC

#define S0SPINT (*(volatile unsigned char *)0xE002001C)
#define S0SPINT_OFFSET 0x1C

#define RTC_BASE 0xE0024000

#define ILR (*(volatile unsigned long *)0xE0024000)
#define ILR_OFFSET 0x0
#define ILR_RTCCIF_MASK 0x1
#define ILR_RTCCIF 0x1
#define ILR_RTCCIF_BIT 0
#define ILR_RTCALF_MASK 0x2
#define ILR_RTCALF 0x2
#define ILR_RTCALF_BIT 1

#define CTC (*(volatile unsigned long *)0xE0024004)
#define CTC_OFFSET 0x4
#define CTC_Clock_Tick_Counter_MASK 0xFFFE
#define CTC_Clock_Tick_Counter_BIT 1

#define CCR (*(volatile unsigned long *)0xE0024008)
#define CCR_OFFSET 0x8
#define CCR_CLKEN_MASK 0x1
#define CCR_CLKEN 0x1
#define CCR_CLKEN_BIT 0
#define CCR_CTCRST_MASK 0x2
#define CCR_CTCRST 0x2
#define CCR_CTCRST_BIT 1
#define CCR_CTTEST_MASK 0xC
#define CCR_CTTEST_BIT 2
#define CCR_CLKSRC_MASK 0x10
#define CCR_CLKSRC 0x10
#define CCR_CLKSRC_BIT 4

#define CIIR (*(volatile unsigned long *)0xE002400C)
#define CIIR_OFFSET 0xC
#define CIIR_IMSEC_MASK 0x1
#define CIIR_IMSEC 0x1
#define CIIR_IMSEC_BIT 0
#define CIIR_IMMIN_MASK 0x2
#define CIIR_IMMIN 0x2
#define CIIR_IMMIN_BIT 1
#define CIIR_IMHOUR_MASK 0x4
#define CIIR_IMHOUR 0x4
#define CIIR_IMHOUR_BIT 2
#define CIIR_IMDOM_MASK 0x8
#define CIIR_IMDOM 0x8
#define CIIR_IMDOM_BIT 3
#define CIIR_IMDOW_MASK 0x10
#define CIIR_IMDOW 0x10
#define CIIR_IMDOW_BIT 4
#define CIIR_IMDOY_MASK 0x20
#define CIIR_IMDOY 0x20
#define CIIR_IMDOY_BIT 5
#define CIIR_IMMON_MASK 0x40
#define CIIR_IMMON 0x40
#define CIIR_IMMON_BIT 6
#define CIIR_IMYEAR_MASK 0x80
#define CIIR_IMYEAR 0x80
#define CIIR_IMYEAR_BIT 7

#define AMR (*(volatile unsigned long *)0xE0024010)
#define AMR_OFFSET 0x10
#define AMR_AMRSEC_MASK 0x1
#define AMR_AMRSEC 0x1
#define AMR_AMRSEC_BIT 0
#define AMR_AMRMIN_MASK 0x2
#define AMR_AMRMIN 0x2
#define AMR_AMRMIN_BIT 1
#define AMR_AMRHOUR_MASK 0x4
#define AMR_AMRHOUR 0x4
#define AMR_AMRHOUR_BIT 2
#define AMR_AMRDOM_MASK 0x8
#define AMR_AMRDOM 0x8
#define AMR_AMRDOM_BIT 3
#define AMR_AMRDOW_MASK 0x10
#define AMR_AMRDOW 0x10
#define AMR_AMRDOW_BIT 4
#define AMR_AMRDOY_MASK 0x20
#define AMR_AMRDOY 0x20
#define AMR_AMRDOY_BIT 5
#define AMR_AMRMON_MASK 0x40
#define AMR_AMRMON 0x40
#define AMR_AMRMON_BIT 6
#define AMR_AMRYEAR_MASK 0x80
#define AMR_AMRYEAR 0x80
#define AMR_AMRYEAR_BIT 7

#define CTIME0 (*(volatile unsigned long *)0xE0024014)
#define CTIME0_OFFSET 0x14
#define CTIME0_Seconds_MASK 0x3F
#define CTIME0_Seconds_BIT 0
#define CTIME0_Minutes_MASK 0x3F00
#define CTIME0_Minutes_BIT 8
#define CTIME0_Hours_MASK 0x1F0000
#define CTIME0_Hours_BIT 16
#define CTIME0_Day_of_Week_MASK 0xF8000000
#define CTIME0_Day_of_Week_BIT 27

#define CTIME1 (*(volatile unsigned long *)0xE0024018)
#define CTIME1_OFFSET 0x18
#define CTIME1_Day_of_Month_MASK 0x1F
#define CTIME1_Day_of_Month_BIT 0
#define CTIME1_Month_MASK 0xF00
#define CTIME1_Month_BIT 8
#define CTIME1_Year_MASK 0xFFF0000
#define CTIME1_Year_BIT 16

#define CTIME2 (*(volatile unsigned long *)0xE002401C)
#define CTIME2_OFFSET 0x1C
#define CTIME2_Day_of_Year_MASK 0xFFF
#define CTIME2_Day_of_Year_BIT 0

#define SEC (*(volatile unsigned long *)0xE0024020)
#define SEC_OFFSET 0x20

#define MIN (*(volatile unsigned long *)0xE0024024)
#define MIN_OFFSET 0x24

#define HOUR (*(volatile unsigned long *)0xE0024028)
#define HOUR_OFFSET 0x28

#define DOM (*(volatile unsigned long *)0xE002402C)
#define DOM_OFFSET 0x2C

#define DOW (*(volatile unsigned long *)0xE0024030)
#define DOW_OFFSET 0x30

#define DOY (*(volatile unsigned long *)0xE0024034)
#define DOY_OFFSET 0x34

#define MONTH (*(volatile unsigned long *)0xE0024038)
#define MONTH_OFFSET 0x38

#define YEAR (*(volatile unsigned long *)0xE002403C)
#define YEAR_OFFSET 0x3C

#define ALSEC (*(volatile unsigned long *)0xE0024060)
#define ALSEC_OFFSET 0x60

#define ALMIN (*(volatile unsigned long *)0xE0024064)
#define ALMIN_OFFSET 0x64

#define ALHOUR (*(volatile unsigned long *)0xE0024068)
#define ALHOUR_OFFSET 0x68

#define ALDOM (*(volatile unsigned long *)0xE002406C)
#define ALDOM_OFFSET 0x6C

#define ALDOW (*(volatile unsigned long *)0xE0024070)
#define ALDOW_OFFSET 0x70

#define ALDOY (*(volatile unsigned long *)0xE0024074)
#define ALDOY_OFFSET 0x74

#define ALMON (*(volatile unsigned long *)0xE0024078)
#define ALMON_OFFSET 0x78

#define ALYEAR (*(volatile unsigned long *)0xE002407C)
#define ALYEAR_OFFSET 0x7C

#define PREINT (*(volatile unsigned long *)0xE0024080)
#define PREINT_OFFSET 0x80

#define PREFRAC (*(volatile unsigned long *)0xE0024084)
#define PREFRAC_OFFSET 0x84

#define GPIO_BASE 0xE0028000

#define IO0PIN (*(volatile unsigned long *)0xE0028000)
#define IO0PIN_OFFSET 0x0

#define IO0SET (*(volatile unsigned long *)0xE0028004)
#define IO0SET_OFFSET 0x4

#define IO0DIR (*(volatile unsigned long *)0xE0028008)
#define IO0DIR_OFFSET 0x8

#define IO0CLR (*(volatile unsigned long *)0xE002800C)
#define IO0CLR_OFFSET 0xC

#define IO1PIN (*(volatile unsigned long *)0xE0028010)
#define IO1PIN_OFFSET 0x10

#define IO1SET (*(volatile unsigned long *)0xE0028014)
#define IO1SET_OFFSET 0x14

#define IO1DIR (*(volatile unsigned long *)0xE0028018)
#define IO1DIR_OFFSET 0x18

#define IO1CLR (*(volatile unsigned long *)0xE002801C)
#define IO1CLR_OFFSET 0x1C

#define IOIntStatus (*(volatile unsigned long *)0xE0028080)
#define IOIntStatus_OFFSET 0x80
#define IOIntStatus_P0Int_MASK 0x1
#define IOIntStatus_P0Int 0x1
#define IOIntStatus_P0Int_BIT 0
#define IOIntStatus_P2Int_MASK 0x4
#define IOIntStatus_P2Int 0x4
#define IOIntStatus_P2Int_BIT 2

#define IO0IntStatR (*(volatile unsigned long *)0xE0028084)
#define IO0IntStatR_OFFSET 0x84

#define IO0IntStatF (*(volatile unsigned long *)0xE0028088)
#define IO0IntStatF_OFFSET 0x88

#define IO0IntClr (*(volatile unsigned long *)0xE002808C)
#define IO0IntClr_OFFSET 0x8C

#define IO0IntEnR (*(volatile unsigned long *)0xE0028090)
#define IO0IntEnR_OFFSET 0x90

#define IO0IntEnF (*(volatile unsigned long *)0xE0028094)
#define IO0IntEnF_OFFSET 0x94

#define IO2IntStatR (*(volatile unsigned long *)0xE00280A4)
#define IO2IntStatR_OFFSET 0xA4

#define IO2IntStatF (*(volatile unsigned long *)0xE00280A8)
#define IO2IntStatF_OFFSET 0xA8

#define IO2IntClr (*(volatile unsigned long *)0xE00280AC)
#define IO2IntClr_OFFSET 0xAC

#define IO2IntEnR (*(volatile unsigned long *)0xE00280B0)
#define IO2IntEnR_OFFSET 0xB0

#define IO2IntEnF (*(volatile unsigned long *)0xE00280B4)
#define IO2IntEnF_OFFSET 0xB4

#define PCB_BASE 0xE002C000

#define PINSEL0 (*(volatile unsigned long *)0xE002C000)
#define PINSEL0_OFFSET 0x0

#define PINSEL1 (*(volatile unsigned long *)0xE002C004)
#define PINSEL1_OFFSET 0x4

#define PINSEL2 (*(volatile unsigned long *)0xE002C008)
#define PINSEL2_OFFSET 0x8

#define PINSEL3 (*(volatile unsigned long *)0xE002C00C)
#define PINSEL3_OFFSET 0xC

#define PINSEL4 (*(volatile unsigned long *)0xE002C010)
#define PINSEL4_OFFSET 0x10

#define PINSEL5 (*(volatile unsigned long *)0xE002C014)
#define PINSEL5_OFFSET 0x14

#define PINSEL6 (*(volatile unsigned long *)0xE002C018)
#define PINSEL6_OFFSET 0x18

#define PINSEL7 (*(volatile unsigned long *)0xE002C01C)
#define PINSEL7_OFFSET 0x1C

#define PINSEL8 (*(volatile unsigned long *)0xE002C020)
#define PINSEL8_OFFSET 0x20

#define PINSEL9 (*(volatile unsigned long *)0xE002C024)
#define PINSEL9_OFFSET 0x24

#define PINSEL10 (*(volatile unsigned long *)0xE002C028)
#define PINSEL10_OFFSET 0x28

#define PINSEL11 (*(volatile unsigned long *)0xE002C02C)
#define PINSEL11_OFFSET 0x2C

#define PINMODE0 (*(volatile unsigned long *)0xE002C040)
#define PINMODE0_OFFSET 0x40

#define PINMODE1 (*(volatile unsigned long *)0xE002C044)
#define PINMODE1_OFFSET 0x44

#define PINMODE2 (*(volatile unsigned long *)0xE002C048)
#define PINMODE2_OFFSET 0x48

#define PINMODE3 (*(volatile unsigned long *)0xE002C04C)
#define PINMODE3_OFFSET 0x4C

#define PINMODE4 (*(volatile unsigned long *)0xE002C050)
#define PINMODE4_OFFSET 0x50

#define PINMODE5 (*(volatile unsigned long *)0xE002C054)
#define PINMODE5_OFFSET 0x54

#define PINMODE6 (*(volatile unsigned long *)0xE002C058)
#define PINMODE6_OFFSET 0x58

#define PINMODE7 (*(volatile unsigned long *)0xE002C05C)
#define PINMODE7_OFFSET 0x5C

#define PINMODE8 (*(volatile unsigned long *)0xE002C060)
#define PINMODE8_OFFSET 0x60

#define PINMODE9 (*(volatile unsigned long *)0xE002C064)
#define PINMODE9_OFFSET 0x64

#define PINMODE10 (*(volatile unsigned long *)0xE002C068)
#define PINMODE10_OFFSET 0x68

#define SSP1_BASE 0xE0030000

#define SSP1CR0 (*(volatile unsigned long *)0xE0030000)
#define SSP1CR0_OFFSET 0x0
#define SSP1CR0_SCR_MASK 0xFF00
#define SSP1CR0_SCR_BIT 8
#define SSP1CR0_CPHA_MASK 0x80
#define SSP1CR0_CPHA 0x80
#define SSP1CR0_CPHA_BIT 7
#define SSP1CR0_CPOL_MASK 0x40
#define SSP1CR0_CPOL 0x40
#define SSP1CR0_CPOL_BIT 6
#define SSP1CR0_FRF_MASK 0x30
#define SSP1CR0_FRF_BIT 4
#define SSP1CR0_DSS_MASK 0xF
#define SSP1CR0_DSS_BIT 0

#define SSP1CR1 (*(volatile unsigned long *)0xE0030004)
#define SSP1CR1_OFFSET 0x4
#define SSP1CR1_SOD_MASK 0x8
#define SSP1CR1_SOD 0x8
#define SSP1CR1_SOD_BIT 3
#define SSP1CR1_MS_MASK 0x4
#define SSP1CR1_MS 0x4
#define SSP1CR1_MS_BIT 2
#define SSP1CR1_SSE_MASK 0x2
#define SSP1CR1_SSE 0x2
#define SSP1CR1_SSE_BIT 1
#define SSP1CR1_LBE_MASK 0x1
#define SSP1CR1_LBE 0x1
#define SSP1CR1_LBE_BIT 0

#define SSP1DR (*(volatile unsigned long *)0xE0030008)
#define SSP1DR_OFFSET 0x8

#define SSP1SR (*(volatile unsigned long *)0xE003000C)
#define SSP1SR_OFFSET 0xC
#define SSP1SR_BSY_MASK 0x10
#define SSP1SR_BSY 0x10
#define SSP1SR_BSY_BIT 4
#define SSP1SR_RFF_MASK 0x8
#define SSP1SR_RFF 0x8
#define SSP1SR_RFF_BIT 3
#define SSP1SR_RNE_MASK 0x4
#define SSP1SR_RNE 0x4
#define SSP1SR_RNE_BIT 2
#define SSP1SR_TNF_MASK 0x2
#define SSP1SR_TNF 0x2
#define SSP1SR_TNF_BIT 1
#define SSP1SR_TFE_MASK 0x1
#define SSP1SR_TFE 0x1
#define SSP1SR_TFE_BIT 0

#define SSP1CPSR (*(volatile unsigned long *)0xE0030010)
#define SSP1CPSR_OFFSET 0x10
#define SSP1CPSR_CPSDVSR_MASK 0xFF
#define SSP1CPSR_CPSDVSR_BIT 0

#define SSP1IMSC (*(volatile unsigned long *)0xE0030014)
#define SSP1IMSC_OFFSET 0x14
#define SSP1IMSC_TXIM_MASK 0x8
#define SSP1IMSC_TXIM 0x8
#define SSP1IMSC_TXIM_BIT 3
#define SSP1IMSC_RXIM_MASK 0x4
#define SSP1IMSC_RXIM 0x4
#define SSP1IMSC_RXIM_BIT 2
#define SSP1IMSC_RTIM_MASK 0x2
#define SSP1IMSC_RTIM 0x2
#define SSP1IMSC_RTIM_BIT 1
#define SSP1IMSC_RORIM_MASK 0x1
#define SSP1IMSC_RORIM 0x1
#define SSP1IMSC_RORIM_BIT 0

#define SSP1RIS (*(volatile unsigned long *)0xE0030018)
#define SSP1RIS_OFFSET 0x18
#define SSP1RIS_TXRIS_MASK 0x8
#define SSP1RIS_TXRIS 0x8
#define SSP1RIS_TXRIS_BIT 3
#define SSP1RIS_RXRIS_MASK 0x4
#define SSP1RIS_RXRIS 0x4
#define SSP1RIS_RXRIS_BIT 2
#define SSP1RIS_RTRIS_MASK 0x2
#define SSP1RIS_RTRIS 0x2
#define SSP1RIS_RTRIS_BIT 1
#define SSP1RIS_RORRIS_MASK 0x1
#define SSP1RIS_RORRIS 0x1
#define SSP1RIS_RORRIS_BIT 0

#define SSP1MIS (*(volatile unsigned long *)0xE003001C)
#define SSP1MIS_OFFSET 0x1C
#define SSP1MIS_TXMIS_MASK 0x8
#define SSP1MIS_TXMIS 0x8
#define SSP1MIS_TXMIS_BIT 3
#define SSP1MIS_RXMIS_MASK 0x4
#define SSP1MIS_RXMIS 0x4
#define SSP1MIS_RXMIS_BIT 2
#define SSP1MIS_RTMIS_MASK 0x2
#define SSP1MIS_RTMIS 0x2
#define SSP1MIS_RTMIS_BIT 1
#define SSP1MIS_RORMIS_MASK 0x1
#define SSP1MIS_RORMIS 0x1
#define SSP1MIS_RORMIS_BIT 0

#define SSP1ICR (*(volatile unsigned long *)0xE0030020)
#define SSP1ICR_OFFSET 0x20
#define SSP1ICR_RTIC_MASK 0x2
#define SSP1ICR_RTIC 0x2
#define SSP1ICR_RTIC_BIT 1
#define SSP1ICR_RORIC_MASK 0x1
#define SSP1ICR_RORIC 0x1
#define SSP1ICR_RORIC_BIT 0

#define SSP1DMACR (*(volatile unsigned long *)0xE0030024)
#define SSP1DMACR_OFFSET 0x24
#define SSP1DMACR_RXDMAE_MASK 0x1
#define SSP1DMACR_RXDMAE 0x1
#define SSP1DMACR_RXDMAE_BIT 0
#define SSP1DMACR_TXDMAE_MASK 0x2
#define SSP1DMACR_TXDMAE 0x2
#define SSP1DMACR_TXDMAE_BIT 1

#define AD0_BASE 0xE0034000

#define AD0CR (*(volatile unsigned *)0xE0034000)
#define AD0CR_OFFSET 0x0
#define AD0CR_SEL_MASK 0xFF
#define AD0CR_SEL_BIT 0
#define AD0CR_CLKDIV_MASK 0xFF00
#define AD0CR_CLKDIV_BIT 8
#define AD0CR_BURST_MASK 0x10000
#define AD0CR_BURST 0x10000
#define AD0CR_BURST_BIT 16
#define AD0CR_CLKS_MASK 0xE0000
#define AD0CR_CLKS_BIT 17
#define AD0CR_PDN_MASK 0x200000
#define AD0CR_PDN 0x200000
#define AD0CR_PDN_BIT 21
#define AD0CR_START_MASK 0x7000000
#define AD0CR_START_BIT 24
#define AD0CR_EDGE_MASK 0x8000000
#define AD0CR_EDGE 0x8000000
#define AD0CR_EDGE_BIT 27

#define AD0GDR (*(volatile unsigned *)0xE0034004)
#define AD0GDR_OFFSET 0x4
#define AD0GDR_RESULT_MASK 0xFFC0
#define AD0GDR_RESULT_BIT 6
#define AD0GDR_CHN_MASK 0x7000000
#define AD0GDR_CHN_BIT 24
#define AD0GDR_OVERUN_MASK 0x40000000
#define AD0GDR_OVERUN 0x40000000
#define AD0GDR_OVERUN_BIT 30
#define AD0GDR_DONE_MASK 0x80000000
#define AD0GDR_DONE 0x80000000
#define AD0GDR_DONE_BIT 31

#define AD0INTEN (*(volatile unsigned *)0xE003400C)
#define AD0INTEN_OFFSET 0xC
#define AD0INTEN_ADINTEN0_MASK 0x1
#define AD0INTEN_ADINTEN0 0x1
#define AD0INTEN_ADINTEN0_BIT 0
#define AD0INTEN_ADINTEN1_MASK 0x2
#define AD0INTEN_ADINTEN1 0x2
#define AD0INTEN_ADINTEN1_BIT 1
#define AD0INTEN_ADINTEN2_MASK 0x4
#define AD0INTEN_ADINTEN2 0x4
#define AD0INTEN_ADINTEN2_BIT 2
#define AD0INTEN_ADINTEN3_MASK 0x8
#define AD0INTEN_ADINTEN3 0x8
#define AD0INTEN_ADINTEN3_BIT 3
#define AD0INTEN_ADINTEN4_MASK 0x10
#define AD0INTEN_ADINTEN4 0x10
#define AD0INTEN_ADINTEN4_BIT 4
#define AD0INTEN_ADINTEN5_MASK 0x20
#define AD0INTEN_ADINTEN5 0x20
#define AD0INTEN_ADINTEN5_BIT 5
#define AD0INTEN_ADINTEN6_MASK 0x40
#define AD0INTEN_ADINTEN6 0x40
#define AD0INTEN_ADINTEN6_BIT 6
#define AD0INTEN_ADINTEN7_MASK 0x80
#define AD0INTEN_ADINTEN7 0x80
#define AD0INTEN_ADINTEN7_BIT 7
#define AD0INTEN_ADGINTEN_MASK 0x100
#define AD0INTEN_ADGINTEN 0x100
#define AD0INTEN_ADGINTEN_BIT 8

#define AD0DR0 (*(volatile unsigned *)0xE0034010)
#define AD0DR0_OFFSET 0x10
#define AD0DR0_RESULT_MASK 0xFFC0
#define AD0DR0_RESULT_BIT 6
#define AD0DR0_OVERRUN_MASK 0x40000000
#define AD0DR0_OVERRUN 0x40000000
#define AD0DR0_OVERRUN_BIT 30
#define AD0DR0_DONE_MASK 0x80000000
#define AD0DR0_DONE 0x80000000
#define AD0DR0_DONE_BIT 31

#define AD0DR1 (*(volatile unsigned *)0xE0034014)
#define AD0DR1_OFFSET 0x14
#define AD0DR1_RESULT_MASK 0xFFC0
#define AD0DR1_RESULT_BIT 6
#define AD0DR1_OVERRUN_MASK 0x40000000
#define AD0DR1_OVERRUN 0x40000000
#define AD0DR1_OVERRUN_BIT 30
#define AD0DR1_DONE_MASK 0x80000000
#define AD0DR1_DONE 0x80000000
#define AD0DR1_DONE_BIT 31

#define AD0DR2 (*(volatile unsigned *)0xE0034018)
#define AD0DR2_OFFSET 0x18
#define AD0DR2_RESULT_MASK 0xFFC0
#define AD0DR2_RESULT_BIT 6
#define AD0DR2_OVERRUN_MASK 0x40000000
#define AD0DR2_OVERRUN 0x40000000
#define AD0DR2_OVERRUN_BIT 30
#define AD0DR2_DONE_MASK 0x80000000
#define AD0DR2_DONE 0x80000000
#define AD0DR2_DONE_BIT 31

#define AD0DR3 (*(volatile unsigned *)0xE003401C)
#define AD0DR3_OFFSET 0x1C
#define AD0DR3_RESULT_MASK 0xFFC0
#define AD0DR3_RESULT_BIT 6
#define AD0DR3_OVERRUN_MASK 0x40000000
#define AD0DR3_OVERRUN 0x40000000
#define AD0DR3_OVERRUN_BIT 30
#define AD0DR3_DONE_MASK 0x80000000
#define AD0DR3_DONE 0x80000000
#define AD0DR3_DONE_BIT 31

#define AD0DR4 (*(volatile unsigned *)0xE0034020)
#define AD0DR4_OFFSET 0x20
#define AD0DR4_RESULT_MASK 0xFFC0
#define AD0DR4_RESULT_BIT 6
#define AD0DR4_OVERRUN_MASK 0x40000000
#define AD0DR4_OVERRUN 0x40000000
#define AD0DR4_OVERRUN_BIT 30
#define AD0DR4_DONE_MASK 0x80000000
#define AD0DR4_DONE 0x80000000
#define AD0DR4_DONE_BIT 31

#define AD0DR5 (*(volatile unsigned *)0xE0034024)
#define AD0DR5_OFFSET 0x24
#define AD0DR5_RESULT_MASK 0xFFC0
#define AD0DR5_RESULT_BIT 6
#define AD0DR5_OVERRUN_MASK 0x40000000
#define AD0DR5_OVERRUN 0x40000000
#define AD0DR5_OVERRUN_BIT 30
#define AD0DR5_DONE_MASK 0x80000000
#define AD0DR5_DONE 0x80000000
#define AD0DR5_DONE_BIT 31

#define AD0DR6 (*(volatile unsigned *)0xE0034028)
#define AD0DR6_OFFSET 0x28
#define AD0DR6_RESULT_MASK 0xFFC0
#define AD0DR6_RESULT_BIT 6
#define AD0DR6_OVERRUN_MASK 0x40000000
#define AD0DR6_OVERRUN 0x40000000
#define AD0DR6_OVERRUN_BIT 30
#define AD0DR6_DONE_MASK 0x80000000
#define AD0DR6_DONE 0x80000000
#define AD0DR6_DONE_BIT 31

#define AD0DR7 (*(volatile unsigned *)0xE003402C)
#define AD0DR7_OFFSET 0x2C
#define AD0DR7_RESULT_MASK 0xFFC0
#define AD0DR7_RESULT_BIT 6
#define AD0DR7_OVERRUN_MASK 0x40000000
#define AD0DR7_OVERRUN 0x40000000
#define AD0DR7_OVERRUN_BIT 30
#define AD0DR7_DONE_MASK 0x80000000
#define AD0DR7_DONE 0x80000000
#define AD0DR7_DONE_BIT 31

#define AD0STAT (*(volatile unsigned *)0xE0034030)
#define AD0STAT_OFFSET 0x30
#define AD0STAT_DONE0_MASK 0x1
#define AD0STAT_DONE0 0x1
#define AD0STAT_DONE0_BIT 0
#define AD0STAT_DONE1_MASK 0x2
#define AD0STAT_DONE1 0x2
#define AD0STAT_DONE1_BIT 1
#define AD0STAT_DONE2_MASK 0x4
#define AD0STAT_DONE2 0x4
#define AD0STAT_DONE2_BIT 2
#define AD0STAT_DONE3_MASK 0x8
#define AD0STAT_DONE3 0x8
#define AD0STAT_DONE3_BIT 3
#define AD0STAT_DONE4_MASK 0x10
#define AD0STAT_DONE4 0x10
#define AD0STAT_DONE4_BIT 4
#define AD0STAT_DONE5_MASK 0x20
#define AD0STAT_DONE5 0x20
#define AD0STAT_DONE5_BIT 5
#define AD0STAT_DONE6_MASK 0x40
#define AD0STAT_DONE6 0x40
#define AD0STAT_DONE6_BIT 6
#define AD0STAT_DONE7_MASK 0x80
#define AD0STAT_DONE7 0x80
#define AD0STAT_DONE7_BIT 7
#define AD0STAT_OVERRUN0_MASK 0x100
#define AD0STAT_OVERRUN0 0x100
#define AD0STAT_OVERRUN0_BIT 8
#define AD0STAT_OVERRUN1_MASK 0x200
#define AD0STAT_OVERRUN1 0x200
#define AD0STAT_OVERRUN1_BIT 9
#define AD0STAT_OVERRUN2_MASK 0x400
#define AD0STAT_OVERRUN2 0x400
#define AD0STAT_OVERRUN2_BIT 10
#define AD0STAT_OVERRUN3_MASK 0x800
#define AD0STAT_OVERRUN3 0x800
#define AD0STAT_OVERRUN3_BIT 11
#define AD0STAT_OVERRUN4_MASK 0x1000
#define AD0STAT_OVERRUN4 0x1000
#define AD0STAT_OVERRUN4_BIT 12
#define AD0STAT_OVERRUN5_MASK 0x2000
#define AD0STAT_OVERRUN5 0x2000
#define AD0STAT_OVERRUN5_BIT 13
#define AD0STAT_OVERRUN6_MASK 0x4000
#define AD0STAT_OVERRUN6 0x4000
#define AD0STAT_OVERRUN6_BIT 14
#define AD0STAT_OVERRUN7_MASK 0x8000
#define AD0STAT_OVERRUN7 0x8000
#define AD0STAT_OVERRUN7_BIT 15
#define AD0STAT_ADINT_MASK 0x10000
#define AD0STAT_ADINT 0x10000
#define AD0STAT_ADINT_BIT 16

#define CAN_BASE 0xE0038000

#define AFMR (*(volatile unsigned long *)0xE003C000)
#define AFMR_OFFSET 0x4000
#define AFMR_AccOff_MASK 0x1
#define AFMR_AccOff 0x1
#define AFMR_AccOff_BIT 0
#define AFMR_AccBP_MASK 0x2
#define AFMR_AccBP 0x2
#define AFMR_AccBP_BIT 1
#define AFMR_eFCAN_MASK 0x4
#define AFMR_eFCAN 0x4
#define AFMR_eFCAN_BIT 2

#define SFF_sa (*(volatile unsigned long *)0xE003C004)
#define SFF_sa_OFFSET 0x4004

#define SFF_GRP_sa (*(volatile unsigned long *)0xE003C008)
#define SFF_GRP_sa_OFFSET 0x4008

#define EFF_sa (*(volatile unsigned long *)0xE003C00C)
#define EFF_sa_OFFSET 0x400C

#define EFF_GRP_sa (*(volatile unsigned long *)0xE003C010)
#define EFF_GRP_sa_OFFSET 0x4010

#define ENDofTable (*(volatile unsigned long *)0xE003C014)
#define ENDofTable_OFFSET 0x4014

#define LUTerrAd (*(volatile unsigned long *)0xE003C018)
#define LUTerrAd_OFFSET 0x4018

#define LUTerr (*(volatile unsigned long *)0xE003C01C)
#define LUTerr_OFFSET 0x401C

#define FCANIE (*(volatile unsigned long *)0xE003C020)
#define FCANIE_OFFSET 0x4020
#define FCANIE_FCANIE_MASK 0x1
#define FCANIE_FCANIE 0x1
#define FCANIE_FCANIE_BIT 0

#define FCANIC0 (*(volatile unsigned long *)0xE003C024)
#define FCANIC0_OFFSET 0x4024
#define FCANIC0_IntPnd0_MASK 0x1
#define FCANIC0_IntPnd0 0x1
#define FCANIC0_IntPnd0_BIT 0
#define FCANIC0_IntPnd1_MASK 0x2
#define FCANIC0_IntPnd1 0x2
#define FCANIC0_IntPnd1_BIT 1
#define FCANIC0_IntPnd2_MASK 0x4
#define FCANIC0_IntPnd2 0x4
#define FCANIC0_IntPnd2_BIT 2
#define FCANIC0_IntPnd3_MASK 0x8
#define FCANIC0_IntPnd3 0x8
#define FCANIC0_IntPnd3_BIT 3
#define FCANIC0_IntPnd4_MASK 0x10
#define FCANIC0_IntPnd4 0x10
#define FCANIC0_IntPnd4_BIT 4
#define FCANIC0_IntPnd5_MASK 0x20
#define FCANIC0_IntPnd5 0x20
#define FCANIC0_IntPnd5_BIT 5
#define FCANIC0_IntPnd6_MASK 0x40
#define FCANIC0_IntPnd6 0x40
#define FCANIC0_IntPnd6_BIT 6
#define FCANIC0_IntPnd7_MASK 0x80
#define FCANIC0_IntPnd7 0x80
#define FCANIC0_IntPnd7_BIT 7
#define FCANIC0_IntPnd8_MASK 0x100
#define FCANIC0_IntPnd8 0x100
#define FCANIC0_IntPnd8_BIT 8
#define FCANIC0_IntPnd9_MASK 0x200
#define FCANIC0_IntPnd9 0x200
#define FCANIC0_IntPnd9_BIT 9
#define FCANIC0_IntPnd10_MASK 0x400
#define FCANIC0_IntPnd10 0x400
#define FCANIC0_IntPnd10_BIT 10
#define FCANIC0_IntPnd11_MASK 0x800
#define FCANIC0_IntPnd11 0x800
#define FCANIC0_IntPnd11_BIT 11
#define FCANIC0_IntPnd12_MASK 0x1000
#define FCANIC0_IntPnd12 0x1000
#define FCANIC0_IntPnd12_BIT 12
#define FCANIC0_IntPnd13_MASK 0x2000
#define FCANIC0_IntPnd13 0x2000
#define FCANIC0_IntPnd13_BIT 13
#define FCANIC0_IntPnd14_MASK 0x4000
#define FCANIC0_IntPnd14 0x4000
#define FCANIC0_IntPnd14_BIT 14
#define FCANIC0_IntPnd15_MASK 0x8000
#define FCANIC0_IntPnd15 0x8000
#define FCANIC0_IntPnd15_BIT 15
#define FCANIC0_IntPnd16_MASK 0x10000
#define FCANIC0_IntPnd16 0x10000
#define FCANIC0_IntPnd16_BIT 16
#define FCANIC0_IntPnd17_MASK 0x20000
#define FCANIC0_IntPnd17 0x20000
#define FCANIC0_IntPnd17_BIT 17
#define FCANIC0_IntPnd18_MASK 0x40000
#define FCANIC0_IntPnd18 0x40000
#define FCANIC0_IntPnd18_BIT 18
#define FCANIC0_IntPnd19_MASK 0x80000
#define FCANIC0_IntPnd19 0x80000
#define FCANIC0_IntPnd19_BIT 19
#define FCANIC0_IntPnd20_MASK 0x100000
#define FCANIC0_IntPnd20 0x100000
#define FCANIC0_IntPnd20_BIT 20
#define FCANIC0_IntPnd21_MASK 0x200000
#define FCANIC0_IntPnd21 0x200000
#define FCANIC0_IntPnd21_BIT 21
#define FCANIC0_IntPnd22_MASK 0x400000
#define FCANIC0_IntPnd22 0x400000
#define FCANIC0_IntPnd22_BIT 22
#define FCANIC0_IntPnd23_MASK 0x800000
#define FCANIC0_IntPnd23 0x800000
#define FCANIC0_IntPnd23_BIT 23
#define FCANIC0_IntPnd24_MASK 0x1000000
#define FCANIC0_IntPnd24 0x1000000
#define FCANIC0_IntPnd24_BIT 24
#define FCANIC0_IntPnd25_MASK 0x2000000
#define FCANIC0_IntPnd25 0x2000000
#define FCANIC0_IntPnd25_BIT 25
#define FCANIC0_IntPnd26_MASK 0x4000000
#define FCANIC0_IntPnd26 0x4000000
#define FCANIC0_IntPnd26_BIT 26
#define FCANIC0_IntPnd27_MASK 0x8000000
#define FCANIC0_IntPnd27 0x8000000
#define FCANIC0_IntPnd27_BIT 27
#define FCANIC0_IntPnd28_MASK 0x10000000
#define FCANIC0_IntPnd28 0x10000000
#define FCANIC0_IntPnd28_BIT 28
#define FCANIC0_IntPnd29_MASK 0x20000000
#define FCANIC0_IntPnd29 0x20000000
#define FCANIC0_IntPnd29_BIT 29
#define FCANIC0_IntPnd30_MASK 0x40000000
#define FCANIC0_IntPnd30 0x40000000
#define FCANIC0_IntPnd30_BIT 30
#define FCANIC0_IntPnd31_MASK 0x80000000
#define FCANIC0_IntPnd31 0x80000000
#define FCANIC0_IntPnd31_BIT 31

#define FCANIC1 (*(volatile unsigned long *)0xE003C028)
#define FCANIC1_OFFSET 0x4028
#define FCANIC1_IntPnd32_MASK 0x1
#define FCANIC1_IntPnd32 0x1
#define FCANIC1_IntPnd32_BIT 0
#define FCANIC1_IntPnd33_MASK 0x2
#define FCANIC1_IntPnd33 0x2
#define FCANIC1_IntPnd33_BIT 1
#define FCANIC1_IntPnd34_MASK 0x4
#define FCANIC1_IntPnd34 0x4
#define FCANIC1_IntPnd34_BIT 2
#define FCANIC1_IntPnd35_MASK 0x8
#define FCANIC1_IntPnd35 0x8
#define FCANIC1_IntPnd35_BIT 3
#define FCANIC1_IntPnd36_MASK 0x10
#define FCANIC1_IntPnd36 0x10
#define FCANIC1_IntPnd36_BIT 4
#define FCANIC1_IntPnd37_MASK 0x20
#define FCANIC1_IntPnd37 0x20
#define FCANIC1_IntPnd37_BIT 5
#define FCANIC1_IntPnd38_MASK 0x40
#define FCANIC1_IntPnd38 0x40
#define FCANIC1_IntPnd38_BIT 6
#define FCANIC1_IntPnd39_MASK 0x80
#define FCANIC1_IntPnd39 0x80
#define FCANIC1_IntPnd39_BIT 7
#define FCANIC1_IntPnd40_MASK 0x100
#define FCANIC1_IntPnd40 0x100
#define FCANIC1_IntPnd40_BIT 8
#define FCANIC1_IntPnd41_MASK 0x200
#define FCANIC1_IntPnd41 0x200
#define FCANIC1_IntPnd41_BIT 9
#define FCANIC1_IntPnd42_MASK 0x400
#define FCANIC1_IntPnd42 0x400
#define FCANIC1_IntPnd42_BIT 10
#define FCANIC1_IntPnd43_MASK 0x800
#define FCANIC1_IntPnd43 0x800
#define FCANIC1_IntPnd43_BIT 11
#define FCANIC1_IntPnd44_MASK 0x1000
#define FCANIC1_IntPnd44 0x1000
#define FCANIC1_IntPnd44_BIT 12
#define FCANIC1_IntPnd45_MASK 0x2000
#define FCANIC1_IntPnd45 0x2000
#define FCANIC1_IntPnd45_BIT 13
#define FCANIC1_IntPnd46_MASK 0x4000
#define FCANIC1_IntPnd46 0x4000
#define FCANIC1_IntPnd46_BIT 14
#define FCANIC1_IntPnd47_MASK 0x8000
#define FCANIC1_IntPnd47 0x8000
#define FCANIC1_IntPnd47_BIT 15
#define FCANIC1_IntPnd48_MASK 0x10000
#define FCANIC1_IntPnd48 0x10000
#define FCANIC1_IntPnd48_BIT 16
#define FCANIC1_IntPnd49_MASK 0x20000
#define FCANIC1_IntPnd49 0x20000
#define FCANIC1_IntPnd49_BIT 17
#define FCANIC1_IntPnd50_MASK 0x40000
#define FCANIC1_IntPnd50 0x40000
#define FCANIC1_IntPnd50_BIT 18
#define FCANIC1_IntPnd51_MASK 0x80000
#define FCANIC1_IntPnd51 0x80000
#define FCANIC1_IntPnd51_BIT 19
#define FCANIC1_IntPnd52_MASK 0x100000
#define FCANIC1_IntPnd52 0x100000
#define FCANIC1_IntPnd52_BIT 20
#define FCANIC1_IntPnd53_MASK 0x200000
#define FCANIC1_IntPnd53 0x200000
#define FCANIC1_IntPnd53_BIT 21
#define FCANIC1_IntPnd54_MASK 0x400000
#define FCANIC1_IntPnd54 0x400000
#define FCANIC1_IntPnd54_BIT 22
#define FCANIC1_IntPnd55_MASK 0x800000
#define FCANIC1_IntPnd55 0x800000
#define FCANIC1_IntPnd55_BIT 23
#define FCANIC1_IntPnd56_MASK 0x1000000
#define FCANIC1_IntPnd56 0x1000000
#define FCANIC1_IntPnd56_BIT 24
#define FCANIC1_IntPnd57_MASK 0x2000000
#define FCANIC1_IntPnd57 0x2000000
#define FCANIC1_IntPnd57_BIT 25
#define FCANIC1_IntPnd58_MASK 0x4000000
#define FCANIC1_IntPnd58 0x4000000
#define FCANIC1_IntPnd58_BIT 26
#define FCANIC1_IntPnd59_MASK 0x8000000
#define FCANIC1_IntPnd59 0x8000000
#define FCANIC1_IntPnd59_BIT 27
#define FCANIC1_IntPnd60_MASK 0x10000000
#define FCANIC1_IntPnd60 0x10000000
#define FCANIC1_IntPnd60_BIT 28
#define FCANIC1_IntPnd61_MASK 0x20000000
#define FCANIC1_IntPnd61 0x20000000
#define FCANIC1_IntPnd61_BIT 29
#define FCANIC1_IntPnd62_MASK 0x40000000
#define FCANIC1_IntPnd62 0x40000000
#define FCANIC1_IntPnd62_BIT 30
#define FCANIC1_IntPnd63_MASK 0x80000000
#define FCANIC1_IntPnd63 0x80000000
#define FCANIC1_IntPnd63_BIT 31

#define CANTxSR (*(volatile unsigned long *)0xE0040000)
#define CANTxSR_OFFSET 0x8000
#define CANTxSR_TS1_MASK 0x1
#define CANTxSR_TS1 0x1
#define CANTxSR_TS1_BIT 0
#define CANTxSR_TS2_MASK 0x2
#define CANTxSR_TS2 0x2
#define CANTxSR_TS2_BIT 1
#define CANTxSR_TBS1_MASK 0x100
#define CANTxSR_TBS1 0x100
#define CANTxSR_TBS1_BIT 8
#define CANTxSR_TBS2_MASK 0x200
#define CANTxSR_TBS2 0x200
#define CANTxSR_TBS2_BIT 9
#define CANTxSR_TCS1_MASK 0x10000
#define CANTxSR_TCS1 0x10000
#define CANTxSR_TCS1_BIT 16
#define CANTxSR_TCS2_MASK 0x20000
#define CANTxSR_TCS2 0x20000
#define CANTxSR_TCS2_BIT 17

#define CANRxSR (*(volatile unsigned long *)0xE0040004)
#define CANRxSR_OFFSET 0x8004
#define CANRxSR_RS1_MASK 0x1
#define CANRxSR_RS1 0x1
#define CANRxSR_RS1_BIT 0
#define CANRxSR_RS2_MASK 0x2
#define CANRxSR_RS2 0x2
#define CANRxSR_RS2_BIT 1
#define CANRxSR_RB1_MASK 0x100
#define CANRxSR_RB1 0x100
#define CANRxSR_RB1_BIT 8
#define CANRxSR_RB2_MASK 0x200
#define CANRxSR_RB2 0x200
#define CANRxSR_RB2_BIT 9
#define CANRxSR_DOS1_MASK 0x10000
#define CANRxSR_DOS1 0x10000
#define CANRxSR_DOS1_BIT 16
#define CANRxSR_DOS2_MASK 0x20000
#define CANRxSR_DOS2 0x20000
#define CANRxSR_DOS2_BIT 17

#define CANMSR (*(volatile unsigned long *)0xE0040008)
#define CANMSR_OFFSET 0x8008
#define CANMSR_ES1_MASK 0x1
#define CANMSR_ES1 0x1
#define CANMSR_ES1_BIT 0
#define CANMSR_ES2_MASK 0x2
#define CANMSR_ES2 0x2
#define CANMSR_ES2_BIT 1
#define CANMSR_BS1_MASK 0x100
#define CANMSR_BS1 0x100
#define CANMSR_BS1_BIT 8
#define CANMSR_BS2_MASK 0x200
#define CANMSR_BS2 0x200
#define CANMSR_BS2_BIT 9

#define CAN1_BASE 0xE0044000

#define C1MOD (*(volatile unsigned long *)0xE0044000)
#define CAN1MOD C1MOD
#define C1MOD_OFFSET 0x0
#define CAN1MOD_OFFSET C1MOD_OFFSET
#define C1MOD_RM_MASK 0x1
#define CAN1MOD_RM_MASK C1MOD_RM_MASK
#define C1MOD_RM 0x1
#define CAN1MOD_RM C1MOD_RM
#define C1MOD_RM_BIT 0
#define CAN1MOD_RM_BIT C1MOD_RM_BIT
#define C1MOD_LOM_MASK 0x2
#define CAN1MOD_LOM_MASK C1MOD_LOM_MASK
#define C1MOD_LOM 0x2
#define CAN1MOD_LOM C1MOD_LOM
#define C1MOD_LOM_BIT 1
#define CAN1MOD_LOM_BIT C1MOD_LOM_BIT
#define C1MOD_STM_MASK 0x4
#define CAN1MOD_STM_MASK C1MOD_STM_MASK
#define C1MOD_STM 0x4
#define CAN1MOD_STM C1MOD_STM
#define C1MOD_STM_BIT 2
#define CAN1MOD_STM_BIT C1MOD_STM_BIT
#define C1MOD_TPM_MASK 0x8
#define CAN1MOD_TPM_MASK C1MOD_TPM_MASK
#define C1MOD_TPM 0x8
#define CAN1MOD_TPM C1MOD_TPM
#define C1MOD_TPM_BIT 3
#define CAN1MOD_TPM_BIT C1MOD_TPM_BIT
#define C1MOD_SM_MASK 0x10
#define CAN1MOD_SM_MASK C1MOD_SM_MASK
#define C1MOD_SM 0x10
#define CAN1MOD_SM C1MOD_SM
#define C1MOD_SM_BIT 4
#define CAN1MOD_SM_BIT C1MOD_SM_BIT
#define C1MOD_RPM_MASK 0x20
#define CAN1MOD_RPM_MASK C1MOD_RPM_MASK
#define C1MOD_RPM 0x20
#define CAN1MOD_RPM C1MOD_RPM
#define C1MOD_RPM_BIT 5
#define CAN1MOD_RPM_BIT C1MOD_RPM_BIT
#define C1MOD_TM_MASK 0x80
#define CAN1MOD_TM_MASK C1MOD_TM_MASK
#define C1MOD_TM 0x80
#define CAN1MOD_TM C1MOD_TM
#define C1MOD_TM_BIT 7
#define CAN1MOD_TM_BIT C1MOD_TM_BIT

#define C1CMR (*(volatile unsigned long *)0xE0044004)
#define CAN1CMR C1CMR
#define C1CMR_OFFSET 0x4
#define CAN1CMR_OFFSET C1CMR_OFFSET
#define C1CMR_TR_MASK 0x1
#define CAN1CMR_TR_MASK C1CMR_TR_MASK
#define C1CMR_TR 0x1
#define CAN1CMR_TR C1CMR_TR
#define C1CMR_TR_BIT 0
#define CAN1CMR_TR_BIT C1CMR_TR_BIT
#define C1CMR_AT_MASK 0x2
#define CAN1CMR_AT_MASK C1CMR_AT_MASK
#define C1CMR_AT 0x2
#define CAN1CMR_AT C1CMR_AT
#define C1CMR_AT_BIT 1
#define CAN1CMR_AT_BIT C1CMR_AT_BIT
#define C1CMR_RRB_MASK 0x4
#define CAN1CMR_RRB_MASK C1CMR_RRB_MASK
#define C1CMR_RRB 0x4
#define CAN1CMR_RRB C1CMR_RRB
#define C1CMR_RRB_BIT 2
#define CAN1CMR_RRB_BIT C1CMR_RRB_BIT
#define C1CMR_CDO_MASK 0x8
#define CAN1CMR_CDO_MASK C1CMR_CDO_MASK
#define C1CMR_CDO 0x8
#define CAN1CMR_CDO C1CMR_CDO
#define C1CMR_CDO_BIT 3
#define CAN1CMR_CDO_BIT C1CMR_CDO_BIT
#define C1CMR_SRR_MASK 0x10
#define CAN1CMR_SRR_MASK C1CMR_SRR_MASK
#define C1CMR_SRR 0x10
#define CAN1CMR_SRR C1CMR_SRR
#define C1CMR_SRR_BIT 4
#define CAN1CMR_SRR_BIT C1CMR_SRR_BIT
#define C1CMR_STB1_MASK 0x20
#define CAN1CMR_STB1_MASK C1CMR_STB1_MASK
#define C1CMR_STB1 0x20
#define CAN1CMR_STB1 C1CMR_STB1
#define C1CMR_STB1_BIT 5
#define CAN1CMR_STB1_BIT C1CMR_STB1_BIT
#define C1CMR_STB2_MASK 0x40
#define CAN1CMR_STB2_MASK C1CMR_STB2_MASK
#define C1CMR_STB2 0x40
#define CAN1CMR_STB2 C1CMR_STB2
#define C1CMR_STB2_BIT 6
#define CAN1CMR_STB2_BIT C1CMR_STB2_BIT
#define C1CMR_STB3_MASK 0x80
#define CAN1CMR_STB3_MASK C1CMR_STB3_MASK
#define C1CMR_STB3 0x80
#define CAN1CMR_STB3 C1CMR_STB3
#define C1CMR_STB3_BIT 7
#define CAN1CMR_STB3_BIT C1CMR_STB3_BIT

#define C1GSR (*(volatile unsigned long *)0xE0044008)
#define CAN1GSR C1GSR
#define C1GSR_OFFSET 0x8
#define CAN1GSR_OFFSET C1GSR_OFFSET
#define C1GSR_RBS_MASK 0x1
#define CAN1GSR_RBS_MASK C1GSR_RBS_MASK
#define C1GSR_RBS 0x1
#define CAN1GSR_RBS C1GSR_RBS
#define C1GSR_RBS_BIT 0
#define CAN1GSR_RBS_BIT C1GSR_RBS_BIT
#define C1GSR_DOS_MASK 0x2
#define CAN1GSR_DOS_MASK C1GSR_DOS_MASK
#define C1GSR_DOS 0x2
#define CAN1GSR_DOS C1GSR_DOS
#define C1GSR_DOS_BIT 1
#define CAN1GSR_DOS_BIT C1GSR_DOS_BIT
#define C1GSR_TBS_MASK 0x4
#define CAN1GSR_TBS_MASK C1GSR_TBS_MASK
#define C1GSR_TBS 0x4
#define CAN1GSR_TBS C1GSR_TBS
#define C1GSR_TBS_BIT 2
#define CAN1GSR_TBS_BIT C1GSR_TBS_BIT
#define C1GSR_TCS_MASK 0x8
#define CAN1GSR_TCS_MASK C1GSR_TCS_MASK
#define C1GSR_TCS 0x8
#define CAN1GSR_TCS C1GSR_TCS
#define C1GSR_TCS_BIT 3
#define CAN1GSR_TCS_BIT C1GSR_TCS_BIT
#define C1GSR_RS_MASK 0x10
#define CAN1GSR_RS_MASK C1GSR_RS_MASK
#define C1GSR_RS 0x10
#define CAN1GSR_RS C1GSR_RS
#define C1GSR_RS_BIT 4
#define CAN1GSR_RS_BIT C1GSR_RS_BIT
#define C1GSR_TS_MASK 0x20
#define CAN1GSR_TS_MASK C1GSR_TS_MASK
#define C1GSR_TS 0x20
#define CAN1GSR_TS C1GSR_TS
#define C1GSR_TS_BIT 5
#define CAN1GSR_TS_BIT C1GSR_TS_BIT
#define C1GSR_ES_MASK 0x40
#define CAN1GSR_ES_MASK C1GSR_ES_MASK
#define C1GSR_ES 0x40
#define CAN1GSR_ES C1GSR_ES
#define C1GSR_ES_BIT 6
#define CAN1GSR_ES_BIT C1GSR_ES_BIT
#define C1GSR_BS_MASK 0x80
#define CAN1GSR_BS_MASK C1GSR_BS_MASK
#define C1GSR_BS 0x80
#define CAN1GSR_BS C1GSR_BS
#define C1GSR_BS_BIT 7
#define CAN1GSR_BS_BIT C1GSR_BS_BIT
#define C1GSR_RXERR_MASK 0xFF0000
#define CAN1GSR_RXERR_MASK C1GSR_RXERR_MASK
#define C1GSR_RXERR_BIT 16
#define CAN1GSR_RXERR_BIT C1GSR_RXERR_BIT
#define C1GSR_TXERR_MASK 0xFF000000
#define CAN1GSR_TXERR_MASK C1GSR_TXERR_MASK
#define C1GSR_TXERR_BIT 24
#define CAN1GSR_TXERR_BIT C1GSR_TXERR_BIT

#define C1ICR (*(volatile unsigned long *)0xE004400C)
#define CAN1ICR C1ICR
#define C1ICR_OFFSET 0xC
#define CAN1ICR_OFFSET C1ICR_OFFSET
#define C1ICR_RI_MASK 0x1
#define CAN1ICR_RI_MASK C1ICR_RI_MASK
#define C1ICR_RI 0x1
#define CAN1ICR_RI C1ICR_RI
#define C1ICR_RI_BIT 0
#define CAN1ICR_RI_BIT C1ICR_RI_BIT
#define C1ICR_TI1_MASK 0x2
#define CAN1ICR_TI1_MASK C1ICR_TI1_MASK
#define C1ICR_TI1 0x2
#define CAN1ICR_TI1 C1ICR_TI1
#define C1ICR_TI1_BIT 1
#define CAN1ICR_TI1_BIT C1ICR_TI1_BIT
#define C1ICR_EI_MASK 0x4
#define CAN1ICR_EI_MASK C1ICR_EI_MASK
#define C1ICR_EI 0x4
#define CAN1ICR_EI C1ICR_EI
#define C1ICR_EI_BIT 2
#define CAN1ICR_EI_BIT C1ICR_EI_BIT
#define C1ICR_DOI_MASK 0x8
#define CAN1ICR_DOI_MASK C1ICR_DOI_MASK
#define C1ICR_DOI 0x8
#define CAN1ICR_DOI C1ICR_DOI
#define C1ICR_DOI_BIT 3
#define CAN1ICR_DOI_BIT C1ICR_DOI_BIT
#define C1ICR_WUI_MASK 0x10
#define CAN1ICR_WUI_MASK C1ICR_WUI_MASK
#define C1ICR_WUI 0x10
#define CAN1ICR_WUI C1ICR_WUI
#define C1ICR_WUI_BIT 4
#define CAN1ICR_WUI_BIT C1ICR_WUI_BIT
#define C1ICR_EPI_MASK 0x20
#define CAN1ICR_EPI_MASK C1ICR_EPI_MASK
#define C1ICR_EPI 0x20
#define CAN1ICR_EPI C1ICR_EPI
#define C1ICR_EPI_BIT 5
#define CAN1ICR_EPI_BIT C1ICR_EPI_BIT
#define C1ICR_ALI_MASK 0x40
#define CAN1ICR_ALI_MASK C1ICR_ALI_MASK
#define C1ICR_ALI 0x40
#define CAN1ICR_ALI C1ICR_ALI
#define C1ICR_ALI_BIT 6
#define CAN1ICR_ALI_BIT C1ICR_ALI_BIT
#define C1ICR_BEI_MASK 0x80
#define CAN1ICR_BEI_MASK C1ICR_BEI_MASK
#define C1ICR_BEI 0x80
#define CAN1ICR_BEI C1ICR_BEI
#define C1ICR_BEI_BIT 7
#define CAN1ICR_BEI_BIT C1ICR_BEI_BIT
#define C1ICR_IDI_MASK 0x100
#define CAN1ICR_IDI_MASK C1ICR_IDI_MASK
#define C1ICR_IDI 0x100
#define CAN1ICR_IDI C1ICR_IDI
#define C1ICR_IDI_BIT 8
#define CAN1ICR_IDI_BIT C1ICR_IDI_BIT
#define C1ICR_TI2_MASK 0x200
#define CAN1ICR_TI2_MASK C1ICR_TI2_MASK
#define C1ICR_TI2 0x200
#define CAN1ICR_TI2 C1ICR_TI2
#define C1ICR_TI2_BIT 9
#define CAN1ICR_TI2_BIT C1ICR_TI2_BIT
#define C1ICR_TI3_MASK 0x400
#define CAN1ICR_TI3_MASK C1ICR_TI3_MASK
#define C1ICR_TI3 0x400
#define CAN1ICR_TI3 C1ICR_TI3
#define C1ICR_TI3_BIT 10
#define CAN1ICR_TI3_BIT C1ICR_TI3_BIT
#define C1ICR_ERRBIT_MASK 0x1F0000
#define CAN1ICR_ERRBIT_MASK C1ICR_ERRBIT_MASK
#define C1ICR_ERRBIT_BIT 16
#define CAN1ICR_ERRBIT_BIT C1ICR_ERRBIT_BIT
#define C1ICR_ERRDIR_MASK 0x200000
#define CAN1ICR_ERRDIR_MASK C1ICR_ERRDIR_MASK
#define C1ICR_ERRDIR 0x200000
#define CAN1ICR_ERRDIR C1ICR_ERRDIR
#define C1ICR_ERRDIR_BIT 21
#define CAN1ICR_ERRDIR_BIT C1ICR_ERRDIR_BIT
#define C1ICR_ERRC_MASK 0xC00000
#define CAN1ICR_ERRC_MASK C1ICR_ERRC_MASK
#define C1ICR_ERRC_BIT 22
#define CAN1ICR_ERRC_BIT C1ICR_ERRC_BIT
#define C1ICR_ALCBIT_MASK 0x1F000000
#define CAN1ICR_ALCBIT_MASK C1ICR_ALCBIT_MASK
#define C1ICR_ALCBIT_BIT 24
#define CAN1ICR_ALCBIT_BIT C1ICR_ALCBIT_BIT

#define C1IER (*(volatile unsigned long *)0xE0044010)
#define CAN1IER C1IER
#define C1IER_OFFSET 0x10
#define CAN1IER_OFFSET C1IER_OFFSET
#define C1IER_RIE_MASK 0x1
#define CAN1IER_RIE_MASK C1IER_RIE_MASK
#define C1IER_RIE 0x1
#define CAN1IER_RIE C1IER_RIE
#define C1IER_RIE_BIT 0
#define CAN1IER_RIE_BIT C1IER_RIE_BIT
#define C1IER_TIE1_MASK 0x2
#define CAN1IER_TIE1_MASK C1IER_TIE1_MASK
#define C1IER_TIE1 0x2
#define CAN1IER_TIE1 C1IER_TIE1
#define C1IER_TIE1_BIT 1
#define CAN1IER_TIE1_BIT C1IER_TIE1_BIT
#define C1IER_EIE_MASK 0x4
#define CAN1IER_EIE_MASK C1IER_EIE_MASK
#define C1IER_EIE 0x4
#define CAN1IER_EIE C1IER_EIE
#define C1IER_EIE_BIT 2
#define CAN1IER_EIE_BIT C1IER_EIE_BIT
#define C1IER_DOIE_MASK 0x8
#define CAN1IER_DOIE_MASK C1IER_DOIE_MASK
#define C1IER_DOIE 0x8
#define CAN1IER_DOIE C1IER_DOIE
#define C1IER_DOIE_BIT 3
#define CAN1IER_DOIE_BIT C1IER_DOIE_BIT
#define C1IER_WUIE_MASK 0x10
#define CAN1IER_WUIE_MASK C1IER_WUIE_MASK
#define C1IER_WUIE 0x10
#define CAN1IER_WUIE C1IER_WUIE
#define C1IER_WUIE_BIT 4
#define CAN1IER_WUIE_BIT C1IER_WUIE_BIT
#define C1IER_EPIE_MASK 0x20
#define CAN1IER_EPIE_MASK C1IER_EPIE_MASK
#define C1IER_EPIE 0x20
#define CAN1IER_EPIE C1IER_EPIE
#define C1IER_EPIE_BIT 5
#define CAN1IER_EPIE_BIT C1IER_EPIE_BIT
#define C1IER_ALIE_MASK 0x40
#define CAN1IER_ALIE_MASK C1IER_ALIE_MASK
#define C1IER_ALIE 0x40
#define CAN1IER_ALIE C1IER_ALIE
#define C1IER_ALIE_BIT 6
#define CAN1IER_ALIE_BIT C1IER_ALIE_BIT
#define C1IER_BEIE_MASK 0x80
#define CAN1IER_BEIE_MASK C1IER_BEIE_MASK
#define C1IER_BEIE 0x80
#define CAN1IER_BEIE C1IER_BEIE
#define C1IER_BEIE_BIT 7
#define CAN1IER_BEIE_BIT C1IER_BEIE_BIT
#define C1IER_IDIE_MASK 0x100
#define CAN1IER_IDIE_MASK C1IER_IDIE_MASK
#define C1IER_IDIE 0x100
#define CAN1IER_IDIE C1IER_IDIE
#define C1IER_IDIE_BIT 8
#define CAN1IER_IDIE_BIT C1IER_IDIE_BIT
#define C1IER_TIE2_MASK 0x200
#define CAN1IER_TIE2_MASK C1IER_TIE2_MASK
#define C1IER_TIE2 0x200
#define CAN1IER_TIE2 C1IER_TIE2
#define C1IER_TIE2_BIT 9
#define CAN1IER_TIE2_BIT C1IER_TIE2_BIT
#define C1IER_TIE3_MASK 0x400
#define CAN1IER_TIE3_MASK C1IER_TIE3_MASK
#define C1IER_TIE3 0x400
#define CAN1IER_TIE3 C1IER_TIE3
#define C1IER_TIE3_BIT 10
#define CAN1IER_TIE3_BIT C1IER_TIE3_BIT

#define C1BTR (*(volatile unsigned long *)0xE0044014)
#define CAN1BTR C1BTR
#define C1BTR_OFFSET 0x14
#define CAN1BTR_OFFSET C1BTR_OFFSET
#define C1BTR_BRP_MASK 0x3FF
#define CAN1BTR_BRP_MASK C1BTR_BRP_MASK
#define C1BTR_BRP_BIT 0
#define CAN1BTR_BRP_BIT C1BTR_BRP_BIT
#define C1BTR_SJW_MASK 0xC000
#define CAN1BTR_SJW_MASK C1BTR_SJW_MASK
#define C1BTR_SJW_BIT 14
#define CAN1BTR_SJW_BIT C1BTR_SJW_BIT
#define C1BTR_TSEG1_MASK 0xF0000
#define CAN1BTR_TSEG1_MASK C1BTR_TSEG1_MASK
#define C1BTR_TSEG1_BIT 16
#define CAN1BTR_TSEG1_BIT C1BTR_TSEG1_BIT
#define C1BTR_TSEG2_MASK 0x700000
#define CAN1BTR_TSEG2_MASK C1BTR_TSEG2_MASK
#define C1BTR_TSEG2_BIT 20
#define CAN1BTR_TSEG2_BIT C1BTR_TSEG2_BIT
#define C1BTR_SAM_MASK 0x800000
#define CAN1BTR_SAM_MASK C1BTR_SAM_MASK
#define C1BTR_SAM 0x800000
#define CAN1BTR_SAM C1BTR_SAM
#define C1BTR_SAM_BIT 23
#define CAN1BTR_SAM_BIT C1BTR_SAM_BIT

#define C1EWL (*(volatile unsigned long *)0xE0044018)
#define CAN1EWL C1EWL
#define C1EWL_OFFSET 0x18
#define CAN1EWL_OFFSET C1EWL_OFFSET
#define C1EWL_EWL_MASK 0xFF
#define CAN1EWL_EWL_MASK C1EWL_EWL_MASK
#define C1EWL_EWL_BIT 0
#define CAN1EWL_EWL_BIT C1EWL_EWL_BIT

#define C1SR (*(volatile unsigned long *)0xE004401C)
#define CAN1SR C1SR
#define C1SR_OFFSET 0x1C
#define CAN1SR_OFFSET C1SR_OFFSET
#define C1SR_RBS_MASK 0x1
#define CAN1SR_RBS_MASK C1SR_RBS_MASK
#define C1SR_RBS 0x1
#define CAN1SR_RBS C1SR_RBS
#define C1SR_RBS_BIT 0
#define CAN1SR_RBS_BIT C1SR_RBS_BIT
#define C1SR_DOS_MASK 0x2
#define CAN1SR_DOS_MASK C1SR_DOS_MASK
#define C1SR_DOS 0x2
#define CAN1SR_DOS C1SR_DOS
#define C1SR_DOS_BIT 1
#define CAN1SR_DOS_BIT C1SR_DOS_BIT
#define C1SR_TBS1_MASK 0x4
#define CAN1SR_TBS1_MASK C1SR_TBS1_MASK
#define C1SR_TBS1 0x4
#define CAN1SR_TBS1 C1SR_TBS1
#define C1SR_TBS1_BIT 2
#define CAN1SR_TBS1_BIT C1SR_TBS1_BIT
#define C1SR_TCS1_MASK 0x8
#define CAN1SR_TCS1_MASK C1SR_TCS1_MASK
#define C1SR_TCS1 0x8
#define CAN1SR_TCS1 C1SR_TCS1
#define C1SR_TCS1_BIT 3
#define CAN1SR_TCS1_BIT C1SR_TCS1_BIT
#define C1SR_RS_MASK 0x10
#define CAN1SR_RS_MASK C1SR_RS_MASK
#define C1SR_RS 0x10
#define CAN1SR_RS C1SR_RS
#define C1SR_RS_BIT 4
#define CAN1SR_RS_BIT C1SR_RS_BIT
#define C1SR_TS1_MASK 0x20
#define CAN1SR_TS1_MASK C1SR_TS1_MASK
#define C1SR_TS1 0x20
#define CAN1SR_TS1 C1SR_TS1
#define C1SR_TS1_BIT 5
#define CAN1SR_TS1_BIT C1SR_TS1_BIT
#define C1SR_ES_MASK 0x40
#define CAN1SR_ES_MASK C1SR_ES_MASK
#define C1SR_ES 0x40
#define CAN1SR_ES C1SR_ES
#define C1SR_ES_BIT 6
#define CAN1SR_ES_BIT C1SR_ES_BIT
#define C1SR_BS_MASK 0x80
#define CAN1SR_BS_MASK C1SR_BS_MASK
#define C1SR_BS 0x80
#define CAN1SR_BS C1SR_BS
#define C1SR_BS_BIT 7
#define CAN1SR_BS_BIT C1SR_BS_BIT
#define C1SR_RBS2_MASK 0x100
#define CAN1SR_RBS2_MASK C1SR_RBS2_MASK
#define C1SR_RBS2 0x100
#define CAN1SR_RBS2 C1SR_RBS2
#define C1SR_RBS2_BIT 8
#define CAN1SR_RBS2_BIT C1SR_RBS2_BIT
#define C1SR_DOS2_MASK 0x200
#define CAN1SR_DOS2_MASK C1SR_DOS2_MASK
#define C1SR_DOS2 0x200
#define CAN1SR_DOS2 C1SR_DOS2
#define C1SR_DOS2_BIT 9
#define CAN1SR_DOS2_BIT C1SR_DOS2_BIT
#define C1SR_TBS2_MASK 0x400
#define CAN1SR_TBS2_MASK C1SR_TBS2_MASK
#define C1SR_TBS2 0x400
#define CAN1SR_TBS2 C1SR_TBS2
#define C1SR_TBS2_BIT 10
#define CAN1SR_TBS2_BIT C1SR_TBS2_BIT
#define C1SR_TCS2_MASK 0x800
#define CAN1SR_TCS2_MASK C1SR_TCS2_MASK
#define C1SR_TCS2 0x800
#define CAN1SR_TCS2 C1SR_TCS2
#define C1SR_TCS2_BIT 11
#define CAN1SR_TCS2_BIT C1SR_TCS2_BIT
#define C1SR_RS2_MASK 0x1000
#define CAN1SR_RS2_MASK C1SR_RS2_MASK
#define C1SR_RS2 0x1000
#define CAN1SR_RS2 C1SR_RS2
#define C1SR_RS2_BIT 12
#define CAN1SR_RS2_BIT C1SR_RS2_BIT
#define C1SR_TS2_MASK 0x2000
#define CAN1SR_TS2_MASK C1SR_TS2_MASK
#define C1SR_TS2 0x2000
#define CAN1SR_TS2 C1SR_TS2
#define C1SR_TS2_BIT 13
#define CAN1SR_TS2_BIT C1SR_TS2_BIT
#define C1SR_ES2_MASK 0x4000
#define CAN1SR_ES2_MASK C1SR_ES2_MASK
#define C1SR_ES2 0x4000
#define CAN1SR_ES2 C1SR_ES2
#define C1SR_ES2_BIT 14
#define CAN1SR_ES2_BIT C1SR_ES2_BIT
#define C1SR_BS2_MASK 0x8000
#define CAN1SR_BS2_MASK C1SR_BS2_MASK
#define C1SR_BS2 0x8000
#define CAN1SR_BS2 C1SR_BS2
#define C1SR_BS2_BIT 15
#define CAN1SR_BS2_BIT C1SR_BS2_BIT
#define C1SR_RBS3_MASK 0x10000
#define CAN1SR_RBS3_MASK C1SR_RBS3_MASK
#define C1SR_RBS3 0x10000
#define CAN1SR_RBS3 C1SR_RBS3
#define C1SR_RBS3_BIT 16
#define CAN1SR_RBS3_BIT C1SR_RBS3_BIT
#define C1SR_DOS3_MASK 0x20000
#define CAN1SR_DOS3_MASK C1SR_DOS3_MASK
#define C1SR_DOS3 0x20000
#define CAN1SR_DOS3 C1SR_DOS3
#define C1SR_DOS3_BIT 17
#define CAN1SR_DOS3_BIT C1SR_DOS3_BIT
#define C1SR_TBS3_MASK 0x40000
#define CAN1SR_TBS3_MASK C1SR_TBS3_MASK
#define C1SR_TBS3 0x40000
#define CAN1SR_TBS3 C1SR_TBS3
#define C1SR_TBS3_BIT 18
#define CAN1SR_TBS3_BIT C1SR_TBS3_BIT
#define C1SR_TCS3_MASK 0x80000
#define CAN1SR_TCS3_MASK C1SR_TCS3_MASK
#define C1SR_TCS3 0x80000
#define CAN1SR_TCS3 C1SR_TCS3
#define C1SR_TCS3_BIT 19
#define CAN1SR_TCS3_BIT C1SR_TCS3_BIT
#define C1SR_RS3_MASK 0x100000
#define CAN1SR_RS3_MASK C1SR_RS3_MASK
#define C1SR_RS3 0x100000
#define CAN1SR_RS3 C1SR_RS3
#define C1SR_RS3_BIT 20
#define CAN1SR_RS3_BIT C1SR_RS3_BIT
#define C1SR_TS3_MASK 0x200000
#define CAN1SR_TS3_MASK C1SR_TS3_MASK
#define C1SR_TS3 0x200000
#define CAN1SR_TS3 C1SR_TS3
#define C1SR_TS3_BIT 21
#define CAN1SR_TS3_BIT C1SR_TS3_BIT
#define C1SR_ES3_MASK 0x400000
#define CAN1SR_ES3_MASK C1SR_ES3_MASK
#define C1SR_ES3 0x400000
#define CAN1SR_ES3 C1SR_ES3
#define C1SR_ES3_BIT 22
#define CAN1SR_ES3_BIT C1SR_ES3_BIT
#define C1SR_BS3_MASK 0x800000
#define CAN1SR_BS3_MASK C1SR_BS3_MASK
#define C1SR_BS3 0x800000
#define CAN1SR_BS3 C1SR_BS3
#define C1SR_BS3_BIT 23
#define CAN1SR_BS3_BIT C1SR_BS3_BIT

#define C1RFS (*(volatile unsigned long *)0xE0044020)
#define CAN1RFS C1RFS
#define C1RFS_OFFSET 0x20
#define CAN1RFS_OFFSET C1RFS_OFFSET
#define C1RFS_ID_Index_MASK 0x3FF
#define CAN1RFS_ID_Index_MASK C1RFS_ID_Index_MASK
#define C1RFS_ID_Index_BIT 0
#define CAN1RFS_ID_Index_BIT C1RFS_ID_Index_BIT
#define C1RFS_BP_MASK 0x400
#define CAN1RFS_BP_MASK C1RFS_BP_MASK
#define C1RFS_BP 0x400
#define CAN1RFS_BP C1RFS_BP
#define C1RFS_BP_BIT 10
#define CAN1RFS_BP_BIT C1RFS_BP_BIT
#define C1RFS_DLC_MASK 0xF0000
#define CAN1RFS_DLC_MASK C1RFS_DLC_MASK
#define C1RFS_DLC_BIT 16
#define CAN1RFS_DLC_BIT C1RFS_DLC_BIT
#define C1RFS_RTR_MASK 0x40000000
#define CAN1RFS_RTR_MASK C1RFS_RTR_MASK
#define C1RFS_RTR 0x40000000
#define CAN1RFS_RTR C1RFS_RTR
#define C1RFS_RTR_BIT 30
#define CAN1RFS_RTR_BIT C1RFS_RTR_BIT
#define C1RFS_FF_MASK 0x80000000
#define CAN1RFS_FF_MASK C1RFS_FF_MASK
#define C1RFS_FF 0x80000000
#define CAN1RFS_FF C1RFS_FF
#define C1RFS_FF_BIT 31
#define CAN1RFS_FF_BIT C1RFS_FF_BIT

#define C1RID (*(volatile unsigned long *)0xE0044024)
#define CAN1RID C1RID
#define C1RID_OFFSET 0x24
#define CAN1RID_OFFSET C1RID_OFFSET
#define C1RID_ID_MASK 0x7FF
#define CAN1RID_ID_MASK C1RID_ID_MASK
#define C1RID_ID_BIT 0
#define CAN1RID_ID_BIT C1RID_ID_BIT

#define C1RDA (*(volatile unsigned long *)0xE0044028)
#define CAN1RDA C1RDA
#define C1RDA_OFFSET 0x28
#define CAN1RDA_OFFSET C1RDA_OFFSET
#define C1RDA_Data_1_MASK 0xFF
#define CAN1RDA_Data_1_MASK C1RDA_Data_1_MASK
#define C1RDA_Data_1_BIT 0
#define CAN1RDA_Data_1_BIT C1RDA_Data_1_BIT
#define C1RDA_Data_2_MASK 0xFF00
#define CAN1RDA_Data_2_MASK C1RDA_Data_2_MASK
#define C1RDA_Data_2_BIT 8
#define CAN1RDA_Data_2_BIT C1RDA_Data_2_BIT
#define C1RDA_Data_3_MASK 0xFF0000
#define CAN1RDA_Data_3_MASK C1RDA_Data_3_MASK
#define C1RDA_Data_3_BIT 16
#define CAN1RDA_Data_3_BIT C1RDA_Data_3_BIT
#define C1RDA_Data_4_MASK 0xFF000000
#define CAN1RDA_Data_4_MASK C1RDA_Data_4_MASK
#define C1RDA_Data_4_BIT 24
#define CAN1RDA_Data_4_BIT C1RDA_Data_4_BIT

#define C1RDB (*(volatile unsigned long *)0xE004402C)
#define CAN1RDB C1RDB
#define C1RDB_OFFSET 0x2C
#define CAN1RDB_OFFSET C1RDB_OFFSET
#define C1RDB_Data_5_MASK 0xFF
#define CAN1RDB_Data_5_MASK C1RDB_Data_5_MASK
#define C1RDB_Data_5_BIT 0
#define CAN1RDB_Data_5_BIT C1RDB_Data_5_BIT
#define C1RDB_Data_6_MASK 0xFF00
#define CAN1RDB_Data_6_MASK C1RDB_Data_6_MASK
#define C1RDB_Data_6_BIT 8
#define CAN1RDB_Data_6_BIT C1RDB_Data_6_BIT
#define C1RDB_Data_7_MASK 0xFF0000
#define CAN1RDB_Data_7_MASK C1RDB_Data_7_MASK
#define C1RDB_Data_7_BIT 16
#define CAN1RDB_Data_7_BIT C1RDB_Data_7_BIT
#define C1RDB_Data_8_MASK 0xFF000000
#define CAN1RDB_Data_8_MASK C1RDB_Data_8_MASK
#define C1RDB_Data_8_BIT 24
#define CAN1RDB_Data_8_BIT C1RDB_Data_8_BIT

#define C1TFI1 (*(volatile unsigned long *)0xE0044030)
#define CAN1TFI1 C1TFI1
#define C1TFI1_OFFSET 0x30
#define CAN1TFI1_OFFSET C1TFI1_OFFSET
#define C1TFI1_PRIO_MASK 0xFF
#define CAN1TFI1_PRIO_MASK C1TFI1_PRIO_MASK
#define C1TFI1_PRIO_BIT 0
#define CAN1TFI1_PRIO_BIT C1TFI1_PRIO_BIT
#define C1TFI1_DLC_MASK 0xF0000
#define CAN1TFI1_DLC_MASK C1TFI1_DLC_MASK
#define C1TFI1_DLC_BIT 16
#define CAN1TFI1_DLC_BIT C1TFI1_DLC_BIT
#define C1TFI1_RTR_MASK 0x40000000
#define CAN1TFI1_RTR_MASK C1TFI1_RTR_MASK
#define C1TFI1_RTR 0x40000000
#define CAN1TFI1_RTR C1TFI1_RTR
#define C1TFI1_RTR_BIT 30
#define CAN1TFI1_RTR_BIT C1TFI1_RTR_BIT
#define C1TFI1_FF_MASK 0x80000000
#define CAN1TFI1_FF_MASK C1TFI1_FF_MASK
#define C1TFI1_FF 0x80000000
#define CAN1TFI1_FF C1TFI1_FF
#define C1TFI1_FF_BIT 31
#define CAN1TFI1_FF_BIT C1TFI1_FF_BIT

#define C1TID1 (*(volatile unsigned long *)0xE0044034)
#define CAN1TID1 C1TID1
#define C1TID1_OFFSET 0x34
#define CAN1TID1_OFFSET C1TID1_OFFSET
#define C1TID1_ID_MASK 0x7FF
#define CAN1TID1_ID_MASK C1TID1_ID_MASK
#define C1TID1_ID_BIT 0
#define CAN1TID1_ID_BIT C1TID1_ID_BIT

#define C1TDA1 (*(volatile unsigned long *)0xE0044038)
#define CAN1TDA1 C1TDA1
#define C1TDA1_OFFSET 0x38
#define CAN1TDA1_OFFSET C1TDA1_OFFSET
#define C1TDA1_Data_1_MASK 0xFF
#define CAN1TDA1_Data_1_MASK C1TDA1_Data_1_MASK
#define C1TDA1_Data_1_BIT 0
#define CAN1TDA1_Data_1_BIT C1TDA1_Data_1_BIT
#define C1TDA1_Data_2_MASK 0xFF00
#define CAN1TDA1_Data_2_MASK C1TDA1_Data_2_MASK
#define C1TDA1_Data_2_BIT 8
#define CAN1TDA1_Data_2_BIT C1TDA1_Data_2_BIT
#define C1TDA1_Data_3_MASK 0xFF0000
#define CAN1TDA1_Data_3_MASK C1TDA1_Data_3_MASK
#define C1TDA1_Data_3_BIT 16
#define CAN1TDA1_Data_3_BIT C1TDA1_Data_3_BIT
#define C1TDA1_Data_4_MASK 0xFF000000
#define CAN1TDA1_Data_4_MASK C1TDA1_Data_4_MASK
#define C1TDA1_Data_4_BIT 24
#define CAN1TDA1_Data_4_BIT C1TDA1_Data_4_BIT

#define C1TDB1 (*(volatile unsigned long *)0xE004403C)
#define CAN1TDB1 C1TDB1
#define C1TDB1_OFFSET 0x3C
#define CAN1TDB1_OFFSET C1TDB1_OFFSET
#define C1TDB1_Data_5_MASK 0xFF
#define CAN1TDB1_Data_5_MASK C1TDB1_Data_5_MASK
#define C1TDB1_Data_5_BIT 0
#define CAN1TDB1_Data_5_BIT C1TDB1_Data_5_BIT
#define C1TDB1_Data_6_MASK 0xFF00
#define CAN1TDB1_Data_6_MASK C1TDB1_Data_6_MASK
#define C1TDB1_Data_6_BIT 8
#define CAN1TDB1_Data_6_BIT C1TDB1_Data_6_BIT
#define C1TDB1_Data_7_MASK 0xFF0000
#define CAN1TDB1_Data_7_MASK C1TDB1_Data_7_MASK
#define C1TDB1_Data_7_BIT 16
#define CAN1TDB1_Data_7_BIT C1TDB1_Data_7_BIT
#define C1TDB1_Data_8_MASK 0xFF000000
#define CAN1TDB1_Data_8_MASK C1TDB1_Data_8_MASK
#define C1TDB1_Data_8_BIT 24
#define CAN1TDB1_Data_8_BIT C1TDB1_Data_8_BIT

#define C1TFI2 (*(volatile unsigned long *)0xE0044040)
#define CAN1TFI2 C1TFI2
#define C1TFI2_OFFSET 0x40
#define CAN1TFI2_OFFSET C1TFI2_OFFSET
#define C1TFI2_PRIO_MASK 0xFF
#define CAN1TFI2_PRIO_MASK C1TFI2_PRIO_MASK
#define C1TFI2_PRIO_BIT 0
#define CAN1TFI2_PRIO_BIT C1TFI2_PRIO_BIT
#define C1TFI2_DLC_MASK 0xF0000
#define CAN1TFI2_DLC_MASK C1TFI2_DLC_MASK
#define C1TFI2_DLC_BIT 16
#define CAN1TFI2_DLC_BIT C1TFI2_DLC_BIT
#define C1TFI2_RTR_MASK 0x40000000
#define CAN1TFI2_RTR_MASK C1TFI2_RTR_MASK
#define C1TFI2_RTR 0x40000000
#define CAN1TFI2_RTR C1TFI2_RTR
#define C1TFI2_RTR_BIT 30
#define CAN1TFI2_RTR_BIT C1TFI2_RTR_BIT
#define C1TFI2_FF_MASK 0x80000000
#define CAN1TFI2_FF_MASK C1TFI2_FF_MASK
#define C1TFI2_FF 0x80000000
#define CAN1TFI2_FF C1TFI2_FF
#define C1TFI2_FF_BIT 31
#define CAN1TFI2_FF_BIT C1TFI2_FF_BIT

#define C1TID2 (*(volatile unsigned long *)0xE0044044)
#define CAN1TID2 C1TID2
#define C1TID2_OFFSET 0x44
#define CAN1TID2_OFFSET C1TID2_OFFSET
#define C1TID2_ID_MASK 0x7FF
#define CAN1TID2_ID_MASK C1TID2_ID_MASK
#define C1TID2_ID_BIT 0
#define CAN1TID2_ID_BIT C1TID2_ID_BIT

#define C1TDA2 (*(volatile unsigned long *)0xE0044048)
#define CAN1TDA2 C1TDA2
#define C1TDA2_OFFSET 0x48
#define CAN1TDA2_OFFSET C1TDA2_OFFSET
#define C1TDA2_Data_1_MASK 0xFF
#define CAN1TDA2_Data_1_MASK C1TDA2_Data_1_MASK
#define C1TDA2_Data_1_BIT 0
#define CAN1TDA2_Data_1_BIT C1TDA2_Data_1_BIT
#define C1TDA2_Data_2_MASK 0xFF00
#define CAN1TDA2_Data_2_MASK C1TDA2_Data_2_MASK
#define C1TDA2_Data_2_BIT 8
#define CAN1TDA2_Data_2_BIT C1TDA2_Data_2_BIT
#define C1TDA2_Data_3_MASK 0xFF0000
#define CAN1TDA2_Data_3_MASK C1TDA2_Data_3_MASK
#define C1TDA2_Data_3_BIT 16
#define CAN1TDA2_Data_3_BIT C1TDA2_Data_3_BIT
#define C1TDA2_Data_4_MASK 0xFF000000
#define CAN1TDA2_Data_4_MASK C1TDA2_Data_4_MASK
#define C1TDA2_Data_4_BIT 24
#define CAN1TDA2_Data_4_BIT C1TDA2_Data_4_BIT

#define C1TDB2 (*(volatile unsigned long *)0xE004404C)
#define CAN1TDB2 C1TDB2
#define C1TDB2_OFFSET 0x4C
#define CAN1TDB2_OFFSET C1TDB2_OFFSET
#define C1TDB2_Data_5_MASK 0xFF
#define CAN1TDB2_Data_5_MASK C1TDB2_Data_5_MASK
#define C1TDB2_Data_5_BIT 0
#define CAN1TDB2_Data_5_BIT C1TDB2_Data_5_BIT
#define C1TDB2_Data_6_MASK 0xFF00
#define CAN1TDB2_Data_6_MASK C1TDB2_Data_6_MASK
#define C1TDB2_Data_6_BIT 8
#define CAN1TDB2_Data_6_BIT C1TDB2_Data_6_BIT
#define C1TDB2_Data_7_MASK 0xFF0000
#define CAN1TDB2_Data_7_MASK C1TDB2_Data_7_MASK
#define C1TDB2_Data_7_BIT 16
#define CAN1TDB2_Data_7_BIT C1TDB2_Data_7_BIT
#define C1TDB2_Data_8_MASK 0xFF000000
#define CAN1TDB2_Data_8_MASK C1TDB2_Data_8_MASK
#define C1TDB2_Data_8_BIT 24
#define CAN1TDB2_Data_8_BIT C1TDB2_Data_8_BIT

#define C1TFI3 (*(volatile unsigned long *)0xE0044050)
#define CAN1TFI3 C1TFI3
#define C1TFI3_OFFSET 0x50
#define CAN1TFI3_OFFSET C1TFI3_OFFSET
#define C1TFI3_PRIO_MASK 0xFF
#define CAN1TFI3_PRIO_MASK C1TFI3_PRIO_MASK
#define C1TFI3_PRIO_BIT 0
#define CAN1TFI3_PRIO_BIT C1TFI3_PRIO_BIT
#define C1TFI3_DLC_MASK 0xF0000
#define CAN1TFI3_DLC_MASK C1TFI3_DLC_MASK
#define C1TFI3_DLC_BIT 16
#define CAN1TFI3_DLC_BIT C1TFI3_DLC_BIT
#define C1TFI3_RTR_MASK 0x40000000
#define CAN1TFI3_RTR_MASK C1TFI3_RTR_MASK
#define C1TFI3_RTR 0x40000000
#define CAN1TFI3_RTR C1TFI3_RTR
#define C1TFI3_RTR_BIT 30
#define CAN1TFI3_RTR_BIT C1TFI3_RTR_BIT
#define C1TFI3_FF_MASK 0x80000000
#define CAN1TFI3_FF_MASK C1TFI3_FF_MASK
#define C1TFI3_FF 0x80000000
#define CAN1TFI3_FF C1TFI3_FF
#define C1TFI3_FF_BIT 31
#define CAN1TFI3_FF_BIT C1TFI3_FF_BIT

#define C1TID3 (*(volatile unsigned long *)0xE0044054)
#define CAN1TID3 C1TID3
#define C1TID3_OFFSET 0x54
#define CAN1TID3_OFFSET C1TID3_OFFSET
#define C1TID3_ID_MASK 0x7FF
#define CAN1TID3_ID_MASK C1TID3_ID_MASK
#define C1TID3_ID_BIT 0
#define CAN1TID3_ID_BIT C1TID3_ID_BIT

#define C1TDA3 (*(volatile unsigned long *)0xE0044058)
#define CAN1TDA3 C1TDA3
#define C1TDA3_OFFSET 0x58
#define CAN1TDA3_OFFSET C1TDA3_OFFSET
#define C1TDA3_Data_1_MASK 0xFF
#define CAN1TDA3_Data_1_MASK C1TDA3_Data_1_MASK
#define C1TDA3_Data_1_BIT 0
#define CAN1TDA3_Data_1_BIT C1TDA3_Data_1_BIT
#define C1TDA3_Data_2_MASK 0xFF00
#define CAN1TDA3_Data_2_MASK C1TDA3_Data_2_MASK
#define C1TDA3_Data_2_BIT 8
#define CAN1TDA3_Data_2_BIT C1TDA3_Data_2_BIT
#define C1TDA3_Data_3_MASK 0xFF0000
#define CAN1TDA3_Data_3_MASK C1TDA3_Data_3_MASK
#define C1TDA3_Data_3_BIT 16
#define CAN1TDA3_Data_3_BIT C1TDA3_Data_3_BIT
#define C1TDA3_Data_4_MASK 0xFF000000
#define CAN1TDA3_Data_4_MASK C1TDA3_Data_4_MASK
#define C1TDA3_Data_4_BIT 24
#define CAN1TDA3_Data_4_BIT C1TDA3_Data_4_BIT

#define C1TDB3 (*(volatile unsigned long *)0xE004405C)
#define CAN1TDB3 C1TDB3
#define C1TDB3_OFFSET 0x5C
#define CAN1TDB3_OFFSET C1TDB3_OFFSET
#define C1TDB3_Data_5_MASK 0xFF
#define CAN1TDB3_Data_5_MASK C1TDB3_Data_5_MASK
#define C1TDB3_Data_5_BIT 0
#define CAN1TDB3_Data_5_BIT C1TDB3_Data_5_BIT
#define C1TDB3_Data_6_MASK 0xFF00
#define CAN1TDB3_Data_6_MASK C1TDB3_Data_6_MASK
#define C1TDB3_Data_6_BIT 8
#define CAN1TDB3_Data_6_BIT C1TDB3_Data_6_BIT
#define C1TDB3_Data_7_MASK 0xFF0000
#define CAN1TDB3_Data_7_MASK C1TDB3_Data_7_MASK
#define C1TDB3_Data_7_BIT 16
#define CAN1TDB3_Data_7_BIT C1TDB3_Data_7_BIT
#define C1TDB3_Data_8_MASK 0xFF000000
#define CAN1TDB3_Data_8_MASK C1TDB3_Data_8_MASK
#define C1TDB3_Data_8_BIT 24
#define CAN1TDB3_Data_8_BIT C1TDB3_Data_8_BIT

#define CAN2_BASE 0xE0048000

#define C2MOD (*(volatile unsigned long *)0xE0048000)
#define CAN2MOD C2MOD
#define C2MOD_OFFSET 0x0
#define CAN2MOD_OFFSET C2MOD_OFFSET
#define C2MOD_RM_MASK 0x1
#define CAN2MOD_RM_MASK C2MOD_RM_MASK
#define C2MOD_RM 0x1
#define CAN2MOD_RM C2MOD_RM
#define C2MOD_RM_BIT 0
#define CAN2MOD_RM_BIT C2MOD_RM_BIT
#define C2MOD_LOM_MASK 0x2
#define CAN2MOD_LOM_MASK C2MOD_LOM_MASK
#define C2MOD_LOM 0x2
#define CAN2MOD_LOM C2MOD_LOM
#define C2MOD_LOM_BIT 1
#define CAN2MOD_LOM_BIT C2MOD_LOM_BIT
#define C2MOD_STM_MASK 0x4
#define CAN2MOD_STM_MASK C2MOD_STM_MASK
#define C2MOD_STM 0x4
#define CAN2MOD_STM C2MOD_STM
#define C2MOD_STM_BIT 2
#define CAN2MOD_STM_BIT C2MOD_STM_BIT
#define C2MOD_TPM_MASK 0x8
#define CAN2MOD_TPM_MASK C2MOD_TPM_MASK
#define C2MOD_TPM 0x8
#define CAN2MOD_TPM C2MOD_TPM
#define C2MOD_TPM_BIT 3
#define CAN2MOD_TPM_BIT C2MOD_TPM_BIT
#define C2MOD_SM_MASK 0x10
#define CAN2MOD_SM_MASK C2MOD_SM_MASK
#define C2MOD_SM 0x10
#define CAN2MOD_SM C2MOD_SM
#define C2MOD_SM_BIT 4
#define CAN2MOD_SM_BIT C2MOD_SM_BIT
#define C2MOD_RPM_MASK 0x20
#define CAN2MOD_RPM_MASK C2MOD_RPM_MASK
#define C2MOD_RPM 0x20
#define CAN2MOD_RPM C2MOD_RPM
#define C2MOD_RPM_BIT 5
#define CAN2MOD_RPM_BIT C2MOD_RPM_BIT
#define C2MOD_TM_MASK 0x80
#define CAN2MOD_TM_MASK C2MOD_TM_MASK
#define C2MOD_TM 0x80
#define CAN2MOD_TM C2MOD_TM
#define C2MOD_TM_BIT 7
#define CAN2MOD_TM_BIT C2MOD_TM_BIT

#define C2CMR (*(volatile unsigned long *)0xE0048004)
#define CAN2CMR C2CMR
#define C2CMR_OFFSET 0x4
#define CAN2CMR_OFFSET C2CMR_OFFSET
#define C2CMR_TR_MASK 0x1
#define CAN2CMR_TR_MASK C2CMR_TR_MASK
#define C2CMR_TR 0x1
#define CAN2CMR_TR C2CMR_TR
#define C2CMR_TR_BIT 0
#define CAN2CMR_TR_BIT C2CMR_TR_BIT
#define C2CMR_AT_MASK 0x2
#define CAN2CMR_AT_MASK C2CMR_AT_MASK
#define C2CMR_AT 0x2
#define CAN2CMR_AT C2CMR_AT
#define C2CMR_AT_BIT 1
#define CAN2CMR_AT_BIT C2CMR_AT_BIT
#define C2CMR_RRB_MASK 0x4
#define CAN2CMR_RRB_MASK C2CMR_RRB_MASK
#define C2CMR_RRB 0x4
#define CAN2CMR_RRB C2CMR_RRB
#define C2CMR_RRB_BIT 2
#define CAN2CMR_RRB_BIT C2CMR_RRB_BIT
#define C2CMR_CDO_MASK 0x8
#define CAN2CMR_CDO_MASK C2CMR_CDO_MASK
#define C2CMR_CDO 0x8
#define CAN2CMR_CDO C2CMR_CDO
#define C2CMR_CDO_BIT 3
#define CAN2CMR_CDO_BIT C2CMR_CDO_BIT
#define C2CMR_SRR_MASK 0x10
#define CAN2CMR_SRR_MASK C2CMR_SRR_MASK
#define C2CMR_SRR 0x10
#define CAN2CMR_SRR C2CMR_SRR
#define C2CMR_SRR_BIT 4
#define CAN2CMR_SRR_BIT C2CMR_SRR_BIT
#define C2CMR_STB1_MASK 0x20
#define CAN2CMR_STB1_MASK C2CMR_STB1_MASK
#define C2CMR_STB1 0x20
#define CAN2CMR_STB1 C2CMR_STB1
#define C2CMR_STB1_BIT 5
#define CAN2CMR_STB1_BIT C2CMR_STB1_BIT
#define C2CMR_STB2_MASK 0x40
#define CAN2CMR_STB2_MASK C2CMR_STB2_MASK
#define C2CMR_STB2 0x40
#define CAN2CMR_STB2 C2CMR_STB2
#define C2CMR_STB2_BIT 6
#define CAN2CMR_STB2_BIT C2CMR_STB2_BIT
#define C2CMR_STB3_MASK 0x80
#define CAN2CMR_STB3_MASK C2CMR_STB3_MASK
#define C2CMR_STB3 0x80
#define CAN2CMR_STB3 C2CMR_STB3
#define C2CMR_STB3_BIT 7
#define CAN2CMR_STB3_BIT C2CMR_STB3_BIT

#define C2GSR (*(volatile unsigned long *)0xE0048008)
#define CAN2GSR C2GSR
#define C2GSR_OFFSET 0x8
#define CAN2GSR_OFFSET C2GSR_OFFSET
#define C2GSR_RBS_MASK 0x1
#define CAN2GSR_RBS_MASK C2GSR_RBS_MASK
#define C2GSR_RBS 0x1
#define CAN2GSR_RBS C2GSR_RBS
#define C2GSR_RBS_BIT 0
#define CAN2GSR_RBS_BIT C2GSR_RBS_BIT
#define C2GSR_DOS_MASK 0x2
#define CAN2GSR_DOS_MASK C2GSR_DOS_MASK
#define C2GSR_DOS 0x2
#define CAN2GSR_DOS C2GSR_DOS
#define C2GSR_DOS_BIT 1
#define CAN2GSR_DOS_BIT C2GSR_DOS_BIT
#define C2GSR_TBS_MASK 0x4
#define CAN2GSR_TBS_MASK C2GSR_TBS_MASK
#define C2GSR_TBS 0x4
#define CAN2GSR_TBS C2GSR_TBS
#define C2GSR_TBS_BIT 2
#define CAN2GSR_TBS_BIT C2GSR_TBS_BIT
#define C2GSR_TCS_MASK 0x8
#define CAN2GSR_TCS_MASK C2GSR_TCS_MASK
#define C2GSR_TCS 0x8
#define CAN2GSR_TCS C2GSR_TCS
#define C2GSR_TCS_BIT 3
#define CAN2GSR_TCS_BIT C2GSR_TCS_BIT
#define C2GSR_RS_MASK 0x10
#define CAN2GSR_RS_MASK C2GSR_RS_MASK
#define C2GSR_RS 0x10
#define CAN2GSR_RS C2GSR_RS
#define C2GSR_RS_BIT 4
#define CAN2GSR_RS_BIT C2GSR_RS_BIT
#define C2GSR_TS_MASK 0x20
#define CAN2GSR_TS_MASK C2GSR_TS_MASK
#define C2GSR_TS 0x20
#define CAN2GSR_TS C2GSR_TS
#define C2GSR_TS_BIT 5
#define CAN2GSR_TS_BIT C2GSR_TS_BIT
#define C2GSR_ES_MASK 0x40
#define CAN2GSR_ES_MASK C2GSR_ES_MASK
#define C2GSR_ES 0x40
#define CAN2GSR_ES C2GSR_ES
#define C2GSR_ES_BIT 6
#define CAN2GSR_ES_BIT C2GSR_ES_BIT
#define C2GSR_BS_MASK 0x80
#define CAN2GSR_BS_MASK C2GSR_BS_MASK
#define C2GSR_BS 0x80
#define CAN2GSR_BS C2GSR_BS
#define C2GSR_BS_BIT 7
#define CAN2GSR_BS_BIT C2GSR_BS_BIT
#define C2GSR_RXERR_MASK 0xFF0000
#define CAN2GSR_RXERR_MASK C2GSR_RXERR_MASK
#define C2GSR_RXERR_BIT 16
#define CAN2GSR_RXERR_BIT C2GSR_RXERR_BIT
#define C2GSR_TXERR_MASK 0xFF000000
#define CAN2GSR_TXERR_MASK C2GSR_TXERR_MASK
#define C2GSR_TXERR_BIT 24
#define CAN2GSR_TXERR_BIT C2GSR_TXERR_BIT

#define C2ICR (*(volatile unsigned long *)0xE004800C)
#define CAN2ICR C2ICR
#define C2ICR_OFFSET 0xC
#define CAN2ICR_OFFSET C2ICR_OFFSET
#define C2ICR_RI_MASK 0x1
#define CAN2ICR_RI_MASK C2ICR_RI_MASK
#define C2ICR_RI 0x1
#define CAN2ICR_RI C2ICR_RI
#define C2ICR_RI_BIT 0
#define CAN2ICR_RI_BIT C2ICR_RI_BIT
#define C2ICR_TI1_MASK 0x2
#define CAN2ICR_TI1_MASK C2ICR_TI1_MASK
#define C2ICR_TI1 0x2
#define CAN2ICR_TI1 C2ICR_TI1
#define C2ICR_TI1_BIT 1
#define CAN2ICR_TI1_BIT C2ICR_TI1_BIT
#define C2ICR_EI_MASK 0x4
#define CAN2ICR_EI_MASK C2ICR_EI_MASK
#define C2ICR_EI 0x4
#define CAN2ICR_EI C2ICR_EI
#define C2ICR_EI_BIT 2
#define CAN2ICR_EI_BIT C2ICR_EI_BIT
#define C2ICR_DOI_MASK 0x8
#define CAN2ICR_DOI_MASK C2ICR_DOI_MASK
#define C2ICR_DOI 0x8
#define CAN2ICR_DOI C2ICR_DOI
#define C2ICR_DOI_BIT 3
#define CAN2ICR_DOI_BIT C2ICR_DOI_BIT
#define C2ICR_WUI_MASK 0x10
#define CAN2ICR_WUI_MASK C2ICR_WUI_MASK
#define C2ICR_WUI 0x10
#define CAN2ICR_WUI C2ICR_WUI
#define C2ICR_WUI_BIT 4
#define CAN2ICR_WUI_BIT C2ICR_WUI_BIT
#define C2ICR_EPI_MASK 0x20
#define CAN2ICR_EPI_MASK C2ICR_EPI_MASK
#define C2ICR_EPI 0x20
#define CAN2ICR_EPI C2ICR_EPI
#define C2ICR_EPI_BIT 5
#define CAN2ICR_EPI_BIT C2ICR_EPI_BIT
#define C2ICR_ALI_MASK 0x40
#define CAN2ICR_ALI_MASK C2ICR_ALI_MASK
#define C2ICR_ALI 0x40
#define CAN2ICR_ALI C2ICR_ALI
#define C2ICR_ALI_BIT 6
#define CAN2ICR_ALI_BIT C2ICR_ALI_BIT
#define C2ICR_BEI_MASK 0x80
#define CAN2ICR_BEI_MASK C2ICR_BEI_MASK
#define C2ICR_BEI 0x80
#define CAN2ICR_BEI C2ICR_BEI
#define C2ICR_BEI_BIT 7
#define CAN2ICR_BEI_BIT C2ICR_BEI_BIT
#define C2ICR_IDI_MASK 0x100
#define CAN2ICR_IDI_MASK C2ICR_IDI_MASK
#define C2ICR_IDI 0x100
#define CAN2ICR_IDI C2ICR_IDI
#define C2ICR_IDI_BIT 8
#define CAN2ICR_IDI_BIT C2ICR_IDI_BIT
#define C2ICR_TI2_MASK 0x200
#define CAN2ICR_TI2_MASK C2ICR_TI2_MASK
#define C2ICR_TI2 0x200
#define CAN2ICR_TI2 C2ICR_TI2
#define C2ICR_TI2_BIT 9
#define CAN2ICR_TI2_BIT C2ICR_TI2_BIT
#define C2ICR_TI3_MASK 0x400
#define CAN2ICR_TI3_MASK C2ICR_TI3_MASK
#define C2ICR_TI3 0x400
#define CAN2ICR_TI3 C2ICR_TI3
#define C2ICR_TI3_BIT 10
#define CAN2ICR_TI3_BIT C2ICR_TI3_BIT
#define C2ICR_ERRBIT_MASK 0x1F0000
#define CAN2ICR_ERRBIT_MASK C2ICR_ERRBIT_MASK
#define C2ICR_ERRBIT_BIT 16
#define CAN2ICR_ERRBIT_BIT C2ICR_ERRBIT_BIT
#define C2ICR_ERRDIR_MASK 0x200000
#define CAN2ICR_ERRDIR_MASK C2ICR_ERRDIR_MASK
#define C2ICR_ERRDIR 0x200000
#define CAN2ICR_ERRDIR C2ICR_ERRDIR
#define C2ICR_ERRDIR_BIT 21
#define CAN2ICR_ERRDIR_BIT C2ICR_ERRDIR_BIT
#define C2ICR_ERRC_MASK 0xC00000
#define CAN2ICR_ERRC_MASK C2ICR_ERRC_MASK
#define C2ICR_ERRC_BIT 22
#define CAN2ICR_ERRC_BIT C2ICR_ERRC_BIT
#define C2ICR_ALCBIT_MASK 0x1F000000
#define CAN2ICR_ALCBIT_MASK C2ICR_ALCBIT_MASK
#define C2ICR_ALCBIT_BIT 24
#define CAN2ICR_ALCBIT_BIT C2ICR_ALCBIT_BIT

#define C2IER (*(volatile unsigned long *)0xE0048010)
#define CAN2IER C2IER
#define C2IER_OFFSET 0x10
#define CAN2IER_OFFSET C2IER_OFFSET
#define C2IER_RIE_MASK 0x1
#define CAN2IER_RIE_MASK C2IER_RIE_MASK
#define C2IER_RIE 0x1
#define CAN2IER_RIE C2IER_RIE
#define C2IER_RIE_BIT 0
#define CAN2IER_RIE_BIT C2IER_RIE_BIT
#define C2IER_TIE1_MASK 0x2
#define CAN2IER_TIE1_MASK C2IER_TIE1_MASK
#define C2IER_TIE1 0x2
#define CAN2IER_TIE1 C2IER_TIE1
#define C2IER_TIE1_BIT 1
#define CAN2IER_TIE1_BIT C2IER_TIE1_BIT
#define C2IER_EIE_MASK 0x4
#define CAN2IER_EIE_MASK C2IER_EIE_MASK
#define C2IER_EIE 0x4
#define CAN2IER_EIE C2IER_EIE
#define C2IER_EIE_BIT 2
#define CAN2IER_EIE_BIT C2IER_EIE_BIT
#define C2IER_DOIE_MASK 0x8
#define CAN2IER_DOIE_MASK C2IER_DOIE_MASK
#define C2IER_DOIE 0x8
#define CAN2IER_DOIE C2IER_DOIE
#define C2IER_DOIE_BIT 3
#define CAN2IER_DOIE_BIT C2IER_DOIE_BIT
#define C2IER_WUIE_MASK 0x10
#define CAN2IER_WUIE_MASK C2IER_WUIE_MASK
#define C2IER_WUIE 0x10
#define CAN2IER_WUIE C2IER_WUIE
#define C2IER_WUIE_BIT 4
#define CAN2IER_WUIE_BIT C2IER_WUIE_BIT
#define C2IER_EPIE_MASK 0x20
#define CAN2IER_EPIE_MASK C2IER_EPIE_MASK
#define C2IER_EPIE 0x20
#define CAN2IER_EPIE C2IER_EPIE
#define C2IER_EPIE_BIT 5
#define CAN2IER_EPIE_BIT C2IER_EPIE_BIT
#define C2IER_ALIE_MASK 0x40
#define CAN2IER_ALIE_MASK C2IER_ALIE_MASK
#define C2IER_ALIE 0x40
#define CAN2IER_ALIE C2IER_ALIE
#define C2IER_ALIE_BIT 6
#define CAN2IER_ALIE_BIT C2IER_ALIE_BIT
#define C2IER_BEIE_MASK 0x80
#define CAN2IER_BEIE_MASK C2IER_BEIE_MASK
#define C2IER_BEIE 0x80
#define CAN2IER_BEIE C2IER_BEIE
#define C2IER_BEIE_BIT 7
#define CAN2IER_BEIE_BIT C2IER_BEIE_BIT
#define C2IER_IDIE_MASK 0x100
#define CAN2IER_IDIE_MASK C2IER_IDIE_MASK
#define C2IER_IDIE 0x100
#define CAN2IER_IDIE C2IER_IDIE
#define C2IER_IDIE_BIT 8
#define CAN2IER_IDIE_BIT C2IER_IDIE_BIT
#define C2IER_TIE2_MASK 0x200
#define CAN2IER_TIE2_MASK C2IER_TIE2_MASK
#define C2IER_TIE2 0x200
#define CAN2IER_TIE2 C2IER_TIE2
#define C2IER_TIE2_BIT 9
#define CAN2IER_TIE2_BIT C2IER_TIE2_BIT
#define C2IER_TIE3_MASK 0x400
#define CAN2IER_TIE3_MASK C2IER_TIE3_MASK
#define C2IER_TIE3 0x400
#define CAN2IER_TIE3 C2IER_TIE3
#define C2IER_TIE3_BIT 10
#define CAN2IER_TIE3_BIT C2IER_TIE3_BIT

#define C2BTR (*(volatile unsigned long *)0xE0048014)
#define CAN2BTR C2BTR
#define C2BTR_OFFSET 0x14
#define CAN2BTR_OFFSET C2BTR_OFFSET
#define C2BTR_BRP_MASK 0x3FF
#define CAN2BTR_BRP_MASK C2BTR_BRP_MASK
#define C2BTR_BRP_BIT 0
#define CAN2BTR_BRP_BIT C2BTR_BRP_BIT
#define C2BTR_SJW_MASK 0xC000
#define CAN2BTR_SJW_MASK C2BTR_SJW_MASK
#define C2BTR_SJW_BIT 14
#define CAN2BTR_SJW_BIT C2BTR_SJW_BIT
#define C2BTR_TSEG1_MASK 0xF0000
#define CAN2BTR_TSEG1_MASK C2BTR_TSEG1_MASK
#define C2BTR_TSEG1_BIT 16
#define CAN2BTR_TSEG1_BIT C2BTR_TSEG1_BIT
#define C2BTR_TSEG2_MASK 0x700000
#define CAN2BTR_TSEG2_MASK C2BTR_TSEG2_MASK
#define C2BTR_TSEG2_BIT 20
#define CAN2BTR_TSEG2_BIT C2BTR_TSEG2_BIT
#define C2BTR_SAM_MASK 0x800000
#define CAN2BTR_SAM_MASK C2BTR_SAM_MASK
#define C2BTR_SAM 0x800000
#define CAN2BTR_SAM C2BTR_SAM
#define C2BTR_SAM_BIT 23
#define CAN2BTR_SAM_BIT C2BTR_SAM_BIT

#define C2EWL (*(volatile unsigned long *)0xE0048018)
#define CAN2EWL C2EWL
#define C2EWL_OFFSET 0x18
#define CAN2EWL_OFFSET C2EWL_OFFSET
#define C2EWL_EWL_MASK 0xFF
#define CAN2EWL_EWL_MASK C2EWL_EWL_MASK
#define C2EWL_EWL_BIT 0
#define CAN2EWL_EWL_BIT C2EWL_EWL_BIT

#define C2SR (*(volatile unsigned long *)0xE004801C)
#define CAN2SR C2SR
#define C2SR_OFFSET 0x1C
#define CAN2SR_OFFSET C2SR_OFFSET
#define C2SR_RBS_MASK 0x1
#define CAN2SR_RBS_MASK C2SR_RBS_MASK
#define C2SR_RBS 0x1
#define CAN2SR_RBS C2SR_RBS
#define C2SR_RBS_BIT 0
#define CAN2SR_RBS_BIT C2SR_RBS_BIT
#define C2SR_DOS_MASK 0x2
#define CAN2SR_DOS_MASK C2SR_DOS_MASK
#define C2SR_DOS 0x2
#define CAN2SR_DOS C2SR_DOS
#define C2SR_DOS_BIT 1
#define CAN2SR_DOS_BIT C2SR_DOS_BIT
#define C2SR_TBS1_MASK 0x4
#define CAN2SR_TBS1_MASK C2SR_TBS1_MASK
#define C2SR_TBS1 0x4
#define CAN2SR_TBS1 C2SR_TBS1
#define C2SR_TBS1_BIT 2
#define CAN2SR_TBS1_BIT C2SR_TBS1_BIT
#define C2SR_TCS1_MASK 0x8
#define CAN2SR_TCS1_MASK C2SR_TCS1_MASK
#define C2SR_TCS1 0x8
#define CAN2SR_TCS1 C2SR_TCS1
#define C2SR_TCS1_BIT 3
#define CAN2SR_TCS1_BIT C2SR_TCS1_BIT
#define C2SR_RS_MASK 0x10
#define CAN2SR_RS_MASK C2SR_RS_MASK
#define C2SR_RS 0x10
#define CAN2SR_RS C2SR_RS
#define C2SR_RS_BIT 4
#define CAN2SR_RS_BIT C2SR_RS_BIT
#define C2SR_TS1_MASK 0x20
#define CAN2SR_TS1_MASK C2SR_TS1_MASK
#define C2SR_TS1 0x20
#define CAN2SR_TS1 C2SR_TS1
#define C2SR_TS1_BIT 5
#define CAN2SR_TS1_BIT C2SR_TS1_BIT
#define C2SR_ES_MASK 0x40
#define CAN2SR_ES_MASK C2SR_ES_MASK
#define C2SR_ES 0x40
#define CAN2SR_ES C2SR_ES
#define C2SR_ES_BIT 6
#define CAN2SR_ES_BIT C2SR_ES_BIT
#define C2SR_BS_MASK 0x80
#define CAN2SR_BS_MASK C2SR_BS_MASK
#define C2SR_BS 0x80
#define CAN2SR_BS C2SR_BS
#define C2SR_BS_BIT 7
#define CAN2SR_BS_BIT C2SR_BS_BIT
#define C2SR_RBS2_MASK 0x100
#define CAN2SR_RBS2_MASK C2SR_RBS2_MASK
#define C2SR_RBS2 0x100
#define CAN2SR_RBS2 C2SR_RBS2
#define C2SR_RBS2_BIT 8
#define CAN2SR_RBS2_BIT C2SR_RBS2_BIT
#define C2SR_DOS2_MASK 0x200
#define CAN2SR_DOS2_MASK C2SR_DOS2_MASK
#define C2SR_DOS2 0x200
#define CAN2SR_DOS2 C2SR_DOS2
#define C2SR_DOS2_BIT 9
#define CAN2SR_DOS2_BIT C2SR_DOS2_BIT
#define C2SR_TBS2_MASK 0x400
#define CAN2SR_TBS2_MASK C2SR_TBS2_MASK
#define C2SR_TBS2 0x400
#define CAN2SR_TBS2 C2SR_TBS2
#define C2SR_TBS2_BIT 10
#define CAN2SR_TBS2_BIT C2SR_TBS2_BIT
#define C2SR_TCS2_MASK 0x800
#define CAN2SR_TCS2_MASK C2SR_TCS2_MASK
#define C2SR_TCS2 0x800
#define CAN2SR_TCS2 C2SR_TCS2
#define C2SR_TCS2_BIT 11
#define CAN2SR_TCS2_BIT C2SR_TCS2_BIT
#define C2SR_RS2_MASK 0x1000
#define CAN2SR_RS2_MASK C2SR_RS2_MASK
#define C2SR_RS2 0x1000
#define CAN2SR_RS2 C2SR_RS2
#define C2SR_RS2_BIT 12
#define CAN2SR_RS2_BIT C2SR_RS2_BIT
#define C2SR_TS2_MASK 0x2000
#define CAN2SR_TS2_MASK C2SR_TS2_MASK
#define C2SR_TS2 0x2000
#define CAN2SR_TS2 C2SR_TS2
#define C2SR_TS2_BIT 13
#define CAN2SR_TS2_BIT C2SR_TS2_BIT
#define C2SR_ES2_MASK 0x4000
#define CAN2SR_ES2_MASK C2SR_ES2_MASK
#define C2SR_ES2 0x4000
#define CAN2SR_ES2 C2SR_ES2
#define C2SR_ES2_BIT 14
#define CAN2SR_ES2_BIT C2SR_ES2_BIT
#define C2SR_BS2_MASK 0x8000
#define CAN2SR_BS2_MASK C2SR_BS2_MASK
#define C2SR_BS2 0x8000
#define CAN2SR_BS2 C2SR_BS2
#define C2SR_BS2_BIT 15
#define CAN2SR_BS2_BIT C2SR_BS2_BIT
#define C2SR_RBS3_MASK 0x10000
#define CAN2SR_RBS3_MASK C2SR_RBS3_MASK
#define C2SR_RBS3 0x10000
#define CAN2SR_RBS3 C2SR_RBS3
#define C2SR_RBS3_BIT 16
#define CAN2SR_RBS3_BIT C2SR_RBS3_BIT
#define C2SR_DOS3_MASK 0x20000
#define CAN2SR_DOS3_MASK C2SR_DOS3_MASK
#define C2SR_DOS3 0x20000
#define CAN2SR_DOS3 C2SR_DOS3
#define C2SR_DOS3_BIT 17
#define CAN2SR_DOS3_BIT C2SR_DOS3_BIT
#define C2SR_TBS3_MASK 0x40000
#define CAN2SR_TBS3_MASK C2SR_TBS3_MASK
#define C2SR_TBS3 0x40000
#define CAN2SR_TBS3 C2SR_TBS3
#define C2SR_TBS3_BIT 18
#define CAN2SR_TBS3_BIT C2SR_TBS3_BIT
#define C2SR_TCS3_MASK 0x80000
#define CAN2SR_TCS3_MASK C2SR_TCS3_MASK
#define C2SR_TCS3 0x80000
#define CAN2SR_TCS3 C2SR_TCS3
#define C2SR_TCS3_BIT 19
#define CAN2SR_TCS3_BIT C2SR_TCS3_BIT
#define C2SR_RS3_MASK 0x100000
#define CAN2SR_RS3_MASK C2SR_RS3_MASK
#define C2SR_RS3 0x100000
#define CAN2SR_RS3 C2SR_RS3
#define C2SR_RS3_BIT 20
#define CAN2SR_RS3_BIT C2SR_RS3_BIT
#define C2SR_TS3_MASK 0x200000
#define CAN2SR_TS3_MASK C2SR_TS3_MASK
#define C2SR_TS3 0x200000
#define CAN2SR_TS3 C2SR_TS3
#define C2SR_TS3_BIT 21
#define CAN2SR_TS3_BIT C2SR_TS3_BIT
#define C2SR_ES3_MASK 0x400000
#define CAN2SR_ES3_MASK C2SR_ES3_MASK
#define C2SR_ES3 0x400000
#define CAN2SR_ES3 C2SR_ES3
#define C2SR_ES3_BIT 22
#define CAN2SR_ES3_BIT C2SR_ES3_BIT
#define C2SR_BS3_MASK 0x800000
#define CAN2SR_BS3_MASK C2SR_BS3_MASK
#define C2SR_BS3 0x800000
#define CAN2SR_BS3 C2SR_BS3
#define C2SR_BS3_BIT 23
#define CAN2SR_BS3_BIT C2SR_BS3_BIT

#define C2RFS (*(volatile unsigned long *)0xE0048020)
#define CAN2RFS C2RFS
#define C2RFS_OFFSET 0x20
#define CAN2RFS_OFFSET C2RFS_OFFSET
#define C2RFS_ID_Index_MASK 0x3FF
#define CAN2RFS_ID_Index_MASK C2RFS_ID_Index_MASK
#define C2RFS_ID_Index_BIT 0
#define CAN2RFS_ID_Index_BIT C2RFS_ID_Index_BIT
#define C2RFS_BP_MASK 0x400
#define CAN2RFS_BP_MASK C2RFS_BP_MASK
#define C2RFS_BP 0x400
#define CAN2RFS_BP C2RFS_BP
#define C2RFS_BP_BIT 10
#define CAN2RFS_BP_BIT C2RFS_BP_BIT
#define C2RFS_DLC_MASK 0xF0000
#define CAN2RFS_DLC_MASK C2RFS_DLC_MASK
#define C2RFS_DLC_BIT 16
#define CAN2RFS_DLC_BIT C2RFS_DLC_BIT
#define C2RFS_RTR_MASK 0x40000000
#define CAN2RFS_RTR_MASK C2RFS_RTR_MASK
#define C2RFS_RTR 0x40000000
#define CAN2RFS_RTR C2RFS_RTR
#define C2RFS_RTR_BIT 30
#define CAN2RFS_RTR_BIT C2RFS_RTR_BIT
#define C2RFS_FF_MASK 0x80000000
#define CAN2RFS_FF_MASK C2RFS_FF_MASK
#define C2RFS_FF 0x80000000
#define CAN2RFS_FF C2RFS_FF
#define C2RFS_FF_BIT 31
#define CAN2RFS_FF_BIT C2RFS_FF_BIT

#define C2RID (*(volatile unsigned long *)0xE0048024)
#define CAN2RID C2RID
#define C2RID_OFFSET 0x24
#define CAN2RID_OFFSET C2RID_OFFSET
#define C2RID_ID_MASK 0x7FF
#define CAN2RID_ID_MASK C2RID_ID_MASK
#define C2RID_ID_BIT 0
#define CAN2RID_ID_BIT C2RID_ID_BIT

#define C2RDA (*(volatile unsigned long *)0xE0048028)
#define CAN2RDA C2RDA
#define C2RDA_OFFSET 0x28
#define CAN2RDA_OFFSET C2RDA_OFFSET
#define C2RDA_Data_1_MASK 0xFF
#define CAN2RDA_Data_1_MASK C2RDA_Data_1_MASK
#define C2RDA_Data_1_BIT 0
#define CAN2RDA_Data_1_BIT C2RDA_Data_1_BIT
#define C2RDA_Data_2_MASK 0xFF00
#define CAN2RDA_Data_2_MASK C2RDA_Data_2_MASK
#define C2RDA_Data_2_BIT 8
#define CAN2RDA_Data_2_BIT C2RDA_Data_2_BIT
#define C2RDA_Data_3_MASK 0xFF0000
#define CAN2RDA_Data_3_MASK C2RDA_Data_3_MASK
#define C2RDA_Data_3_BIT 16
#define CAN2RDA_Data_3_BIT C2RDA_Data_3_BIT
#define C2RDA_Data_4_MASK 0xFF000000
#define CAN2RDA_Data_4_MASK C2RDA_Data_4_MASK
#define C2RDA_Data_4_BIT 24
#define CAN2RDA_Data_4_BIT C2RDA_Data_4_BIT

#define C2RDB (*(volatile unsigned long *)0xE004802C)
#define CAN2RDB C2RDB
#define C2RDB_OFFSET 0x2C
#define CAN2RDB_OFFSET C2RDB_OFFSET
#define C2RDB_Data_5_MASK 0xFF
#define CAN2RDB_Data_5_MASK C2RDB_Data_5_MASK
#define C2RDB_Data_5_BIT 0
#define CAN2RDB_Data_5_BIT C2RDB_Data_5_BIT
#define C2RDB_Data_6_MASK 0xFF00
#define CAN2RDB_Data_6_MASK C2RDB_Data_6_MASK
#define C2RDB_Data_6_BIT 8
#define CAN2RDB_Data_6_BIT C2RDB_Data_6_BIT
#define C2RDB_Data_7_MASK 0xFF0000
#define CAN2RDB_Data_7_MASK C2RDB_Data_7_MASK
#define C2RDB_Data_7_BIT 16
#define CAN2RDB_Data_7_BIT C2RDB_Data_7_BIT
#define C2RDB_Data_8_MASK 0xFF000000
#define CAN2RDB_Data_8_MASK C2RDB_Data_8_MASK
#define C2RDB_Data_8_BIT 24
#define CAN2RDB_Data_8_BIT C2RDB_Data_8_BIT

#define C2TFI1 (*(volatile unsigned long *)0xE0048030)
#define CAN2TFI1 C2TFI1
#define C2TFI1_OFFSET 0x30
#define CAN2TFI1_OFFSET C2TFI1_OFFSET
#define C2TFI1_PRIO_MASK 0xFF
#define CAN2TFI1_PRIO_MASK C2TFI1_PRIO_MASK
#define C2TFI1_PRIO_BIT 0
#define CAN2TFI1_PRIO_BIT C2TFI1_PRIO_BIT
#define C2TFI1_DLC_MASK 0xF0000
#define CAN2TFI1_DLC_MASK C2TFI1_DLC_MASK
#define C2TFI1_DLC_BIT 16
#define CAN2TFI1_DLC_BIT C2TFI1_DLC_BIT
#define C2TFI1_RTR_MASK 0x40000000
#define CAN2TFI1_RTR_MASK C2TFI1_RTR_MASK
#define C2TFI1_RTR 0x40000000
#define CAN2TFI1_RTR C2TFI1_RTR
#define C2TFI1_RTR_BIT 30
#define CAN2TFI1_RTR_BIT C2TFI1_RTR_BIT
#define C2TFI1_FF_MASK 0x80000000
#define CAN2TFI1_FF_MASK C2TFI1_FF_MASK
#define C2TFI1_FF 0x80000000
#define CAN2TFI1_FF C2TFI1_FF
#define C2TFI1_FF_BIT 31
#define CAN2TFI1_FF_BIT C2TFI1_FF_BIT

#define C2TID1 (*(volatile unsigned long *)0xE0048034)
#define CAN2TID1 C2TID1
#define C2TID1_OFFSET 0x34
#define CAN2TID1_OFFSET C2TID1_OFFSET
#define C2TID1_ID_MASK 0x7FF
#define CAN2TID1_ID_MASK C2TID1_ID_MASK
#define C2TID1_ID_BIT 0
#define CAN2TID1_ID_BIT C2TID1_ID_BIT

#define C2TDA1 (*(volatile unsigned long *)0xE0048038)
#define CAN2TDA1 C2TDA1
#define C2TDA1_OFFSET 0x38
#define CAN2TDA1_OFFSET C2TDA1_OFFSET
#define C2TDA1_Data_1_MASK 0xFF
#define CAN2TDA1_Data_1_MASK C2TDA1_Data_1_MASK
#define C2TDA1_Data_1_BIT 0
#define CAN2TDA1_Data_1_BIT C2TDA1_Data_1_BIT
#define C2TDA1_Data_2_MASK 0xFF00
#define CAN2TDA1_Data_2_MASK C2TDA1_Data_2_MASK
#define C2TDA1_Data_2_BIT 8
#define CAN2TDA1_Data_2_BIT C2TDA1_Data_2_BIT
#define C2TDA1_Data_3_MASK 0xFF0000
#define CAN2TDA1_Data_3_MASK C2TDA1_Data_3_MASK
#define C2TDA1_Data_3_BIT 16
#define CAN2TDA1_Data_3_BIT C2TDA1_Data_3_BIT
#define C2TDA1_Data_4_MASK 0xFF000000
#define CAN2TDA1_Data_4_MASK C2TDA1_Data_4_MASK
#define C2TDA1_Data_4_BIT 24
#define CAN2TDA1_Data_4_BIT C2TDA1_Data_4_BIT

#define C2TDB1 (*(volatile unsigned long *)0xE004803C)
#define CAN2TDB1 C2TDB1
#define C2TDB1_OFFSET 0x3C
#define CAN2TDB1_OFFSET C2TDB1_OFFSET
#define C2TDB1_Data_5_MASK 0xFF
#define CAN2TDB1_Data_5_MASK C2TDB1_Data_5_MASK
#define C2TDB1_Data_5_BIT 0
#define CAN2TDB1_Data_5_BIT C2TDB1_Data_5_BIT
#define C2TDB1_Data_6_MASK 0xFF00
#define CAN2TDB1_Data_6_MASK C2TDB1_Data_6_MASK
#define C2TDB1_Data_6_BIT 8
#define CAN2TDB1_Data_6_BIT C2TDB1_Data_6_BIT
#define C2TDB1_Data_7_MASK 0xFF0000
#define CAN2TDB1_Data_7_MASK C2TDB1_Data_7_MASK
#define C2TDB1_Data_7_BIT 16
#define CAN2TDB1_Data_7_BIT C2TDB1_Data_7_BIT
#define C2TDB1_Data_8_MASK 0xFF000000
#define CAN2TDB1_Data_8_MASK C2TDB1_Data_8_MASK
#define C2TDB1_Data_8_BIT 24
#define CAN2TDB1_Data_8_BIT C2TDB1_Data_8_BIT

#define C2TFI2 (*(volatile unsigned long *)0xE0048040)
#define CAN2TFI2 C2TFI2
#define C2TFI2_OFFSET 0x40
#define CAN2TFI2_OFFSET C2TFI2_OFFSET
#define C2TFI2_PRIO_MASK 0xFF
#define CAN2TFI2_PRIO_MASK C2TFI2_PRIO_MASK
#define C2TFI2_PRIO_BIT 0
#define CAN2TFI2_PRIO_BIT C2TFI2_PRIO_BIT
#define C2TFI2_DLC_MASK 0xF0000
#define CAN2TFI2_DLC_MASK C2TFI2_DLC_MASK
#define C2TFI2_DLC_BIT 16
#define CAN2TFI2_DLC_BIT C2TFI2_DLC_BIT
#define C2TFI2_RTR_MASK 0x40000000
#define CAN2TFI2_RTR_MASK C2TFI2_RTR_MASK
#define C2TFI2_RTR 0x40000000
#define CAN2TFI2_RTR C2TFI2_RTR
#define C2TFI2_RTR_BIT 30
#define CAN2TFI2_RTR_BIT C2TFI2_RTR_BIT
#define C2TFI2_FF_MASK 0x80000000
#define CAN2TFI2_FF_MASK C2TFI2_FF_MASK
#define C2TFI2_FF 0x80000000
#define CAN2TFI2_FF C2TFI2_FF
#define C2TFI2_FF_BIT 31
#define CAN2TFI2_FF_BIT C2TFI2_FF_BIT

#define C2TID2 (*(volatile unsigned long *)0xE0048044)
#define CAN2TID2 C2TID2
#define C2TID2_OFFSET 0x44
#define CAN2TID2_OFFSET C2TID2_OFFSET
#define C2TID2_ID_MASK 0x7FF
#define CAN2TID2_ID_MASK C2TID2_ID_MASK
#define C2TID2_ID_BIT 0
#define CAN2TID2_ID_BIT C2TID2_ID_BIT

#define C2TDA2 (*(volatile unsigned long *)0xE0048048)
#define CAN2TDA2 C2TDA2
#define C2TDA2_OFFSET 0x48
#define CAN2TDA2_OFFSET C2TDA2_OFFSET
#define C2TDA2_Data_1_MASK 0xFF
#define CAN2TDA2_Data_1_MASK C2TDA2_Data_1_MASK
#define C2TDA2_Data_1_BIT 0
#define CAN2TDA2_Data_1_BIT C2TDA2_Data_1_BIT
#define C2TDA2_Data_2_MASK 0xFF00
#define CAN2TDA2_Data_2_MASK C2TDA2_Data_2_MASK
#define C2TDA2_Data_2_BIT 8
#define CAN2TDA2_Data_2_BIT C2TDA2_Data_2_BIT
#define C2TDA2_Data_3_MASK 0xFF0000
#define CAN2TDA2_Data_3_MASK C2TDA2_Data_3_MASK
#define C2TDA2_Data_3_BIT 16
#define CAN2TDA2_Data_3_BIT C2TDA2_Data_3_BIT
#define C2TDA2_Data_4_MASK 0xFF000000
#define CAN2TDA2_Data_4_MASK C2TDA2_Data_4_MASK
#define C2TDA2_Data_4_BIT 24
#define CAN2TDA2_Data_4_BIT C2TDA2_Data_4_BIT

#define C2TDB2 (*(volatile unsigned long *)0xE004804C)
#define CAN2TDB2 C2TDB2
#define C2TDB2_OFFSET 0x4C
#define CAN2TDB2_OFFSET C2TDB2_OFFSET
#define C2TDB2_Data_5_MASK 0xFF
#define CAN2TDB2_Data_5_MASK C2TDB2_Data_5_MASK
#define C2TDB2_Data_5_BIT 0
#define CAN2TDB2_Data_5_BIT C2TDB2_Data_5_BIT
#define C2TDB2_Data_6_MASK 0xFF00
#define CAN2TDB2_Data_6_MASK C2TDB2_Data_6_MASK
#define C2TDB2_Data_6_BIT 8
#define CAN2TDB2_Data_6_BIT C2TDB2_Data_6_BIT
#define C2TDB2_Data_7_MASK 0xFF0000
#define CAN2TDB2_Data_7_MASK C2TDB2_Data_7_MASK
#define C2TDB2_Data_7_BIT 16
#define CAN2TDB2_Data_7_BIT C2TDB2_Data_7_BIT
#define C2TDB2_Data_8_MASK 0xFF000000
#define CAN2TDB2_Data_8_MASK C2TDB2_Data_8_MASK
#define C2TDB2_Data_8_BIT 24
#define CAN2TDB2_Data_8_BIT C2TDB2_Data_8_BIT

#define C2TFI3 (*(volatile unsigned long *)0xE0048050)
#define CAN2TFI3 C2TFI3
#define C2TFI3_OFFSET 0x50
#define CAN2TFI3_OFFSET C2TFI3_OFFSET
#define C2TFI3_PRIO_MASK 0xFF
#define CAN2TFI3_PRIO_MASK C2TFI3_PRIO_MASK
#define C2TFI3_PRIO_BIT 0
#define CAN2TFI3_PRIO_BIT C2TFI3_PRIO_BIT
#define C2TFI3_DLC_MASK 0xF0000
#define CAN2TFI3_DLC_MASK C2TFI3_DLC_MASK
#define C2TFI3_DLC_BIT 16
#define CAN2TFI3_DLC_BIT C2TFI3_DLC_BIT
#define C2TFI3_RTR_MASK 0x40000000
#define CAN2TFI3_RTR_MASK C2TFI3_RTR_MASK
#define C2TFI3_RTR 0x40000000
#define CAN2TFI3_RTR C2TFI3_RTR
#define C2TFI3_RTR_BIT 30
#define CAN2TFI3_RTR_BIT C2TFI3_RTR_BIT
#define C2TFI3_FF_MASK 0x80000000
#define CAN2TFI3_FF_MASK C2TFI3_FF_MASK
#define C2TFI3_FF 0x80000000
#define CAN2TFI3_FF C2TFI3_FF
#define C2TFI3_FF_BIT 31
#define CAN2TFI3_FF_BIT C2TFI3_FF_BIT

#define C2TID3 (*(volatile unsigned long *)0xE0048054)
#define CAN2TID3 C2TID3
#define C2TID3_OFFSET 0x54
#define CAN2TID3_OFFSET C2TID3_OFFSET
#define C2TID3_ID_MASK 0x7FF
#define CAN2TID3_ID_MASK C2TID3_ID_MASK
#define C2TID3_ID_BIT 0
#define CAN2TID3_ID_BIT C2TID3_ID_BIT

#define C2TDA3 (*(volatile unsigned long *)0xE0048058)
#define CAN2TDA3 C2TDA3
#define C2TDA3_OFFSET 0x58
#define CAN2TDA3_OFFSET C2TDA3_OFFSET
#define C2TDA3_Data_1_MASK 0xFF
#define CAN2TDA3_Data_1_MASK C2TDA3_Data_1_MASK
#define C2TDA3_Data_1_BIT 0
#define CAN2TDA3_Data_1_BIT C2TDA3_Data_1_BIT
#define C2TDA3_Data_2_MASK 0xFF00
#define CAN2TDA3_Data_2_MASK C2TDA3_Data_2_MASK
#define C2TDA3_Data_2_BIT 8
#define CAN2TDA3_Data_2_BIT C2TDA3_Data_2_BIT
#define C2TDA3_Data_3_MASK 0xFF0000
#define CAN2TDA3_Data_3_MASK C2TDA3_Data_3_MASK
#define C2TDA3_Data_3_BIT 16
#define CAN2TDA3_Data_3_BIT C2TDA3_Data_3_BIT
#define C2TDA3_Data_4_MASK 0xFF000000
#define CAN2TDA3_Data_4_MASK C2TDA3_Data_4_MASK
#define C2TDA3_Data_4_BIT 24
#define CAN2TDA3_Data_4_BIT C2TDA3_Data_4_BIT

#define C2TDB3 (*(volatile unsigned long *)0xE004805C)
#define CAN2TDB3 C2TDB3
#define C2TDB3_OFFSET 0x5C
#define CAN2TDB3_OFFSET C2TDB3_OFFSET
#define C2TDB3_Data_5_MASK 0xFF
#define CAN2TDB3_Data_5_MASK C2TDB3_Data_5_MASK
#define C2TDB3_Data_5_BIT 0
#define CAN2TDB3_Data_5_BIT C2TDB3_Data_5_BIT
#define C2TDB3_Data_6_MASK 0xFF00
#define CAN2TDB3_Data_6_MASK C2TDB3_Data_6_MASK
#define C2TDB3_Data_6_BIT 8
#define CAN2TDB3_Data_6_BIT C2TDB3_Data_6_BIT
#define C2TDB3_Data_7_MASK 0xFF0000
#define CAN2TDB3_Data_7_MASK C2TDB3_Data_7_MASK
#define C2TDB3_Data_7_BIT 16
#define CAN2TDB3_Data_7_BIT C2TDB3_Data_7_BIT
#define C2TDB3_Data_8_MASK 0xFF000000
#define CAN2TDB3_Data_8_MASK C2TDB3_Data_8_MASK
#define C2TDB3_Data_8_BIT 24
#define CAN2TDB3_Data_8_BIT C2TDB3_Data_8_BIT

#define I2C1_BASE 0xE005C000

#define I2C1CONSET (*(volatile unsigned char *)0xE005C000)
#define I2C1CONSET_OFFSET 0x0
#define I2C1CONSET_AA_MASK 0x4
#define I2C1CONSET_AA 0x4
#define I2C1CONSET_AA_BIT 2
#define I2C1CONSET_SI_MASK 0x8
#define I2C1CONSET_SI 0x8
#define I2C1CONSET_SI_BIT 3
#define I2C1CONSET_STO_MASK 0x10
#define I2C1CONSET_STO 0x10
#define I2C1CONSET_STO_BIT 4
#define I2C1CONSET_STA_MASK 0x20
#define I2C1CONSET_STA 0x20
#define I2C1CONSET_STA_BIT 5
#define I2C1CONSET_I2EN_MASK 0x40
#define I2C1CONSET_I2EN 0x40
#define I2C1CONSET_I2EN_BIT 6

#define I2C1STAT (*(volatile unsigned char *)0xE005C004)
#define I2C1STAT_OFFSET 0x4
#define I2C1STAT_Status_MASK 0x7C
#define I2C1STAT_Status_BIT 2

#define I2C1DAT (*(volatile unsigned char *)0xE005C008)
#define I2C1DAT_OFFSET 0x8

#define I2C1ADR (*(volatile unsigned char *)0xE005C00C)
#define I2C1ADR_OFFSET 0xC
#define I2C1ADR_GC_MASK 0x1
#define I2C1ADR_GC 0x1
#define I2C1ADR_GC_BIT 0
#define I2C1ADR_Address_MASK 0x7E
#define I2C1ADR_Address_BIT 1

#define I2C1SCLH (*(volatile unsigned short *)0xE005C010)
#define I2C1SCLH_OFFSET 0x10

#define I2C1SCLL (*(volatile unsigned short *)0xE005C014)
#define I2C1SCLL_OFFSET 0x14

#define I2C1CONCLR (*(volatile unsigned char *)0xE005C018)
#define I2C1CONCLR_OFFSET 0x18
#define I2C1CONCLR_AAC_MASK 0x4
#define I2C1CONCLR_AAC 0x4
#define I2C1CONCLR_AAC_BIT 2
#define I2C1CONCLR_SIC_MASK 0x8
#define I2C1CONCLR_SIC 0x8
#define I2C1CONCLR_SIC_BIT 3
#define I2C1CONCLR_STAC_MASK 0x20
#define I2C1CONCLR_STAC 0x20
#define I2C1CONCLR_STAC_BIT 5
#define I2C1CONCLR_I2ENC_MASK 0x40
#define I2C1CONCLR_I2ENC 0x40
#define I2C1CONCLR_I2ENC_BIT 6

#define SSP0_BASE 0xE0068000

#define SSP0CR0 (*(volatile unsigned long *)0xE0068000)
#define SSP0CR0_OFFSET 0x0
#define SSP0CR0_SCR_MASK 0xFF00
#define SSP0CR0_SCR_BIT 8
#define SSP0CR0_CPHA_MASK 0x80
#define SSP0CR0_CPHA 0x80
#define SSP0CR0_CPHA_BIT 7
#define SSP0CR0_CPOL_MASK 0x40
#define SSP0CR0_CPOL 0x40
#define SSP0CR0_CPOL_BIT 6
#define SSP0CR0_FRF_MASK 0x30
#define SSP0CR0_FRF_BIT 4
#define SSP0CR0_DSS_MASK 0xF
#define SSP0CR0_DSS_BIT 0

#define SSP0CR1 (*(volatile unsigned long *)0xE0068004)
#define SSP0CR1_OFFSET 0x4
#define SSP0CR1_SOD_MASK 0x8
#define SSP0CR1_SOD 0x8
#define SSP0CR1_SOD_BIT 3
#define SSP0CR1_MS_MASK 0x4
#define SSP0CR1_MS 0x4
#define SSP0CR1_MS_BIT 2
#define SSP0CR1_SSE_MASK 0x2
#define SSP0CR1_SSE 0x2
#define SSP0CR1_SSE_BIT 1
#define SSP0CR1_LBE_MASK 0x1
#define SSP0CR1_LBE 0x1
#define SSP0CR1_LBE_BIT 0

#define SSP0DR (*(volatile unsigned long *)0xE0068008)
#define SSP0DR_OFFSET 0x8

#define SSP0SR (*(volatile unsigned long *)0xE006800C)
#define SSP0SR_OFFSET 0xC
#define SSP0SR_BSY_MASK 0x10
#define SSP0SR_BSY 0x10
#define SSP0SR_BSY_BIT 4
#define SSP0SR_RFF_MASK 0x8
#define SSP0SR_RFF 0x8
#define SSP0SR_RFF_BIT 3
#define SSP0SR_RNE_MASK 0x4
#define SSP0SR_RNE 0x4
#define SSP0SR_RNE_BIT 2
#define SSP0SR_TNF_MASK 0x2
#define SSP0SR_TNF 0x2
#define SSP0SR_TNF_BIT 1
#define SSP0SR_TFE_MASK 0x1
#define SSP0SR_TFE 0x1
#define SSP0SR_TFE_BIT 0

#define SSP0CPSR (*(volatile unsigned long *)0xE0068010)
#define SSP0CPSR_OFFSET 0x10
#define SSP0CPSR_CPSDVSR_MASK 0xFF
#define SSP0CPSR_CPSDVSR_BIT 0

#define SSP0IMSC (*(volatile unsigned long *)0xE0068014)
#define SSP0IMSC_OFFSET 0x14
#define SSP0IMSC_TXIM_MASK 0x8
#define SSP0IMSC_TXIM 0x8
#define SSP0IMSC_TXIM_BIT 3
#define SSP0IMSC_RXIM_MASK 0x4
#define SSP0IMSC_RXIM 0x4
#define SSP0IMSC_RXIM_BIT 2
#define SSP0IMSC_RTIM_MASK 0x2
#define SSP0IMSC_RTIM 0x2
#define SSP0IMSC_RTIM_BIT 1
#define SSP0IMSC_RORIM_MASK 0x1
#define SSP0IMSC_RORIM 0x1
#define SSP0IMSC_RORIM_BIT 0

#define SSP0RIS (*(volatile unsigned long *)0xE0068018)
#define SSP0RIS_OFFSET 0x18
#define SSP0RIS_TXRIS_MASK 0x8
#define SSP0RIS_TXRIS 0x8
#define SSP0RIS_TXRIS_BIT 3
#define SSP0RIS_RXRIS_MASK 0x4
#define SSP0RIS_RXRIS 0x4
#define SSP0RIS_RXRIS_BIT 2
#define SSP0RIS_RTRIS_MASK 0x2
#define SSP0RIS_RTRIS 0x2
#define SSP0RIS_RTRIS_BIT 1
#define SSP0RIS_RORRIS_MASK 0x1
#define SSP0RIS_RORRIS 0x1
#define SSP0RIS_RORRIS_BIT 0

#define SSP0MIS (*(volatile unsigned long *)0xE006801C)
#define SSP0MIS_OFFSET 0x1C
#define SSP0MIS_TXMIS_MASK 0x8
#define SSP0MIS_TXMIS 0x8
#define SSP0MIS_TXMIS_BIT 3
#define SSP0MIS_RXMIS_MASK 0x4
#define SSP0MIS_RXMIS 0x4
#define SSP0MIS_RXMIS_BIT 2
#define SSP0MIS_RTMIS_MASK 0x2
#define SSP0MIS_RTMIS 0x2
#define SSP0MIS_RTMIS_BIT 1
#define SSP0MIS_RORMIS_MASK 0x1
#define SSP0MIS_RORMIS 0x1
#define SSP0MIS_RORMIS_BIT 0

#define SSP0ICR (*(volatile unsigned long *)0xE0068020)
#define SSP0ICR_OFFSET 0x20
#define SSP0ICR_RTIC_MASK 0x2
#define SSP0ICR_RTIC 0x2
#define SSP0ICR_RTIC_BIT 1
#define SSP0ICR_RORIC_MASK 0x1
#define SSP0ICR_RORIC 0x1
#define SSP0ICR_RORIC_BIT 0

#define SSP0DMACR (*(volatile unsigned long *)0xE0068024)
#define SSP0DMACR_OFFSET 0x24
#define SSP0DMACR_RXDMAE_MASK 0x1
#define SSP0DMACR_RXDMAE 0x1
#define SSP0DMACR_RXDMAE_BIT 0
#define SSP0DMACR_TXDMAE_MASK 0x2
#define SSP0DMACR_TXDMAE 0x2
#define SSP0DMACR_TXDMAE_BIT 1

#define DAC_BASE 0xE006C000

#define DACR (*(volatile unsigned long *)0xE006C000)
#define DACR_OFFSET 0x0
#define DACR_VALUE_MASK 0xFFC0
#define DACR_VALUE_BIT 6
#define DACR_BIAS_MASK 0x10000
#define DACR_BIAS 0x10000
#define DACR_BIAS_BIT 16

#define TIMER2_BASE 0xE0070000

#define T2IR (*(volatile unsigned char *)0xE0070000)
#define T2IR_OFFSET 0x0
#define T2IR_MR0_MASK 0x1
#define T2IR_MR0 0x1
#define T2IR_MR0_BIT 0
#define T2IR_MR1_MASK 0x2
#define T2IR_MR1 0x2
#define T2IR_MR1_BIT 1
#define T2IR_MR2_MASK 0x4
#define T2IR_MR2 0x4
#define T2IR_MR2_BIT 2
#define T2IR_MR3_MASK 0x8
#define T2IR_MR3 0x8
#define T2IR_MR3_BIT 3
#define T2IR_CR0_MASK 0x10
#define T2IR_CR0 0x10
#define T2IR_CR0_BIT 4
#define T2IR_CR1_MASK 0x20
#define T2IR_CR1 0x20
#define T2IR_CR1_BIT 5
#define T2IR_CR2_MASK 0x40
#define T2IR_CR2 0x40
#define T2IR_CR2_BIT 6
#define T2IR_CR3_MASK 0x80
#define T2IR_CR3 0x80
#define T2IR_CR3_BIT 7

#define T2TCR (*(volatile unsigned char *)0xE0070004)
#define T2TCR_OFFSET 0x4
#define T2TCR_Counter_Enable_MASK 0x1
#define T2TCR_Counter_Enable 0x1
#define T2TCR_Counter_Enable_BIT 0
#define T2TCR_Counter_Reset_MASK 0x2
#define T2TCR_Counter_Reset 0x2
#define T2TCR_Counter_Reset_BIT 1

#define T2TC (*(volatile unsigned long *)0xE0070008)
#define T2TC_OFFSET 0x8

#define T2PR (*(volatile unsigned long *)0xE007000C)
#define T2PR_OFFSET 0xC

#define T2PC (*(volatile unsigned long *)0xE0070010)
#define T2PC_OFFSET 0x10

#define T2MCR (*(volatile unsigned short *)0xE0070014)
#define T2MCR_OFFSET 0x14
#define T2MCR_MR0I_MASK 0x1
#define T2MCR_MR0I 0x1
#define T2MCR_MR0I_BIT 0
#define T2MCR_MR0R_MASK 0x2
#define T2MCR_MR0R 0x2
#define T2MCR_MR0R_BIT 1
#define T2MCR_MR0S_MASK 0x4
#define T2MCR_MR0S 0x4
#define T2MCR_MR0S_BIT 2
#define T2MCR_MR1I_MASK 0x8
#define T2MCR_MR1I 0x8
#define T2MCR_MR1I_BIT 3
#define T2MCR_MR1R_MASK 0x10
#define T2MCR_MR1R 0x10
#define T2MCR_MR1R_BIT 4
#define T2MCR_MR1S_MASK 0x20
#define T2MCR_MR1S 0x20
#define T2MCR_MR1S_BIT 5
#define T2MCR_MR2I_MASK 0x40
#define T2MCR_MR2I 0x40
#define T2MCR_MR2I_BIT 6
#define T2MCR_MR2R_MASK 0x80
#define T2MCR_MR2R 0x80
#define T2MCR_MR2R_BIT 7
#define T2MCR_MR2S_MASK 0x100
#define T2MCR_MR2S 0x100
#define T2MCR_MR2S_BIT 8
#define T2MCR_MR3I_MASK 0x200
#define T2MCR_MR3I 0x200
#define T2MCR_MR3I_BIT 9
#define T2MCR_MR3R_MASK 0x400
#define T2MCR_MR3R 0x400
#define T2MCR_MR3R_BIT 10
#define T2MCR_MR3S_MASK 0x800
#define T2MCR_MR3S 0x800
#define T2MCR_MR3S_BIT 11

#define T2MR0 (*(volatile unsigned long *)0xE0070018)
#define T2MR0_OFFSET 0x18

#define T2MR1 (*(volatile unsigned long *)0xE007001C)
#define T2MR1_OFFSET 0x1C

#define T2MR2 (*(volatile unsigned long *)0xE0070020)
#define T2MR2_OFFSET 0x20

#define T2MR3 (*(volatile unsigned long *)0xE0070024)
#define T2MR3_OFFSET 0x24

#define T2CCR (*(volatile unsigned short *)0xE0070028)
#define T2CCR_OFFSET 0x28
#define T2CCR_CAP0RE_MASK 0x1
#define T2CCR_CAP0RE 0x1
#define T2CCR_CAP0RE_BIT 0
#define T2CCR_CAP0FE_MASK 0x2
#define T2CCR_CAP0FE 0x2
#define T2CCR_CAP0FE_BIT 1
#define T2CCR_CAP0I_MASK 0x4
#define T2CCR_CAP0I 0x4
#define T2CCR_CAP0I_BIT 2
#define T2CCR_CAP1RE_MASK 0x8
#define T2CCR_CAP1RE 0x8
#define T2CCR_CAP1RE_BIT 3
#define T2CCR_CAP1FE_MASK 0x10
#define T2CCR_CAP1FE 0x10
#define T2CCR_CAP1FE_BIT 4
#define T2CCR_CAP1I_MASK 0x20
#define T2CCR_CAP1I 0x20
#define T2CCR_CAP1I_BIT 5
#define T2CCR_CAP2RE_MASK 0x40
#define T2CCR_CAP2RE 0x40
#define T2CCR_CAP2RE_BIT 6
#define T2CCR_CAP2FE_MASK 0x80
#define T2CCR_CAP2FE 0x80
#define T2CCR_CAP2FE_BIT 7
#define T2CCR_CAP2I_MASK 0x100
#define T2CCR_CAP2I 0x100
#define T2CCR_CAP2I_BIT 8
#define T2CCR_CAP3RE_MASK 0x200
#define T2CCR_CAP3RE 0x200
#define T2CCR_CAP3RE_BIT 9
#define T2CCR_CAP3FE_MASK 0x400
#define T2CCR_CAP3FE 0x400
#define T2CCR_CAP3FE_BIT 10
#define T2CCR_CAP3I_MASK 0x800
#define T2CCR_CAP3I 0x800
#define T2CCR_CAP3I_BIT 11

#define T2CR0 (*(volatile unsigned long *)0xE007002C)
#define T2CR0_OFFSET 0x2C

#define T2CR1 (*(volatile unsigned long *)0xE0070030)
#define T2CR1_OFFSET 0x30

#define T2CR2 (*(volatile unsigned long *)0xE0070034)
#define T2CR2_OFFSET 0x34

#define T2CR3 (*(volatile unsigned long *)0xE0070038)
#define T2CR3_OFFSET 0x38

#define T2EMR (*(volatile unsigned short *)0xE007003C)
#define T2EMR_OFFSET 0x3C
#define T2EMR_EM0_MASK 0x1
#define T2EMR_EM0 0x1
#define T2EMR_EM0_BIT 0
#define T2EMR_EM1_MASK 0x2
#define T2EMR_EM1 0x2
#define T2EMR_EM1_BIT 1
#define T2EMR_EM2_MASK 0x4
#define T2EMR_EM2 0x4
#define T2EMR_EM2_BIT 2
#define T2EMR_EM3_MASK 0x8
#define T2EMR_EM3 0x8
#define T2EMR_EM3_BIT 3
#define T2EMR_EMC0_MASK 0x30
#define T2EMR_EMC0_BIT 4
#define T2EMR_EMC1_MASK 0xC0
#define T2EMR_EMC1_BIT 6
#define T2EMR_EMC2_MASK 0x300
#define T2EMR_EMC2_BIT 8
#define T2EMR_EMC3_MASK 0xC00
#define T2EMR_EMC3_BIT 10

#define T2CTCR (*(volatile unsigned long *)0xE0070070)
#define T2CTCR_OFFSET 0x70
#define T2CTCR_Counter_Timer_Mode_MASK 0x3
#define T2CTCR_Counter_Timer_Mode_BIT 0
#define T2CTCR_Count_Input_Select_MASK 0xC
#define T2CTCR_Count_Input_Select_BIT 2

#define TIMER3_BASE 0xE0074000

#define T3IR (*(volatile unsigned char *)0xE0074000)
#define T3IR_OFFSET 0x0
#define T3IR_MR0_MASK 0x1
#define T3IR_MR0 0x1
#define T3IR_MR0_BIT 0
#define T3IR_MR1_MASK 0x2
#define T3IR_MR1 0x2
#define T3IR_MR1_BIT 1
#define T3IR_MR2_MASK 0x4
#define T3IR_MR2 0x4
#define T3IR_MR2_BIT 2
#define T3IR_MR3_MASK 0x8
#define T3IR_MR3 0x8
#define T3IR_MR3_BIT 3
#define T3IR_CR0_MASK 0x10
#define T3IR_CR0 0x10
#define T3IR_CR0_BIT 4
#define T3IR_CR1_MASK 0x20
#define T3IR_CR1 0x20
#define T3IR_CR1_BIT 5
#define T3IR_CR2_MASK 0x40
#define T3IR_CR2 0x40
#define T3IR_CR2_BIT 6
#define T3IR_CR3_MASK 0x80
#define T3IR_CR3 0x80
#define T3IR_CR3_BIT 7

#define T3TCR (*(volatile unsigned char *)0xE0074004)
#define T3TCR_OFFSET 0x4
#define T3TCR_Counter_Enable_MASK 0x1
#define T3TCR_Counter_Enable 0x1
#define T3TCR_Counter_Enable_BIT 0
#define T3TCR_Counter_Reset_MASK 0x2
#define T3TCR_Counter_Reset 0x2
#define T3TCR_Counter_Reset_BIT 1

#define T3TC (*(volatile unsigned long *)0xE0074008)
#define T3TC_OFFSET 0x8

#define T3PR (*(volatile unsigned long *)0xE007400C)
#define T3PR_OFFSET 0xC

#define T3PC (*(volatile unsigned long *)0xE0074010)
#define T3PC_OFFSET 0x10

#define T3MCR (*(volatile unsigned short *)0xE0074014)
#define T3MCR_OFFSET 0x14
#define T3MCR_MR0I_MASK 0x1
#define T3MCR_MR0I 0x1
#define T3MCR_MR0I_BIT 0
#define T3MCR_MR0R_MASK 0x2
#define T3MCR_MR0R 0x2
#define T3MCR_MR0R_BIT 1
#define T3MCR_MR0S_MASK 0x4
#define T3MCR_MR0S 0x4
#define T3MCR_MR0S_BIT 2
#define T3MCR_MR1I_MASK 0x8
#define T3MCR_MR1I 0x8
#define T3MCR_MR1I_BIT 3
#define T3MCR_MR1R_MASK 0x10
#define T3MCR_MR1R 0x10
#define T3MCR_MR1R_BIT 4
#define T3MCR_MR1S_MASK 0x20
#define T3MCR_MR1S 0x20
#define T3MCR_MR1S_BIT 5
#define T3MCR_MR2I_MASK 0x40
#define T3MCR_MR2I 0x40
#define T3MCR_MR2I_BIT 6
#define T3MCR_MR2R_MASK 0x80
#define T3MCR_MR2R 0x80
#define T3MCR_MR2R_BIT 7
#define T3MCR_MR2S_MASK 0x100
#define T3MCR_MR2S 0x100
#define T3MCR_MR2S_BIT 8
#define T3MCR_MR3I_MASK 0x200
#define T3MCR_MR3I 0x200
#define T3MCR_MR3I_BIT 9
#define T3MCR_MR3R_MASK 0x400
#define T3MCR_MR3R 0x400
#define T3MCR_MR3R_BIT 10
#define T3MCR_MR3S_MASK 0x800
#define T3MCR_MR3S 0x800
#define T3MCR_MR3S_BIT 11

#define T3MR0 (*(volatile unsigned long *)0xE0074018)
#define T3MR0_OFFSET 0x18

#define T3MR1 (*(volatile unsigned long *)0xE007401C)
#define T3MR1_OFFSET 0x1C

#define T3MR2 (*(volatile unsigned long *)0xE0074020)
#define T3MR2_OFFSET 0x20

#define T3MR3 (*(volatile unsigned long *)0xE0074024)
#define T3MR3_OFFSET 0x24

#define T3CCR (*(volatile unsigned short *)0xE0074028)
#define T3CCR_OFFSET 0x28
#define T3CCR_CAP0RE_MASK 0x1
#define T3CCR_CAP0RE 0x1
#define T3CCR_CAP0RE_BIT 0
#define T3CCR_CAP0FE_MASK 0x2
#define T3CCR_CAP0FE 0x2
#define T3CCR_CAP0FE_BIT 1
#define T3CCR_CAP0I_MASK 0x4
#define T3CCR_CAP0I 0x4
#define T3CCR_CAP0I_BIT 2
#define T3CCR_CAP1RE_MASK 0x8
#define T3CCR_CAP1RE 0x8
#define T3CCR_CAP1RE_BIT 3
#define T3CCR_CAP1FE_MASK 0x10
#define T3CCR_CAP1FE 0x10
#define T3CCR_CAP1FE_BIT 4
#define T3CCR_CAP1I_MASK 0x20
#define T3CCR_CAP1I 0x20
#define T3CCR_CAP1I_BIT 5
#define T3CCR_CAP2RE_MASK 0x40
#define T3CCR_CAP2RE 0x40
#define T3CCR_CAP2RE_BIT 6
#define T3CCR_CAP2FE_MASK 0x80
#define T3CCR_CAP2FE 0x80
#define T3CCR_CAP2FE_BIT 7
#define T3CCR_CAP2I_MASK 0x100
#define T3CCR_CAP2I 0x100
#define T3CCR_CAP2I_BIT 8
#define T3CCR_CAP3RE_MASK 0x200
#define T3CCR_CAP3RE 0x200
#define T3CCR_CAP3RE_BIT 9
#define T3CCR_CAP3FE_MASK 0x400
#define T3CCR_CAP3FE 0x400
#define T3CCR_CAP3FE_BIT 10
#define T3CCR_CAP3I_MASK 0x800
#define T3CCR_CAP3I 0x800
#define T3CCR_CAP3I_BIT 11

#define T3CR0 (*(volatile unsigned long *)0xE007402C)
#define T3CR0_OFFSET 0x2C

#define T3CR1 (*(volatile unsigned long *)0xE0074030)
#define T3CR1_OFFSET 0x30

#define T3CR2 (*(volatile unsigned long *)0xE0074034)
#define T3CR2_OFFSET 0x34

#define T3CR3 (*(volatile unsigned long *)0xE0074038)
#define T3CR3_OFFSET 0x38

#define T3EMR (*(volatile unsigned short *)0xE007403C)
#define T3EMR_OFFSET 0x3C
#define T3EMR_EM0_MASK 0x1
#define T3EMR_EM0 0x1
#define T3EMR_EM0_BIT 0
#define T3EMR_EM1_MASK 0x2
#define T3EMR_EM1 0x2
#define T3EMR_EM1_BIT 1
#define T3EMR_EM2_MASK 0x4
#define T3EMR_EM2 0x4
#define T3EMR_EM2_BIT 2
#define T3EMR_EM3_MASK 0x8
#define T3EMR_EM3 0x8
#define T3EMR_EM3_BIT 3
#define T3EMR_EMC0_MASK 0x30
#define T3EMR_EMC0_BIT 4
#define T3EMR_EMC1_MASK 0xC0
#define T3EMR_EMC1_BIT 6
#define T3EMR_EMC2_MASK 0x300
#define T3EMR_EMC2_BIT 8
#define T3EMR_EMC3_MASK 0xC00
#define T3EMR_EMC3_BIT 10

#define T3CTCR (*(volatile unsigned long *)0xE0074070)
#define T3CTCR_OFFSET 0x70
#define T3CTCR_Counter_Timer_Mode_MASK 0x3
#define T3CTCR_Counter_Timer_Mode_BIT 0
#define T3CTCR_Count_Input_Select_MASK 0xC
#define T3CTCR_Count_Input_Select_BIT 2

#define UART2_BASE 0xE0078000

#define U2RBR (*(volatile unsigned char *)0xE0078000)
#define U2RBR_OFFSET 0x0

#define U2THR (*(volatile unsigned char *)0xE0078000)
#define U2THR_OFFSET 0x0

#define U2DLL (*(volatile unsigned char *)0xE0078000)
#define U2DLL_OFFSET 0x0

#define U2DLM (*(volatile unsigned char *)0xE0078004)
#define U2DLM_OFFSET 0x4

#define U2IER (*(volatile unsigned long *)0xE0078004)
#define U2IER_OFFSET 0x4
#define U2IER_RBR_Interrupt_Enable_MASK 0x1
#define U2IER_RBR_Interrupt_Enable 0x1
#define U2IER_RBR_Interrupt_Enable_BIT 0
#define U2IER_THRE_Interrupt_Enable_MASK 0x2
#define U2IER_THRE_Interrupt_Enable 0x2
#define U2IER_THRE_Interrupt_Enable_BIT 1
#define U2IER_Rx_Line_Status_Interrupt_Enable_MASK 0x4
#define U2IER_Rx_Line_Status_Interrupt_Enable 0x4
#define U2IER_Rx_Line_Status_Interrupt_Enable_BIT 2
#define U2IER_ABTOIntEn_MASK 0x100
#define U2IER_ABTOIntEn 0x100
#define U2IER_ABTOIntEn_BIT 8
#define U2IER_ABEOIntEn_MASK 0x200
#define U2IER_ABEOIntEn 0x200
#define U2IER_ABEOIntEn_BIT 9

#define U2IIR (*(volatile unsigned long *)0xE0078008)
#define U2IIR_OFFSET 0x8
#define U2IIR_IntStatus_MASK 0x1
#define U2IIR_IntStatus 0x1
#define U2IIR_IntStatus_BIT 0
#define U2IIR_IntId_MASK 0xE
#define U2IIR_IntId_BIT 1
#define U2IIR_FIFO_Enable_MASK 0xC0
#define U2IIR_FIFO_Enable_BIT 6
#define U2IIR_ABEOInt_MASK 0x100
#define U2IIR_ABEOInt 0x100
#define U2IIR_ABEOInt_BIT 8
#define U2IIR_ABTOInt_MASK 0x200
#define U2IIR_ABTOInt 0x200
#define U2IIR_ABTOInt_BIT 9

#define U2FCR (*(volatile unsigned char *)0xE0078008)
#define U2FCR_OFFSET 0x8
#define U2FCR_FIFO_Enable_MASK 0x1
#define U2FCR_FIFO_Enable 0x1
#define U2FCR_FIFO_Enable_BIT 0
#define U2FCR_Rx_FIFO_Reset_MASK 0x2
#define U2FCR_Rx_FIFO_Reset 0x2
#define U2FCR_Rx_FIFO_Reset_BIT 1
#define U2FCR_Tx_FIFO_Reset_MASK 0x4
#define U2FCR_Tx_FIFO_Reset 0x4
#define U2FCR_Tx_FIFO_Reset_BIT 2
#define U2FCR_Rx_Trigger_Level_Select_MASK 0xC0
#define U2FCR_Rx_Trigger_Level_Select_BIT 6

#define U2LCR (*(volatile unsigned char *)0xE007800C)
#define U2LCR_OFFSET 0xC
#define U2LCR_Word_Length_Select_MASK 0x3
#define U2LCR_Word_Length_Select_BIT 0
#define U2LCR_Stop_Bit_Select_MASK 0x4
#define U2LCR_Stop_Bit_Select 0x4
#define U2LCR_Stop_Bit_Select_BIT 2
#define U2LCR_Parity_Enable_MASK 0x8
#define U2LCR_Parity_Enable 0x8
#define U2LCR_Parity_Enable_BIT 3
#define U2LCR_Parity_Select_MASK 0x30
#define U2LCR_Parity_Select_BIT 4
#define U2LCR_Break_Control_MASK 0x40
#define U2LCR_Break_Control 0x40
#define U2LCR_Break_Control_BIT 6
#define U2LCR_Divisor_Latch_Access_Bit_MASK 0x80
#define U2LCR_Divisor_Latch_Access_Bit 0x80
#define U2LCR_Divisor_Latch_Access_Bit_BIT 7

#define U2LSR (*(volatile unsigned char *)0xE0078014)
#define U2LSR_OFFSET 0x14
#define U2LSR_RDR_MASK 0x1
#define U2LSR_RDR 0x1
#define U2LSR_RDR_BIT 0
#define U2LSR_OE_MASK 0x2
#define U2LSR_OE 0x2
#define U2LSR_OE_BIT 1
#define U2LSR_PE_MASK 0x4
#define U2LSR_PE 0x4
#define U2LSR_PE_BIT 2
#define U2LSR_FE_MASK 0x8
#define U2LSR_FE 0x8
#define U2LSR_FE_BIT 3
#define U2LSR_BI_MASK 0x10
#define U2LSR_BI 0x10
#define U2LSR_BI_BIT 4
#define U2LSR_THRE_MASK 0x20
#define U2LSR_THRE 0x20
#define U2LSR_THRE_BIT 5
#define U2LSR_TEMT_MASK 0x40
#define U2LSR_TEMT 0x40
#define U2LSR_TEMT_BIT 6
#define U2LSR_RXFE_MASK 0x80
#define U2LSR_RXFE 0x80
#define U2LSR_RXFE_BIT 7

#define U2SCR (*(volatile unsigned char *)0xE007801C)
#define U2SCR_OFFSET 0x1C

#define U2ACR (*(volatile unsigned long *)0xE0078020)
#define U2ACR_OFFSET 0x20
#define U2ACR_Start_MASK 0x1
#define U2ACR_Start 0x1
#define U2ACR_Start_BIT 0
#define U2ACR_Mode_MASK 0x2
#define U2ACR_Mode 0x2
#define U2ACR_Mode_BIT 1
#define U2ACR_AutoRestart_MASK 0x4
#define U2ACR_AutoRestart 0x4
#define U2ACR_AutoRestart_BIT 2
#define U2ACR_ABEOIntClr_MASK 0x100
#define U2ACR_ABEOIntClr 0x100
#define U2ACR_ABEOIntClr_BIT 8
#define U2ACR_ABTOIntClr_MASK 0x200
#define U2ACR_ABTOIntClr 0x200
#define U2ACR_ABTOIntClr_BIT 9

#define U2FDR (*(volatile unsigned long *)0xE0078028)
#define U2FDR_OFFSET 0x28
#define U2FDR_DIVADDVAL_MASK 0xF
#define U2FDR_DIVADDVAL_BIT 0
#define U2FDR_MULVAL_MASK 0xF0
#define U2FDR_MULVAL_BIT 4

#define U2TER (*(volatile unsigned char *)0xE0078030)
#define U2TER_OFFSET 0x30
#define U2TER_TXEN_MASK 0x80
#define U2TER_TXEN 0x80
#define U2TER_TXEN_BIT 7

#define UART3_BASE 0xE007C000

#define U3RBR (*(volatile unsigned char *)0xE007C000)
#define U3RBR_OFFSET 0x0

#define U3THR (*(volatile unsigned char *)0xE007C000)
#define U3THR_OFFSET 0x0

#define U3DLL (*(volatile unsigned char *)0xE007C000)
#define U3DLL_OFFSET 0x0

#define U3DLM (*(volatile unsigned char *)0xE007C004)
#define U3DLM_OFFSET 0x4

#define U3IER (*(volatile unsigned long *)0xE007C004)
#define U3IER_OFFSET 0x4
#define U3IER_RBR_Interrupt_Enable_MASK 0x1
#define U3IER_RBR_Interrupt_Enable 0x1
#define U3IER_RBR_Interrupt_Enable_BIT 0
#define U3IER_THRE_Interrupt_Enable_MASK 0x2
#define U3IER_THRE_Interrupt_Enable 0x2
#define U3IER_THRE_Interrupt_Enable_BIT 1
#define U3IER_Rx_Line_Status_Interrupt_Enable_MASK 0x4
#define U3IER_Rx_Line_Status_Interrupt_Enable 0x4
#define U3IER_Rx_Line_Status_Interrupt_Enable_BIT 2
#define U3IER_ABTOIntEn_MASK 0x100
#define U3IER_ABTOIntEn 0x100
#define U3IER_ABTOIntEn_BIT 8
#define U3IER_ABEOIntEn_MASK 0x200
#define U3IER_ABEOIntEn 0x200
#define U3IER_ABEOIntEn_BIT 9

#define U3IIR (*(volatile unsigned long *)0xE007C008)
#define U3IIR_OFFSET 0x8
#define U3IIR_IntStatus_MASK 0x1
#define U3IIR_IntStatus 0x1
#define U3IIR_IntStatus_BIT 0
#define U3IIR_IntId_MASK 0xE
#define U3IIR_IntId_BIT 1
#define U3IIR_FIFO_Enable_MASK 0xC0
#define U3IIR_FIFO_Enable_BIT 6
#define U3IIR_ABEOInt_MASK 0x100
#define U3IIR_ABEOInt 0x100
#define U3IIR_ABEOInt_BIT 8
#define U3IIR_ABTOInt_MASK 0x200
#define U3IIR_ABTOInt 0x200
#define U3IIR_ABTOInt_BIT 9

#define U3FCR (*(volatile unsigned char *)0xE007C008)
#define U3FCR_OFFSET 0x8
#define U3FCR_FIFO_Enable_MASK 0x1
#define U3FCR_FIFO_Enable 0x1
#define U3FCR_FIFO_Enable_BIT 0
#define U3FCR_Rx_FIFO_Reset_MASK 0x2
#define U3FCR_Rx_FIFO_Reset 0x2
#define U3FCR_Rx_FIFO_Reset_BIT 1
#define U3FCR_Tx_FIFO_Reset_MASK 0x4
#define U3FCR_Tx_FIFO_Reset 0x4
#define U3FCR_Tx_FIFO_Reset_BIT 2
#define U3FCR_Rx_Trigger_Level_Select_MASK 0xC0
#define U3FCR_Rx_Trigger_Level_Select_BIT 6

#define U3LCR (*(volatile unsigned char *)0xE007C00C)
#define U3LCR_OFFSET 0xC
#define U3LCR_Word_Length_Select_MASK 0x3
#define U3LCR_Word_Length_Select_BIT 0
#define U3LCR_Stop_Bit_Select_MASK 0x4
#define U3LCR_Stop_Bit_Select 0x4
#define U3LCR_Stop_Bit_Select_BIT 2
#define U3LCR_Parity_Enable_MASK 0x8
#define U3LCR_Parity_Enable 0x8
#define U3LCR_Parity_Enable_BIT 3
#define U3LCR_Parity_Select_MASK 0x30
#define U3LCR_Parity_Select_BIT 4
#define U3LCR_Break_Control_MASK 0x40
#define U3LCR_Break_Control 0x40
#define U3LCR_Break_Control_BIT 6
#define U3LCR_Divisor_Latch_Access_Bit_MASK 0x80
#define U3LCR_Divisor_Latch_Access_Bit 0x80
#define U3LCR_Divisor_Latch_Access_Bit_BIT 7

#define U3LSR (*(volatile unsigned char *)0xE007C014)
#define U3LSR_OFFSET 0x14
#define U3LSR_RDR_MASK 0x1
#define U3LSR_RDR 0x1
#define U3LSR_RDR_BIT 0
#define U3LSR_OE_MASK 0x2
#define U3LSR_OE 0x2
#define U3LSR_OE_BIT 1
#define U3LSR_PE_MASK 0x4
#define U3LSR_PE 0x4
#define U3LSR_PE_BIT 2
#define U3LSR_FE_MASK 0x8
#define U3LSR_FE 0x8
#define U3LSR_FE_BIT 3
#define U3LSR_BI_MASK 0x10
#define U3LSR_BI 0x10
#define U3LSR_BI_BIT 4
#define U3LSR_THRE_MASK 0x20
#define U3LSR_THRE 0x20
#define U3LSR_THRE_BIT 5
#define U3LSR_TEMT_MASK 0x40
#define U3LSR_TEMT 0x40
#define U3LSR_TEMT_BIT 6
#define U3LSR_RXFE_MASK 0x80
#define U3LSR_RXFE 0x80
#define U3LSR_RXFE_BIT 7

#define U3SCR (*(volatile unsigned char *)0xE007C01C)
#define U3SCR_OFFSET 0x1C

#define U3ACR (*(volatile unsigned long *)0xE007C020)
#define U3ACR_OFFSET 0x20
#define U3ACR_Start_MASK 0x1
#define U3ACR_Start 0x1
#define U3ACR_Start_BIT 0
#define U3ACR_Mode_MASK 0x2
#define U3ACR_Mode 0x2
#define U3ACR_Mode_BIT 1
#define U3ACR_AutoRestart_MASK 0x4
#define U3ACR_AutoRestart 0x4
#define U3ACR_AutoRestart_BIT 2
#define U3ACR_ABEOIntClr_MASK 0x100
#define U3ACR_ABEOIntClr 0x100
#define U3ACR_ABEOIntClr_BIT 8
#define U3ACR_ABTOIntClr_MASK 0x200
#define U3ACR_ABTOIntClr 0x200
#define U3ACR_ABTOIntClr_BIT 9

#define U3ICR (*(volatile unsigned long *)0xE007C024)
#define U3ICR_OFFSET 0x24
#define U3ICR_IrDAEn_MASK 0x1
#define U3ICR_IrDAEn 0x1
#define U3ICR_IrDAEn_BIT 0
#define U3ICR_IrDAInv_MASK 0x2
#define U3ICR_IrDAInv 0x2
#define U3ICR_IrDAInv_BIT 1
#define U3ICR_FixPulse_En_MASK 0x4
#define U3ICR_FixPulse_En 0x4
#define U3ICR_FixPulse_En_BIT 2
#define U3ICR_PulseDiv_MASK 0x38
#define U3ICR_PulseDiv_BIT 3

#define U3FDR (*(volatile unsigned long *)0xE007C028)
#define U3FDR_OFFSET 0x28
#define U3FDR_DIVADDVAL_MASK 0xF
#define U3FDR_DIVADDVAL_BIT 0
#define U3FDR_MULVAL_MASK 0xF0
#define U3FDR_MULVAL_BIT 4

#define U3TER (*(volatile unsigned char *)0xE007C030)
#define U3TER_OFFSET 0x30
#define U3TER_TXEN_MASK 0x80
#define U3TER_TXEN 0x80
#define U3TER_TXEN_BIT 7

#define I2C2_BASE 0xE0080000

#define I2C2CONSET (*(volatile unsigned char *)0xE0080000)
#define I2C2CONSET_OFFSET 0x0
#define I2C2CONSET_AA_MASK 0x4
#define I2C2CONSET_AA 0x4
#define I2C2CONSET_AA_BIT 2
#define I2C2CONSET_SI_MASK 0x8
#define I2C2CONSET_SI 0x8
#define I2C2CONSET_SI_BIT 3
#define I2C2CONSET_STO_MASK 0x10
#define I2C2CONSET_STO 0x10
#define I2C2CONSET_STO_BIT 4
#define I2C2CONSET_STA_MASK 0x20
#define I2C2CONSET_STA 0x20
#define I2C2CONSET_STA_BIT 5
#define I2C2CONSET_I2EN_MASK 0x40
#define I2C2CONSET_I2EN 0x40
#define I2C2CONSET_I2EN_BIT 6

#define I2C2STAT (*(volatile unsigned char *)0xE0080004)
#define I2C2STAT_OFFSET 0x4
#define I2C2STAT_Status_MASK 0x7C
#define I2C2STAT_Status_BIT 2

#define I2C2DAT (*(volatile unsigned char *)0xE0080008)
#define I2C2DAT_OFFSET 0x8

#define I2C2ADR (*(volatile unsigned char *)0xE008000C)
#define I2C2ADR_OFFSET 0xC
#define I2C2ADR_GC_MASK 0x1
#define I2C2ADR_GC 0x1
#define I2C2ADR_GC_BIT 0
#define I2C2ADR_Address_MASK 0x7E
#define I2C2ADR_Address_BIT 1

#define I2C2SCLH (*(volatile unsigned short *)0xE0080010)
#define I2C2SCLH_OFFSET 0x10

#define I2C2SCLL (*(volatile unsigned short *)0xE0080014)
#define I2C2SCLL_OFFSET 0x14

#define I2C2CONCLR (*(volatile unsigned char *)0xE0080018)
#define I2C2CONCLR_OFFSET 0x18
#define I2C2CONCLR_AAC_MASK 0x4
#define I2C2CONCLR_AAC 0x4
#define I2C2CONCLR_AAC_BIT 2
#define I2C2CONCLR_SIC_MASK 0x8
#define I2C2CONCLR_SIC 0x8
#define I2C2CONCLR_SIC_BIT 3
#define I2C2CONCLR_STAC_MASK 0x20
#define I2C2CONCLR_STAC 0x20
#define I2C2CONCLR_STAC_BIT 5
#define I2C2CONCLR_I2ENC_MASK 0x40
#define I2C2CONCLR_I2ENC 0x40
#define I2C2CONCLR_I2ENC_BIT 6

#define I2S_BASE 0xE0088000

#define I2SDAO (*(volatile unsigned long *)0xE0088000)
#define I2SDAO_OFFSET 0x0
#define I2SDAO_wordwidth_MASK 0x3
#define I2SDAO_wordwidth_BIT 0
#define I2SDAO_mono_MASK 0x4
#define I2SDAO_mono 0x4
#define I2SDAO_mono_BIT 2
#define I2SDAO_stop_MASK 0x8
#define I2SDAO_stop 0x8
#define I2SDAO_stop_BIT 3
#define I2SDAO_reset_MASK 0x10
#define I2SDAO_reset 0x10
#define I2SDAO_reset_BIT 4
#define I2SDAO_ws_sel_MASK 0x20
#define I2SDAO_ws_sel 0x20
#define I2SDAO_ws_sel_BIT 5
#define I2SDAO_ws_halfperiod_MASK 0x3FC0
#define I2SDAO_ws_halfperiod_BIT 6
#define I2SDAO_mute_MASK 0x8000
#define I2SDAO_mute 0x8000
#define I2SDAO_mute_BIT 15

#define I2SDAI (*(volatile unsigned long *)0xE0088004)
#define I2SDAI_OFFSET 0x4
#define I2SDAI_wordwidth_MASK 0x3
#define I2SDAI_wordwidth_BIT 0
#define I2SDAI_mono_MASK 0x4
#define I2SDAI_mono 0x4
#define I2SDAI_mono_BIT 2
#define I2SDAI_stop_MASK 0x8
#define I2SDAI_stop 0x8
#define I2SDAI_stop_BIT 3
#define I2SDAI_reset_MASK 0x10
#define I2SDAI_reset 0x10
#define I2SDAI_reset_BIT 4
#define I2SDAI_ws_sel_MASK 0x20
#define I2SDAI_ws_sel 0x20
#define I2SDAI_ws_sel_BIT 5
#define I2SDAI_ws_halfperiod_MASK 0x3FC0
#define I2SDAI_ws_halfperiod_BIT 6

#define I2STXFIFO (*(volatile unsigned long *)0xE0088008)
#define I2STXFIFO_OFFSET 0x8

#define I2SRXFIFO (*(volatile unsigned long *)0xE008800C)
#define I2SRXFIFO_OFFSET 0xC

#define I2SSTATE (*(volatile unsigned long *)0xE0088010)
#define I2SSTATE_OFFSET 0x10
#define I2SSTATE_irq_MASK 0x1
#define I2SSTATE_irq 0x1
#define I2SSTATE_irq_BIT 0
#define I2SSTATE_dmareq1_MASK 0x2
#define I2SSTATE_dmareq1 0x2
#define I2SSTATE_dmareq1_BIT 1
#define I2SSTATE_dmareq2_MASK 0x4
#define I2SSTATE_dmareq2 0x4
#define I2SSTATE_dmareq2_BIT 2
#define I2SSTATE_rx_level_MASK 0xFF00
#define I2SSTATE_rx_level_BIT 8
#define I2SSTATE_tx_level_MASK 0xFF0000
#define I2SSTATE_tx_level_BIT 16

#define I2SDMA1 (*(volatile unsigned long *)0xE0088014)
#define I2SDMA1_OFFSET 0x14
#define I2SDMA1_rx_dma1_enable_MASK 0x1
#define I2SDMA1_rx_dma1_enable 0x1
#define I2SDMA1_rx_dma1_enable_BIT 0
#define I2SDMA1_tx_dma1_enable_MASK 0x2
#define I2SDMA1_tx_dma1_enable 0x2
#define I2SDMA1_tx_dma1_enable_BIT 1
#define I2SDMA1_rx_depth_dma1_MASK 0xFF00
#define I2SDMA1_rx_depth_dma1_BIT 8
#define I2SDMA1_tx_depth_dma1_MASK 0xFF0000
#define I2SDMA1_tx_depth_dma1_BIT 16

#define I2SDMA2 (*(volatile unsigned long *)0xE0088018)
#define I2SDMA2_OFFSET 0x18
#define I2SDMA2_rx_dma2_enable_MASK 0x1
#define I2SDMA2_rx_dma2_enable 0x1
#define I2SDMA2_rx_dma2_enable_BIT 0
#define I2SDMA2_tx_dma2_enable_MASK 0x2
#define I2SDMA2_tx_dma2_enable 0x2
#define I2SDMA2_tx_dma2_enable_BIT 1
#define I2SDMA2_rx_depth_dma2_MASK 0xFF00
#define I2SDMA2_rx_depth_dma2_BIT 8
#define I2SDMA2_tx_depth_dma2_MASK 0xFF0000
#define I2SDMA2_tx_depth_dma2_BIT 16

#define I2SIRQ (*(volatile unsigned long *)0xE008801C)
#define I2SIRQ_OFFSET 0x1C
#define I2SIRQ_rx_irq_enable_MASK 0x1
#define I2SIRQ_rx_irq_enable 0x1
#define I2SIRQ_rx_irq_enable_BIT 0
#define I2SIRQ_tx_irq_enable_MASK 0x2
#define I2SIRQ_tx_irq_enable 0x2
#define I2SIRQ_tx_irq_enable_BIT 1
#define I2SIRQ_rx_depth_irq_MASK 0xFF00
#define I2SIRQ_rx_depth_irq_BIT 8
#define I2SIRQ_tx_depth_irq_MASK 0xFF0000
#define I2SIRQ_tx_depth_irq_BIT 16

#define I2STXRATE (*(volatile unsigned long *)0xE0088020)
#define I2STXRATE_OFFSET 0x20
#define I2STXRATE_tx_rate_MASK 0x3FF
#define I2STXRATE_tx_rate_BIT 0

#define I2SRXRATE (*(volatile unsigned long *)0xE0088024)
#define I2SRXRATE_OFFSET 0x24
#define I2SRXRATE_rx_rate_MASK 0x3FF
#define I2SRXRATE_rx_rate_BIT 0

#define MCI_BASE 0xE008C000

#define MCIPower (*(volatile unsigned long *)0xE008C000)
#define MCIPower_OFFSET 0x0
#define MCIPower_Ctrl_MASK 0x3
#define MCIPower_Ctrl_BIT 0
#define MCIPower_OpenDrain_MASK 0x40
#define MCIPower_OpenDrain 0x40
#define MCIPower_OpenDrain_BIT 6
#define MCIPower_Rod_MASK 0x80
#define MCIPower_Rod 0x80
#define MCIPower_Rod_BIT 7

#define MCIClock (*(volatile unsigned long *)0xE008C004)
#define MCIClock_OFFSET 0x4
#define MCIClock_ClkDiv_MASK 0xFF
#define MCIClock_ClkDiv_BIT 0
#define MCIClock_Enable_MASK 0x100
#define MCIClock_Enable 0x100
#define MCIClock_Enable_BIT 8
#define MCIClock_PwrSave_MASK 0x200
#define MCIClock_PwrSave 0x200
#define MCIClock_PwrSave_BIT 9
#define MCIClock_Bypass_MASK 0x400
#define MCIClock_Bypass 0x400
#define MCIClock_Bypass_BIT 10
#define MCIClock_WideBus_MASK 0x800
#define MCIClock_WideBus 0x800
#define MCIClock_WideBus_BIT 11

#define MCIArgument (*(volatile unsigned long *)0xE008C008)
#define MCIArgument_OFFSET 0x8

#define MCICommand (*(volatile unsigned long *)0xE008C00C)
#define MCICommand_OFFSET 0xC
#define MCICommand_CmdIndex_MASK 0x3F
#define MCICommand_CmdIndex_BIT 0
#define MCICommand_Response_MASK 0x40
#define MCICommand_Response 0x40
#define MCICommand_Response_BIT 6
#define MCICommand_LongRsp_MASK 0x80
#define MCICommand_LongRsp 0x80
#define MCICommand_LongRsp_BIT 7
#define MCICommand_Interrupt_MASK 0x100
#define MCICommand_Interrupt 0x100
#define MCICommand_Interrupt_BIT 8
#define MCICommand_Pending_MASK 0x200
#define MCICommand_Pending 0x200
#define MCICommand_Pending_BIT 9
#define MCICommand_Enable_MASK 0x400
#define MCICommand_Enable 0x400
#define MCICommand_Enable_BIT 10

#define MCIRespCmd (*(volatile unsigned long *)0xE008C010)
#define MCIRespCmd_OFFSET 0x10
#define MCIRespCmd_RespCmd_MASK 0x3F
#define MCIRespCmd_RespCmd_BIT 0

#define MCIResponse0 (*(volatile unsigned long *)0xE008C014)
#define MCIResponse0_OFFSET 0x14

#define MCIResponse1 (*(volatile unsigned long *)0xE008C018)
#define MCIResponse1_OFFSET 0x18

#define MCIResponse2 (*(volatile unsigned long *)0xE008C01C)
#define MCIResponse2_OFFSET 0x1C

#define MCIResponse3 (*(volatile unsigned long *)0xE008C020)
#define MCIResponse3_OFFSET 0x20

#define MCIDataTimer (*(volatile unsigned long *)0xE008C024)
#define MCIDataTimer_OFFSET 0x24

#define MCIDataLength (*(volatile unsigned long *)0xE008C028)
#define MCIDataLength_OFFSET 0x28
#define MCIDataLength_DataLength_MASK 0xFFFF
#define MCIDataLength_DataLength_BIT 0

#define MCIDataCtrl (*(volatile unsigned long *)0xE008C02C)
#define MCIDataCtrl_OFFSET 0x2C
#define MCIDataCtrl_Enable_MASK 0x1
#define MCIDataCtrl_Enable 0x1
#define MCIDataCtrl_Enable_BIT 0
#define MCIDataCtrl_Direction_MASK 0x2
#define MCIDataCtrl_Direction 0x2
#define MCIDataCtrl_Direction_BIT 1
#define MCIDataCtrl_Mode_MASK 0x4
#define MCIDataCtrl_Mode 0x4
#define MCIDataCtrl_Mode_BIT 2
#define MCIDataCtrl_DMAEnable_MASK 0x8
#define MCIDataCtrl_DMAEnable 0x8
#define MCIDataCtrl_DMAEnable_BIT 3
#define MCIDataCtrl_BlockSize_MASK 0xF0
#define MCIDataCtrl_BlockSize_BIT 4

#define MCIDataCnt (*(volatile unsigned long *)0xE008C030)
#define MCIDataCnt_OFFSET 0x30
#define MCIDataCnt_DataCount_MASK 0xFFFF
#define MCIDataCnt_DataCount_BIT 0

#define MCIStatus (*(volatile unsigned long *)0xE008C034)
#define MCIStatus_OFFSET 0x34
#define MCIStatus_CmdCrcFail_MASK 0x1
#define MCIStatus_CmdCrcFail 0x1
#define MCIStatus_CmdCrcFail_BIT 0
#define MCIStatus_DataCrcFail_MASK 0x2
#define MCIStatus_DataCrcFail 0x2
#define MCIStatus_DataCrcFail_BIT 1
#define MCIStatus_CmdTimeOut_MASK 0x4
#define MCIStatus_CmdTimeOut 0x4
#define MCIStatus_CmdTimeOut_BIT 2
#define MCIStatus_DataTimeOut_MASK 0x8
#define MCIStatus_DataTimeOut 0x8
#define MCIStatus_DataTimeOut_BIT 3
#define MCIStatus_TxUnderrun_MASK 0x10
#define MCIStatus_TxUnderrun 0x10
#define MCIStatus_TxUnderrun_BIT 4
#define MCIStatus_RxOverrun_MASK 0x20
#define MCIStatus_RxOverrun 0x20
#define MCIStatus_RxOverrun_BIT 5
#define MCIStatus_CmdRespEnd_MASK 0x40
#define MCIStatus_CmdRespEnd 0x40
#define MCIStatus_CmdRespEnd_BIT 6
#define MCIStatus_CmdSent_MASK 0x80
#define MCIStatus_CmdSent 0x80
#define MCIStatus_CmdSent_BIT 7
#define MCIStatus_DataEnd_MASK 0x100
#define MCIStatus_DataEnd 0x100
#define MCIStatus_DataEnd_BIT 8
#define MCIStatus_StartBitErr_MASK 0x200
#define MCIStatus_StartBitErr 0x200
#define MCIStatus_StartBitErr_BIT 9
#define MCIStatus_DataBlockEnd_MASK 0x400
#define MCIStatus_DataBlockEnd 0x400
#define MCIStatus_DataBlockEnd_BIT 10
#define MCIStatus_CmdActive_MASK 0x800
#define MCIStatus_CmdActive 0x800
#define MCIStatus_CmdActive_BIT 11
#define MCIStatus_TxActive_MASK 0x1000
#define MCIStatus_TxActive 0x1000
#define MCIStatus_TxActive_BIT 12
#define MCIStatus_RxActive_MASK 0x2000
#define MCIStatus_RxActive 0x2000
#define MCIStatus_RxActive_BIT 13
#define MCIStatus_TxFifoHalfEmpty_MASK 0x4000
#define MCIStatus_TxFifoHalfEmpty 0x4000
#define MCIStatus_TxFifoHalfEmpty_BIT 14
#define MCIStatus_RxFifoHalfEmpty_MASK 0x8000
#define MCIStatus_RxFifoHalfEmpty 0x8000
#define MCIStatus_RxFifoHalfEmpty_BIT 15
#define MCIStatus_TxFifoFull_MASK 0x10000
#define MCIStatus_TxFifoFull 0x10000
#define MCIStatus_TxFifoFull_BIT 16
#define MCIStatus_RxFifoFull_MASK 0x20000
#define MCIStatus_RxFifoFull 0x20000
#define MCIStatus_RxFifoFull_BIT 17
#define MCIStatus_TxFifoEmpty_MASK 0x40000
#define MCIStatus_TxFifoEmpty 0x40000
#define MCIStatus_TxFifoEmpty_BIT 18
#define MCIStatus_RxFifoEmpty_MASK 0x80000
#define MCIStatus_RxFifoEmpty 0x80000
#define MCIStatus_RxFifoEmpty_BIT 19
#define MCIStatus_TxDataAvlbl_MASK 0x100000
#define MCIStatus_TxDataAvlbl 0x100000
#define MCIStatus_TxDataAvlbl_BIT 20
#define MCIStatus_RxDataAvlbl_MASK 0x200000
#define MCIStatus_RxDataAvlbl 0x200000
#define MCIStatus_RxDataAvlbl_BIT 21

#define MCIClear (*(volatile unsigned long *)0xE008C038)
#define MCIClear_OFFSET 0x38
#define MCIClear_CmdCrcFailClr_MASK 0x1
#define MCIClear_CmdCrcFailClr 0x1
#define MCIClear_CmdCrcFailClr_BIT 0
#define MCIClear_DataCrcFailClr_MASK 0x2
#define MCIClear_DataCrcFailClr 0x2
#define MCIClear_DataCrcFailClr_BIT 1
#define MCIClear_CmdTimeOutClr_MASK 0x4
#define MCIClear_CmdTimeOutClr 0x4
#define MCIClear_CmdTimeOutClr_BIT 2
#define MCIClear_DataTimeOutClr_MASK 0x8
#define MCIClear_DataTimeOutClr 0x8
#define MCIClear_DataTimeOutClr_BIT 3
#define MCIClear_TxUnderrunClr_MASK 0x10
#define MCIClear_TxUnderrunClr 0x10
#define MCIClear_TxUnderrunClr_BIT 4
#define MCIClear_RxOverrunClr_MASK 0x20
#define MCIClear_RxOverrunClr 0x20
#define MCIClear_RxOverrunClr_BIT 5
#define MCIClear_CmdRespEndClr_MASK 0x40
#define MCIClear_CmdRespEndClr 0x40
#define MCIClear_CmdRespEndClr_BIT 6
#define MCIClear_CmdSentClr_MASK 0x80
#define MCIClear_CmdSentClr 0x80
#define MCIClear_CmdSentClr_BIT 7
#define MCIClear_DataEndClr_MASK 0x100
#define MCIClear_DataEndClr 0x100
#define MCIClear_DataEndClr_BIT 8
#define MCIClear_StartBitErrClr_MASK 0x200
#define MCIClear_StartBitErrClr 0x200
#define MCIClear_StartBitErrClr_BIT 9
#define MCIClear_DataBlockEndClr_MASK 0x400
#define MCIClear_DataBlockEndClr 0x400
#define MCIClear_DataBlockEndClr_BIT 10

#define MCIMask0 (*(volatile unsigned long *)0xE008C03C)
#define MCIMask0_OFFSET 0x3C

#define MCIMask1 (*(volatile unsigned long *)0xE008C040)
#define MCIMask1_OFFSET 0x40

#define MCIFifoCnt (*(volatile unsigned long *)0xE008C048)
#define MCIFifoCnt_OFFSET 0x48
#define MCIFifoCnt_DataCount_MASK 0x7FFF
#define MCIFifoCnt_DataCount_BIT 0

#define MCIFIFO (*(volatile unsigned long *)0xE008C080)
#define MCIFIFO_OFFSET 0x80

#define SCB_BASE 0xE01FC000

#define MAMCR (*(volatile unsigned char *)0xE01FC000)
#define MAMCR_OFFSET 0x0
#define MAMCR_MAM_mode_control_MASK 0x3
#define MAMCR_MAM_mode_control_BIT 0

#define MAMTIM (*(volatile unsigned char *)0xE01FC004)
#define MAMTIM_OFFSET 0x4
#define MAMTIM_MAM_fetch_cycle_timing_MASK 0x7
#define MAMTIM_MAM_fetch_cycle_timing_BIT 0

#define MEMMAP (*(volatile unsigned char *)0xE01FC040)
#define MEMMAP_OFFSET 0x40
#define MEMMAP_MAP_MASK 0x3
#define MEMMAP_MAP_BIT 0

#define PLLCON (*(volatile unsigned char *)0xE01FC080)
#define PLLCON_OFFSET 0x80
#define PLLCON_PLLE_MASK 0x1
#define PLLCON_PLLE 0x1
#define PLLCON_PLLE_BIT 0
#define PLLCON_PLLC_MASK 0x2
#define PLLCON_PLLC 0x2
#define PLLCON_PLLC_BIT 1

#define PLLCFG (*(volatile unsigned long *)0xE01FC084)
#define PLLCFG_OFFSET 0x84
#define PLLCFG_MSEL_MASK 0x7FFF
#define PLLCFG_MSEL_BIT 0
#define PLLCFG_NSEL_MASK 0xFF0000
#define PLLCFG_NSEL_BIT 16

#define PLLSTAT (*(volatile unsigned long *)0xE01FC088)
#define PLLSTAT_OFFSET 0x88
#define PLLSTAT_MSEL_MASK 0x7FFF
#define PLLSTAT_MSEL_BIT 0
#define PLLSTAT_NSEL_MASK 0xFF0000
#define PLLSTAT_NSEL_BIT 16
#define PLLSTAT_PLLE_MASK 0x1000000
#define PLLSTAT_PLLE 0x1000000
#define PLLSTAT_PLLE_BIT 24
#define PLLSTAT_PLLC_MASK 0x2000000
#define PLLSTAT_PLLC 0x2000000
#define PLLSTAT_PLLC_BIT 25
#define PLLSTAT_PLOCK_MASK 0x4000000
#define PLLSTAT_PLOCK 0x4000000
#define PLLSTAT_PLOCK_BIT 26

#define PLLFEED (*(volatile unsigned char *)0xE01FC08C)
#define PLLFEED_OFFSET 0x8C

#define PCON (*(volatile unsigned char *)0xE01FC0C0)
#define PCON_OFFSET 0xC0
#define PCON_IDL_MASK 0x1
#define PCON_IDL 0x1
#define PCON_IDL_BIT 0
#define PCON_PD_MASK 0x2
#define PCON_PD 0x2
#define PCON_PD_BIT 1
#define PCON_BODPDM_MASK 0x4
#define PCON_BODPDM 0x4
#define PCON_BODPDM_BIT 2
#define PCON_BOGD_MASK 0x8
#define PCON_BOGD 0x8
#define PCON_BOGD_BIT 3
#define PCON_BORD_MASK 0x10
#define PCON_BORD 0x10
#define PCON_BORD_BIT 4
#define PCON_PM2_MASK 0x80
#define PCON_PM2 0x80
#define PCON_PM2_BIT 7

#define PCONP (*(volatile unsigned long *)0xE01FC0C4)
#define PCONP_OFFSET 0xC4
#define PCONP_PCTIM0_MASK 0x2
#define PCONP_PCTIM0 0x2
#define PCONP_PCTIM0_BIT 1
#define PCONP_PCTIM1_MASK 0x4
#define PCONP_PCTIM1 0x4
#define PCONP_PCTIM1_BIT 2
#define PCONP_PCUART0_MASK 0x8
#define PCONP_PCUART0 0x8
#define PCONP_PCUART0_BIT 3
#define PCONP_PCUART1_MASK 0x10
#define PCONP_PCUART1 0x10
#define PCONP_PCUART1_BIT 4
#define PCONP_PCPWM0_MASK 0x20
#define PCONP_PCPWM0 0x20
#define PCONP_PCPWM0_BIT 5
#define PCONP_PCPWM1_MASK 0x40
#define PCONP_PCPWM1 0x40
#define PCONP_PCPWM1_BIT 6
#define PCONP_PCI2C0_MASK 0x80
#define PCONP_PCI2C0 0x80
#define PCONP_PCI2C0_BIT 7
#define PCONP_PCSPI_MASK 0x100
#define PCONP_PCSPI 0x100
#define PCONP_PCSPI_BIT 8
#define PCONP_PCRTC_MASK 0x200
#define PCONP_PCRTC 0x200
#define PCONP_PCRTC_BIT 9
#define PCONP_PCSSP1_MASK 0x400
#define PCONP_PCSSP1 0x400
#define PCONP_PCSSP1_BIT 10
#define PCONP_PCEMC_MASK 0x800
#define PCONP_PCEMC 0x800
#define PCONP_PCEMC_BIT 11
#define PCONP_PCAD_MASK 0x1000
#define PCONP_PCAD 0x1000
#define PCONP_PCAD_BIT 12
#define PCONP_PCAN1_MASK 0x2000
#define PCONP_PCAN1 0x2000
#define PCONP_PCAN1_BIT 13
#define PCONP_PCAN2_MASK 0x4000
#define PCONP_PCAN2 0x4000
#define PCONP_PCAN2_BIT 14
#define PCONP_PCI2C1_MASK 0x80000
#define PCONP_PCI2C1 0x80000
#define PCONP_PCI2C1_BIT 19
#define PCONP_PCLCD_MASK 0x100000
#define PCONP_PCLCD 0x100000
#define PCONP_PCLCD_BIT 20
#define PCONP_PCSSP0_MASK 0x200000
#define PCONP_PCSSP0 0x200000
#define PCONP_PCSSP0_BIT 21
#define PCONP_PCTIM2_MASK 0x400000
#define PCONP_PCTIM2 0x400000
#define PCONP_PCTIM2_BIT 22
#define PCONP_PCTIM3_MASK 0x800000
#define PCONP_PCTIM3 0x800000
#define PCONP_PCTIM3_BIT 23
#define PCONP_PCUART2_MASK 0x1000000
#define PCONP_PCUART2 0x1000000
#define PCONP_PCUART2_BIT 24
#define PCONP_PCUART3_MASK 0x2000000
#define PCONP_PCUART3 0x2000000
#define PCONP_PCUART3_BIT 25
#define PCONP_PCI2C2_MASK 0x4000000
#define PCONP_PCI2C2 0x4000000
#define PCONP_PCI2C2_BIT 26
#define PCONP_PCI2CS_MASK 0x8000000
#define PCONP_PCI2CS 0x8000000
#define PCONP_PCI2CS_BIT 27
#define PCONP_PCSDC_MASK 0x10000000
#define PCONP_PCSDC 0x10000000
#define PCONP_PCSDC_BIT 28
#define PCONP_PCGPDMA_MASK 0x20000000
#define PCONP_PCGPDMA 0x20000000
#define PCONP_PCGPDMA_BIT 29
#define PCONP_PCENET_MASK 0x40000000
#define PCONP_PCENET 0x40000000
#define PCONP_PCENET_BIT 30
#define PCONP_PUSB_MASK 0x80000000
#define PCONP_PUSB 0x80000000
#define PCONP_PUSB_BIT 31

#define CCLKCFG (*(volatile unsigned long *)0xE01FC104)
#define CCLKCFG_OFFSET 0x104
#define CCLKCFG_CCLKSEL_MASK 0xFF
#define CCLKCFG_CCLKSEL_BIT 0

#define USBCLKCFG (*(volatile unsigned long *)0xE01FC108)
#define USBCLKCFG_OFFSET 0x108
#define USBCLKCFG_USBSEL_MASK 0xF
#define USBCLKCFG_USBSEL_BIT 0

#define CLKSRCSEL (*(volatile unsigned char *)0xE01FC10C)
#define CLKSRCSEL_OFFSET 0x10C
#define CLKSRCSEL_CLKSRC_MASK 0x3
#define CLKSRCSEL_CLKSRC_BIT 0

#define EXTINT (*(volatile unsigned char *)0xE01FC140)
#define EXTINT_OFFSET 0x140
#define EXTINT_EINT0_MASK 0x1
#define EXTINT_EINT0 0x1
#define EXTINT_EINT0_BIT 0
#define EXTINT_EINT1_MASK 0x2
#define EXTINT_EINT1 0x2
#define EXTINT_EINT1_BIT 1
#define EXTINT_EINT2_MASK 0x4
#define EXTINT_EINT2 0x4
#define EXTINT_EINT2_BIT 2
#define EXTINT_EINT3_MASK 0x8
#define EXTINT_EINT3 0x8
#define EXTINT_EINT3_BIT 3

#define INTWAKE (*(volatile unsigned short *)0xE01FC144)
#define INTWAKE_OFFSET 0x144
#define INTWAKE_EXTWAKE0_MASK 0x1
#define INTWAKE_EXTWAKE0 0x1
#define INTWAKE_EXTWAKE0_BIT 0
#define INTWAKE_EXTWAKE1_MASK 0x2
#define INTWAKE_EXTWAKE1 0x2
#define INTWAKE_EXTWAKE1_BIT 1
#define INTWAKE_EXTWAKE2_MASK 0x4
#define INTWAKE_EXTWAKE2 0x4
#define INTWAKE_EXTWAKE2_BIT 2
#define INTWAKE_EXTWAKE3_MASK 0x8
#define INTWAKE_EXTWAKE3 0x8
#define INTWAKE_EXTWAKE3_BIT 3
#define INTWAKE_ETHWAKE_MASK 0x10
#define INTWAKE_ETHWAKE 0x10
#define INTWAKE_ETHWAKE_BIT 4
#define INTWAKE_USBWAKE_MASK 0x20
#define INTWAKE_USBWAKE 0x20
#define INTWAKE_USBWAKE_BIT 5
#define INTWAKE_CANWAKE_MASK 0x40
#define INTWAKE_CANWAKE 0x40
#define INTWAKE_CANWAKE_BIT 6
#define INTWAKE_GPIOWAKE_MASK 0x80
#define INTWAKE_GPIOWAKE 0x80
#define INTWAKE_GPIOWAKE_BIT 7
#define INTWAKE_BODWAKE_MASK 0x4000
#define INTWAKE_BODWAKE 0x4000
#define INTWAKE_BODWAKE_BIT 14
#define INTWAKE_RTCWAKE_MASK 0x8000
#define INTWAKE_RTCWAKE 0x8000
#define INTWAKE_RTCWAKE_BIT 15

#define EXTMODE (*(volatile unsigned char *)0xE01FC148)
#define EXTMODE_OFFSET 0x148
#define EXTMODE_EXTMODE0_MASK 0x1
#define EXTMODE_EXTMODE0 0x1
#define EXTMODE_EXTMODE0_BIT 0
#define EXTMODE_EXTMODE1_MASK 0x2
#define EXTMODE_EXTMODE1 0x2
#define EXTMODE_EXTMODE1_BIT 1
#define EXTMODE_EXTMODE2_MASK 0x4
#define EXTMODE_EXTMODE2 0x4
#define EXTMODE_EXTMODE2_BIT 2
#define EXTMODE_EXTMODE3_MASK 0x8
#define EXTMODE_EXTMODE3 0x8
#define EXTMODE_EXTMODE3_BIT 3

#define EXTPOLAR (*(volatile unsigned char *)0xE01FC14C)
#define EXTPOLAR_OFFSET 0x14C
#define EXTPOLAR_EXTPOLAR0_MASK 0x1
#define EXTPOLAR_EXTPOLAR0 0x1
#define EXTPOLAR_EXTPOLAR0_BIT 0
#define EXTPOLAR_EXTPOLAR1_MASK 0x2
#define EXTPOLAR_EXTPOLAR1 0x2
#define EXTPOLAR_EXTPOLAR1_BIT 1
#define EXTPOLAR_EXTPOLAR2_MASK 0x4
#define EXTPOLAR_EXTPOLAR2 0x4
#define EXTPOLAR_EXTPOLAR2_BIT 2
#define EXTPOLAR_EXTPOLAR3_MASK 0x8
#define EXTPOLAR_EXTPOLAR3 0x8
#define EXTPOLAR_EXTPOLAR3_BIT 3

#define RSID (*(volatile unsigned char *)0xE01FC180)
#define RSID_OFFSET 0x180
#define RSID_POR_MASK 0x1
#define RSID_POR 0x1
#define RSID_POR_BIT 0
#define RSID_EXTR_MASK 0x2
#define RSID_EXTR 0x2
#define RSID_EXTR_BIT 1
#define RSID_WDTR_MASK 0x4
#define RSID_WDTR 0x4
#define RSID_WDTR_BIT 2
#define RSID_BODR_MASK 0x8
#define RSID_BODR 0x8
#define RSID_BODR_BIT 3

#define CSPR (*(volatile unsigned long *)0xE01FC184)
#define CSPR_OFFSET 0x184

#define AHBCFG1 (*(volatile unsigned long *)0xE01FC188)
#define AHBCFG1_OFFSET 0x188
#define AHBCFG1_scheduler_MASK 0x1
#define AHBCFG1_scheduler 0x1
#define AHBCFG1_scheduler_BIT 0
#define AHBCFG1_break_burst_MASK 0x6
#define AHBCFG1_break_burst_BIT 1
#define AHBCFG1_quantum_type_MASK 0x8
#define AHBCFG1_quantum_type 0x8
#define AHBCFG1_quantum_type_BIT 3
#define AHBCFG1_quantum_size_MASK 0xF0
#define AHBCFG1_quantum_size_BIT 4
#define AHBCFG1_default_master_MASK 0xF00
#define AHBCFG1_default_master_BIT 8
#define AHBCFG1_EP1_MASK 0xF000
#define AHBCFG1_EP1_BIT 12
#define AHBCFG1_EP2_MASK 0xF0000
#define AHBCFG1_EP2_BIT 16
#define AHBCFG1_EP3_MASK 0xF00000
#define AHBCFG1_EP3_BIT 20
#define AHBCFG1_EP4_MASK 0xF000000
#define AHBCFG1_EP4_BIT 24

#define AHBCFG2 (*(volatile unsigned long *)0xE01FC18C)
#define AHBCFG2_OFFSET 0x18C
#define AHBCFG2_scheduler_MASK 0x1
#define AHBCFG2_scheduler 0x1
#define AHBCFG2_scheduler_BIT 0
#define AHBCFG2_break_burst_MASK 0x6
#define AHBCFG2_break_burst_BIT 1
#define AHBCFG2_quantum_type_MASK 0x8
#define AHBCFG2_quantum_type 0x8
#define AHBCFG2_quantum_type_BIT 3
#define AHBCFG2_quantum_size_MASK 0xF0
#define AHBCFG2_quantum_size_BIT 4
#define AHBCFG2_default_master_MASK 0xF00
#define AHBCFG2_default_master_BIT 8
#define AHBCFG2_EP1_MASK 0xF000
#define AHBCFG2_EP1_BIT 12
#define AHBCFG2_EP2_MASK 0xF0000
#define AHBCFG2_EP2_BIT 16

#define SCS (*(volatile unsigned long *)0xE01FC1A0)
#define SCS_OFFSET 0x1A0
#define SCS_GPIOM_MASK 0x1
#define SCS_GPIOM 0x1
#define SCS_GPIOM_BIT 0
#define SCS_EMC_Reset_Disable_MASK 0x2
#define SCS_EMC_Reset_Disable 0x2
#define SCS_EMC_Reset_Disable_BIT 1
#define SCS_MCIPWR_Active_Level_MASK 0x8
#define SCS_MCIPWR_Active_Level 0x8
#define SCS_MCIPWR_Active_Level_BIT 3
#define SCS_OSCRANGE_MASK 0x10
#define SCS_OSCRANGE 0x10
#define SCS_OSCRANGE_BIT 4
#define SCS_OSCEN_MASK 0x20
#define SCS_OSCEN 0x20
#define SCS_OSCEN_BIT 5
#define SCS_OSCSTAT_MASK 0x40
#define SCS_OSCSTAT 0x40
#define SCS_OSCSTAT_BIT 6

#define IRCTRIM (*(volatile unsigned short *)0xE01FC1A4)
#define IRCTRIM_OFFSET 0x1A4
#define IRCTRIM_IRCtrim_MASK 0xFF
#define IRCTRIM_IRCtrim_BIT 0

#define PCLKSEL0 (*(volatile unsigned long *)0xE01FC1A8)
#define PCLKSEL0_OFFSET 0x1A8
#define PCLKSEL0_PCLK_WDT_MASK 0x3
#define PCLKSEL0_PCLK_WDT_BIT 0
#define PCLKSEL0_PCLK_TIMER0_MASK 0xC
#define PCLKSEL0_PCLK_TIMER0_BIT 2
#define PCLKSEL0_PCLK_TIMER1_MASK 0x30
#define PCLKSEL0_PCLK_TIMER1_BIT 4
#define PCLKSEL0_PCLK_UART0_MASK 0xC0
#define PCLKSEL0_PCLK_UART0_BIT 6
#define PCLKSEL0_PCLK_UART1_MASK 0x300
#define PCLKSEL0_PCLK_UART1_BIT 8
#define PCLKSEL0_PCLK_PWM0_MASK 0xC00
#define PCLKSEL0_PCLK_PWM0_BIT 10
#define PCLKSEL0_PCLK_PWM1_MASK 0x3000
#define PCLKSEL0_PCLK_PWM1_BIT 12
#define PCLKSEL0_PCLK_I2C0_MASK 0xC000
#define PCLKSEL0_PCLK_I2C0_BIT 14
#define PCLKSEL0_PCLK_SPI_MASK 0x30000
#define PCLKSEL0_PCLK_SPI_BIT 16
#define PCLKSEL0_PCLK_RTC_MASK 0xC0000
#define PCLKSEL0_PCLK_RTC_BIT 18
#define PCLKSEL0_PCLK_SSP1_MASK 0x300000
#define PCLKSEL0_PCLK_SSP1_BIT 20
#define PCLKSEL0_PCLK_DAC_MASK 0xC00000
#define PCLKSEL0_PCLK_DAC_BIT 22
#define PCLKSEL0_PCLK_ADC_MASK 0x3000000
#define PCLKSEL0_PCLK_ADC_BIT 24
#define PCLKSEL0_PCLK_CAN1_MASK 0xC000000
#define PCLKSEL0_PCLK_CAN1_BIT 26
#define PCLKSEL0_PCLK_CAN2_MASK 0x30000000
#define PCLKSEL0_PCLK_CAN2_BIT 28
#define PCLKSEL0_PCLK_ACF_MASK 0xC0000000
#define PCLKSEL0_PCLK_ACF_BIT 30

#define PCLKSEL1 (*(volatile unsigned long *)0xE01FC1AC)
#define PCLKSEL1_OFFSET 0x1AC
#define PCLKSEL1_PCLK_BAT_RAM_MASK 0x3
#define PCLKSEL1_PCLK_BAT_RAM_BIT 0
#define PCLKSEL1_PCLK_GPIO_MASK 0xC
#define PCLKSEL1_PCLK_GPIO_BIT 2
#define PCLKSEL1_PCLK_PCB_MASK 0x30
#define PCLKSEL1_PCLK_PCB_BIT 4
#define PCLKSEL1_PCLK_I2C1_MASK 0xC0
#define PCLKSEL1_PCLK_I2C1_BIT 6
#define PCLKSEL1_PCLK_SSP0_MASK 0xC00
#define PCLKSEL1_PCLK_SSP0_BIT 10
#define PCLKSEL1_PCLK_TIMER2_MASK 0x3000
#define PCLKSEL1_PCLK_TIMER2_BIT 12
#define PCLKSEL1_PCLK_TIMER3_MASK 0xC000
#define PCLKSEL1_PCLK_TIMER3_BIT 14
#define PCLKSEL1_PCLK_UART2_MASK 0x30000
#define PCLKSEL1_PCLK_UART2_BIT 16
#define PCLKSEL1_PCLK_UART3_MASK 0xC0000
#define PCLKSEL1_PCLK_UART3_BIT 18
#define PCLKSEL1_PCLK_I2C2_MASK 0x300000
#define PCLKSEL1_PCLK_I2C2_BIT 20
#define PCLKSEL1_PCLK_I2S_MASK 0xC00000
#define PCLKSEL1_PCLK_I2S_BIT 22
#define PCLKSEL1_PCLK_MCI_MASK 0x3000000
#define PCLKSEL1_PCLK_MCI_BIT 24
#define PCLKSEL1_PCLK_SYSCON_MASK 0x30000000
#define PCLKSEL1_PCLK_SYSCON_BIT 28

#define LCD_CFG (*(volatile unsigned long *)0xE01FC1B8)
#define LCD_CFG_OFFSET 0x1B8
#define LCD_CFG_CLKDIV_MASK 0x1F
#define LCD_CFG_CLKDIV_BIT 0

#define USBIntSt (*(volatile unsigned long *)0xE01FC1C0)
#define USBIntSt_OFFSET 0x1C0
#define USBIntSt_USB_INT_REQ_LP_MASK 0x1
#define USBIntSt_USB_INT_REQ_LP 0x1
#define USBIntSt_USB_INT_REQ_LP_BIT 0
#define USBIntSt_USB_INT_REQ_HP_MASK 0x2
#define USBIntSt_USB_INT_REQ_HP 0x2
#define USBIntSt_USB_INT_REQ_HP_BIT 1
#define USBIntSt_USB_INT_REQ_DMA_MASK 0x4
#define USBIntSt_USB_INT_REQ_DMA 0x4
#define USBIntSt_USB_INT_REQ_DMA_BIT 2
#define USBIntSt_USB_HOST_INT_MASK 0x8
#define USBIntSt_USB_HOST_INT 0x8
#define USBIntSt_USB_HOST_INT_BIT 3
#define USBIntSt_USB_ATX_INT_MASK 0x10
#define USBIntSt_USB_ATX_INT 0x10
#define USBIntSt_USB_ATX_INT_BIT 4
#define USBIntSt_USB_OTG_INT_MASK 0x20
#define USBIntSt_USB_OTG_INT 0x20
#define USBIntSt_USB_OTG_INT_BIT 5
#define USBIntSt_USB_I2C_INT_MASK 0x40
#define USBIntSt_USB_I2C_INT 0x40
#define USBIntSt_USB_I2C_INT_BIT 6
#define USBIntSt_USB_NEED_CLK_MASK 0x100
#define USBIntSt_USB_NEED_CLK 0x100
#define USBIntSt_USB_NEED_CLK_BIT 8
#define USBIntSt_EN_USB_INTS_MASK 0x80000000
#define USBIntSt_EN_USB_INTS 0x80000000
#define USBIntSt_EN_USB_INTS_BIT 31

#define MAC_BASE 0xFFE00000

#define MAC1 (*(volatile unsigned *)0xFFE00000)
#define MAC1_OFFSET 0x0
#define MAC1_RECEIVE_ENABLE_MASK 0x1
#define MAC1_RECEIVE_ENABLE 0x1
#define MAC1_RECEIVE_ENABLE_BIT 0
#define MAC1_PASS_ALL_RECEIVE_FRAMES_MASK 0x2
#define MAC1_PASS_ALL_RECEIVE_FRAMES 0x2
#define MAC1_PASS_ALL_RECEIVE_FRAMES_BIT 1
#define MAC1_RX_FLOW_CONTROL_MASK 0x4
#define MAC1_RX_FLOW_CONTROL 0x4
#define MAC1_RX_FLOW_CONTROL_BIT 2
#define MAC1_TX_FLOW_CONTROL_MASK 0x8
#define MAC1_TX_FLOW_CONTROL 0x8
#define MAC1_TX_FLOW_CONTROL_BIT 3
#define MAC1_LOOPBACK_MASK 0x10
#define MAC1_LOOPBACK 0x10
#define MAC1_LOOPBACK_BIT 4
#define MAC1_RESET_TX_MASK 0x100
#define MAC1_RESET_TX 0x100
#define MAC1_RESET_TX_BIT 8
#define MAC1_RESET_MCS_TX_MASK 0x200
#define MAC1_RESET_MCS_TX 0x200
#define MAC1_RESET_MCS_TX_BIT 9
#define MAC1_RESET_RX_MASK 0x400
#define MAC1_RESET_RX 0x400
#define MAC1_RESET_RX_BIT 10
#define MAC1_RESET_MCS_RX_MASK 0x800
#define MAC1_RESET_MCS_RX 0x800
#define MAC1_RESET_MCS_RX_BIT 11
#define MAC1_SIMULATION_RESET_MASK 0x4000
#define MAC1_SIMULATION_RESET 0x4000
#define MAC1_SIMULATION_RESET_BIT 14
#define MAC1_SOFT_RESET_MASK 0x8000
#define MAC1_SOFT_RESET 0x8000
#define MAC1_SOFT_RESET_BIT 15

#define MAC2 (*(volatile unsigned *)0xFFE00004)
#define MAC2_OFFSET 0x4
#define MAC2_FULL_DUPLEX_MASK 0x1
#define MAC2_FULL_DUPLEX 0x1
#define MAC2_FULL_DUPLEX_BIT 0
#define MAC2_FRAME_LENGTH_CHECKING_MASK 0x2
#define MAC2_FRAME_LENGTH_CHECKING 0x2
#define MAC2_FRAME_LENGTH_CHECKING_BIT 1
#define MAC2_HUGE_FRAME_ENABLE_MASK 0x4
#define MAC2_HUGE_FRAME_ENABLE 0x4
#define MAC2_HUGE_FRAME_ENABLE_BIT 2
#define MAC2_DELAYED_CRC_MASK 0x8
#define MAC2_DELAYED_CRC 0x8
#define MAC2_DELAYED_CRC_BIT 3
#define MAC2_CRC_ENABLE_MASK 0x10
#define MAC2_CRC_ENABLE 0x10
#define MAC2_CRC_ENABLE_BIT 4
#define MAC2_PAD_CRC_ENABLE_MASK 0x20
#define MAC2_PAD_CRC_ENABLE 0x20
#define MAC2_PAD_CRC_ENABLE_BIT 5
#define MAC2_VLAN_PAD_ENABLE_MASK 0x40
#define MAC2_VLAN_PAD_ENABLE 0x40
#define MAC2_VLAN_PAD_ENABLE_BIT 6
#define MAC2_AUTO_DETECT_PAD_ENABLE_MASK 0x80
#define MAC2_AUTO_DETECT_PAD_ENABLE 0x80
#define MAC2_AUTO_DETECT_PAD_ENABLE_BIT 7
#define MAC2_PURE_PREAMBLE_ENFORCEMENT_MASK 0x100
#define MAC2_PURE_PREAMBLE_ENFORCEMENT 0x100
#define MAC2_PURE_PREAMBLE_ENFORCEMENT_BIT 8
#define MAC2_LONG_PREAMBLE_ENFORCEMENT_MASK 0x200
#define MAC2_LONG_PREAMBLE_ENFORCEMENT 0x200
#define MAC2_LONG_PREAMBLE_ENFORCEMENT_BIT 9
#define MAC2_NO_BACKOFF_MASK 0x1000
#define MAC2_NO_BACKOFF 0x1000
#define MAC2_NO_BACKOFF_BIT 12
#define MAC2_BACK_PRESSURE_NO_BACKOFF_MASK 0x2000
#define MAC2_BACK_PRESSURE_NO_BACKOFF 0x2000
#define MAC2_BACK_PRESSURE_NO_BACKOFF_BIT 13
#define MAC2_EXCESS_DEFER_MASK 0x4000
#define MAC2_EXCESS_DEFER 0x4000
#define MAC2_EXCESS_DEFER_BIT 14

#define IPGT (*(volatile unsigned *)0xFFE00008)
#define IPGT_OFFSET 0x8
#define IPGT_BACK_TO_BACK_INTER_PACKET_GAP_MASK 0x7F
#define IPGT_BACK_TO_BACK_INTER_PACKET_GAP_BIT 0

#define IPGR (*(volatile unsigned *)0xFFE0000C)
#define IPGR_OFFSET 0xC
#define IPGR_NON_BACK_TO_BACK_INTER_PACKET_GAP_PART2_MASK 0x7F
#define IPGR_NON_BACK_TO_BACK_INTER_PACKET_GAP_PART2_BIT 0
#define IPGR_NON_BACK_TO_BACK_INTER_PACKET_GAP_PART1_MASK 0x7F00
#define IPGR_NON_BACK_TO_BACK_INTER_PACKET_GAP_PART1_BIT 8

#define CLRT (*(volatile unsigned *)0xFFE00010)
#define CLRT_OFFSET 0x10
#define CLRT_RETRANSMISSION_MAXIMUM_MASK 0xF
#define CLRT_RETRANSMISSION_MAXIMUM_BIT 0
#define CLRT_COLLISION_WINDOW_MASK 0x3F00
#define CLRT_COLLISION_WINDOW_BIT 8

#define MAXF (*(volatile unsigned *)0xFFE00014)
#define MAXF_OFFSET 0x14
#define MAXF_MAXIMUM_FRAME_LENGTH_MASK 0xFFFF
#define MAXF_MAXIMUM_FRAME_LENGTH_BIT 0

#define SUPP (*(volatile unsigned *)0xFFE00018)
#define SUPP_OFFSET 0x18
#define SUPP_SPEED_MASK 0x100
#define SUPP_SPEED 0x100
#define SUPP_SPEED_BIT 8

#define TEST (*(volatile unsigned *)0xFFE0001C)
#define TEST_OFFSET 0x1C
#define TEST_SHORTCUT_PAUSE_QUANTA_MASK 0x1
#define TEST_SHORTCUT_PAUSE_QUANTA 0x1
#define TEST_SHORTCUT_PAUSE_QUANTA_BIT 0
#define TEST_TEST_PAUSE_MASK 0x2
#define TEST_TEST_PAUSE 0x2
#define TEST_TEST_PAUSE_BIT 1
#define TEST_TEST_BACKPRESSURE_MASK 0x4
#define TEST_TEST_BACKPRESSURE 0x4
#define TEST_TEST_BACKPRESSURE_BIT 2

#define MCFG (*(volatile unsigned *)0xFFE00020)
#define MCFG_OFFSET 0x20
#define MCFG_SCAN_INCREMENT_MASK 0x1
#define MCFG_SCAN_INCREMENT 0x1
#define MCFG_SCAN_INCREMENT_BIT 0
#define MCFG_SUPPRESS_PREAMBLE_MASK 0x2
#define MCFG_SUPPRESS_PREAMBLE 0x2
#define MCFG_SUPPRESS_PREAMBLE_BIT 1
#define MCFG_CLOCK_SELECT_MASK 0x1C
#define MCFG_CLOCK_SELECT_BIT 2
#define MCFG_RESET_MII_MGMT_MASK 0x8000
#define MCFG_RESET_MII_MGMT 0x8000
#define MCFG_RESET_MII_MGMT_BIT 15

#define MCMD (*(volatile unsigned *)0xFFE00024)
#define MCMD_OFFSET 0x24
#define MCMD_READ_MASK 0x1
#define MCMD_READ 0x1
#define MCMD_READ_BIT 0
#define MCMD_SCAN_MASK 0x2
#define MCMD_SCAN 0x2
#define MCMD_SCAN_BIT 1

#define MADR (*(volatile unsigned *)0xFFE00028)
#define MADR_OFFSET 0x28
#define MADR_REGISTER_ADDRESS_MASK 0x1F
#define MADR_REGISTER_ADDRESS_BIT 0
#define MADR_PHY_ADDRESS_MASK 0x1F00
#define MADR_PHY_ADDRESS_BIT 8

#define MWTD (*(volatile unsigned *)0xFFE0002C)
#define MWTD_OFFSET 0x2C
#define MWTD_WRITE_DATA_MASK 0xFFFF
#define MWTD_WRITE_DATA_BIT 0

#define MRDD (*(volatile unsigned *)0xFFE00030)
#define MRDD_OFFSET 0x30
#define MRDD_WRITE_DATA_MASK 0xFFFF
#define MRDD_WRITE_DATA_BIT 0

#define MIND (*(volatile unsigned *)0xFFE00034)
#define MIND_OFFSET 0x34
#define MIND_BUSY_MASK 0x1
#define MIND_BUSY 0x1
#define MIND_BUSY_BIT 0
#define MIND_SCANNING_MASK 0x2
#define MIND_SCANNING 0x2
#define MIND_SCANNING_BIT 1
#define MIND_NOT_VALID_MASK 0x4
#define MIND_NOT_VALID 0x4
#define MIND_NOT_VALID_BIT 2
#define MIND_MII_Link_Fail_MASK 0x8
#define MIND_MII_Link_Fail 0x8
#define MIND_MII_Link_Fail_BIT 3

#define SA0 (*(volatile unsigned *)0xFFE00040)
#define SA0_OFFSET 0x40
#define SA0_STATION_ADDRESS_2nd_octet_MASK 0xFF
#define SA0_STATION_ADDRESS_2nd_octet_BIT 0
#define SA0_STATION_ADDRESS_1st_octet_MASK 0xFF00
#define SA0_STATION_ADDRESS_1st_octet_BIT 8

#define SA1 (*(volatile unsigned *)0xFFE00044)
#define SA1_OFFSET 0x44
#define SA1_STATION_ADDRESS_4th_octet_MASK 0xFF
#define SA1_STATION_ADDRESS_4th_octet_BIT 0
#define SA1_STATION_ADDRESS_3rd_octet_MASK 0xFF00
#define SA1_STATION_ADDRESS_3rd_octet_BIT 8

#define SA2 (*(volatile unsigned *)0xFFE00048)
#define SA2_OFFSET 0x48
#define SA2_STATION_ADDRESS_6th_octet_MASK 0xFF
#define SA2_STATION_ADDRESS_6th_octet_BIT 0
#define SA2_STATION_ADDRESS_5th_octet_MASK 0xFF00
#define SA2_STATION_ADDRESS_5th_octet_BIT 8

#define Command (*(volatile unsigned *)0xFFE00100)
#define Command_OFFSET 0x100
#define Command_RxEnable_MASK 0x1
#define Command_RxEnable 0x1
#define Command_RxEnable_BIT 0
#define Command_TxEnable_MASK 0x2
#define Command_TxEnable 0x2
#define Command_TxEnable_BIT 1
#define Command_RegReset_MASK 0x8
#define Command_RegReset 0x8
#define Command_RegReset_BIT 3
#define Command_TxReset_MASK 0x10
#define Command_TxReset 0x10
#define Command_TxReset_BIT 4
#define Command_RxReset_MASK 0x20
#define Command_RxReset 0x20
#define Command_RxReset_BIT 5
#define Command_PassRuntFrame_MASK 0x40
#define Command_PassRuntFrame 0x40
#define Command_PassRuntFrame_BIT 6
#define Command_PassRxFilter_MASK 0x80
#define Command_PassRxFilter 0x80
#define Command_PassRxFilter_BIT 7
#define Command_TxFlowControl_MASK 0x100
#define Command_TxFlowControl 0x100
#define Command_TxFlowControl_BIT 8
#define Command_RMII_MASK 0x200
#define Command_RMII 0x200
#define Command_RMII_BIT 9
#define Command_FullDuplex_MASK 0x400
#define Command_FullDuplex 0x400
#define Command_FullDuplex_BIT 10

#define Status (*(volatile unsigned *)0xFFE00104)
#define Status_OFFSET 0x104
#define Status_RxStatus_MASK 0x1
#define Status_RxStatus 0x1
#define Status_RxStatus_BIT 0
#define Status_TxStatus_MASK 0x2
#define Status_TxStatus 0x2
#define Status_TxStatus_BIT 1

#define RxDescriptor (*(volatile unsigned *)0xFFE00108)
#define RxDescriptor_OFFSET 0x108

#define RxStatus (*(volatile unsigned *)0xFFE0010C)
#define RxStatus_OFFSET 0x10C

#define RxDescriptorNumber (*(volatile unsigned *)0xFFE00110)
#define RxDescriptorNumber_OFFSET 0x110

#define RxProduceIndex (*(volatile unsigned *)0xFFE00114)
#define RxProduceIndex_OFFSET 0x114

#define RxConsumeIndex (*(volatile unsigned *)0xFFE00118)
#define RxConsumeIndex_OFFSET 0x118

#define TxDescriptor (*(volatile unsigned *)0xFFE0011C)
#define TxDescriptor_OFFSET 0x11C

#define TxStatus (*(volatile unsigned *)0xFFE00120)
#define TxStatus_OFFSET 0x120

#define TxDescriptorNumber (*(volatile unsigned *)0xFFE00124)
#define TxDescriptorNumber_OFFSET 0x124

#define TxProduceIndex (*(volatile unsigned *)0xFFE00128)
#define TxProduceIndex_OFFSET 0x128

#define TxConsumeIndex (*(volatile unsigned *)0xFFE0012C)
#define TxConsumeIndex_OFFSET 0x12C

#define TSV0 (*(volatile unsigned *)0xFFE00158)
#define TSV0_OFFSET 0x158
#define TSV0_CRC_error_MASK 0x1
#define TSV0_CRC_error 0x1
#define TSV0_CRC_error_BIT 0
#define TSV0_Length_check_error_MASK 0x2
#define TSV0_Length_check_error 0x2
#define TSV0_Length_check_error_BIT 1
#define TSV0_Length_out_of_range_MASK 0x4
#define TSV0_Length_out_of_range 0x4
#define TSV0_Length_out_of_range_BIT 2
#define TSV0_Done_MASK 0x8
#define TSV0_Done 0x8
#define TSV0_Done_BIT 3
#define TSV0_Multicast_MASK 0x10
#define TSV0_Multicast 0x10
#define TSV0_Multicast_BIT 4
#define TSV0_Broadcast_MASK 0x20
#define TSV0_Broadcast 0x20
#define TSV0_Broadcast_BIT 5
#define TSV0_Packet_Defer_MASK 0x40
#define TSV0_Packet_Defer 0x40
#define TSV0_Packet_Defer_BIT 6
#define TSV0_Excessive_Defer_MASK 0x80
#define TSV0_Excessive_Defer 0x80
#define TSV0_Excessive_Defer_BIT 7
#define TSV0_Excessive_Collison_MASK 0x100
#define TSV0_Excessive_Collison 0x100
#define TSV0_Excessive_Collison_BIT 8
#define TSV0_Late_Collison_MASK 0x200
#define TSV0_Late_Collison 0x200
#define TSV0_Late_Collison_BIT 9
#define TSV0_Giant_MASK 0x400
#define TSV0_Giant 0x400
#define TSV0_Giant_BIT 10
#define TSV0_Underrun_MASK 0x800
#define TSV0_Underrun 0x800
#define TSV0_Underrun_BIT 11
#define TSV0_Total_bytes_MASK 0xFFFF000
#define TSV0_Total_bytes_BIT 12
#define TSV0_Control_frame_MASK 0x10000000
#define TSV0_Control_frame 0x10000000
#define TSV0_Control_frame_BIT 28
#define TSV0_Pause_MASK 0x20000000
#define TSV0_Pause 0x20000000
#define TSV0_Pause_BIT 29
#define TSV0_Backpressure_MASK 0x40000000
#define TSV0_Backpressure 0x40000000
#define TSV0_Backpressure_BIT 30
#define TSV0_VLAN_MASK 0x80000000
#define TSV0_VLAN 0x80000000
#define TSV0_VLAN_BIT 31

#define TSV1 (*(volatile unsigned *)0xFFE0015C)
#define TSV1_OFFSET 0x15C
#define TSV1_Transmit_byte_count_MASK 0xFFFF
#define TSV1_Transmit_byte_count_BIT 0
#define TSV1_Transmit_collision_count_MASK 0xF0000
#define TSV1_Transmit_collision_count_BIT 16

#define RSV (*(volatile unsigned *)0xFFE00160)
#define RSV_OFFSET 0x160
#define RSV_Receive_byte_count_MASK 0xFFFF
#define RSV_Receive_byte_count_BIT 0
#define RSV_Packet_previously_ignored_MASK 0x10000
#define RSV_Packet_previously_ignored 0x10000
#define RSV_Packet_previously_ignored_BIT 16
#define RSV_RXDV_event_previously_seen_MASK 0x20000
#define RSV_RXDV_event_previously_seen 0x20000
#define RSV_RXDV_event_previously_seen_BIT 17
#define RSV_Carrier_event_previously_seen_MASK 0x40000
#define RSV_Carrier_event_previously_seen 0x40000
#define RSV_Carrier_event_previously_seen_BIT 18
#define RSV_Receive_code_violation_MASK 0x80000
#define RSV_Receive_code_violation 0x80000
#define RSV_Receive_code_violation_BIT 19
#define RSV_CRC_error_MASK 0x100000
#define RSV_CRC_error 0x100000
#define RSV_CRC_error_BIT 20
#define RSV_Length_check_error_MASK 0x200000
#define RSV_Length_check_error 0x200000
#define RSV_Length_check_error_BIT 21
#define RSV_Length_out_of_range_MASK 0x400000
#define RSV_Length_out_of_range 0x400000
#define RSV_Length_out_of_range_BIT 22
#define RSV_Receive_OK_MASK 0x800000
#define RSV_Receive_OK 0x800000
#define RSV_Receive_OK_BIT 23
#define RSV_Multicast_MASK 0x1000000
#define RSV_Multicast 0x1000000
#define RSV_Multicast_BIT 24
#define RSV_Broadcast_MASK 0x2000000
#define RSV_Broadcast 0x2000000
#define RSV_Broadcast_BIT 25
#define RSV_Dribble_Nibble_MASK 0x4000000
#define RSV_Dribble_Nibble 0x4000000
#define RSV_Dribble_Nibble_BIT 26
#define RSV_Control_frame_MASK 0x8000000
#define RSV_Control_frame 0x8000000
#define RSV_Control_frame_BIT 27
#define RSV_PAUSE_MASK 0x10000000
#define RSV_PAUSE 0x10000000
#define RSV_PAUSE_BIT 28
#define RSV_Unsupported_Opcode_MASK 0x20000000
#define RSV_Unsupported_Opcode 0x20000000
#define RSV_Unsupported_Opcode_BIT 29
#define RSV_VLAN_MASK 0x40000000
#define RSV_VLAN 0x40000000
#define RSV_VLAN_BIT 30

#define FlowControlCounter (*(volatile unsigned *)0xFFE00170)
#define FlowControlCounter_OFFSET 0x170
#define FlowControlCounter_MirrorCounter_MASK 0xFFFF
#define FlowControlCounter_MirrorCounter_BIT 0
#define FlowControlCounter_Pause_Timer_MASK 0xFFFF0000
#define FlowControlCounter_Pause_Timer_BIT 16

#define FlowControlStatus (*(volatile unsigned *)0xFFE00174)
#define FlowControlStatus_OFFSET 0x174

#define RxFilterCtrl (*(volatile unsigned *)0xFFE00200)
#define RxFilterCtrl_OFFSET 0x200
#define RxFilterCtrl_AcceptUnicastEn_MASK 0x1
#define RxFilterCtrl_AcceptUnicastEn 0x1
#define RxFilterCtrl_AcceptUnicastEn_BIT 0
#define RxFilterCtrl_AcceptBroadcastEn_MASK 0x2
#define RxFilterCtrl_AcceptBroadcastEn 0x2
#define RxFilterCtrl_AcceptBroadcastEn_BIT 1
#define RxFilterCtrl_AcceptMulticastEn_MASK 0x4
#define RxFilterCtrl_AcceptMulticastEn 0x4
#define RxFilterCtrl_AcceptMulticastEn_BIT 2
#define RxFilterCtrl_AcceptUnicastHashEn_MASK 0x8
#define RxFilterCtrl_AcceptUnicastHashEn 0x8
#define RxFilterCtrl_AcceptUnicastHashEn_BIT 3
#define RxFilterCtrl_AcceptMulticastHashEn_MASK 0x10
#define RxFilterCtrl_AcceptMulticastHashEn 0x10
#define RxFilterCtrl_AcceptMulticastHashEn_BIT 4
#define RxFilterCtrl_AcceptPerfectEn_MASK 0x20
#define RxFilterCtrl_AcceptPerfectEn 0x20
#define RxFilterCtrl_AcceptPerfectEn_BIT 5
#define RxFilterCtrl_MagicPacketEnWoL_MASK 0x1000
#define RxFilterCtrl_MagicPacketEnWoL 0x1000
#define RxFilterCtrl_MagicPacketEnWoL_BIT 12
#define RxFilterCtrl_RxFilterEnWoL_MASK 0x2000
#define RxFilterCtrl_RxFilterEnWoL 0x2000
#define RxFilterCtrl_RxFilterEnWoL_BIT 13

#define RxFilterWoLStatus (*(volatile unsigned *)0xFFE00204)
#define RxFilterWoLStatus_OFFSET 0x204
#define RxFilterWoLStatus_AcceptUnicastWoL_MASK 0x1
#define RxFilterWoLStatus_AcceptUnicastWoL 0x1
#define RxFilterWoLStatus_AcceptUnicastWoL_BIT 0
#define RxFilterWoLStatus_AcceptBroadcastWoL_MASK 0x2
#define RxFilterWoLStatus_AcceptBroadcastWoL 0x2
#define RxFilterWoLStatus_AcceptBroadcastWoL_BIT 1
#define RxFilterWoLStatus_AcceptMulticastWoL_MASK 0x4
#define RxFilterWoLStatus_AcceptMulticastWoL 0x4
#define RxFilterWoLStatus_AcceptMulticastWoL_BIT 2
#define RxFilterWoLStatus_AcceptUnicastHashWoL_MASK 0x8
#define RxFilterWoLStatus_AcceptUnicastHashWoL 0x8
#define RxFilterWoLStatus_AcceptUnicastHashWoL_BIT 3
#define RxFilterWoLStatus_AcceptMulticastHashWoL_MASK 0x10
#define RxFilterWoLStatus_AcceptMulticastHashWoL 0x10
#define RxFilterWoLStatus_AcceptMulticastHashWoL_BIT 4
#define RxFilterWoLStatus_AcceptPerfectWoL_MASK 0x20
#define RxFilterWoLStatus_AcceptPerfectWoL 0x20
#define RxFilterWoLStatus_AcceptPerfectWoL_BIT 5
#define RxFilterWoLStatus_RxFilterWoL_MASK 0x80
#define RxFilterWoLStatus_RxFilterWoL 0x80
#define RxFilterWoLStatus_RxFilterWoL_BIT 7
#define RxFilterWoLStatus_MagicPacketWoL_MASK 0x100
#define RxFilterWoLStatus_MagicPacketWoL 0x100
#define RxFilterWoLStatus_MagicPacketWoL_BIT 8

#define RxFilterWoLClear (*(volatile unsigned *)0xFFE00208)
#define RxFilterWoLClear_OFFSET 0x208
#define RxFilterWoLClear_AcceptUnicastWoLClr_MASK 0x1
#define RxFilterWoLClear_AcceptUnicastWoLClr 0x1
#define RxFilterWoLClear_AcceptUnicastWoLClr_BIT 0
#define RxFilterWoLClear_AcceptBroadcastWoLClr_MASK 0x2
#define RxFilterWoLClear_AcceptBroadcastWoLClr 0x2
#define RxFilterWoLClear_AcceptBroadcastWoLClr_BIT 1
#define RxFilterWoLClear_AcceptMulticastWoLClr_MASK 0x4
#define RxFilterWoLClear_AcceptMulticastWoLClr 0x4
#define RxFilterWoLClear_AcceptMulticastWoLClr_BIT 2
#define RxFilterWoLClear_AcceptUnicastHashWoLClr_MASK 0x8
#define RxFilterWoLClear_AcceptUnicastHashWoLClr 0x8
#define RxFilterWoLClear_AcceptUnicastHashWoLClr_BIT 3
#define RxFilterWoLClear_AcceptMulticastHashWoLClr_MASK 0x10
#define RxFilterWoLClear_AcceptMulticastHashWoLClr 0x10
#define RxFilterWoLClear_AcceptMulticastHashWoLClr_BIT 4
#define RxFilterWoLClear_AcceptPerfectWoLClr_MASK 0x20
#define RxFilterWoLClear_AcceptPerfectWoLClr 0x20
#define RxFilterWoLClear_AcceptPerfectWoLClr_BIT 5
#define RxFilterWoLClear_RxFilterWoLClr_MASK 0x80
#define RxFilterWoLClear_RxFilterWoLClr 0x80
#define RxFilterWoLClear_RxFilterWoLClr_BIT 7
#define RxFilterWoLClear_MagicPacketWoLClr_MASK 0x100
#define RxFilterWoLClear_MagicPacketWoLClr 0x100
#define RxFilterWoLClear_MagicPacketWoLClr_BIT 8

#define HashFilterL (*(volatile unsigned *)0xFFE00210)
#define HashFilterL_OFFSET 0x210

#define HashFilterH (*(volatile unsigned *)0xFFE00214)
#define HashFilterH_OFFSET 0x214

#define IntStatus (*(volatile unsigned *)0xFFE00FE0)
#define IntStatus_OFFSET 0xFE0
#define IntStatus_RxOverrunInt_MASK 0x1
#define IntStatus_RxOverrunInt 0x1
#define IntStatus_RxOverrunInt_BIT 0
#define IntStatus_RxErrorInt_MASK 0x2
#define IntStatus_RxErrorInt 0x2
#define IntStatus_RxErrorInt_BIT 1
#define IntStatus_RxFinishedInt_MASK 0x4
#define IntStatus_RxFinishedInt 0x4
#define IntStatus_RxFinishedInt_BIT 2
#define IntStatus_RxDoneInt_MASK 0x8
#define IntStatus_RxDoneInt 0x8
#define IntStatus_RxDoneInt_BIT 3
#define IntStatus_TxUnderrunInt_MASK 0x10
#define IntStatus_TxUnderrunInt 0x10
#define IntStatus_TxUnderrunInt_BIT 4
#define IntStatus_TxErrorInt_MASK 0x20
#define IntStatus_TxErrorInt 0x20
#define IntStatus_TxErrorInt_BIT 5
#define IntStatus_TxFinishedInt_MASK 0x40
#define IntStatus_TxFinishedInt 0x40
#define IntStatus_TxFinishedInt_BIT 6
#define IntStatus_TxDoneInt_MASK 0x80
#define IntStatus_TxDoneInt 0x80
#define IntStatus_TxDoneInt_BIT 7
#define IntStatus_SoftInt_MASK 0x1000
#define IntStatus_SoftInt 0x1000
#define IntStatus_SoftInt_BIT 12
#define IntStatus_WakeupInt_MASK 0x2000
#define IntStatus_WakeupInt 0x2000
#define IntStatus_WakeupInt_BIT 13

#define IntEnable (*(volatile unsigned *)0xFFE00FE4)
#define IntEnable_OFFSET 0xFE4
#define IntEnable_RxOverrunIntEn_MASK 0x1
#define IntEnable_RxOverrunIntEn 0x1
#define IntEnable_RxOverrunIntEn_BIT 0
#define IntEnable_RxErrorIntEn_MASK 0x2
#define IntEnable_RxErrorIntEn 0x2
#define IntEnable_RxErrorIntEn_BIT 1
#define IntEnable_RxFinishedIntEn_MASK 0x4
#define IntEnable_RxFinishedIntEn 0x4
#define IntEnable_RxFinishedIntEn_BIT 2
#define IntEnable_RxDoneIntEn_MASK 0x8
#define IntEnable_RxDoneIntEn 0x8
#define IntEnable_RxDoneIntEn_BIT 3
#define IntEnable_TxUnderrunIntEn_MASK 0x10
#define IntEnable_TxUnderrunIntEn 0x10
#define IntEnable_TxUnderrunIntEn_BIT 4
#define IntEnable_TxErrorIntEn_MASK 0x20
#define IntEnable_TxErrorIntEn 0x20
#define IntEnable_TxErrorIntEn_BIT 5
#define IntEnable_TxFinishedIntEn_MASK 0x40
#define IntEnable_TxFinishedIntEn 0x40
#define IntEnable_TxFinishedIntEn_BIT 6
#define IntEnable_TxDoneIntEn_MASK 0x80
#define IntEnable_TxDoneIntEn 0x80
#define IntEnable_TxDoneIntEn_BIT 7
#define IntEnable_SoftIntEn_MASK 0x1000
#define IntEnable_SoftIntEn 0x1000
#define IntEnable_SoftIntEn_BIT 12
#define IntEnable_WakeupIntEn_MASK 0x2000
#define IntEnable_WakeupIntEn 0x2000
#define IntEnable_WakeupIntEn_BIT 13

#define IntClear (*(volatile unsigned *)0xFFE00FE8)
#define IntClear_OFFSET 0xFE8
#define IntClear_RxOverrunIntClr_MASK 0x1
#define IntClear_RxOverrunIntClr 0x1
#define IntClear_RxOverrunIntClr_BIT 0
#define IntClear_RxErrorIntClr_MASK 0x2
#define IntClear_RxErrorIntClr 0x2
#define IntClear_RxErrorIntClr_BIT 1
#define IntClear_RxFinishedIntClr_MASK 0x4
#define IntClear_RxFinishedIntClr 0x4
#define IntClear_RxFinishedIntClr_BIT 2
#define IntClear_RxDoneIntClr_MASK 0x8
#define IntClear_RxDoneIntClr 0x8
#define IntClear_RxDoneIntClr_BIT 3
#define IntClear_TxUnderrunIntClr_MASK 0x10
#define IntClear_TxUnderrunIntClr 0x10
#define IntClear_TxUnderrunIntClr_BIT 4
#define IntClear_TxErrorIntClr_MASK 0x20
#define IntClear_TxErrorIntClr 0x20
#define IntClear_TxErrorIntClr_BIT 5
#define IntClear_TxFinishedIntClr_MASK 0x40
#define IntClear_TxFinishedIntClr 0x40
#define IntClear_TxFinishedIntClr_BIT 6
#define IntClear_TxDoneIntClr_MASK 0x80
#define IntClear_TxDoneIntClr 0x80
#define IntClear_TxDoneIntClr_BIT 7
#define IntClear_SoftIntClr_MASK 0x1000
#define IntClear_SoftIntClr 0x1000
#define IntClear_SoftIntClr_BIT 12
#define IntClear_WakeupIntClr_MASK 0x2000
#define IntClear_WakeupIntClr 0x2000
#define IntClear_WakeupIntClr_BIT 13

#define IntSet (*(volatile unsigned *)0xFFE00FEC)
#define IntSet_OFFSET 0xFEC
#define IntSet_RxOverrunIntSet_MASK 0x1
#define IntSet_RxOverrunIntSet 0x1
#define IntSet_RxOverrunIntSet_BIT 0
#define IntSet_RxErrorIntSet_MASK 0x2
#define IntSet_RxErrorIntSet 0x2
#define IntSet_RxErrorIntSet_BIT 1
#define IntSet_RxFinishedIntSet_MASK 0x4
#define IntSet_RxFinishedIntSet 0x4
#define IntSet_RxFinishedIntSet_BIT 2
#define IntSet_RxDoneIntSet_MASK 0x8
#define IntSet_RxDoneIntSet 0x8
#define IntSet_RxDoneIntSet_BIT 3
#define IntSet_TxUnderrunIntSet_MASK 0x10
#define IntSet_TxUnderrunIntSet 0x10
#define IntSet_TxUnderrunIntSet_BIT 4
#define IntSet_TxErrorIntSet_MASK 0x20
#define IntSet_TxErrorIntSet 0x20
#define IntSet_TxErrorIntSet_BIT 5
#define IntSet_TxFinishedIntSet_MASK 0x40
#define IntSet_TxFinishedIntSet 0x40
#define IntSet_TxFinishedIntSet_BIT 6
#define IntSet_TxDoneIntSet_MASK 0x80
#define IntSet_TxDoneIntSet 0x80
#define IntSet_TxDoneIntSet_BIT 7
#define IntSet_SoftIntSet_MASK 0x1000
#define IntSet_SoftIntSet 0x1000
#define IntSet_SoftIntSet_BIT 12
#define IntSet_WakeupIntSet_MASK 0x2000
#define IntSet_WakeupIntSet 0x2000
#define IntSet_WakeupIntSet_BIT 13

#define PowerDown (*(volatile unsigned *)0xFFE00FF4)
#define PowerDown_OFFSET 0xFF4
#define PowerDown_PowerDownMACAHB_MASK 0x80000000
#define PowerDown_PowerDownMACAHB 0x80000000
#define PowerDown_PowerDownMACAHB_BIT 31

#define GPDMA_BASE 0xFFE04000

#define DMACIntStatus (*(volatile unsigned *)0xFFE04000)
#define DMACIntStatus_OFFSET 0x0
#define DMACIntStatus_IntStatus0_MASK 0x1
#define DMACIntStatus_IntStatus0 0x1
#define DMACIntStatus_IntStatus0_BIT 0
#define DMACIntStatus_IntStatus1_MASK 0x2
#define DMACIntStatus_IntStatus1 0x2
#define DMACIntStatus_IntStatus1_BIT 1

#define DMACIntTCStatus (*(volatile unsigned *)0xFFE04004)
#define DMACIntTCStatus_OFFSET 0x4
#define DMACIntTCStatus_IntTCStatus0_MASK 0x1
#define DMACIntTCStatus_IntTCStatus0 0x1
#define DMACIntTCStatus_IntTCStatus0_BIT 0
#define DMACIntTCStatus_IntTCStatus1_MASK 0x2
#define DMACIntTCStatus_IntTCStatus1 0x2
#define DMACIntTCStatus_IntTCStatus1_BIT 1

#define DMACIntTCClear (*(volatile unsigned *)0xFFE04008)
#define DMACIntTCClear_OFFSET 0x8
#define DMACIntTCClear_IntTCClear0_MASK 0x1
#define DMACIntTCClear_IntTCClear0 0x1
#define DMACIntTCClear_IntTCClear0_BIT 0
#define DMACIntTCClear_IntTCClear1_MASK 0x2
#define DMACIntTCClear_IntTCClear1 0x2
#define DMACIntTCClear_IntTCClear1_BIT 1

#define DMACIntErrorStatus (*(volatile unsigned *)0xFFE0400C)
#define DMACIntErrorStatus_OFFSET 0xC
#define DMACIntErrorStatus_IntErrorStatus0_MASK 0x1
#define DMACIntErrorStatus_IntErrorStatus0 0x1
#define DMACIntErrorStatus_IntErrorStatus0_BIT 0
#define DMACIntErrorStatus_IntErrorStatus1_MASK 0x2
#define DMACIntErrorStatus_IntErrorStatus1 0x2
#define DMACIntErrorStatus_IntErrorStatus1_BIT 1

#define DMACIntErrClr (*(volatile unsigned *)0xFFE04010)
#define DMACIntErrClr_OFFSET 0x10
#define DMACIntErrClr_IntErrClr0_MASK 0x1
#define DMACIntErrClr_IntErrClr0 0x1
#define DMACIntErrClr_IntErrClr0_BIT 0
#define DMACIntErrClr_IntErrClr1_MASK 0x2
#define DMACIntErrClr_IntErrClr1 0x2
#define DMACIntErrClr_IntErrClr1_BIT 1

#define DMACRawIntTCStatus (*(volatile unsigned *)0xFFE04014)
#define DMACRawIntTCStatus_OFFSET 0x14
#define DMACRawIntTCStatus_RawIntTCStatus0_MASK 0x1
#define DMACRawIntTCStatus_RawIntTCStatus0 0x1
#define DMACRawIntTCStatus_RawIntTCStatus0_BIT 0
#define DMACRawIntTCStatus_RawIntTCStatus1_MASK 0x2
#define DMACRawIntTCStatus_RawIntTCStatus1 0x2
#define DMACRawIntTCStatus_RawIntTCStatus1_BIT 1

#define DMACRawIntErrorStatus (*(volatile unsigned *)0xFFE04018)
#define DMACRawIntErrorStatus_OFFSET 0x18
#define DMACRawIntErrorStatus_RawIntErrorStatus0_MASK 0x1
#define DMACRawIntErrorStatus_RawIntErrorStatus0 0x1
#define DMACRawIntErrorStatus_RawIntErrorStatus0_BIT 0
#define DMACRawIntErrorStatus_RawIntErrorStatus1_MASK 0x2
#define DMACRawIntErrorStatus_RawIntErrorStatus1 0x2
#define DMACRawIntErrorStatus_RawIntErrorStatus1_BIT 1

#define DMACEnbldChns (*(volatile unsigned *)0xFFE0401C)
#define DMACEnbldChns_OFFSET 0x1C
#define DMACEnbldChns_EnabledChannels0_MASK 0x1
#define DMACEnbldChns_EnabledChannels0 0x1
#define DMACEnbldChns_EnabledChannels0_BIT 0
#define DMACEnbldChns_EnabledChannels1_MASK 0x2
#define DMACEnbldChns_EnabledChannels1 0x2
#define DMACEnbldChns_EnabledChannels1_BIT 1

#define DMACSoftBReq (*(volatile unsigned *)0xFFE04020)
#define DMACSoftBReq_OFFSET 0x20
#define DMACSoftBReq_SoftBReqSSP0Tx_MASK 0x1
#define DMACSoftBReq_SoftBReqSSP0Tx 0x1
#define DMACSoftBReq_SoftBReqSSP0Tx_BIT 0
#define DMACSoftBReq_SoftBReqSSP0Rx_MASK 0x2
#define DMACSoftBReq_SoftBReqSSP0Rx 0x2
#define DMACSoftBReq_SoftBReqSSP0Rx_BIT 1
#define DMACSoftBReq_SoftBReqSSP1Tx_MASK 0x4
#define DMACSoftBReq_SoftBReqSSP1Tx 0x4
#define DMACSoftBReq_SoftBReqSSP1Tx_BIT 2
#define DMACSoftBReq_SoftBReqSSP1Rx_MASK 0x8
#define DMACSoftBReq_SoftBReqSSP1Rx 0x8
#define DMACSoftBReq_SoftBReqSSP1Rx_BIT 3
#define DMACSoftBReq_SoftBReqSDMMC_MASK 0x10
#define DMACSoftBReq_SoftBReqSDMMC 0x10
#define DMACSoftBReq_SoftBReqSDMMC_BIT 4

#define DMACSoftSReq (*(volatile unsigned *)0xFFE04024)
#define DMACSoftSReq_OFFSET 0x24
#define DMACSoftSReq_SoftReqSSP0Tx_MASK 0x1
#define DMACSoftSReq_SoftReqSSP0Tx 0x1
#define DMACSoftSReq_SoftReqSSP0Tx_BIT 0
#define DMACSoftSReq_SoftReqSSP0Rx_MASK 0x2
#define DMACSoftSReq_SoftReqSSP0Rx 0x2
#define DMACSoftSReq_SoftReqSSP0Rx_BIT 1
#define DMACSoftSReq_SoftReqSSP1Tx_MASK 0x4
#define DMACSoftSReq_SoftReqSSP1Tx 0x4
#define DMACSoftSReq_SoftReqSSP1Tx_BIT 2
#define DMACSoftSReq_SoftReqSSP1Rx_MASK 0x8
#define DMACSoftSReq_SoftReqSSP1Rx 0x8
#define DMACSoftSReq_SoftReqSSP1Rx_BIT 3
#define DMACSoftSReq_SoftReqSDMMC_MASK 0x10
#define DMACSoftSReq_SoftReqSDMMC 0x10
#define DMACSoftSReq_SoftReqSDMMC_BIT 4
#define DMACSoftSReq_SoftReqI2S0_MASK 0x20
#define DMACSoftSReq_SoftReqI2S0 0x20
#define DMACSoftSReq_SoftReqI2S0_BIT 5
#define DMACSoftSReq_SoftReqI2S1_MASK 0x40
#define DMACSoftSReq_SoftReqI2S1 0x40
#define DMACSoftSReq_SoftReqI2S1_BIT 6

#define DMACSoftLBReq (*(volatile unsigned *)0xFFE04028)
#define DMACSoftLBReq_OFFSET 0x28
#define DMACSoftLBReq_SoftLBReqSDMMC_MASK 0x10
#define DMACSoftLBReq_SoftLBReqSDMMC 0x10
#define DMACSoftLBReq_SoftLBReqSDMMC_BIT 4

#define DMACSoftSLReq (*(volatile unsigned *)0xFFE0402C)
#define DMACSoftSLReq_OFFSET 0x2C
#define DMACSoftSLReq_SoftLSReqSDMMC_MASK 0x10
#define DMACSoftSLReq_SoftLSReqSDMMC 0x10
#define DMACSoftSLReq_SoftLSReqSDMMC_BIT 4

#define DMACConfiguration (*(volatile unsigned *)0xFFE04030)
#define DMACConfiguration_OFFSET 0x30
#define DMACConfiguration_E_MASK 0x1
#define DMACConfiguration_E 0x1
#define DMACConfiguration_E_BIT 0
#define DMACConfiguration_M_MASK 0x1
#define DMACConfiguration_M 0x1
#define DMACConfiguration_M_BIT 0

#define DMACSync (*(volatile unsigned *)0xFFE04034)
#define DMACSync_OFFSET 0x34
#define DMACSync_DMACSync_MASK 0xFFFF
#define DMACSync_DMACSync_BIT 0

#define DMACC0SrcAddr (*(volatile unsigned *)0xFFE04100)
#define DMACC0SrcAddr_OFFSET 0x100

#define DMACC0DestAddr (*(volatile unsigned *)0xFFE04104)
#define DMACC0DestAddr_OFFSET 0x104

#define DMACC0LLI (*(volatile unsigned *)0xFFE04108)
#define DMACC0LLI_OFFSET 0x108

#define DMACC0Control (*(volatile unsigned *)0xFFE0410C)
#define DMACC0Control_OFFSET 0x10C
#define DMACC0Control_TransferSize_MASK 0xFFF
#define DMACC0Control_TransferSize_BIT 0
#define DMACC0Control_SBSize_MASK 0x7000
#define DMACC0Control_SBSize_BIT 12
#define DMACC0Control_DBSize_MASK 0x38000
#define DMACC0Control_DBSize_BIT 15
#define DMACC0Control_SWidth_MASK 0x1C0000
#define DMACC0Control_SWidth_BIT 18
#define DMACC0Control_DWidth_MASK 0xE00000
#define DMACC0Control_DWidth_BIT 21
#define DMACC0Control_SI_MASK 0x4000000
#define DMACC0Control_SI 0x4000000
#define DMACC0Control_SI_BIT 26
#define DMACC0Control_DI_MASK 0x8000000
#define DMACC0Control_DI 0x8000000
#define DMACC0Control_DI_BIT 27
#define DMACC0Control_Prot_MASK 0x70000000
#define DMACC0Control_Prot_BIT 28
#define DMACC0Control_I_MASK 0x80000000
#define DMACC0Control_I 0x80000000
#define DMACC0Control_I_BIT 31

#define DMACC0Configuration (*(volatile unsigned *)0xFFE04110)
#define DMACC0Configuration_OFFSET 0x110
#define DMACC0Configuration_E_MASK 0x1
#define DMACC0Configuration_E 0x1
#define DMACC0Configuration_E_BIT 0
#define DMACC0Configuration_SrcPeripheral_MASK 0x1E
#define DMACC0Configuration_SrcPeripheral_BIT 1
#define DMACC0Configuration_DestPeripheral_MASK 0x3C0
#define DMACC0Configuration_DestPeripheral_BIT 6
#define DMACC0Configuration_FlowCntrl_MASK 0x3800
#define DMACC0Configuration_FlowCntrl_BIT 11
#define DMACC0Configuration_IE_MASK 0x4000
#define DMACC0Configuration_IE 0x4000
#define DMACC0Configuration_IE_BIT 14
#define DMACC0Configuration_ITC_MASK 0x8000
#define DMACC0Configuration_ITC 0x8000
#define DMACC0Configuration_ITC_BIT 15
#define DMACC0Configuration_L_MASK 0x10000
#define DMACC0Configuration_L 0x10000
#define DMACC0Configuration_L_BIT 16
#define DMACC0Configuration_A_MASK 0x20000
#define DMACC0Configuration_A 0x20000
#define DMACC0Configuration_A_BIT 17
#define DMACC0Configuration_H_MASK 0x40000
#define DMACC0Configuration_H 0x40000
#define DMACC0Configuration_H_BIT 18

#define DMACC1SrcAddr (*(volatile unsigned *)0xFFE04120)
#define DMACC1SrcAddr_OFFSET 0x120

#define DMACC1DestAddr (*(volatile unsigned *)0xFFE04124)
#define DMACC1DestAddr_OFFSET 0x124

#define DMACC1LLI (*(volatile unsigned *)0xFFE04128)
#define DMACC1LLI_OFFSET 0x128

#define DMACC1Control (*(volatile unsigned *)0xFFE0412C)
#define DMACC1Control_OFFSET 0x12C
#define DMACC1Control_TransferSize_MASK 0xFFF
#define DMACC1Control_TransferSize_BIT 0
#define DMACC1Control_SBSize_MASK 0x7000
#define DMACC1Control_SBSize_BIT 12
#define DMACC1Control_DBSize_MASK 0x38000
#define DMACC1Control_DBSize_BIT 15
#define DMACC1Control_SWidth_MASK 0x1C0000
#define DMACC1Control_SWidth_BIT 18
#define DMACC1Control_DWidth_MASK 0xE00000
#define DMACC1Control_DWidth_BIT 21
#define DMACC1Control_SI_MASK 0x4000000
#define DMACC1Control_SI 0x4000000
#define DMACC1Control_SI_BIT 26
#define DMACC1Control_DI_MASK 0x8000000
#define DMACC1Control_DI 0x8000000
#define DMACC1Control_DI_BIT 27
#define DMACC1Control_Prot_MASK 0x70000000
#define DMACC1Control_Prot_BIT 28
#define DMACC1Control_I_MASK 0x80000000
#define DMACC1Control_I 0x80000000
#define DMACC1Control_I_BIT 31

#define DMACC1Configuration (*(volatile unsigned *)0xFFE04130)
#define DMACC1Configuration_OFFSET 0x130
#define DMACC1Configuration_E_MASK 0x1
#define DMACC1Configuration_E 0x1
#define DMACC1Configuration_E_BIT 0
#define DMACC1Configuration_SrcPeripheral_MASK 0x1E
#define DMACC1Configuration_SrcPeripheral_BIT 1
#define DMACC1Configuration_DestPeripheral_MASK 0x3C0
#define DMACC1Configuration_DestPeripheral_BIT 6
#define DMACC1Configuration_FlowCntrl_MASK 0x3800
#define DMACC1Configuration_FlowCntrl_BIT 11
#define DMACC1Configuration_IE_MASK 0x4000
#define DMACC1Configuration_IE 0x4000
#define DMACC1Configuration_IE_BIT 14
#define DMACC1Configuration_ITC_MASK 0x8000
#define DMACC1Configuration_ITC 0x8000
#define DMACC1Configuration_ITC_BIT 15
#define DMACC1Configuration_L_MASK 0x10000
#define DMACC1Configuration_L 0x10000
#define DMACC1Configuration_L_BIT 16
#define DMACC1Configuration_A_MASK 0x20000
#define DMACC1Configuration_A 0x20000
#define DMACC1Configuration_A_BIT 17
#define DMACC1Configuration_H_MASK 0x40000
#define DMACC1Configuration_H 0x40000
#define DMACC1Configuration_H_BIT 18

#define EMC_BASE 0xFFE08000

#define EMCControl (*(volatile unsigned long *)0xFFE08000)
#define EMCControl_OFFSET 0x0
#define EMCControl_E_MASK 0x1
#define EMCControl_E 0x1
#define EMCControl_E_BIT 0
#define EMCControl_M_MASK 0x2
#define EMCControl_M 0x2
#define EMCControl_M_BIT 1
#define EMCControl_L_MASK 0x4
#define EMCControl_L 0x4
#define EMCControl_L_BIT 2

#define EMCStatus (*(volatile unsigned long *)0xFFE08004)
#define EMCStatus_OFFSET 0x4
#define EMCStatus_B_MASK 0x1
#define EMCStatus_B 0x1
#define EMCStatus_B_BIT 0
#define EMCStatus_S_MASK 0x2
#define EMCStatus_S 0x2
#define EMCStatus_S_BIT 1
#define EMCStatus_SA_MASK 0x4
#define EMCStatus_SA 0x4
#define EMCStatus_SA_BIT 2

#define EMCConfig (*(volatile unsigned long *)0xFFE08008)
#define EMCConfig_OFFSET 0x8
#define EMCConfig_Endian_mode_MASK 0x1
#define EMCConfig_Endian_mode 0x1
#define EMCConfig_Endian_mode_BIT 0

#define EMCStaticExtendedWait (*(volatile unsigned long *)0xFFE08080)
#define EMCStaticExtendedWait_OFFSET 0x80
#define EMCStaticExtendedWait_EXTENDEDWAIT_MASK 0x3FF
#define EMCStaticExtendedWait_EXTENDEDWAIT_BIT 0

#define EMCDynamicControl (*(volatile unsigned long *)0xFFE08020)
#define EMCDynamicControl_OFFSET 0x20
#define EMCDynamicControl_CE_MASK 0x1
#define EMCDynamicControl_CE 0x1
#define EMCDynamicControl_CE_BIT 0
#define EMCDynamicControl_CS_MASK 0x2
#define EMCDynamicControl_CS 0x2
#define EMCDynamicControl_CS_BIT 1
#define EMCDynamicControl_SR_MASK 0x4
#define EMCDynamicControl_SR 0x4
#define EMCDynamicControl_SR_BIT 2
#define EMCDynamicControl_MMC_MASK 0x20
#define EMCDynamicControl_MMC 0x20
#define EMCDynamicControl_MMC_BIT 5
#define EMCDynamicControl_I_MASK 0x180
#define EMCDynamicControl_I_BIT 7
#define EMCDynamicControl_DP_MASK 0x2000
#define EMCDynamicControl_DP 0x2000
#define EMCDynamicControl_DP_BIT 13

#define EMCDynamicRefresh (*(volatile unsigned long *)0xFFE08024)
#define EMCDynamicRefresh_OFFSET 0x24
#define EMCDynamicRefresh_REFRESH_MASK 0x7FF
#define EMCDynamicRefresh_REFRESH_BIT 0

#define EMCDynamicReadConfig (*(volatile unsigned long *)0xFFE08028)
#define EMCDynamicReadConfig_OFFSET 0x28
#define EMCDynamicReadConfig_RD_MASK 0x3
#define EMCDynamicReadConfig_RD_BIT 0

#define EMCDynamictRP (*(volatile unsigned long *)0xFFE08030)
#define EMCDynamictRP_OFFSET 0x30
#define EMCDynamictRP_tRP_MASK 0xF
#define EMCDynamictRP_tRP_BIT 0

#define EMCDynamictRAS (*(volatile unsigned long *)0xFFE08034)
#define EMCDynamictRAS_OFFSET 0x34
#define EMCDynamictRAS_tRAS_MASK 0xF
#define EMCDynamictRAS_tRAS_BIT 0

#define EMCDynamictSREX (*(volatile unsigned long *)0xFFE08038)
#define EMCDynamictSREX_OFFSET 0x38
#define EMCDynamictSREX_tSREX_MASK 0xF
#define EMCDynamictSREX_tSREX_BIT 0

#define EMCDynamictAPR (*(volatile unsigned long *)0xFFE0803C)
#define EMCDynamictAPR_OFFSET 0x3C
#define EMCDynamictAPR_tAPR_MASK 0xF
#define EMCDynamictAPR_tAPR_BIT 0

#define EMCDynamictDAL (*(volatile unsigned long *)0xFFE08040)
#define EMCDynamictDAL_OFFSET 0x40
#define EMCDynamictDAL_tDAL_MASK 0xF
#define EMCDynamictDAL_tDAL_BIT 0

#define EMCDynamictWR (*(volatile unsigned long *)0xFFE08044)
#define EMCDynamictWR_OFFSET 0x44
#define EMCDynamictWR_tWR_MASK 0xF
#define EMCDynamictWR_tWR_BIT 0

#define EMCDynamictRC (*(volatile unsigned long *)0xFFE08048)
#define EMCDynamictRC_OFFSET 0x48
#define EMCDynamictRC_tRC_MASK 0xF
#define EMCDynamictRC_tRC_BIT 0

#define EMCDynamictRFC (*(volatile unsigned long *)0xFFE0804C)
#define EMCDynamictRFC_OFFSET 0x4C
#define EMCDynamictRFC_tRFC_MASK 0xF
#define EMCDynamictRFC_tRFC_BIT 0

#define EMCDynamictXSR (*(volatile unsigned long *)0xFFE08050)
#define EMCDynamictXSR_OFFSET 0x50
#define EMCDynamictXSR_tXSR_MASK 0xF
#define EMCDynamictXSR_tXSR_BIT 0

#define EMCDynamictRRD (*(volatile unsigned long *)0xFFE08054)
#define EMCDynamictRRD_OFFSET 0x54
#define EMCDynamictRRD_tRRD_MASK 0xF
#define EMCDynamictRRD_tRRD_BIT 0

#define EMCDynamictMRD (*(volatile unsigned long *)0xFFE08058)
#define EMCDynamictMRD_OFFSET 0x58
#define EMCDynamictMRD_tMRD_MASK 0xF
#define EMCDynamictMRD_tMRD_BIT 0

#define EMCDynamicConfig0 (*(volatile unsigned *)0xFFE08100)
#define EMCDynamicConfig0_OFFSET 0x100
#define EMCDynamicConfig0_MD_MASK 0x18
#define EMCDynamicConfig0_MD_BIT 3
#define EMCDynamicConfig0_AM_bits7_12_MASK 0x1F80
#define EMCDynamicConfig0_AM_bits7_12_BIT 7
#define EMCDynamicConfig0_AM_bit14_MASK 0x4000
#define EMCDynamicConfig0_AM_bit14 0x4000
#define EMCDynamicConfig0_AM_bit14_BIT 14
#define EMCDynamicConfig0_B_MASK 0x80000
#define EMCDynamicConfig0_B 0x80000
#define EMCDynamicConfig0_B_BIT 19
#define EMCDynamicConfig0_P_MASK 0x100000
#define EMCDynamicConfig0_P 0x100000
#define EMCDynamicConfig0_P_BIT 20

#define EMCDynamicRasCas0 (*(volatile unsigned *)0xFFE08104)
#define EMCDynamicRasCas0_OFFSET 0x104
#define EMCDynamicRasCas0_RAS_MASK 0x3
#define EMCDynamicRasCas0_RAS_BIT 0
#define EMCDynamicRasCas0_CAS_MASK 0x300
#define EMCDynamicRasCas0_CAS_BIT 8

#define EMCDynamicConfig1 (*(volatile unsigned *)0xFFE08120)
#define EMCDynamicConfig1_OFFSET 0x120
#define EMCDynamicConfig1_MD_MASK 0x18
#define EMCDynamicConfig1_MD_BIT 3
#define EMCDynamicConfig1_AM_bits7_12_MASK 0x1F80
#define EMCDynamicConfig1_AM_bits7_12_BIT 7
#define EMCDynamicConfig1_AM_bit14_MASK 0x4000
#define EMCDynamicConfig1_AM_bit14 0x4000
#define EMCDynamicConfig1_AM_bit14_BIT 14
#define EMCDynamicConfig1_B_MASK 0x80000
#define EMCDynamicConfig1_B 0x80000
#define EMCDynamicConfig1_B_BIT 19
#define EMCDynamicConfig1_P_MASK 0x100000
#define EMCDynamicConfig1_P 0x100000
#define EMCDynamicConfig1_P_BIT 20

#define EMCDynamicRasCas1 (*(volatile unsigned *)0xFFE08124)
#define EMCDynamicRasCas1_OFFSET 0x124
#define EMCDynamicRasCas1_RAS_MASK 0x3
#define EMCDynamicRasCas1_RAS_BIT 0
#define EMCDynamicRasCas1_CAS_MASK 0x300
#define EMCDynamicRasCas1_CAS_BIT 8

#define EMCDynamicConfig2 (*(volatile unsigned *)0xFFE08140)
#define EMCDynamicConfig2_OFFSET 0x140
#define EMCDynamicConfig2_MD_MASK 0x18
#define EMCDynamicConfig2_MD_BIT 3
#define EMCDynamicConfig2_AM_bits7_12_MASK 0x1F80
#define EMCDynamicConfig2_AM_bits7_12_BIT 7
#define EMCDynamicConfig2_AM_bit14_MASK 0x4000
#define EMCDynamicConfig2_AM_bit14 0x4000
#define EMCDynamicConfig2_AM_bit14_BIT 14
#define EMCDynamicConfig2_B_MASK 0x80000
#define EMCDynamicConfig2_B 0x80000
#define EMCDynamicConfig2_B_BIT 19
#define EMCDynamicConfig2_P_MASK 0x100000
#define EMCDynamicConfig2_P 0x100000
#define EMCDynamicConfig2_P_BIT 20

#define EMCDynamicRasCas2 (*(volatile unsigned *)0xFFE08144)
#define EMCDynamicRasCas2_OFFSET 0x144
#define EMCDynamicRasCas2_RAS_MASK 0x3
#define EMCDynamicRasCas2_RAS_BIT 0
#define EMCDynamicRasCas2_CAS_MASK 0x300
#define EMCDynamicRasCas2_CAS_BIT 8

#define EMCDynamicConfig3 (*(volatile unsigned *)0xFFE08160)
#define EMCDynamicConfig3_OFFSET 0x160
#define EMCDynamicConfig3_MD_MASK 0x18
#define EMCDynamicConfig3_MD_BIT 3
#define EMCDynamicConfig3_AM_bits7_12_MASK 0x1F80
#define EMCDynamicConfig3_AM_bits7_12_BIT 7
#define EMCDynamicConfig3_AM_bit14_MASK 0x4000
#define EMCDynamicConfig3_AM_bit14 0x4000
#define EMCDynamicConfig3_AM_bit14_BIT 14
#define EMCDynamicConfig3_B_MASK 0x80000
#define EMCDynamicConfig3_B 0x80000
#define EMCDynamicConfig3_B_BIT 19
#define EMCDynamicConfig3_P_MASK 0x100000
#define EMCDynamicConfig3_P 0x100000
#define EMCDynamicConfig3_P_BIT 20

#define EMCDynamicRasCas3 (*(volatile unsigned *)0xFFE08164)
#define EMCDynamicRasCas3_OFFSET 0x164
#define EMCDynamicRasCas3_RAS_MASK 0x3
#define EMCDynamicRasCas3_RAS_BIT 0
#define EMCDynamicRasCas3_CAS_MASK 0x300
#define EMCDynamicRasCas3_CAS_BIT 8

#define EMCStaticConfig0 (*(volatile unsigned *)0xFFE08200)
#define EMCStaticConfig0_OFFSET 0x200
#define EMCStaticConfig0_MW_MASK 0x3
#define EMCStaticConfig0_MW_BIT 0
#define EMCStaticConfig0_PM_MASK 0x8
#define EMCStaticConfig0_PM 0x8
#define EMCStaticConfig0_PM_BIT 3
#define EMCStaticConfig0_PC_MASK 0x40
#define EMCStaticConfig0_PC 0x40
#define EMCStaticConfig0_PC_BIT 6
#define EMCStaticConfig0_PB_MASK 0x80
#define EMCStaticConfig0_PB 0x80
#define EMCStaticConfig0_PB_BIT 7
#define EMCStaticConfig0_EW_MASK 0x100
#define EMCStaticConfig0_EW 0x100
#define EMCStaticConfig0_EW_BIT 8
#define EMCStaticConfig0_B_MASK 0x80000
#define EMCStaticConfig0_B 0x80000
#define EMCStaticConfig0_B_BIT 19
#define EMCStaticConfig0_P_MASK 0x100000
#define EMCStaticConfig0_P 0x100000
#define EMCStaticConfig0_P_BIT 20

#define EMCStaticWaitWen0 (*(volatile unsigned *)0xFFE08204)
#define EMCStaticWaitWen0_OFFSET 0x204
#define EMCStaticWaitWen0_WAITWEN_MASK 0xF
#define EMCStaticWaitWen0_WAITWEN_BIT 0

#define EMCStaticWaitOen0 (*(volatile unsigned *)0xFFE08208)
#define EMCStaticWaitOen0_OFFSET 0x208
#define EMCStaticWaitOen0_WAITOEN_MASK 0xF
#define EMCStaticWaitOen0_WAITOEN_BIT 0

#define EMCStaticWaitRd0 (*(volatile unsigned *)0xFFE0820C)
#define EMCStaticWaitRd0_OFFSET 0x20C
#define EMCStaticWaitRd0_WAITRD_MASK 0x1F
#define EMCStaticWaitRd0_WAITRD_BIT 0

#define EMCStaticWaitPage0 (*(volatile unsigned *)0xFFE08210)
#define EMCStaticWaitPage0_OFFSET 0x210
#define EMCStaticWaitPage0_WAITPAGE_MASK 0x1F
#define EMCStaticWaitPage0_WAITPAGE_BIT 0

#define EMCStaticWaitWr0 (*(volatile unsigned *)0xFFE08214)
#define EMCStaticWaitWr0_OFFSET 0x214
#define EMCStaticWaitWr0_WAITWR_MASK 0x1F
#define EMCStaticWaitWr0_WAITWR_BIT 0

#define EMCStaticWaitTurn0 (*(volatile unsigned *)0xFFE08218)
#define EMCStaticWaitTurn0_OFFSET 0x218
#define EMCStaticWaitTurn0_WAITTURN_MASK 0xF
#define EMCStaticWaitTurn0_WAITTURN_BIT 0

#define EMCStaticConfig1 (*(volatile unsigned *)0xFFE08220)
#define EMCStaticConfig1_OFFSET 0x220
#define EMCStaticConfig1_MW_MASK 0x3
#define EMCStaticConfig1_MW_BIT 0
#define EMCStaticConfig1_PM_MASK 0x8
#define EMCStaticConfig1_PM 0x8
#define EMCStaticConfig1_PM_BIT 3
#define EMCStaticConfig1_PC_MASK 0x40
#define EMCStaticConfig1_PC 0x40
#define EMCStaticConfig1_PC_BIT 6
#define EMCStaticConfig1_PB_MASK 0x80
#define EMCStaticConfig1_PB 0x80
#define EMCStaticConfig1_PB_BIT 7
#define EMCStaticConfig1_EW_MASK 0x100
#define EMCStaticConfig1_EW 0x100
#define EMCStaticConfig1_EW_BIT 8
#define EMCStaticConfig1_B_MASK 0x80000
#define EMCStaticConfig1_B 0x80000
#define EMCStaticConfig1_B_BIT 19
#define EMCStaticConfig1_P_MASK 0x100000
#define EMCStaticConfig1_P 0x100000
#define EMCStaticConfig1_P_BIT 20

#define EMCStaticWaitWen1 (*(volatile unsigned *)0xFFE08224)
#define EMCStaticWaitWen1_OFFSET 0x224
#define EMCStaticWaitWen1_WAITWEN_MASK 0xF
#define EMCStaticWaitWen1_WAITWEN_BIT 0

#define EMCStaticWaitOen1 (*(volatile unsigned *)0xFFE08228)
#define EMCStaticWaitOen1_OFFSET 0x228
#define EMCStaticWaitOen1_WAITOEN_MASK 0xF
#define EMCStaticWaitOen1_WAITOEN_BIT 0

#define EMCStaticWaitRd1 (*(volatile unsigned *)0xFFE0822C)
#define EMCStaticWaitRd1_OFFSET 0x22C
#define EMCStaticWaitRd1_WAITRD_MASK 0x1F
#define EMCStaticWaitRd1_WAITRD_BIT 0

#define EMCStaticWaitPage1 (*(volatile unsigned *)0xFFE08230)
#define EMCStaticWaitPage1_OFFSET 0x230
#define EMCStaticWaitPage1_WAITPAGE_MASK 0x1F
#define EMCStaticWaitPage1_WAITPAGE_BIT 0

#define EMCStaticWaitWr1 (*(volatile unsigned *)0xFFE08234)
#define EMCStaticWaitWr1_OFFSET 0x234
#define EMCStaticWaitWr1_WAITWR_MASK 0x1F
#define EMCStaticWaitWr1_WAITWR_BIT 0

#define EMCStaticWaitTurn1 (*(volatile unsigned *)0xFFE08238)
#define EMCStaticWaitTurn1_OFFSET 0x238
#define EMCStaticWaitTurn1_WAITTURN_MASK 0xF
#define EMCStaticWaitTurn1_WAITTURN_BIT 0

#define EMCStaticConfig2 (*(volatile unsigned *)0xFFE08240)
#define EMCStaticConfig2_OFFSET 0x240
#define EMCStaticConfig2_MW_MASK 0x3
#define EMCStaticConfig2_MW_BIT 0
#define EMCStaticConfig2_PM_MASK 0x8
#define EMCStaticConfig2_PM 0x8
#define EMCStaticConfig2_PM_BIT 3
#define EMCStaticConfig2_PC_MASK 0x40
#define EMCStaticConfig2_PC 0x40
#define EMCStaticConfig2_PC_BIT 6
#define EMCStaticConfig2_PB_MASK 0x80
#define EMCStaticConfig2_PB 0x80
#define EMCStaticConfig2_PB_BIT 7
#define EMCStaticConfig2_EW_MASK 0x100
#define EMCStaticConfig2_EW 0x100
#define EMCStaticConfig2_EW_BIT 8
#define EMCStaticConfig2_B_MASK 0x80000
#define EMCStaticConfig2_B 0x80000
#define EMCStaticConfig2_B_BIT 19
#define EMCStaticConfig2_P_MASK 0x100000
#define EMCStaticConfig2_P 0x100000
#define EMCStaticConfig2_P_BIT 20

#define EMCStaticWaitWen2 (*(volatile unsigned *)0xFFE08244)
#define EMCStaticWaitWen2_OFFSET 0x244
#define EMCStaticWaitWen2_WAITWEN_MASK 0xF
#define EMCStaticWaitWen2_WAITWEN_BIT 0

#define EMCStaticWaitOen2 (*(volatile unsigned *)0xFFE08248)
#define EMCStaticWaitOen2_OFFSET 0x248
#define EMCStaticWaitOen2_WAITOEN_MASK 0xF
#define EMCStaticWaitOen2_WAITOEN_BIT 0

#define EMCStaticWaitRd2 (*(volatile unsigned *)0xFFE0824C)
#define EMCStaticWaitRd2_OFFSET 0x24C
#define EMCStaticWaitRd2_WAITRD_MASK 0x1F
#define EMCStaticWaitRd2_WAITRD_BIT 0

#define EMCStaticWaitPage2 (*(volatile unsigned *)0xFFE08250)
#define EMCStaticWaitPage2_OFFSET 0x250
#define EMCStaticWaitPage2_WAITPAGE_MASK 0x1F
#define EMCStaticWaitPage2_WAITPAGE_BIT 0

#define EMCStaticWaitWr2 (*(volatile unsigned *)0xFFE08254)
#define EMCStaticWaitWr2_OFFSET 0x254
#define EMCStaticWaitWr2_WAITWR_MASK 0x1F
#define EMCStaticWaitWr2_WAITWR_BIT 0

#define EMCStaticWaitTurn2 (*(volatile unsigned *)0xFFE08258)
#define EMCStaticWaitTurn2_OFFSET 0x258
#define EMCStaticWaitTurn2_WAITTURN_MASK 0xF
#define EMCStaticWaitTurn2_WAITTURN_BIT 0

#define EMCStaticConfig3 (*(volatile unsigned *)0xFFE08260)
#define EMCStaticConfig3_OFFSET 0x260
#define EMCStaticConfig3_MW_MASK 0x3
#define EMCStaticConfig3_MW_BIT 0
#define EMCStaticConfig3_PM_MASK 0x8
#define EMCStaticConfig3_PM 0x8
#define EMCStaticConfig3_PM_BIT 3
#define EMCStaticConfig3_PC_MASK 0x40
#define EMCStaticConfig3_PC 0x40
#define EMCStaticConfig3_PC_BIT 6
#define EMCStaticConfig3_PB_MASK 0x80
#define EMCStaticConfig3_PB 0x80
#define EMCStaticConfig3_PB_BIT 7
#define EMCStaticConfig3_EW_MASK 0x100
#define EMCStaticConfig3_EW 0x100
#define EMCStaticConfig3_EW_BIT 8
#define EMCStaticConfig3_B_MASK 0x80000
#define EMCStaticConfig3_B 0x80000
#define EMCStaticConfig3_B_BIT 19
#define EMCStaticConfig3_P_MASK 0x100000
#define EMCStaticConfig3_P 0x100000
#define EMCStaticConfig3_P_BIT 20

#define EMCStaticWaitWen3 (*(volatile unsigned *)0xFFE08264)
#define EMCStaticWaitWen3_OFFSET 0x264
#define EMCStaticWaitWen3_WAITWEN_MASK 0xF
#define EMCStaticWaitWen3_WAITWEN_BIT 0

#define EMCStaticWaitOen3 (*(volatile unsigned *)0xFFE08268)
#define EMCStaticWaitOen3_OFFSET 0x268
#define EMCStaticWaitOen3_WAITOEN_MASK 0xF
#define EMCStaticWaitOen3_WAITOEN_BIT 0

#define EMCStaticWaitRd3 (*(volatile unsigned *)0xFFE0826C)
#define EMCStaticWaitRd3_OFFSET 0x26C
#define EMCStaticWaitRd3_WAITRD_MASK 0x1F
#define EMCStaticWaitRd3_WAITRD_BIT 0

#define EMCStaticWaitPage3 (*(volatile unsigned *)0xFFE08270)
#define EMCStaticWaitPage3_OFFSET 0x270
#define EMCStaticWaitPage3_WAITPAGE_MASK 0x1F
#define EMCStaticWaitPage3_WAITPAGE_BIT 0

#define EMCStaticWaitWr3 (*(volatile unsigned *)0xFFE08274)
#define EMCStaticWaitWr3_OFFSET 0x274
#define EMCStaticWaitWr3_WAITWR_MASK 0x1F
#define EMCStaticWaitWr3_WAITWR_BIT 0

#define EMCStaticWaitTurn3 (*(volatile unsigned *)0xFFE08278)
#define EMCStaticWaitTurn3_OFFSET 0x278
#define EMCStaticWaitTurn3_WAITTURN_MASK 0xF
#define EMCStaticWaitTurn3_WAITTURN_BIT 0

#define USB_BASE 0xFFE0C000

#define OTGIntSt (*(volatile unsigned *)0xFFE0C100)
#define OTGIntSt_OFFSET 0x100
#define OTGIntSt_TMR_MASK 0x1
#define OTGIntSt_TMR 0x1
#define OTGIntSt_TMR_BIT 0
#define OTGIntSt_REMOVE_PU_MASK 0x2
#define OTGIntSt_REMOVE_PU 0x2
#define OTGIntSt_REMOVE_PU_BIT 1
#define OTGIntSt_HNP_FAILURE_MASK 0x4
#define OTGIntSt_HNP_FAILURE 0x4
#define OTGIntSt_HNP_FAILURE_BIT 2
#define OTGIntSt_HNP_SUCCESS_MASK 0x8
#define OTGIntSt_HNP_SUCCESS 0x8
#define OTGIntSt_HNP_SUCCESS_BIT 3

#define OTGIntEn (*(volatile unsigned *)0xFFE0C104)
#define OTGIntEn_OFFSET 0x104
#define OTGIntEn_TMR_MASK 0x1
#define OTGIntEn_TMR 0x1
#define OTGIntEn_TMR_BIT 0
#define OTGIntEn_REMOVE_PU_MASK 0x2
#define OTGIntEn_REMOVE_PU 0x2
#define OTGIntEn_REMOVE_PU_BIT 1
#define OTGIntEn_HNP_FAILURE_MASK 0x4
#define OTGIntEn_HNP_FAILURE 0x4
#define OTGIntEn_HNP_FAILURE_BIT 2
#define OTGIntEn_HNP_SUCCESS_MASK 0x8
#define OTGIntEn_HNP_SUCCESS 0x8
#define OTGIntEn_HNP_SUCCESS_BIT 3

#define OTGIntSet (*(volatile unsigned *)0xFFE0C108)
#define OTGIntSet_OFFSET 0x108
#define OTGIntSet_TMR_MASK 0x1
#define OTGIntSet_TMR 0x1
#define OTGIntSet_TMR_BIT 0
#define OTGIntSet_REMOVE_PU_MASK 0x2
#define OTGIntSet_REMOVE_PU 0x2
#define OTGIntSet_REMOVE_PU_BIT 1
#define OTGIntSet_HNP_FAILURE_MASK 0x4
#define OTGIntSet_HNP_FAILURE 0x4
#define OTGIntSet_HNP_FAILURE_BIT 2
#define OTGIntSet_HNP_SUCCESS_MASK 0x8
#define OTGIntSet_HNP_SUCCESS 0x8
#define OTGIntSet_HNP_SUCCESS_BIT 3

#define OTGIntClr (*(volatile unsigned *)0xFFE0C10C)
#define OTGIntClr_OFFSET 0x10C
#define OTGIntClr_TMR_MASK 0x1
#define OTGIntClr_TMR 0x1
#define OTGIntClr_TMR_BIT 0
#define OTGIntClr_REMOVE_PU_MASK 0x2
#define OTGIntClr_REMOVE_PU 0x2
#define OTGIntClr_REMOVE_PU_BIT 1
#define OTGIntClr_HNP_FAILURE_MASK 0x4
#define OTGIntClr_HNP_FAILURE 0x4
#define OTGIntClr_HNP_FAILURE_BIT 2
#define OTGIntClr_HNP_SUCCESS_MASK 0x8
#define OTGIntClr_HNP_SUCCESS 0x8
#define OTGIntClr_HNP_SUCCESS_BIT 3

#define OTGStCtrl (*(volatile unsigned *)0xFFE0C110)
#define OTGStCtrl_OFFSET 0x110
#define OTGStCtrl_PORT_FUNC_MASK 0x3
#define OTGStCtrl_PORT_FUNC_BIT 0
#define OTGStCtrl_TMR_SCALE_MASK 0xC
#define OTGStCtrl_TMR_SCALE_BIT 2
#define OTGStCtrl_TMR_MODE_MASK 0x10
#define OTGStCtrl_TMR_MODE 0x10
#define OTGStCtrl_TMR_MODE_BIT 4
#define OTGStCtrl_TMR_EN_MASK 0x20
#define OTGStCtrl_TMR_EN 0x20
#define OTGStCtrl_TMR_EN_BIT 5
#define OTGStCtrl_TMR_RST_MASK 0x40
#define OTGStCtrl_TMR_RST 0x40
#define OTGStCtrl_TMR_RST_BIT 6
#define OTGStCtrl_B_HNP_TRACK_MASK 0x100
#define OTGStCtrl_B_HNP_TRACK 0x100
#define OTGStCtrl_B_HNP_TRACK_BIT 8
#define OTGStCtrl_A_HNP_TRACK_MASK 0x200
#define OTGStCtrl_A_HNP_TRACK 0x200
#define OTGStCtrl_A_HNP_TRACK_BIT 9
#define OTGStCtrl_PU_REMOVED_MASK 0x400
#define OTGStCtrl_PU_REMOVED 0x400
#define OTGStCtrl_PU_REMOVED_BIT 10
#define OTGStCtrl_TMR_CNT_MASK 0xFFFF0000
#define OTGStCtrl_TMR_CNT_BIT 16

#define OTGTmr (*(volatile unsigned *)0xFFE0C114)
#define OTGTmr_OFFSET 0x114
#define OTGTmr_TIMEOUT_CNT_MASK 0xFFFF
#define OTGTmr_TIMEOUT_CNT_BIT 0

#define USBPortSel (*(volatile unsigned *)0xFFE0C110)
#define USBPortSel_OFFSET 0x110

#define USBDevIntSt (*(volatile unsigned *)0xFFE0C200)
#define USBDevIntSt_OFFSET 0x200
#define USBDevIntSt_FRAME_MASK 0x1
#define USBDevIntSt_FRAME 0x1
#define USBDevIntSt_FRAME_BIT 0
#define USBDevIntSt_EP_FAST_MASK 0x2
#define USBDevIntSt_EP_FAST 0x2
#define USBDevIntSt_EP_FAST_BIT 1
#define USBDevIntSt_EP_SLOW_MASK 0x4
#define USBDevIntSt_EP_SLOW 0x4
#define USBDevIntSt_EP_SLOW_BIT 2
#define USBDevIntSt_DEV_STAT_MASK 0x8
#define USBDevIntSt_DEV_STAT 0x8
#define USBDevIntSt_DEV_STAT_BIT 3
#define USBDevIntSt_CCEMTY_MASK 0x10
#define USBDevIntSt_CCEMTY 0x10
#define USBDevIntSt_CCEMTY_BIT 4
#define USBDevIntSt_CDFULL_MASK 0x20
#define USBDevIntSt_CDFULL 0x20
#define USBDevIntSt_CDFULL_BIT 5
#define USBDevIntSt_RxENDPKT_MASK 0x40
#define USBDevIntSt_RxENDPKT 0x40
#define USBDevIntSt_RxENDPKT_BIT 6
#define USBDevIntSt_TxENDPKT_MASK 0x80
#define USBDevIntSt_TxENDPKT 0x80
#define USBDevIntSt_TxENDPKT_BIT 7
#define USBDevIntSt_EP_RLZED_MASK 0x100
#define USBDevIntSt_EP_RLZED 0x100
#define USBDevIntSt_EP_RLZED_BIT 8
#define USBDevIntSt_ERR_INT_MASK 0x200
#define USBDevIntSt_ERR_INT 0x200
#define USBDevIntSt_ERR_INT_BIT 9

#define USBDevIntEn (*(volatile unsigned *)0xFFE0C204)
#define USBDevIntEn_OFFSET 0x204
#define USBDevIntEn_FRAME_MASK 0x1
#define USBDevIntEn_FRAME 0x1
#define USBDevIntEn_FRAME_BIT 0
#define USBDevIntEn_EP_FAST_MASK 0x2
#define USBDevIntEn_EP_FAST 0x2
#define USBDevIntEn_EP_FAST_BIT 1
#define USBDevIntEn_EP_SLOW_MASK 0x4
#define USBDevIntEn_EP_SLOW 0x4
#define USBDevIntEn_EP_SLOW_BIT 2
#define USBDevIntEn_DEV_STAT_MASK 0x8
#define USBDevIntEn_DEV_STAT 0x8
#define USBDevIntEn_DEV_STAT_BIT 3
#define USBDevIntEn_CCEMTY_MASK 0x10
#define USBDevIntEn_CCEMTY 0x10
#define USBDevIntEn_CCEMTY_BIT 4
#define USBDevIntEn_CDFULL_MASK 0x20
#define USBDevIntEn_CDFULL 0x20
#define USBDevIntEn_CDFULL_BIT 5
#define USBDevIntEn_RxENDPKT_MASK 0x40
#define USBDevIntEn_RxENDPKT 0x40
#define USBDevIntEn_RxENDPKT_BIT 6
#define USBDevIntEn_TxENDPKT_MASK 0x80
#define USBDevIntEn_TxENDPKT 0x80
#define USBDevIntEn_TxENDPKT_BIT 7
#define USBDevIntEn_EP_RLZED_MASK 0x100
#define USBDevIntEn_EP_RLZED 0x100
#define USBDevIntEn_EP_RLZED_BIT 8
#define USBDevIntEn_ERR_INT_MASK 0x200
#define USBDevIntEn_ERR_INT 0x200
#define USBDevIntEn_ERR_INT_BIT 9

#define USBDevIntClr (*(volatile unsigned *)0xFFE0C208)
#define USBDevIntClr_OFFSET 0x208
#define USBDevIntClr_FRAME_MASK 0x1
#define USBDevIntClr_FRAME 0x1
#define USBDevIntClr_FRAME_BIT 0
#define USBDevIntClr_EP_FAST_MASK 0x2
#define USBDevIntClr_EP_FAST 0x2
#define USBDevIntClr_EP_FAST_BIT 1
#define USBDevIntClr_EP_SLOW_MASK 0x4
#define USBDevIntClr_EP_SLOW 0x4
#define USBDevIntClr_EP_SLOW_BIT 2
#define USBDevIntClr_DEV_STAT_MASK 0x8
#define USBDevIntClr_DEV_STAT 0x8
#define USBDevIntClr_DEV_STAT_BIT 3
#define USBDevIntClr_CCEMTY_MASK 0x10
#define USBDevIntClr_CCEMTY 0x10
#define USBDevIntClr_CCEMTY_BIT 4
#define USBDevIntClr_CDFULL_MASK 0x20
#define USBDevIntClr_CDFULL 0x20
#define USBDevIntClr_CDFULL_BIT 5
#define USBDevIntClr_RxENDPKT_MASK 0x40
#define USBDevIntClr_RxENDPKT 0x40
#define USBDevIntClr_RxENDPKT_BIT 6
#define USBDevIntClr_TxENDPKT_MASK 0x80
#define USBDevIntClr_TxENDPKT 0x80
#define USBDevIntClr_TxENDPKT_BIT 7
#define USBDevIntClr_EP_RLZED_MASK 0x100
#define USBDevIntClr_EP_RLZED 0x100
#define USBDevIntClr_EP_RLZED_BIT 8
#define USBDevIntClr_EPR_INT_MASK 0x200
#define USBDevIntClr_EPR_INT 0x200
#define USBDevIntClr_EPR_INT_BIT 9

#define USBDevIntSet (*(volatile unsigned *)0xFFE0C20C)
#define USBDevIntSet_OFFSET 0x20C
#define USBDevIntSet_FRAME_MASK 0x1
#define USBDevIntSet_FRAME 0x1
#define USBDevIntSet_FRAME_BIT 0
#define USBDevIntSet_EP_FAST_MASK 0x2
#define USBDevIntSet_EP_FAST 0x2
#define USBDevIntSet_EP_FAST_BIT 1
#define USBDevIntSet_EP_SLOW_MASK 0x4
#define USBDevIntSet_EP_SLOW 0x4
#define USBDevIntSet_EP_SLOW_BIT 2
#define USBDevIntSet_DEV_STAT_MASK 0x8
#define USBDevIntSet_DEV_STAT 0x8
#define USBDevIntSet_DEV_STAT_BIT 3
#define USBDevIntSet_CCEMTY_MASK 0x10
#define USBDevIntSet_CCEMTY 0x10
#define USBDevIntSet_CCEMTY_BIT 4
#define USBDevIntSet_CDFULL_MASK 0x20
#define USBDevIntSet_CDFULL 0x20
#define USBDevIntSet_CDFULL_BIT 5
#define USBDevIntSet_RxENDPKT_MASK 0x40
#define USBDevIntSet_RxENDPKT 0x40
#define USBDevIntSet_RxENDPKT_BIT 6
#define USBDevIntSet_TxENDPKT_MASK 0x80
#define USBDevIntSet_TxENDPKT 0x80
#define USBDevIntSet_TxENDPKT_BIT 7
#define USBDevIntSet_EP_RLZED_MASK 0x100
#define USBDevIntSet_EP_RLZED 0x100
#define USBDevIntSet_EP_RLZED_BIT 8
#define USBDevIntSet_EPR_INT_MASK 0x200
#define USBDevIntSet_EPR_INT 0x200
#define USBDevIntSet_EPR_INT_BIT 9

#define USBCmdCode (*(volatile unsigned *)0xFFE0C210)
#define USBCmdCode_OFFSET 0x210
#define USBCmdCode_CMD_PHASE_MASK 0xFF00
#define USBCmdCode_CMD_PHASE_BIT 8
#define USBCmdCode_CMD_CODE_MASK 0x7F0000
#define USBCmdCode_CMD_CODE_BIT 16

#define USBCmdData (*(volatile unsigned *)0xFFE0C214)
#define USBCmdData_OFFSET 0x214
#define USBCmdData_CommandData_MASK 0xFF
#define USBCmdData_CommandData_BIT 0

#define USBRxData (*(volatile unsigned *)0xFFE0C218)
#define USBRxData_OFFSET 0x218

#define USBTxData (*(volatile unsigned *)0xFFE0C21C)
#define USBTxData_OFFSET 0x21C

#define USBRxPLen (*(volatile unsigned *)0xFFE0C220)
#define USBRxPLen_OFFSET 0x220
#define USBRxPLen_PKT_LNGTH_MASK 0x3FF
#define USBRxPLen_PKT_LNGTH_BIT 0
#define USBRxPLen_DV_MASK 0x400
#define USBRxPLen_DV 0x400
#define USBRxPLen_DV_BIT 10
#define USBRxPLen_PKT_RDY_MASK 0x800
#define USBRxPLen_PKT_RDY 0x800
#define USBRxPLen_PKT_RDY_BIT 11

#define USBTxPLen (*(volatile unsigned *)0xFFE0C224)
#define USBTxPLen_OFFSET 0x224
#define USBTxPLen_PKT_LNGTH_MASK 0x3FF
#define USBTxPLen_PKT_LNGTH_BIT 0

#define USBCtrl (*(volatile unsigned *)0xFFE0C228)
#define USBCtrl_OFFSET 0x228
#define USBCtrl_RD_EN_MASK 0x1
#define USBCtrl_RD_EN 0x1
#define USBCtrl_RD_EN_BIT 0
#define USBCtrl_WR_EN_MASK 0x2
#define USBCtrl_WR_EN 0x2
#define USBCtrl_WR_EN_BIT 1
#define USBCtrl_LOG_ENDPOINT_MASK 0x3C
#define USBCtrl_LOG_ENDPOINT_BIT 2

#define USBDevIntPri (*(volatile unsigned *)0xFFE0C22C)
#define USBDevIntPri_OFFSET 0x22C

#define USBEpIntSt (*(volatile unsigned *)0xFFE0C230)
#define USBEpIntSt_OFFSET 0x230
#define USBEpIntSt_EP0RX_MASK 0x1
#define USBEpIntSt_EP0RX 0x1
#define USBEpIntSt_EP0RX_BIT 0
#define USBEpIntSt_EP0TX_MASK 0x2
#define USBEpIntSt_EP0TX 0x2
#define USBEpIntSt_EP0TX_BIT 1
#define USBEpIntSt_EP1RX_MASK 0x4
#define USBEpIntSt_EP1RX 0x4
#define USBEpIntSt_EP1RX_BIT 2
#define USBEpIntSt_EP1TX_MASK 0x8
#define USBEpIntSt_EP1TX 0x8
#define USBEpIntSt_EP1TX_BIT 3
#define USBEpIntSt_EP2RX_MASK 0x10
#define USBEpIntSt_EP2RX 0x10
#define USBEpIntSt_EP2RX_BIT 4
#define USBEpIntSt_EP2TX_MASK 0x20
#define USBEpIntSt_EP2TX 0x20
#define USBEpIntSt_EP2TX_BIT 5
#define USBEpIntSt_EP3RX_MASK 0x40
#define USBEpIntSt_EP3RX 0x40
#define USBEpIntSt_EP3RX_BIT 6
#define USBEpIntSt_EP3TX_MASK 0x80
#define USBEpIntSt_EP3TX 0x80
#define USBEpIntSt_EP3TX_BIT 7
#define USBEpIntSt_EP4RX_MASK 0x100
#define USBEpIntSt_EP4RX 0x100
#define USBEpIntSt_EP4RX_BIT 8
#define USBEpIntSt_EP4TX_MASK 0x200
#define USBEpIntSt_EP4TX 0x200
#define USBEpIntSt_EP4TX_BIT 9
#define USBEpIntSt_EP5RX_MASK 0x400
#define USBEpIntSt_EP5RX 0x400
#define USBEpIntSt_EP5RX_BIT 10
#define USBEpIntSt_EP5TX_MASK 0x800
#define USBEpIntSt_EP5TX 0x800
#define USBEpIntSt_EP5TX_BIT 11
#define USBEpIntSt_EP6RX_MASK 0x1000
#define USBEpIntSt_EP6RX 0x1000
#define USBEpIntSt_EP6RX_BIT 12
#define USBEpIntSt_EP6TX_MASK 0x2000
#define USBEpIntSt_EP6TX 0x2000
#define USBEpIntSt_EP6TX_BIT 13
#define USBEpIntSt_EP7RX_MASK 0x4000
#define USBEpIntSt_EP7RX 0x4000
#define USBEpIntSt_EP7RX_BIT 14
#define USBEpIntSt_EP7TX_MASK 0x8000
#define USBEpIntSt_EP7TX 0x8000
#define USBEpIntSt_EP7TX_BIT 15
#define USBEpIntSt_EP8RX_MASK 0x10000
#define USBEpIntSt_EP8RX 0x10000
#define USBEpIntSt_EP8RX_BIT 16
#define USBEpIntSt_EP8TX_MASK 0x20000
#define USBEpIntSt_EP8TX 0x20000
#define USBEpIntSt_EP8TX_BIT 17
#define USBEpIntSt_EP9RX_MASK 0x40000
#define USBEpIntSt_EP9RX 0x40000
#define USBEpIntSt_EP9RX_BIT 18
#define USBEpIntSt_EP9TX_MASK 0x80000
#define USBEpIntSt_EP9TX 0x80000
#define USBEpIntSt_EP9TX_BIT 19
#define USBEpIntSt_EP10RX_MASK 0x100000
#define USBEpIntSt_EP10RX 0x100000
#define USBEpIntSt_EP10RX_BIT 20
#define USBEpIntSt_EP10TX_MASK 0x200000
#define USBEpIntSt_EP10TX 0x200000
#define USBEpIntSt_EP10TX_BIT 21
#define USBEpIntSt_EP11RX_MASK 0x400000
#define USBEpIntSt_EP11RX 0x400000
#define USBEpIntSt_EP11RX_BIT 22
#define USBEpIntSt_EP11TX_MASK 0x800000
#define USBEpIntSt_EP11TX 0x800000
#define USBEpIntSt_EP11TX_BIT 23
#define USBEpIntSt_EP12RX_MASK 0x1000000
#define USBEpIntSt_EP12RX 0x1000000
#define USBEpIntSt_EP12RX_BIT 24
#define USBEpIntSt_EP12TX_MASK 0x2000000
#define USBEpIntSt_EP12TX 0x2000000
#define USBEpIntSt_EP12TX_BIT 25
#define USBEpIntSt_EP13RX_MASK 0x4000000
#define USBEpIntSt_EP13RX 0x4000000
#define USBEpIntSt_EP13RX_BIT 26
#define USBEpIntSt_EP13TX_MASK 0x8000000
#define USBEpIntSt_EP13TX 0x8000000
#define USBEpIntSt_EP13TX_BIT 27
#define USBEpIntSt_EP14RX_MASK 0x10000000
#define USBEpIntSt_EP14RX 0x10000000
#define USBEpIntSt_EP14RX_BIT 28
#define USBEpIntSt_EP14TX_MASK 0x20000000
#define USBEpIntSt_EP14TX 0x20000000
#define USBEpIntSt_EP14TX_BIT 29
#define USBEpIntSt_EP15RX_MASK 0x40000000
#define USBEpIntSt_EP15RX 0x40000000
#define USBEpIntSt_EP15RX_BIT 30
#define USBEpIntSt_EP15TX_MASK 0x80000000
#define USBEpIntSt_EP15TX 0x80000000
#define USBEpIntSt_EP15TX_BIT 31

#define USBEpIntEn (*(volatile unsigned *)0xFFE0C234)
#define USBEpIntEn_OFFSET 0x234
#define USBEpIntEn_EP0RX_MASK 0x1
#define USBEpIntEn_EP0RX 0x1
#define USBEpIntEn_EP0RX_BIT 0
#define USBEpIntEn_EP0TX_MASK 0x2
#define USBEpIntEn_EP0TX 0x2
#define USBEpIntEn_EP0TX_BIT 1
#define USBEpIntEn_EP1RX_MASK 0x4
#define USBEpIntEn_EP1RX 0x4
#define USBEpIntEn_EP1RX_BIT 2
#define USBEpIntEn_EP1TX_MASK 0x8
#define USBEpIntEn_EP1TX 0x8
#define USBEpIntEn_EP1TX_BIT 3
#define USBEpIntEn_EP2RX_MASK 0x10
#define USBEpIntEn_EP2RX 0x10
#define USBEpIntEn_EP2RX_BIT 4
#define USBEpIntEn_EP2TX_MASK 0x20
#define USBEpIntEn_EP2TX 0x20
#define USBEpIntEn_EP2TX_BIT 5
#define USBEpIntEn_EP3RX_MASK 0x40
#define USBEpIntEn_EP3RX 0x40
#define USBEpIntEn_EP3RX_BIT 6
#define USBEpIntEn_EP3TX_MASK 0x80
#define USBEpIntEn_EP3TX 0x80
#define USBEpIntEn_EP3TX_BIT 7
#define USBEpIntEn_EP4RX_MASK 0x100
#define USBEpIntEn_EP4RX 0x100
#define USBEpIntEn_EP4RX_BIT 8
#define USBEpIntEn_EP4TX_MASK 0x200
#define USBEpIntEn_EP4TX 0x200
#define USBEpIntEn_EP4TX_BIT 9
#define USBEpIntEn_EP5RX_MASK 0x400
#define USBEpIntEn_EP5RX 0x400
#define USBEpIntEn_EP5RX_BIT 10
#define USBEpIntEn_EP5TX_MASK 0x800
#define USBEpIntEn_EP5TX 0x800
#define USBEpIntEn_EP5TX_BIT 11
#define USBEpIntEn_EP6RX_MASK 0x1000
#define USBEpIntEn_EP6RX 0x1000
#define USBEpIntEn_EP6RX_BIT 12
#define USBEpIntEn_EP6TX_MASK 0x2000
#define USBEpIntEn_EP6TX 0x2000
#define USBEpIntEn_EP6TX_BIT 13
#define USBEpIntEn_EP7RX_MASK 0x4000
#define USBEpIntEn_EP7RX 0x4000
#define USBEpIntEn_EP7RX_BIT 14
#define USBEpIntEn_EP7TX_MASK 0x8000
#define USBEpIntEn_EP7TX 0x8000
#define USBEpIntEn_EP7TX_BIT 15
#define USBEpIntEn_EP8RX_MASK 0x10000
#define USBEpIntEn_EP8RX 0x10000
#define USBEpIntEn_EP8RX_BIT 16
#define USBEpIntEn_EP8TX_MASK 0x20000
#define USBEpIntEn_EP8TX 0x20000
#define USBEpIntEn_EP8TX_BIT 17
#define USBEpIntEn_EP9RX_MASK 0x40000
#define USBEpIntEn_EP9RX 0x40000
#define USBEpIntEn_EP9RX_BIT 18
#define USBEpIntEn_EP9TX_MASK 0x80000
#define USBEpIntEn_EP9TX 0x80000
#define USBEpIntEn_EP9TX_BIT 19
#define USBEpIntEn_EP10RX_MASK 0x100000
#define USBEpIntEn_EP10RX 0x100000
#define USBEpIntEn_EP10RX_BIT 20
#define USBEpIntEn_EP10TX_MASK 0x200000
#define USBEpIntEn_EP10TX 0x200000
#define USBEpIntEn_EP10TX_BIT 21
#define USBEpIntEn_EP11RX_MASK 0x400000
#define USBEpIntEn_EP11RX 0x400000
#define USBEpIntEn_EP11RX_BIT 22
#define USBEpIntEn_EP11TX_MASK 0x800000
#define USBEpIntEn_EP11TX 0x800000
#define USBEpIntEn_EP11TX_BIT 23
#define USBEpIntEn_EP12RX_MASK 0x1000000
#define USBEpIntEn_EP12RX 0x1000000
#define USBEpIntEn_EP12RX_BIT 24
#define USBEpIntEn_EP12TX_MASK 0x2000000
#define USBEpIntEn_EP12TX 0x2000000
#define USBEpIntEn_EP12TX_BIT 25
#define USBEpIntEn_EP13RX_MASK 0x4000000
#define USBEpIntEn_EP13RX 0x4000000
#define USBEpIntEn_EP13RX_BIT 26
#define USBEpIntEn_EP13TX_MASK 0x8000000
#define USBEpIntEn_EP13TX 0x8000000
#define USBEpIntEn_EP13TX_BIT 27
#define USBEpIntEn_EP14RX_MASK 0x10000000
#define USBEpIntEn_EP14RX 0x10000000
#define USBEpIntEn_EP14RX_BIT 28
#define USBEpIntEn_EP14TX_MASK 0x20000000
#define USBEpIntEn_EP14TX 0x20000000
#define USBEpIntEn_EP14TX_BIT 29
#define USBEpIntEn_EP15RX_MASK 0x40000000
#define USBEpIntEn_EP15RX 0x40000000
#define USBEpIntEn_EP15RX_BIT 30
#define USBEpIntEn_EP15TX_MASK 0x80000000
#define USBEpIntEn_EP15TX 0x80000000
#define USBEpIntEn_EP15TX_BIT 31

#define USBEpIntClr (*(volatile unsigned *)0xFFE0C238)
#define USBEpIntClr_OFFSET 0x238
#define USBEpIntClr_EP0RX_MASK 0x1
#define USBEpIntClr_EP0RX 0x1
#define USBEpIntClr_EP0RX_BIT 0
#define USBEpIntClr_EP0TX_MASK 0x2
#define USBEpIntClr_EP0TX 0x2
#define USBEpIntClr_EP0TX_BIT 1
#define USBEpIntClr_EP1RX_MASK 0x4
#define USBEpIntClr_EP1RX 0x4
#define USBEpIntClr_EP1RX_BIT 2
#define USBEpIntClr_EP1TX_MASK 0x8
#define USBEpIntClr_EP1TX 0x8
#define USBEpIntClr_EP1TX_BIT 3
#define USBEpIntClr_EP2RX_MASK 0x10
#define USBEpIntClr_EP2RX 0x10
#define USBEpIntClr_EP2RX_BIT 4
#define USBEpIntClr_EP2TX_MASK 0x20
#define USBEpIntClr_EP2TX 0x20
#define USBEpIntClr_EP2TX_BIT 5
#define USBEpIntClr_EP3RX_MASK 0x40
#define USBEpIntClr_EP3RX 0x40
#define USBEpIntClr_EP3RX_BIT 6
#define USBEpIntClr_EP3TX_MASK 0x80
#define USBEpIntClr_EP3TX 0x80
#define USBEpIntClr_EP3TX_BIT 7
#define USBEpIntClr_EP4RX_MASK 0x100
#define USBEpIntClr_EP4RX 0x100
#define USBEpIntClr_EP4RX_BIT 8
#define USBEpIntClr_EP4TX_MASK 0x200
#define USBEpIntClr_EP4TX 0x200
#define USBEpIntClr_EP4TX_BIT 9
#define USBEpIntClr_EP5RX_MASK 0x400
#define USBEpIntClr_EP5RX 0x400
#define USBEpIntClr_EP5RX_BIT 10
#define USBEpIntClr_EP5TX_MASK 0x800
#define USBEpIntClr_EP5TX 0x800
#define USBEpIntClr_EP5TX_BIT 11
#define USBEpIntClr_EP6RX_MASK 0x1000
#define USBEpIntClr_EP6RX 0x1000
#define USBEpIntClr_EP6RX_BIT 12
#define USBEpIntClr_EP6TX_MASK 0x2000
#define USBEpIntClr_EP6TX 0x2000
#define USBEpIntClr_EP6TX_BIT 13
#define USBEpIntClr_EP7RX_MASK 0x4000
#define USBEpIntClr_EP7RX 0x4000
#define USBEpIntClr_EP7RX_BIT 14
#define USBEpIntClr_EP7TX_MASK 0x8000
#define USBEpIntClr_EP7TX 0x8000
#define USBEpIntClr_EP7TX_BIT 15
#define USBEpIntClr_EP8RX_MASK 0x10000
#define USBEpIntClr_EP8RX 0x10000
#define USBEpIntClr_EP8RX_BIT 16
#define USBEpIntClr_EP8TX_MASK 0x20000
#define USBEpIntClr_EP8TX 0x20000
#define USBEpIntClr_EP8TX_BIT 17
#define USBEpIntClr_EP9RX_MASK 0x40000
#define USBEpIntClr_EP9RX 0x40000
#define USBEpIntClr_EP9RX_BIT 18
#define USBEpIntClr_EP9TX_MASK 0x80000
#define USBEpIntClr_EP9TX 0x80000
#define USBEpIntClr_EP9TX_BIT 19
#define USBEpIntClr_EP10RX_MASK 0x100000
#define USBEpIntClr_EP10RX 0x100000
#define USBEpIntClr_EP10RX_BIT 20
#define USBEpIntClr_EP10TX_MASK 0x200000
#define USBEpIntClr_EP10TX 0x200000
#define USBEpIntClr_EP10TX_BIT 21
#define USBEpIntClr_EP11RX_MASK 0x400000
#define USBEpIntClr_EP11RX 0x400000
#define USBEpIntClr_EP11RX_BIT 22
#define USBEpIntClr_EP11TX_MASK 0x800000
#define USBEpIntClr_EP11TX 0x800000
#define USBEpIntClr_EP11TX_BIT 23
#define USBEpIntClr_EP12RX_MASK 0x1000000
#define USBEpIntClr_EP12RX 0x1000000
#define USBEpIntClr_EP12RX_BIT 24
#define USBEpIntClr_EP12TX_MASK 0x2000000
#define USBEpIntClr_EP12TX 0x2000000
#define USBEpIntClr_EP12TX_BIT 25
#define USBEpIntClr_EP13RX_MASK 0x4000000
#define USBEpIntClr_EP13RX 0x4000000
#define USBEpIntClr_EP13RX_BIT 26
#define USBEpIntClr_EP13TX_MASK 0x8000000
#define USBEpIntClr_EP13TX 0x8000000
#define USBEpIntClr_EP13TX_BIT 27
#define USBEpIntClr_EP14RX_MASK 0x10000000
#define USBEpIntClr_EP14RX 0x10000000
#define USBEpIntClr_EP14RX_BIT 28
#define USBEpIntClr_EP14TX_MASK 0x20000000
#define USBEpIntClr_EP14TX 0x20000000
#define USBEpIntClr_EP14TX_BIT 29
#define USBEpIntClr_EP15RX_MASK 0x40000000
#define USBEpIntClr_EP15RX 0x40000000
#define USBEpIntClr_EP15RX_BIT 30
#define USBEpIntClr_EP15TX_MASK 0x80000000
#define USBEpIntClr_EP15TX 0x80000000
#define USBEpIntClr_EP15TX_BIT 31

#define USBEpIntSet (*(volatile unsigned *)0xFFE0C23C)
#define USBEpIntSet_OFFSET 0x23C
#define USBEpIntSet_EP0RX_MASK 0x1
#define USBEpIntSet_EP0RX 0x1
#define USBEpIntSet_EP0RX_BIT 0
#define USBEpIntSet_EP0TX_MASK 0x2
#define USBEpIntSet_EP0TX 0x2
#define USBEpIntSet_EP0TX_BIT 1
#define USBEpIntSet_EP1RX_MASK 0x4
#define USBEpIntSet_EP1RX 0x4
#define USBEpIntSet_EP1RX_BIT 2
#define USBEpIntSet_EP1TX_MASK 0x8
#define USBEpIntSet_EP1TX 0x8
#define USBEpIntSet_EP1TX_BIT 3
#define USBEpIntSet_EP2RX_MASK 0x10
#define USBEpIntSet_EP2RX 0x10
#define USBEpIntSet_EP2RX_BIT 4
#define USBEpIntSet_EP2TX_MASK 0x20
#define USBEpIntSet_EP2TX 0x20
#define USBEpIntSet_EP2TX_BIT 5
#define USBEpIntSet_EP3RX_MASK 0x40
#define USBEpIntSet_EP3RX 0x40
#define USBEpIntSet_EP3RX_BIT 6
#define USBEpIntSet_EP3TX_MASK 0x80
#define USBEpIntSet_EP3TX 0x80
#define USBEpIntSet_EP3TX_BIT 7
#define USBEpIntSet_EP4RX_MASK 0x100
#define USBEpIntSet_EP4RX 0x100
#define USBEpIntSet_EP4RX_BIT 8
#define USBEpIntSet_EP4TX_MASK 0x200
#define USBEpIntSet_EP4TX 0x200
#define USBEpIntSet_EP4TX_BIT 9
#define USBEpIntSet_EP5RX_MASK 0x400
#define USBEpIntSet_EP5RX 0x400
#define USBEpIntSet_EP5RX_BIT 10
#define USBEpIntSet_EP5TX_MASK 0x800
#define USBEpIntSet_EP5TX 0x800
#define USBEpIntSet_EP5TX_BIT 11
#define USBEpIntSet_EP6RX_MASK 0x1000
#define USBEpIntSet_EP6RX 0x1000
#define USBEpIntSet_EP6RX_BIT 12
#define USBEpIntSet_EP6TX_MASK 0x2000
#define USBEpIntSet_EP6TX 0x2000
#define USBEpIntSet_EP6TX_BIT 13
#define USBEpIntSet_EP7RX_MASK 0x4000
#define USBEpIntSet_EP7RX 0x4000
#define USBEpIntSet_EP7RX_BIT 14
#define USBEpIntSet_EP7TX_MASK 0x8000
#define USBEpIntSet_EP7TX 0x8000
#define USBEpIntSet_EP7TX_BIT 15
#define USBEpIntSet_EP8RX_MASK 0x10000
#define USBEpIntSet_EP8RX 0x10000
#define USBEpIntSet_EP8RX_BIT 16
#define USBEpIntSet_EP8TX_MASK 0x20000
#define USBEpIntSet_EP8TX 0x20000
#define USBEpIntSet_EP8TX_BIT 17
#define USBEpIntSet_EP9RX_MASK 0x40000
#define USBEpIntSet_EP9RX 0x40000
#define USBEpIntSet_EP9RX_BIT 18
#define USBEpIntSet_EP9TX_MASK 0x80000
#define USBEpIntSet_EP9TX 0x80000
#define USBEpIntSet_EP9TX_BIT 19
#define USBEpIntSet_EP10RX_MASK 0x100000
#define USBEpIntSet_EP10RX 0x100000
#define USBEpIntSet_EP10RX_BIT 20
#define USBEpIntSet_EP10TX_MASK 0x200000
#define USBEpIntSet_EP10TX 0x200000
#define USBEpIntSet_EP10TX_BIT 21
#define USBEpIntSet_EP11RX_MASK 0x400000
#define USBEpIntSet_EP11RX 0x400000
#define USBEpIntSet_EP11RX_BIT 22
#define USBEpIntSet_EP11TX_MASK 0x800000
#define USBEpIntSet_EP11TX 0x800000
#define USBEpIntSet_EP11TX_BIT 23
#define USBEpIntSet_EP12RX_MASK 0x1000000
#define USBEpIntSet_EP12RX 0x1000000
#define USBEpIntSet_EP12RX_BIT 24
#define USBEpIntSet_EP12TX_MASK 0x2000000
#define USBEpIntSet_EP12TX 0x2000000
#define USBEpIntSet_EP12TX_BIT 25
#define USBEpIntSet_EP13RX_MASK 0x4000000
#define USBEpIntSet_EP13RX 0x4000000
#define USBEpIntSet_EP13RX_BIT 26
#define USBEpIntSet_EP13TX_MASK 0x8000000
#define USBEpIntSet_EP13TX 0x8000000
#define USBEpIntSet_EP13TX_BIT 27
#define USBEpIntSet_EP14RX_MASK 0x10000000
#define USBEpIntSet_EP14RX 0x10000000
#define USBEpIntSet_EP14RX_BIT 28
#define USBEpIntSet_EP14TX_MASK 0x20000000
#define USBEpIntSet_EP14TX 0x20000000
#define USBEpIntSet_EP14TX_BIT 29
#define USBEpIntSet_EP15RX_MASK 0x40000000
#define USBEpIntSet_EP15RX 0x40000000
#define USBEpIntSet_EP15RX_BIT 30
#define USBEpIntSet_EP15TX_MASK 0x80000000
#define USBEpIntSet_EP15TX 0x80000000
#define USBEpIntSet_EP15TX_BIT 31

#define USBEpIntPri (*(volatile unsigned *)0xFFE0C240)
#define USBEpIntPri_OFFSET 0x240
#define USBEpIntPri_EP0RX_MASK 0x1
#define USBEpIntPri_EP0RX 0x1
#define USBEpIntPri_EP0RX_BIT 0
#define USBEpIntPri_EP0TX_MASK 0x2
#define USBEpIntPri_EP0TX 0x2
#define USBEpIntPri_EP0TX_BIT 1
#define USBEpIntPri_EP1RX_MASK 0x4
#define USBEpIntPri_EP1RX 0x4
#define USBEpIntPri_EP1RX_BIT 2
#define USBEpIntPri_EP1TX_MASK 0x8
#define USBEpIntPri_EP1TX 0x8
#define USBEpIntPri_EP1TX_BIT 3
#define USBEpIntPri_EP2RX_MASK 0x10
#define USBEpIntPri_EP2RX 0x10
#define USBEpIntPri_EP2RX_BIT 4
#define USBEpIntPri_EP2TX_MASK 0x20
#define USBEpIntPri_EP2TX 0x20
#define USBEpIntPri_EP2TX_BIT 5
#define USBEpIntPri_EP3RX_MASK 0x40
#define USBEpIntPri_EP3RX 0x40
#define USBEpIntPri_EP3RX_BIT 6
#define USBEpIntPri_EP3TX_MASK 0x80
#define USBEpIntPri_EP3TX 0x80
#define USBEpIntPri_EP3TX_BIT 7
#define USBEpIntPri_EP4RX_MASK 0x100
#define USBEpIntPri_EP4RX 0x100
#define USBEpIntPri_EP4RX_BIT 8
#define USBEpIntPri_EP4TX_MASK 0x200
#define USBEpIntPri_EP4TX 0x200
#define USBEpIntPri_EP4TX_BIT 9
#define USBEpIntPri_EP5RX_MASK 0x400
#define USBEpIntPri_EP5RX 0x400
#define USBEpIntPri_EP5RX_BIT 10
#define USBEpIntPri_EP5TX_MASK 0x800
#define USBEpIntPri_EP5TX 0x800
#define USBEpIntPri_EP5TX_BIT 11
#define USBEpIntPri_EP6RX_MASK 0x1000
#define USBEpIntPri_EP6RX 0x1000
#define USBEpIntPri_EP6RX_BIT 12
#define USBEpIntPri_EP6TX_MASK 0x2000
#define USBEpIntPri_EP6TX 0x2000
#define USBEpIntPri_EP6TX_BIT 13
#define USBEpIntPri_EP7RX_MASK 0x4000
#define USBEpIntPri_EP7RX 0x4000
#define USBEpIntPri_EP7RX_BIT 14
#define USBEpIntPri_EP7TX_MASK 0x8000
#define USBEpIntPri_EP7TX 0x8000
#define USBEpIntPri_EP7TX_BIT 15
#define USBEpIntPri_EP8RX_MASK 0x10000
#define USBEpIntPri_EP8RX 0x10000
#define USBEpIntPri_EP8RX_BIT 16
#define USBEpIntPri_EP8TX_MASK 0x20000
#define USBEpIntPri_EP8TX 0x20000
#define USBEpIntPri_EP8TX_BIT 17
#define USBEpIntPri_EP9RX_MASK 0x40000
#define USBEpIntPri_EP9RX 0x40000
#define USBEpIntPri_EP9RX_BIT 18
#define USBEpIntPri_EP9TX_MASK 0x80000
#define USBEpIntPri_EP9TX 0x80000
#define USBEpIntPri_EP9TX_BIT 19
#define USBEpIntPri_EP10RX_MASK 0x100000
#define USBEpIntPri_EP10RX 0x100000
#define USBEpIntPri_EP10RX_BIT 20
#define USBEpIntPri_EP10TX_MASK 0x200000
#define USBEpIntPri_EP10TX 0x200000
#define USBEpIntPri_EP10TX_BIT 21
#define USBEpIntPri_EP11RX_MASK 0x400000
#define USBEpIntPri_EP11RX 0x400000
#define USBEpIntPri_EP11RX_BIT 22
#define USBEpIntPri_EP11TX_MASK 0x800000
#define USBEpIntPri_EP11TX 0x800000
#define USBEpIntPri_EP11TX_BIT 23
#define USBEpIntPri_EP12RX_MASK 0x1000000
#define USBEpIntPri_EP12RX 0x1000000
#define USBEpIntPri_EP12RX_BIT 24
#define USBEpIntPri_EP12TX_MASK 0x2000000
#define USBEpIntPri_EP12TX 0x2000000
#define USBEpIntPri_EP12TX_BIT 25
#define USBEpIntPri_EP13RX_MASK 0x4000000
#define USBEpIntPri_EP13RX 0x4000000
#define USBEpIntPri_EP13RX_BIT 26
#define USBEpIntPri_EP13TX_MASK 0x8000000
#define USBEpIntPri_EP13TX 0x8000000
#define USBEpIntPri_EP13TX_BIT 27
#define USBEpIntPri_EP14RX_MASK 0x10000000
#define USBEpIntPri_EP14RX 0x10000000
#define USBEpIntPri_EP14RX_BIT 28
#define USBEpIntPri_EP14TX_MASK 0x20000000
#define USBEpIntPri_EP14TX 0x20000000
#define USBEpIntPri_EP14TX_BIT 29
#define USBEpIntPri_EP15RX_MASK 0x40000000
#define USBEpIntPri_EP15RX 0x40000000
#define USBEpIntPri_EP15RX_BIT 30
#define USBEpIntPri_EP15TX_MASK 0x80000000
#define USBEpIntPri_EP15TX 0x80000000
#define USBEpIntPri_EP15TX_BIT 31

#define USBReEp (*(volatile unsigned *)0xFFE0C244)
#define USBReEp_OFFSET 0x244
#define USBReEp_EP0_MASK 0x1
#define USBReEp_EP0 0x1
#define USBReEp_EP0_BIT 0
#define USBReEp_EP1_MASK 0x2
#define USBReEp_EP1 0x2
#define USBReEp_EP1_BIT 1
#define USBReEp_EP2_MASK 0x4
#define USBReEp_EP2 0x4
#define USBReEp_EP2_BIT 2
#define USBReEp_EP3_MASK 0x8
#define USBReEp_EP3 0x8
#define USBReEp_EP3_BIT 3
#define USBReEp_EP4_MASK 0x10
#define USBReEp_EP4 0x10
#define USBReEp_EP4_BIT 4
#define USBReEp_EP5_MASK 0x20
#define USBReEp_EP5 0x20
#define USBReEp_EP5_BIT 5
#define USBReEp_EP6_MASK 0x40
#define USBReEp_EP6 0x40
#define USBReEp_EP6_BIT 6
#define USBReEp_EP7_MASK 0x80
#define USBReEp_EP7 0x80
#define USBReEp_EP7_BIT 7
#define USBReEp_EP8_MASK 0x100
#define USBReEp_EP8 0x100
#define USBReEp_EP8_BIT 8
#define USBReEp_EP9_MASK 0x200
#define USBReEp_EP9 0x200
#define USBReEp_EP9_BIT 9
#define USBReEp_EP10_MASK 0x400
#define USBReEp_EP10 0x400
#define USBReEp_EP10_BIT 10
#define USBReEp_EP11_MASK 0x800
#define USBReEp_EP11 0x800
#define USBReEp_EP11_BIT 11
#define USBReEp_EP12_MASK 0x1000
#define USBReEp_EP12 0x1000
#define USBReEp_EP12_BIT 12
#define USBReEp_EP13_MASK 0x2000
#define USBReEp_EP13 0x2000
#define USBReEp_EP13_BIT 13
#define USBReEp_EP14_MASK 0x4000
#define USBReEp_EP14 0x4000
#define USBReEp_EP14_BIT 14
#define USBReEp_EP15_MASK 0x8000
#define USBReEp_EP15 0x8000
#define USBReEp_EP15_BIT 15
#define USBReEp_EP16_MASK 0x10000
#define USBReEp_EP16 0x10000
#define USBReEp_EP16_BIT 16
#define USBReEp_EP17_MASK 0x20000
#define USBReEp_EP17 0x20000
#define USBReEp_EP17_BIT 17
#define USBReEp_EP18_MASK 0x40000
#define USBReEp_EP18 0x40000
#define USBReEp_EP18_BIT 18
#define USBReEp_EP19_MASK 0x80000
#define USBReEp_EP19 0x80000
#define USBReEp_EP19_BIT 19
#define USBReEp_EP20_MASK 0x100000
#define USBReEp_EP20 0x100000
#define USBReEp_EP20_BIT 20
#define USBReEp_EP21_MASK 0x200000
#define USBReEp_EP21 0x200000
#define USBReEp_EP21_BIT 21
#define USBReEp_EP22_MASK 0x400000
#define USBReEp_EP22 0x400000
#define USBReEp_EP22_BIT 22
#define USBReEp_EP23_MASK 0x800000
#define USBReEp_EP23 0x800000
#define USBReEp_EP23_BIT 23
#define USBReEp_EP24_MASK 0x1000000
#define USBReEp_EP24 0x1000000
#define USBReEp_EP24_BIT 24
#define USBReEp_EP25_MASK 0x2000000
#define USBReEp_EP25 0x2000000
#define USBReEp_EP25_BIT 25
#define USBReEp_EP26_MASK 0x4000000
#define USBReEp_EP26 0x4000000
#define USBReEp_EP26_BIT 26
#define USBReEp_EP27_MASK 0x8000000
#define USBReEp_EP27 0x8000000
#define USBReEp_EP27_BIT 27
#define USBReEp_EP28_MASK 0x10000000
#define USBReEp_EP28 0x10000000
#define USBReEp_EP28_BIT 28
#define USBReEp_EP29_MASK 0x20000000
#define USBReEp_EP29 0x20000000
#define USBReEp_EP29_BIT 29
#define USBReEp_EP30_MASK 0x40000000
#define USBReEp_EP30 0x40000000
#define USBReEp_EP30_BIT 30
#define USBReEp_EP31_MASK 0x80000000
#define USBReEp_EP31 0x80000000
#define USBReEp_EP31_BIT 31

#define USBEpInd (*(volatile unsigned *)0xFFE0C248)
#define USBEpInd_OFFSET 0x248
#define USBEpInd_Phy_endpoint_MASK 0x1F
#define USBEpInd_Phy_endpoint_BIT 0

#define USBEpMaxPSize (*(volatile unsigned *)0xFFE0C24C)
#define USBEpMaxPSize_OFFSET 0x24C
#define USBEpMaxPSize_MaxPacketSize_MASK 0x3FF
#define USBEpMaxPSize_MaxPacketSize_BIT 0

#define USBDMARSt (*(volatile unsigned *)0xFFE0C250)
#define USBDMARSt_OFFSET 0x250
#define USBDMARSt_EP0_MASK 0x1
#define USBDMARSt_EP0 0x1
#define USBDMARSt_EP0_BIT 0
#define USBDMARSt_EP1_MASK 0x2
#define USBDMARSt_EP1 0x2
#define USBDMARSt_EP1_BIT 1
#define USBDMARSt_EP2_MASK 0x4
#define USBDMARSt_EP2 0x4
#define USBDMARSt_EP2_BIT 2
#define USBDMARSt_EP3_MASK 0x8
#define USBDMARSt_EP3 0x8
#define USBDMARSt_EP3_BIT 3
#define USBDMARSt_EP4_MASK 0x10
#define USBDMARSt_EP4 0x10
#define USBDMARSt_EP4_BIT 4
#define USBDMARSt_EP5_MASK 0x20
#define USBDMARSt_EP5 0x20
#define USBDMARSt_EP5_BIT 5
#define USBDMARSt_EP6_MASK 0x40
#define USBDMARSt_EP6 0x40
#define USBDMARSt_EP6_BIT 6
#define USBDMARSt_EP7_MASK 0x80
#define USBDMARSt_EP7 0x80
#define USBDMARSt_EP7_BIT 7
#define USBDMARSt_EP8_MASK 0x100
#define USBDMARSt_EP8 0x100
#define USBDMARSt_EP8_BIT 8
#define USBDMARSt_EP9_MASK 0x200
#define USBDMARSt_EP9 0x200
#define USBDMARSt_EP9_BIT 9
#define USBDMARSt_EP10_MASK 0x400
#define USBDMARSt_EP10 0x400
#define USBDMARSt_EP10_BIT 10
#define USBDMARSt_EP11_MASK 0x800
#define USBDMARSt_EP11 0x800
#define USBDMARSt_EP11_BIT 11
#define USBDMARSt_EP12_MASK 0x1000
#define USBDMARSt_EP12 0x1000
#define USBDMARSt_EP12_BIT 12
#define USBDMARSt_EP13_MASK 0x2000
#define USBDMARSt_EP13 0x2000
#define USBDMARSt_EP13_BIT 13
#define USBDMARSt_EP14_MASK 0x4000
#define USBDMARSt_EP14 0x4000
#define USBDMARSt_EP14_BIT 14
#define USBDMARSt_EP15_MASK 0x8000
#define USBDMARSt_EP15 0x8000
#define USBDMARSt_EP15_BIT 15
#define USBDMARSt_EP16_MASK 0x10000
#define USBDMARSt_EP16 0x10000
#define USBDMARSt_EP16_BIT 16
#define USBDMARSt_EP17_MASK 0x20000
#define USBDMARSt_EP17 0x20000
#define USBDMARSt_EP17_BIT 17
#define USBDMARSt_EP18_MASK 0x40000
#define USBDMARSt_EP18 0x40000
#define USBDMARSt_EP18_BIT 18
#define USBDMARSt_EP19_MASK 0x80000
#define USBDMARSt_EP19 0x80000
#define USBDMARSt_EP19_BIT 19
#define USBDMARSt_EP20_MASK 0x100000
#define USBDMARSt_EP20 0x100000
#define USBDMARSt_EP20_BIT 20
#define USBDMARSt_EP21_MASK 0x200000
#define USBDMARSt_EP21 0x200000
#define USBDMARSt_EP21_BIT 21
#define USBDMARSt_EP22_MASK 0x400000
#define USBDMARSt_EP22 0x400000
#define USBDMARSt_EP22_BIT 22
#define USBDMARSt_EP23_MASK 0x800000
#define USBDMARSt_EP23 0x800000
#define USBDMARSt_EP23_BIT 23
#define USBDMARSt_EP24_MASK 0x1000000
#define USBDMARSt_EP24 0x1000000
#define USBDMARSt_EP24_BIT 24
#define USBDMARSt_EP25_MASK 0x2000000
#define USBDMARSt_EP25 0x2000000
#define USBDMARSt_EP25_BIT 25
#define USBDMARSt_EP26_MASK 0x4000000
#define USBDMARSt_EP26 0x4000000
#define USBDMARSt_EP26_BIT 26
#define USBDMARSt_EP27_MASK 0x8000000
#define USBDMARSt_EP27 0x8000000
#define USBDMARSt_EP27_BIT 27
#define USBDMARSt_EP28_MASK 0x10000000
#define USBDMARSt_EP28 0x10000000
#define USBDMARSt_EP28_BIT 28
#define USBDMARSt_EP29_MASK 0x20000000
#define USBDMARSt_EP29 0x20000000
#define USBDMARSt_EP29_BIT 29
#define USBDMARSt_EP30_MASK 0x40000000
#define USBDMARSt_EP30 0x40000000
#define USBDMARSt_EP30_BIT 30
#define USBDMARSt_EP31_MASK 0x80000000
#define USBDMARSt_EP31 0x80000000
#define USBDMARSt_EP31_BIT 31

#define USBDMARClr (*(volatile unsigned *)0xFFE0C254)
#define USBDMARClr_OFFSET 0x254
#define USBDMARClr_EP0_MASK 0x1
#define USBDMARClr_EP0 0x1
#define USBDMARClr_EP0_BIT 0
#define USBDMARClr_EP1_MASK 0x2
#define USBDMARClr_EP1 0x2
#define USBDMARClr_EP1_BIT 1
#define USBDMARClr_EP2_MASK 0x4
#define USBDMARClr_EP2 0x4
#define USBDMARClr_EP2_BIT 2
#define USBDMARClr_EP3_MASK 0x8
#define USBDMARClr_EP3 0x8
#define USBDMARClr_EP3_BIT 3
#define USBDMARClr_EP4_MASK 0x10
#define USBDMARClr_EP4 0x10
#define USBDMARClr_EP4_BIT 4
#define USBDMARClr_EP5_MASK 0x20
#define USBDMARClr_EP5 0x20
#define USBDMARClr_EP5_BIT 5
#define USBDMARClr_EP6_MASK 0x40
#define USBDMARClr_EP6 0x40
#define USBDMARClr_EP6_BIT 6
#define USBDMARClr_EP7_MASK 0x80
#define USBDMARClr_EP7 0x80
#define USBDMARClr_EP7_BIT 7
#define USBDMARClr_EP8_MASK 0x100
#define USBDMARClr_EP8 0x100
#define USBDMARClr_EP8_BIT 8
#define USBDMARClr_EP9_MASK 0x200
#define USBDMARClr_EP9 0x200
#define USBDMARClr_EP9_BIT 9
#define USBDMARClr_EP10_MASK 0x400
#define USBDMARClr_EP10 0x400
#define USBDMARClr_EP10_BIT 10
#define USBDMARClr_EP11_MASK 0x800
#define USBDMARClr_EP11 0x800
#define USBDMARClr_EP11_BIT 11
#define USBDMARClr_EP12_MASK 0x1000
#define USBDMARClr_EP12 0x1000
#define USBDMARClr_EP12_BIT 12
#define USBDMARClr_EP13_MASK 0x2000
#define USBDMARClr_EP13 0x2000
#define USBDMARClr_EP13_BIT 13
#define USBDMARClr_EP14_MASK 0x4000
#define USBDMARClr_EP14 0x4000
#define USBDMARClr_EP14_BIT 14
#define USBDMARClr_EP15_MASK 0x8000
#define USBDMARClr_EP15 0x8000
#define USBDMARClr_EP15_BIT 15
#define USBDMARClr_EP16_MASK 0x10000
#define USBDMARClr_EP16 0x10000
#define USBDMARClr_EP16_BIT 16
#define USBDMARClr_EP17_MASK 0x20000
#define USBDMARClr_EP17 0x20000
#define USBDMARClr_EP17_BIT 17
#define USBDMARClr_EP18_MASK 0x40000
#define USBDMARClr_EP18 0x40000
#define USBDMARClr_EP18_BIT 18
#define USBDMARClr_EP19_MASK 0x80000
#define USBDMARClr_EP19 0x80000
#define USBDMARClr_EP19_BIT 19
#define USBDMARClr_EP20_MASK 0x100000
#define USBDMARClr_EP20 0x100000
#define USBDMARClr_EP20_BIT 20
#define USBDMARClr_EP21_MASK 0x200000
#define USBDMARClr_EP21 0x200000
#define USBDMARClr_EP21_BIT 21
#define USBDMARClr_EP22_MASK 0x400000
#define USBDMARClr_EP22 0x400000
#define USBDMARClr_EP22_BIT 22
#define USBDMARClr_EP23_MASK 0x800000
#define USBDMARClr_EP23 0x800000
#define USBDMARClr_EP23_BIT 23
#define USBDMARClr_EP24_MASK 0x1000000
#define USBDMARClr_EP24 0x1000000
#define USBDMARClr_EP24_BIT 24
#define USBDMARClr_EP25_MASK 0x2000000
#define USBDMARClr_EP25 0x2000000
#define USBDMARClr_EP25_BIT 25
#define USBDMARClr_EP26_MASK 0x4000000
#define USBDMARClr_EP26 0x4000000
#define USBDMARClr_EP26_BIT 26
#define USBDMARClr_EP27_MASK 0x8000000
#define USBDMARClr_EP27 0x8000000
#define USBDMARClr_EP27_BIT 27
#define USBDMARClr_EP28_MASK 0x10000000
#define USBDMARClr_EP28 0x10000000
#define USBDMARClr_EP28_BIT 28
#define USBDMARClr_EP29_MASK 0x20000000
#define USBDMARClr_EP29 0x20000000
#define USBDMARClr_EP29_BIT 29
#define USBDMARClr_EP30_MASK 0x40000000
#define USBDMARClr_EP30 0x40000000
#define USBDMARClr_EP30_BIT 30
#define USBDMARClr_EP31_MASK 0x80000000
#define USBDMARClr_EP31 0x80000000
#define USBDMARClr_EP31_BIT 31

#define USBDMARSet (*(volatile unsigned *)0xFFE0C258)
#define USBDMARSet_OFFSET 0x258
#define USBDMARSet_EP0_MASK 0x1
#define USBDMARSet_EP0 0x1
#define USBDMARSet_EP0_BIT 0
#define USBDMARSet_EP1_MASK 0x2
#define USBDMARSet_EP1 0x2
#define USBDMARSet_EP1_BIT 1
#define USBDMARSet_EP2_MASK 0x4
#define USBDMARSet_EP2 0x4
#define USBDMARSet_EP2_BIT 2
#define USBDMARSet_EP3_MASK 0x8
#define USBDMARSet_EP3 0x8
#define USBDMARSet_EP3_BIT 3
#define USBDMARSet_EP4_MASK 0x10
#define USBDMARSet_EP4 0x10
#define USBDMARSet_EP4_BIT 4
#define USBDMARSet_EP5_MASK 0x20
#define USBDMARSet_EP5 0x20
#define USBDMARSet_EP5_BIT 5
#define USBDMARSet_EP6_MASK 0x40
#define USBDMARSet_EP6 0x40
#define USBDMARSet_EP6_BIT 6
#define USBDMARSet_EP7_MASK 0x80
#define USBDMARSet_EP7 0x80
#define USBDMARSet_EP7_BIT 7
#define USBDMARSet_EP8_MASK 0x100
#define USBDMARSet_EP8 0x100
#define USBDMARSet_EP8_BIT 8
#define USBDMARSet_EP9_MASK 0x200
#define USBDMARSet_EP9 0x200
#define USBDMARSet_EP9_BIT 9
#define USBDMARSet_EP10_MASK 0x400
#define USBDMARSet_EP10 0x400
#define USBDMARSet_EP10_BIT 10
#define USBDMARSet_EP11_MASK 0x800
#define USBDMARSet_EP11 0x800
#define USBDMARSet_EP11_BIT 11
#define USBDMARSet_EP12_MASK 0x1000
#define USBDMARSet_EP12 0x1000
#define USBDMARSet_EP12_BIT 12
#define USBDMARSet_EP13_MASK 0x2000
#define USBDMARSet_EP13 0x2000
#define USBDMARSet_EP13_BIT 13
#define USBDMARSet_EP14_MASK 0x4000
#define USBDMARSet_EP14 0x4000
#define USBDMARSet_EP14_BIT 14
#define USBDMARSet_EP15_MASK 0x8000
#define USBDMARSet_EP15 0x8000
#define USBDMARSet_EP15_BIT 15
#define USBDMARSet_EP16_MASK 0x10000
#define USBDMARSet_EP16 0x10000
#define USBDMARSet_EP16_BIT 16
#define USBDMARSet_EP17_MASK 0x20000
#define USBDMARSet_EP17 0x20000
#define USBDMARSet_EP17_BIT 17
#define USBDMARSet_EP18_MASK 0x40000
#define USBDMARSet_EP18 0x40000
#define USBDMARSet_EP18_BIT 18
#define USBDMARSet_EP19_MASK 0x80000
#define USBDMARSet_EP19 0x80000
#define USBDMARSet_EP19_BIT 19
#define USBDMARSet_EP20_MASK 0x100000
#define USBDMARSet_EP20 0x100000
#define USBDMARSet_EP20_BIT 20
#define USBDMARSet_EP21_MASK 0x200000
#define USBDMARSet_EP21 0x200000
#define USBDMARSet_EP21_BIT 21
#define USBDMARSet_EP22_MASK 0x400000
#define USBDMARSet_EP22 0x400000
#define USBDMARSet_EP22_BIT 22
#define USBDMARSet_EP23_MASK 0x800000
#define USBDMARSet_EP23 0x800000
#define USBDMARSet_EP23_BIT 23
#define USBDMARSet_EP24_MASK 0x1000000
#define USBDMARSet_EP24 0x1000000
#define USBDMARSet_EP24_BIT 24
#define USBDMARSet_EP25_MASK 0x2000000
#define USBDMARSet_EP25 0x2000000
#define USBDMARSet_EP25_BIT 25
#define USBDMARSet_EP26_MASK 0x4000000
#define USBDMARSet_EP26 0x4000000
#define USBDMARSet_EP26_BIT 26
#define USBDMARSet_EP27_MASK 0x8000000
#define USBDMARSet_EP27 0x8000000
#define USBDMARSet_EP27_BIT 27
#define USBDMARSet_EP28_MASK 0x10000000
#define USBDMARSet_EP28 0x10000000
#define USBDMARSet_EP28_BIT 28
#define USBDMARSet_EP29_MASK 0x20000000
#define USBDMARSet_EP29 0x20000000
#define USBDMARSet_EP29_BIT 29
#define USBDMARSet_EP30_MASK 0x40000000
#define USBDMARSet_EP30 0x40000000
#define USBDMARSet_EP30_BIT 30
#define USBDMARSet_EP31_MASK 0x80000000
#define USBDMARSet_EP31 0x80000000
#define USBDMARSet_EP31_BIT 31

#define USBUDCAH (*(volatile unsigned *)0xFFE0C280)
#define USBUDCAH_OFFSET 0x280
#define USBUDCAH_UDCA_Header_MASK 0xFFFFFF80
#define USBUDCAH_UDCA_Header_BIT 7

#define USBEpDMASt (*(volatile unsigned *)0xFFE0C284)
#define USBEpDMASt_OFFSET 0x284
#define USBEpDMASt_EP0_DMA_ENABLE_MASK 0x1
#define USBEpDMASt_EP0_DMA_ENABLE 0x1
#define USBEpDMASt_EP0_DMA_ENABLE_BIT 0
#define USBEpDMASt_EP1_DMA_ENABLE_MASK 0x2
#define USBEpDMASt_EP1_DMA_ENABLE 0x2
#define USBEpDMASt_EP1_DMA_ENABLE_BIT 1
#define USBEpDMASt_EP2_DMA_ENABLE_MASK 0x4
#define USBEpDMASt_EP2_DMA_ENABLE 0x4
#define USBEpDMASt_EP2_DMA_ENABLE_BIT 2
#define USBEpDMASt_EP3_DMA_ENABLE_MASK 0x8
#define USBEpDMASt_EP3_DMA_ENABLE 0x8
#define USBEpDMASt_EP3_DMA_ENABLE_BIT 3
#define USBEpDMASt_EP4_DMA_ENABLE_MASK 0x10
#define USBEpDMASt_EP4_DMA_ENABLE 0x10
#define USBEpDMASt_EP4_DMA_ENABLE_BIT 4
#define USBEpDMASt_EP5_DMA_ENABLE_MASK 0x20
#define USBEpDMASt_EP5_DMA_ENABLE 0x20
#define USBEpDMASt_EP5_DMA_ENABLE_BIT 5
#define USBEpDMASt_EP6_DMA_ENABLE_MASK 0x40
#define USBEpDMASt_EP6_DMA_ENABLE 0x40
#define USBEpDMASt_EP6_DMA_ENABLE_BIT 6
#define USBEpDMASt_EP7_DMA_ENABLE_MASK 0x80
#define USBEpDMASt_EP7_DMA_ENABLE 0x80
#define USBEpDMASt_EP7_DMA_ENABLE_BIT 7
#define USBEpDMASt_EP8_DMA_ENABLE_MASK 0x100
#define USBEpDMASt_EP8_DMA_ENABLE 0x100
#define USBEpDMASt_EP8_DMA_ENABLE_BIT 8
#define USBEpDMASt_EP9_DMA_ENABLE_MASK 0x200
#define USBEpDMASt_EP9_DMA_ENABLE 0x200
#define USBEpDMASt_EP9_DMA_ENABLE_BIT 9
#define USBEpDMASt_EP10_DMA_ENABLE_MASK 0x400
#define USBEpDMASt_EP10_DMA_ENABLE 0x400
#define USBEpDMASt_EP10_DMA_ENABLE_BIT 10
#define USBEpDMASt_EP11_DMA_ENABLE_MASK 0x800
#define USBEpDMASt_EP11_DMA_ENABLE 0x800
#define USBEpDMASt_EP11_DMA_ENABLE_BIT 11
#define USBEpDMASt_EP12_DMA_ENABLE_MASK 0x1000
#define USBEpDMASt_EP12_DMA_ENABLE 0x1000
#define USBEpDMASt_EP12_DMA_ENABLE_BIT 12
#define USBEpDMASt_EP13_DMA_ENABLE_MASK 0x2000
#define USBEpDMASt_EP13_DMA_ENABLE 0x2000
#define USBEpDMASt_EP13_DMA_ENABLE_BIT 13
#define USBEpDMASt_EP14_DMA_ENABLE_MASK 0x4000
#define USBEpDMASt_EP14_DMA_ENABLE 0x4000
#define USBEpDMASt_EP14_DMA_ENABLE_BIT 14
#define USBEpDMASt_EP15_DMA_ENABLE_MASK 0x8000
#define USBEpDMASt_EP15_DMA_ENABLE 0x8000
#define USBEpDMASt_EP15_DMA_ENABLE_BIT 15
#define USBEpDMASt_EP16_DMA_ENABLE_MASK 0x10000
#define USBEpDMASt_EP16_DMA_ENABLE 0x10000
#define USBEpDMASt_EP16_DMA_ENABLE_BIT 16
#define USBEpDMASt_EP17_DMA_ENABLE_MASK 0x20000
#define USBEpDMASt_EP17_DMA_ENABLE 0x20000
#define USBEpDMASt_EP17_DMA_ENABLE_BIT 17
#define USBEpDMASt_EP18_DMA_ENABLE_MASK 0x40000
#define USBEpDMASt_EP18_DMA_ENABLE 0x40000
#define USBEpDMASt_EP18_DMA_ENABLE_BIT 18
#define USBEpDMASt_EP19_DMA_ENABLE_MASK 0x80000
#define USBEpDMASt_EP19_DMA_ENABLE 0x80000
#define USBEpDMASt_EP19_DMA_ENABLE_BIT 19
#define USBEpDMASt_EP20_DMA_ENABLE_MASK 0x100000
#define USBEpDMASt_EP20_DMA_ENABLE 0x100000
#define USBEpDMASt_EP20_DMA_ENABLE_BIT 20
#define USBEpDMASt_EP21_DMA_ENABLE_MASK 0x200000
#define USBEpDMASt_EP21_DMA_ENABLE 0x200000
#define USBEpDMASt_EP21_DMA_ENABLE_BIT 21
#define USBEpDMASt_EP22_DMA_ENABLE_MASK 0x400000
#define USBEpDMASt_EP22_DMA_ENABLE 0x400000
#define USBEpDMASt_EP22_DMA_ENABLE_BIT 22
#define USBEpDMASt_EP23_DMA_ENABLE_MASK 0x800000
#define USBEpDMASt_EP23_DMA_ENABLE 0x800000
#define USBEpDMASt_EP23_DMA_ENABLE_BIT 23
#define USBEpDMASt_EP24_DMA_ENABLE_MASK 0x1000000
#define USBEpDMASt_EP24_DMA_ENABLE 0x1000000
#define USBEpDMASt_EP24_DMA_ENABLE_BIT 24
#define USBEpDMASt_EP25_DMA_ENABLE_MASK 0x2000000
#define USBEpDMASt_EP25_DMA_ENABLE 0x2000000
#define USBEpDMASt_EP25_DMA_ENABLE_BIT 25
#define USBEpDMASt_EP26_DMA_ENABLE_MASK 0x4000000
#define USBEpDMASt_EP26_DMA_ENABLE 0x4000000
#define USBEpDMASt_EP26_DMA_ENABLE_BIT 26
#define USBEpDMASt_EP27_DMA_ENABLE_MASK 0x8000000
#define USBEpDMASt_EP27_DMA_ENABLE 0x8000000
#define USBEpDMASt_EP27_DMA_ENABLE_BIT 27
#define USBEpDMASt_EP28_DMA_ENABLE_MASK 0x10000000
#define USBEpDMASt_EP28_DMA_ENABLE 0x10000000
#define USBEpDMASt_EP28_DMA_ENABLE_BIT 28
#define USBEpDMASt_EP29_DMA_ENABLE_MASK 0x20000000
#define USBEpDMASt_EP29_DMA_ENABLE 0x20000000
#define USBEpDMASt_EP29_DMA_ENABLE_BIT 29
#define USBEpDMASt_EP30_DMA_ENABLE_MASK 0x40000000
#define USBEpDMASt_EP30_DMA_ENABLE 0x40000000
#define USBEpDMASt_EP30_DMA_ENABLE_BIT 30
#define USBEpDMASt_EP31_DMA_ENABLE_MASK 0x80000000
#define USBEpDMASt_EP31_DMA_ENABLE 0x80000000
#define USBEpDMASt_EP31_DMA_ENABLE_BIT 31

#define USBEpDMAEn (*(volatile unsigned *)0xFFE0C288)
#define USBEpDMAEn_OFFSET 0x288
#define USBEpDMAEn_EP0_DMA_ENABLE_MASK 0x1
#define USBEpDMAEn_EP0_DMA_ENABLE 0x1
#define USBEpDMAEn_EP0_DMA_ENABLE_BIT 0
#define USBEpDMAEn_EP1_DMA_ENABLE_MASK 0x2
#define USBEpDMAEn_EP1_DMA_ENABLE 0x2
#define USBEpDMAEn_EP1_DMA_ENABLE_BIT 1
#define USBEpDMAEn_EP2_DMA_ENABLE_MASK 0x4
#define USBEpDMAEn_EP2_DMA_ENABLE 0x4
#define USBEpDMAEn_EP2_DMA_ENABLE_BIT 2
#define USBEpDMAEn_EP3_DMA_ENABLE_MASK 0x8
#define USBEpDMAEn_EP3_DMA_ENABLE 0x8
#define USBEpDMAEn_EP3_DMA_ENABLE_BIT 3
#define USBEpDMAEn_EP4_DMA_ENABLE_MASK 0x10
#define USBEpDMAEn_EP4_DMA_ENABLE 0x10
#define USBEpDMAEn_EP4_DMA_ENABLE_BIT 4
#define USBEpDMAEn_EP5_DMA_ENABLE_MASK 0x20
#define USBEpDMAEn_EP5_DMA_ENABLE 0x20
#define USBEpDMAEn_EP5_DMA_ENABLE_BIT 5
#define USBEpDMAEn_EP6_DMA_ENABLE_MASK 0x40
#define USBEpDMAEn_EP6_DMA_ENABLE 0x40
#define USBEpDMAEn_EP6_DMA_ENABLE_BIT 6
#define USBEpDMAEn_EP7_DMA_ENABLE_MASK 0x80
#define USBEpDMAEn_EP7_DMA_ENABLE 0x80
#define USBEpDMAEn_EP7_DMA_ENABLE_BIT 7
#define USBEpDMAEn_EP8_DMA_ENABLE_MASK 0x100
#define USBEpDMAEn_EP8_DMA_ENABLE 0x100
#define USBEpDMAEn_EP8_DMA_ENABLE_BIT 8
#define USBEpDMAEn_EP9_DMA_ENABLE_MASK 0x200
#define USBEpDMAEn_EP9_DMA_ENABLE 0x200
#define USBEpDMAEn_EP9_DMA_ENABLE_BIT 9
#define USBEpDMAEn_EP10_DMA_ENABLE_MASK 0x400
#define USBEpDMAEn_EP10_DMA_ENABLE 0x400
#define USBEpDMAEn_EP10_DMA_ENABLE_BIT 10
#define USBEpDMAEn_EP11_DMA_ENABLE_MASK 0x800
#define USBEpDMAEn_EP11_DMA_ENABLE 0x800
#define USBEpDMAEn_EP11_DMA_ENABLE_BIT 11
#define USBEpDMAEn_EP12_DMA_ENABLE_MASK 0x1000
#define USBEpDMAEn_EP12_DMA_ENABLE 0x1000
#define USBEpDMAEn_EP12_DMA_ENABLE_BIT 12
#define USBEpDMAEn_EP13_DMA_ENABLE_MASK 0x2000
#define USBEpDMAEn_EP13_DMA_ENABLE 0x2000
#define USBEpDMAEn_EP13_DMA_ENABLE_BIT 13
#define USBEpDMAEn_EP14_DMA_ENABLE_MASK 0x4000
#define USBEpDMAEn_EP14_DMA_ENABLE 0x4000
#define USBEpDMAEn_EP14_DMA_ENABLE_BIT 14
#define USBEpDMAEn_EP15_DMA_ENABLE_MASK 0x8000
#define USBEpDMAEn_EP15_DMA_ENABLE 0x8000
#define USBEpDMAEn_EP15_DMA_ENABLE_BIT 15
#define USBEpDMAEn_EP16_DMA_ENABLE_MASK 0x10000
#define USBEpDMAEn_EP16_DMA_ENABLE 0x10000
#define USBEpDMAEn_EP16_DMA_ENABLE_BIT 16
#define USBEpDMAEn_EP17_DMA_ENABLE_MASK 0x20000
#define USBEpDMAEn_EP17_DMA_ENABLE 0x20000
#define USBEpDMAEn_EP17_DMA_ENABLE_BIT 17
#define USBEpDMAEn_EP18_DMA_ENABLE_MASK 0x40000
#define USBEpDMAEn_EP18_DMA_ENABLE 0x40000
#define USBEpDMAEn_EP18_DMA_ENABLE_BIT 18
#define USBEpDMAEn_EP19_DMA_ENABLE_MASK 0x80000
#define USBEpDMAEn_EP19_DMA_ENABLE 0x80000
#define USBEpDMAEn_EP19_DMA_ENABLE_BIT 19
#define USBEpDMAEn_EP20_DMA_ENABLE_MASK 0x100000
#define USBEpDMAEn_EP20_DMA_ENABLE 0x100000
#define USBEpDMAEn_EP20_DMA_ENABLE_BIT 20
#define USBEpDMAEn_EP21_DMA_ENABLE_MASK 0x200000
#define USBEpDMAEn_EP21_DMA_ENABLE 0x200000
#define USBEpDMAEn_EP21_DMA_ENABLE_BIT 21
#define USBEpDMAEn_EP22_DMA_ENABLE_MASK 0x400000
#define USBEpDMAEn_EP22_DMA_ENABLE 0x400000
#define USBEpDMAEn_EP22_DMA_ENABLE_BIT 22
#define USBEpDMAEn_EP23_DMA_ENABLE_MASK 0x800000
#define USBEpDMAEn_EP23_DMA_ENABLE 0x800000
#define USBEpDMAEn_EP23_DMA_ENABLE_BIT 23
#define USBEpDMAEn_EP24_DMA_ENABLE_MASK 0x1000000
#define USBEpDMAEn_EP24_DMA_ENABLE 0x1000000
#define USBEpDMAEn_EP24_DMA_ENABLE_BIT 24
#define USBEpDMAEn_EP25_DMA_ENABLE_MASK 0x2000000
#define USBEpDMAEn_EP25_DMA_ENABLE 0x2000000
#define USBEpDMAEn_EP25_DMA_ENABLE_BIT 25
#define USBEpDMAEn_EP26_DMA_ENABLE_MASK 0x4000000
#define USBEpDMAEn_EP26_DMA_ENABLE 0x4000000
#define USBEpDMAEn_EP26_DMA_ENABLE_BIT 26
#define USBEpDMAEn_EP27_DMA_ENABLE_MASK 0x8000000
#define USBEpDMAEn_EP27_DMA_ENABLE 0x8000000
#define USBEpDMAEn_EP27_DMA_ENABLE_BIT 27
#define USBEpDMAEn_EP28_DMA_ENABLE_MASK 0x10000000
#define USBEpDMAEn_EP28_DMA_ENABLE 0x10000000
#define USBEpDMAEn_EP28_DMA_ENABLE_BIT 28
#define USBEpDMAEn_EP29_DMA_ENABLE_MASK 0x20000000
#define USBEpDMAEn_EP29_DMA_ENABLE 0x20000000
#define USBEpDMAEn_EP29_DMA_ENABLE_BIT 29
#define USBEpDMAEn_EP30_DMA_ENABLE_MASK 0x40000000
#define USBEpDMAEn_EP30_DMA_ENABLE 0x40000000
#define USBEpDMAEn_EP30_DMA_ENABLE_BIT 30
#define USBEpDMAEn_EP31_DMA_ENABLE_MASK 0x80000000
#define USBEpDMAEn_EP31_DMA_ENABLE 0x80000000
#define USBEpDMAEn_EP31_DMA_ENABLE_BIT 31

#define USBEpDMADis (*(volatile unsigned *)0xFFE0C28C)
#define USBEpDMADis_OFFSET 0x28C
#define USBEpDMADis_EP0_DMA_ENABLE_MASK 0x1
#define USBEpDMADis_EP0_DMA_ENABLE 0x1
#define USBEpDMADis_EP0_DMA_ENABLE_BIT 0
#define USBEpDMADis_EP1_DMA_ENABLE_MASK 0x2
#define USBEpDMADis_EP1_DMA_ENABLE 0x2
#define USBEpDMADis_EP1_DMA_ENABLE_BIT 1
#define USBEpDMADis_EP2_DMA_ENABLE_MASK 0x4
#define USBEpDMADis_EP2_DMA_ENABLE 0x4
#define USBEpDMADis_EP2_DMA_ENABLE_BIT 2
#define USBEpDMADis_EP3_DMA_ENABLE_MASK 0x8
#define USBEpDMADis_EP3_DMA_ENABLE 0x8
#define USBEpDMADis_EP3_DMA_ENABLE_BIT 3
#define USBEpDMADis_EP4_DMA_ENABLE_MASK 0x10
#define USBEpDMADis_EP4_DMA_ENABLE 0x10
#define USBEpDMADis_EP4_DMA_ENABLE_BIT 4
#define USBEpDMADis_EP5_DMA_ENABLE_MASK 0x20
#define USBEpDMADis_EP5_DMA_ENABLE 0x20
#define USBEpDMADis_EP5_DMA_ENABLE_BIT 5
#define USBEpDMADis_EP6_DMA_ENABLE_MASK 0x40
#define USBEpDMADis_EP6_DMA_ENABLE 0x40
#define USBEpDMADis_EP6_DMA_ENABLE_BIT 6
#define USBEpDMADis_EP7_DMA_ENABLE_MASK 0x80
#define USBEpDMADis_EP7_DMA_ENABLE 0x80
#define USBEpDMADis_EP7_DMA_ENABLE_BIT 7
#define USBEpDMADis_EP8_DMA_ENABLE_MASK 0x100
#define USBEpDMADis_EP8_DMA_ENABLE 0x100
#define USBEpDMADis_EP8_DMA_ENABLE_BIT 8
#define USBEpDMADis_EP9_DMA_ENABLE_MASK 0x200
#define USBEpDMADis_EP9_DMA_ENABLE 0x200
#define USBEpDMADis_EP9_DMA_ENABLE_BIT 9
#define USBEpDMADis_EP10_DMA_ENABLE_MASK 0x400
#define USBEpDMADis_EP10_DMA_ENABLE 0x400
#define USBEpDMADis_EP10_DMA_ENABLE_BIT 10
#define USBEpDMADis_EP11_DMA_ENABLE_MASK 0x800
#define USBEpDMADis_EP11_DMA_ENABLE 0x800
#define USBEpDMADis_EP11_DMA_ENABLE_BIT 11
#define USBEpDMADis_EP12_DMA_ENABLE_MASK 0x1000
#define USBEpDMADis_EP12_DMA_ENABLE 0x1000
#define USBEpDMADis_EP12_DMA_ENABLE_BIT 12
#define USBEpDMADis_EP13_DMA_ENABLE_MASK 0x2000
#define USBEpDMADis_EP13_DMA_ENABLE 0x2000
#define USBEpDMADis_EP13_DMA_ENABLE_BIT 13
#define USBEpDMADis_EP14_DMA_ENABLE_MASK 0x4000
#define USBEpDMADis_EP14_DMA_ENABLE 0x4000
#define USBEpDMADis_EP14_DMA_ENABLE_BIT 14
#define USBEpDMADis_EP15_DMA_ENABLE_MASK 0x8000
#define USBEpDMADis_EP15_DMA_ENABLE 0x8000
#define USBEpDMADis_EP15_DMA_ENABLE_BIT 15
#define USBEpDMADis_EP16_DMA_ENABLE_MASK 0x10000
#define USBEpDMADis_EP16_DMA_ENABLE 0x10000
#define USBEpDMADis_EP16_DMA_ENABLE_BIT 16
#define USBEpDMADis_EP17_DMA_ENABLE_MASK 0x20000
#define USBEpDMADis_EP17_DMA_ENABLE 0x20000
#define USBEpDMADis_EP17_DMA_ENABLE_BIT 17
#define USBEpDMADis_EP18_DMA_ENABLE_MASK 0x40000
#define USBEpDMADis_EP18_DMA_ENABLE 0x40000
#define USBEpDMADis_EP18_DMA_ENABLE_BIT 18
#define USBEpDMADis_EP19_DMA_ENABLE_MASK 0x80000
#define USBEpDMADis_EP19_DMA_ENABLE 0x80000
#define USBEpDMADis_EP19_DMA_ENABLE_BIT 19
#define USBEpDMADis_EP20_DMA_ENABLE_MASK 0x100000
#define USBEpDMADis_EP20_DMA_ENABLE 0x100000
#define USBEpDMADis_EP20_DMA_ENABLE_BIT 20
#define USBEpDMADis_EP21_DMA_ENABLE_MASK 0x200000
#define USBEpDMADis_EP21_DMA_ENABLE 0x200000
#define USBEpDMADis_EP21_DMA_ENABLE_BIT 21
#define USBEpDMADis_EP22_DMA_ENABLE_MASK 0x400000
#define USBEpDMADis_EP22_DMA_ENABLE 0x400000
#define USBEpDMADis_EP22_DMA_ENABLE_BIT 22
#define USBEpDMADis_EP23_DMA_ENABLE_MASK 0x800000
#define USBEpDMADis_EP23_DMA_ENABLE 0x800000
#define USBEpDMADis_EP23_DMA_ENABLE_BIT 23
#define USBEpDMADis_EP24_DMA_ENABLE_MASK 0x1000000
#define USBEpDMADis_EP24_DMA_ENABLE 0x1000000
#define USBEpDMADis_EP24_DMA_ENABLE_BIT 24
#define USBEpDMADis_EP25_DMA_ENABLE_MASK 0x2000000
#define USBEpDMADis_EP25_DMA_ENABLE 0x2000000
#define USBEpDMADis_EP25_DMA_ENABLE_BIT 25
#define USBEpDMADis_EP26_DMA_ENABLE_MASK 0x4000000
#define USBEpDMADis_EP26_DMA_ENABLE 0x4000000
#define USBEpDMADis_EP26_DMA_ENABLE_BIT 26
#define USBEpDMADis_EP27_DMA_ENABLE_MASK 0x8000000
#define USBEpDMADis_EP27_DMA_ENABLE 0x8000000
#define USBEpDMADis_EP27_DMA_ENABLE_BIT 27
#define USBEpDMADis_EP28_DMA_ENABLE_MASK 0x10000000
#define USBEpDMADis_EP28_DMA_ENABLE 0x10000000
#define USBEpDMADis_EP28_DMA_ENABLE_BIT 28
#define USBEpDMADis_EP29_DMA_ENABLE_MASK 0x20000000
#define USBEpDMADis_EP29_DMA_ENABLE 0x20000000
#define USBEpDMADis_EP29_DMA_ENABLE_BIT 29
#define USBEpDMADis_EP30_DMA_ENABLE_MASK 0x40000000
#define USBEpDMADis_EP30_DMA_ENABLE 0x40000000
#define USBEpDMADis_EP30_DMA_ENABLE_BIT 30
#define USBEpDMADis_EP31_DMA_ENABLE_MASK 0x80000000
#define USBEpDMADis_EP31_DMA_ENABLE 0x80000000
#define USBEpDMADis_EP31_DMA_ENABLE_BIT 31

#define USBDMAIntSt (*(volatile unsigned *)0xFFE0C290)
#define USBDMAIntSt_OFFSET 0x290
#define USBDMAIntSt_End_of_Transfer_Interrupt_MASK 0x1
#define USBDMAIntSt_End_of_Transfer_Interrupt 0x1
#define USBDMAIntSt_End_of_Transfer_Interrupt_BIT 0
#define USBDMAIntSt_New_DD_Request_Interrupt_MASK 0x2
#define USBDMAIntSt_New_DD_Request_Interrupt 0x2
#define USBDMAIntSt_New_DD_Request_Interrupt_BIT 1
#define USBDMAIntSt_System_Error_Interrupt_MASK 0x4
#define USBDMAIntSt_System_Error_Interrupt 0x4
#define USBDMAIntSt_System_Error_Interrupt_BIT 2

#define USBDMAIntEn (*(volatile unsigned *)0xFFE0C294)
#define USBDMAIntEn_OFFSET 0x294
#define USBDMAIntEn_End_of_Transfer_Interrupt_MASK 0x1
#define USBDMAIntEn_End_of_Transfer_Interrupt 0x1
#define USBDMAIntEn_End_of_Transfer_Interrupt_BIT 0
#define USBDMAIntEn_New_DD_Request_Interrupt_MASK 0x2
#define USBDMAIntEn_New_DD_Request_Interrupt 0x2
#define USBDMAIntEn_New_DD_Request_Interrupt_BIT 1
#define USBDMAIntEn_System_Error_Interrupt_MASK 0x4
#define USBDMAIntEn_System_Error_Interrupt 0x4
#define USBDMAIntEn_System_Error_Interrupt_BIT 2

#define USBEoTIntSt (*(volatile unsigned *)0xFFE0C2A0)
#define USBEoTIntSt_OFFSET 0x2A0
#define USBEoTIntSt_EP0_MASK 0x1
#define USBEoTIntSt_EP0 0x1
#define USBEoTIntSt_EP0_BIT 0
#define USBEoTIntSt_EP1_MASK 0x2
#define USBEoTIntSt_EP1 0x2
#define USBEoTIntSt_EP1_BIT 1
#define USBEoTIntSt_EP2_MASK 0x4
#define USBEoTIntSt_EP2 0x4
#define USBEoTIntSt_EP2_BIT 2
#define USBEoTIntSt_EP3_MASK 0x8
#define USBEoTIntSt_EP3 0x8
#define USBEoTIntSt_EP3_BIT 3
#define USBEoTIntSt_EP4_MASK 0x10
#define USBEoTIntSt_EP4 0x10
#define USBEoTIntSt_EP4_BIT 4
#define USBEoTIntSt_EP5_MASK 0x20
#define USBEoTIntSt_EP5 0x20
#define USBEoTIntSt_EP5_BIT 5
#define USBEoTIntSt_EP6_MASK 0x40
#define USBEoTIntSt_EP6 0x40
#define USBEoTIntSt_EP6_BIT 6
#define USBEoTIntSt_EP7_MASK 0x80
#define USBEoTIntSt_EP7 0x80
#define USBEoTIntSt_EP7_BIT 7
#define USBEoTIntSt_EP8_MASK 0x100
#define USBEoTIntSt_EP8 0x100
#define USBEoTIntSt_EP8_BIT 8
#define USBEoTIntSt_EP9_MASK 0x200
#define USBEoTIntSt_EP9 0x200
#define USBEoTIntSt_EP9_BIT 9
#define USBEoTIntSt_EP10_MASK 0x400
#define USBEoTIntSt_EP10 0x400
#define USBEoTIntSt_EP10_BIT 10
#define USBEoTIntSt_EP11_MASK 0x800
#define USBEoTIntSt_EP11 0x800
#define USBEoTIntSt_EP11_BIT 11
#define USBEoTIntSt_EP12_MASK 0x1000
#define USBEoTIntSt_EP12 0x1000
#define USBEoTIntSt_EP12_BIT 12
#define USBEoTIntSt_EP13_MASK 0x2000
#define USBEoTIntSt_EP13 0x2000
#define USBEoTIntSt_EP13_BIT 13
#define USBEoTIntSt_EP14_MASK 0x4000
#define USBEoTIntSt_EP14 0x4000
#define USBEoTIntSt_EP14_BIT 14
#define USBEoTIntSt_EP15_MASK 0x8000
#define USBEoTIntSt_EP15 0x8000
#define USBEoTIntSt_EP15_BIT 15
#define USBEoTIntSt_EP16_MASK 0x10000
#define USBEoTIntSt_EP16 0x10000
#define USBEoTIntSt_EP16_BIT 16
#define USBEoTIntSt_EP17_MASK 0x20000
#define USBEoTIntSt_EP17 0x20000
#define USBEoTIntSt_EP17_BIT 17
#define USBEoTIntSt_EP18_MASK 0x40000
#define USBEoTIntSt_EP18 0x40000
#define USBEoTIntSt_EP18_BIT 18
#define USBEoTIntSt_EP19_MASK 0x80000
#define USBEoTIntSt_EP19 0x80000
#define USBEoTIntSt_EP19_BIT 19
#define USBEoTIntSt_EP20_MASK 0x100000
#define USBEoTIntSt_EP20 0x100000
#define USBEoTIntSt_EP20_BIT 20
#define USBEoTIntSt_EP21_MASK 0x200000
#define USBEoTIntSt_EP21 0x200000
#define USBEoTIntSt_EP21_BIT 21
#define USBEoTIntSt_EP22_MASK 0x400000
#define USBEoTIntSt_EP22 0x400000
#define USBEoTIntSt_EP22_BIT 22
#define USBEoTIntSt_EP23_MASK 0x800000
#define USBEoTIntSt_EP23 0x800000
#define USBEoTIntSt_EP23_BIT 23
#define USBEoTIntSt_EP24_MASK 0x1000000
#define USBEoTIntSt_EP24 0x1000000
#define USBEoTIntSt_EP24_BIT 24
#define USBEoTIntSt_EP25_MASK 0x2000000
#define USBEoTIntSt_EP25 0x2000000
#define USBEoTIntSt_EP25_BIT 25
#define USBEoTIntSt_EP26_MASK 0x4000000
#define USBEoTIntSt_EP26 0x4000000
#define USBEoTIntSt_EP26_BIT 26
#define USBEoTIntSt_EP27_MASK 0x8000000
#define USBEoTIntSt_EP27 0x8000000
#define USBEoTIntSt_EP27_BIT 27
#define USBEoTIntSt_EP28_MASK 0x10000000
#define USBEoTIntSt_EP28 0x10000000
#define USBEoTIntSt_EP28_BIT 28
#define USBEoTIntSt_EP29_MASK 0x20000000
#define USBEoTIntSt_EP29 0x20000000
#define USBEoTIntSt_EP29_BIT 29
#define USBEoTIntSt_EP30_MASK 0x40000000
#define USBEoTIntSt_EP30 0x40000000
#define USBEoTIntSt_EP30_BIT 30
#define USBEoTIntSt_EP31_MASK 0x80000000
#define USBEoTIntSt_EP31 0x80000000
#define USBEoTIntSt_EP31_BIT 31

#define USBEoTIntClr (*(volatile unsigned *)0xFFE0C2A4)
#define USBEoTIntClr_OFFSET 0x2A4
#define USBEoTIntClr_EP0_MASK 0x1
#define USBEoTIntClr_EP0 0x1
#define USBEoTIntClr_EP0_BIT 0
#define USBEoTIntClr_EP1_MASK 0x2
#define USBEoTIntClr_EP1 0x2
#define USBEoTIntClr_EP1_BIT 1
#define USBEoTIntClr_EP2_MASK 0x4
#define USBEoTIntClr_EP2 0x4
#define USBEoTIntClr_EP2_BIT 2
#define USBEoTIntClr_EP3_MASK 0x8
#define USBEoTIntClr_EP3 0x8
#define USBEoTIntClr_EP3_BIT 3
#define USBEoTIntClr_EP4_MASK 0x10
#define USBEoTIntClr_EP4 0x10
#define USBEoTIntClr_EP4_BIT 4
#define USBEoTIntClr_EP5_MASK 0x20
#define USBEoTIntClr_EP5 0x20
#define USBEoTIntClr_EP5_BIT 5
#define USBEoTIntClr_EP6_MASK 0x40
#define USBEoTIntClr_EP6 0x40
#define USBEoTIntClr_EP6_BIT 6
#define USBEoTIntClr_EP7_MASK 0x80
#define USBEoTIntClr_EP7 0x80
#define USBEoTIntClr_EP7_BIT 7
#define USBEoTIntClr_EP8_MASK 0x100
#define USBEoTIntClr_EP8 0x100
#define USBEoTIntClr_EP8_BIT 8
#define USBEoTIntClr_EP9_MASK 0x200
#define USBEoTIntClr_EP9 0x200
#define USBEoTIntClr_EP9_BIT 9
#define USBEoTIntClr_EP10_MASK 0x400
#define USBEoTIntClr_EP10 0x400
#define USBEoTIntClr_EP10_BIT 10
#define USBEoTIntClr_EP11_MASK 0x800
#define USBEoTIntClr_EP11 0x800
#define USBEoTIntClr_EP11_BIT 11
#define USBEoTIntClr_EP12_MASK 0x1000
#define USBEoTIntClr_EP12 0x1000
#define USBEoTIntClr_EP12_BIT 12
#define USBEoTIntClr_EP13_MASK 0x2000
#define USBEoTIntClr_EP13 0x2000
#define USBEoTIntClr_EP13_BIT 13
#define USBEoTIntClr_EP14_MASK 0x4000
#define USBEoTIntClr_EP14 0x4000
#define USBEoTIntClr_EP14_BIT 14
#define USBEoTIntClr_EP15_MASK 0x8000
#define USBEoTIntClr_EP15 0x8000
#define USBEoTIntClr_EP15_BIT 15
#define USBEoTIntClr_EP16_MASK 0x10000
#define USBEoTIntClr_EP16 0x10000
#define USBEoTIntClr_EP16_BIT 16
#define USBEoTIntClr_EP17_MASK 0x20000
#define USBEoTIntClr_EP17 0x20000
#define USBEoTIntClr_EP17_BIT 17
#define USBEoTIntClr_EP18_MASK 0x40000
#define USBEoTIntClr_EP18 0x40000
#define USBEoTIntClr_EP18_BIT 18
#define USBEoTIntClr_EP19_MASK 0x80000
#define USBEoTIntClr_EP19 0x80000
#define USBEoTIntClr_EP19_BIT 19
#define USBEoTIntClr_EP20_MASK 0x100000
#define USBEoTIntClr_EP20 0x100000
#define USBEoTIntClr_EP20_BIT 20
#define USBEoTIntClr_EP21_MASK 0x200000
#define USBEoTIntClr_EP21 0x200000
#define USBEoTIntClr_EP21_BIT 21
#define USBEoTIntClr_EP22_MASK 0x400000
#define USBEoTIntClr_EP22 0x400000
#define USBEoTIntClr_EP22_BIT 22
#define USBEoTIntClr_EP23_MASK 0x800000
#define USBEoTIntClr_EP23 0x800000
#define USBEoTIntClr_EP23_BIT 23
#define USBEoTIntClr_EP24_MASK 0x1000000
#define USBEoTIntClr_EP24 0x1000000
#define USBEoTIntClr_EP24_BIT 24
#define USBEoTIntClr_EP25_MASK 0x2000000
#define USBEoTIntClr_EP25 0x2000000
#define USBEoTIntClr_EP25_BIT 25
#define USBEoTIntClr_EP26_MASK 0x4000000
#define USBEoTIntClr_EP26 0x4000000
#define USBEoTIntClr_EP26_BIT 26
#define USBEoTIntClr_EP27_MASK 0x8000000
#define USBEoTIntClr_EP27 0x8000000
#define USBEoTIntClr_EP27_BIT 27
#define USBEoTIntClr_EP28_MASK 0x10000000
#define USBEoTIntClr_EP28 0x10000000
#define USBEoTIntClr_EP28_BIT 28
#define USBEoTIntClr_EP29_MASK 0x20000000
#define USBEoTIntClr_EP29 0x20000000
#define USBEoTIntClr_EP29_BIT 29
#define USBEoTIntClr_EP30_MASK 0x40000000
#define USBEoTIntClr_EP30 0x40000000
#define USBEoTIntClr_EP30_BIT 30
#define USBEoTIntClr_EP31_MASK 0x80000000
#define USBEoTIntClr_EP31 0x80000000
#define USBEoTIntClr_EP31_BIT 31

#define USBEoTIntSet (*(volatile unsigned *)0xFFE0C2A8)
#define USBEoTIntSet_OFFSET 0x2A8
#define USBEoTIntSet_EP0_MASK 0x1
#define USBEoTIntSet_EP0 0x1
#define USBEoTIntSet_EP0_BIT 0
#define USBEoTIntSet_EP1_MASK 0x2
#define USBEoTIntSet_EP1 0x2
#define USBEoTIntSet_EP1_BIT 1
#define USBEoTIntSet_EP2_MASK 0x4
#define USBEoTIntSet_EP2 0x4
#define USBEoTIntSet_EP2_BIT 2
#define USBEoTIntSet_EP3_MASK 0x8
#define USBEoTIntSet_EP3 0x8
#define USBEoTIntSet_EP3_BIT 3
#define USBEoTIntSet_EP4_MASK 0x10
#define USBEoTIntSet_EP4 0x10
#define USBEoTIntSet_EP4_BIT 4
#define USBEoTIntSet_EP5_MASK 0x20
#define USBEoTIntSet_EP5 0x20
#define USBEoTIntSet_EP5_BIT 5
#define USBEoTIntSet_EP6_MASK 0x40
#define USBEoTIntSet_EP6 0x40
#define USBEoTIntSet_EP6_BIT 6
#define USBEoTIntSet_EP7_MASK 0x80
#define USBEoTIntSet_EP7 0x80
#define USBEoTIntSet_EP7_BIT 7
#define USBEoTIntSet_EP8_MASK 0x100
#define USBEoTIntSet_EP8 0x100
#define USBEoTIntSet_EP8_BIT 8
#define USBEoTIntSet_EP9_MASK 0x200
#define USBEoTIntSet_EP9 0x200
#define USBEoTIntSet_EP9_BIT 9
#define USBEoTIntSet_EP10_MASK 0x400
#define USBEoTIntSet_EP10 0x400
#define USBEoTIntSet_EP10_BIT 10
#define USBEoTIntSet_EP11_MASK 0x800
#define USBEoTIntSet_EP11 0x800
#define USBEoTIntSet_EP11_BIT 11
#define USBEoTIntSet_EP12_MASK 0x1000
#define USBEoTIntSet_EP12 0x1000
#define USBEoTIntSet_EP12_BIT 12
#define USBEoTIntSet_EP13_MASK 0x2000
#define USBEoTIntSet_EP13 0x2000
#define USBEoTIntSet_EP13_BIT 13
#define USBEoTIntSet_EP14_MASK 0x4000
#define USBEoTIntSet_EP14 0x4000
#define USBEoTIntSet_EP14_BIT 14
#define USBEoTIntSet_EP15_MASK 0x8000
#define USBEoTIntSet_EP15 0x8000
#define USBEoTIntSet_EP15_BIT 15
#define USBEoTIntSet_EP16_MASK 0x10000
#define USBEoTIntSet_EP16 0x10000
#define USBEoTIntSet_EP16_BIT 16
#define USBEoTIntSet_EP17_MASK 0x20000
#define USBEoTIntSet_EP17 0x20000
#define USBEoTIntSet_EP17_BIT 17
#define USBEoTIntSet_EP18_MASK 0x40000
#define USBEoTIntSet_EP18 0x40000
#define USBEoTIntSet_EP18_BIT 18
#define USBEoTIntSet_EP19_MASK 0x80000
#define USBEoTIntSet_EP19 0x80000
#define USBEoTIntSet_EP19_BIT 19
#define USBEoTIntSet_EP20_MASK 0x100000
#define USBEoTIntSet_EP20 0x100000
#define USBEoTIntSet_EP20_BIT 20
#define USBEoTIntSet_EP21_MASK 0x200000
#define USBEoTIntSet_EP21 0x200000
#define USBEoTIntSet_EP21_BIT 21
#define USBEoTIntSet_EP22_MASK 0x400000
#define USBEoTIntSet_EP22 0x400000
#define USBEoTIntSet_EP22_BIT 22
#define USBEoTIntSet_EP23_MASK 0x800000
#define USBEoTIntSet_EP23 0x800000
#define USBEoTIntSet_EP23_BIT 23
#define USBEoTIntSet_EP24_MASK 0x1000000
#define USBEoTIntSet_EP24 0x1000000
#define USBEoTIntSet_EP24_BIT 24
#define USBEoTIntSet_EP25_MASK 0x2000000
#define USBEoTIntSet_EP25 0x2000000
#define USBEoTIntSet_EP25_BIT 25
#define USBEoTIntSet_EP26_MASK 0x4000000
#define USBEoTIntSet_EP26 0x4000000
#define USBEoTIntSet_EP26_BIT 26
#define USBEoTIntSet_EP27_MASK 0x8000000
#define USBEoTIntSet_EP27 0x8000000
#define USBEoTIntSet_EP27_BIT 27
#define USBEoTIntSet_EP28_MASK 0x10000000
#define USBEoTIntSet_EP28 0x10000000
#define USBEoTIntSet_EP28_BIT 28
#define USBEoTIntSet_EP29_MASK 0x20000000
#define USBEoTIntSet_EP29 0x20000000
#define USBEoTIntSet_EP29_BIT 29
#define USBEoTIntSet_EP30_MASK 0x40000000
#define USBEoTIntSet_EP30 0x40000000
#define USBEoTIntSet_EP30_BIT 30
#define USBEoTIntSet_EP31_MASK 0x80000000
#define USBEoTIntSet_EP31 0x80000000
#define USBEoTIntSet_EP31_BIT 31

#define USBNDDRIntSt (*(volatile unsigned *)0xFFE0C2AC)
#define USBNDDRIntSt_OFFSET 0x2AC
#define USBNDDRIntSt_EP0_MASK 0x1
#define USBNDDRIntSt_EP0 0x1
#define USBNDDRIntSt_EP0_BIT 0
#define USBNDDRIntSt_EP1_MASK 0x2
#define USBNDDRIntSt_EP1 0x2
#define USBNDDRIntSt_EP1_BIT 1
#define USBNDDRIntSt_EP2_MASK 0x4
#define USBNDDRIntSt_EP2 0x4
#define USBNDDRIntSt_EP2_BIT 2
#define USBNDDRIntSt_EP3_MASK 0x8
#define USBNDDRIntSt_EP3 0x8
#define USBNDDRIntSt_EP3_BIT 3
#define USBNDDRIntSt_EP4_MASK 0x10
#define USBNDDRIntSt_EP4 0x10
#define USBNDDRIntSt_EP4_BIT 4
#define USBNDDRIntSt_EP5_MASK 0x20
#define USBNDDRIntSt_EP5 0x20
#define USBNDDRIntSt_EP5_BIT 5
#define USBNDDRIntSt_EP6_MASK 0x40
#define USBNDDRIntSt_EP6 0x40
#define USBNDDRIntSt_EP6_BIT 6
#define USBNDDRIntSt_EP7_MASK 0x80
#define USBNDDRIntSt_EP7 0x80
#define USBNDDRIntSt_EP7_BIT 7
#define USBNDDRIntSt_EP8_MASK 0x100
#define USBNDDRIntSt_EP8 0x100
#define USBNDDRIntSt_EP8_BIT 8
#define USBNDDRIntSt_EP9_MASK 0x200
#define USBNDDRIntSt_EP9 0x200
#define USBNDDRIntSt_EP9_BIT 9
#define USBNDDRIntSt_EP10_MASK 0x400
#define USBNDDRIntSt_EP10 0x400
#define USBNDDRIntSt_EP10_BIT 10
#define USBNDDRIntSt_EP11_MASK 0x800
#define USBNDDRIntSt_EP11 0x800
#define USBNDDRIntSt_EP11_BIT 11
#define USBNDDRIntSt_EP12_MASK 0x1000
#define USBNDDRIntSt_EP12 0x1000
#define USBNDDRIntSt_EP12_BIT 12
#define USBNDDRIntSt_EP13_MASK 0x2000
#define USBNDDRIntSt_EP13 0x2000
#define USBNDDRIntSt_EP13_BIT 13
#define USBNDDRIntSt_EP14_MASK 0x4000
#define USBNDDRIntSt_EP14 0x4000
#define USBNDDRIntSt_EP14_BIT 14
#define USBNDDRIntSt_EP15_MASK 0x8000
#define USBNDDRIntSt_EP15 0x8000
#define USBNDDRIntSt_EP15_BIT 15
#define USBNDDRIntSt_EP16_MASK 0x10000
#define USBNDDRIntSt_EP16 0x10000
#define USBNDDRIntSt_EP16_BIT 16
#define USBNDDRIntSt_EP17_MASK 0x20000
#define USBNDDRIntSt_EP17 0x20000
#define USBNDDRIntSt_EP17_BIT 17
#define USBNDDRIntSt_EP18_MASK 0x40000
#define USBNDDRIntSt_EP18 0x40000
#define USBNDDRIntSt_EP18_BIT 18
#define USBNDDRIntSt_EP19_MASK 0x80000
#define USBNDDRIntSt_EP19 0x80000
#define USBNDDRIntSt_EP19_BIT 19
#define USBNDDRIntSt_EP20_MASK 0x100000
#define USBNDDRIntSt_EP20 0x100000
#define USBNDDRIntSt_EP20_BIT 20
#define USBNDDRIntSt_EP21_MASK 0x200000
#define USBNDDRIntSt_EP21 0x200000
#define USBNDDRIntSt_EP21_BIT 21
#define USBNDDRIntSt_EP22_MASK 0x400000
#define USBNDDRIntSt_EP22 0x400000
#define USBNDDRIntSt_EP22_BIT 22
#define USBNDDRIntSt_EP23_MASK 0x800000
#define USBNDDRIntSt_EP23 0x800000
#define USBNDDRIntSt_EP23_BIT 23
#define USBNDDRIntSt_EP24_MASK 0x1000000
#define USBNDDRIntSt_EP24 0x1000000
#define USBNDDRIntSt_EP24_BIT 24
#define USBNDDRIntSt_EP25_MASK 0x2000000
#define USBNDDRIntSt_EP25 0x2000000
#define USBNDDRIntSt_EP25_BIT 25
#define USBNDDRIntSt_EP26_MASK 0x4000000
#define USBNDDRIntSt_EP26 0x4000000
#define USBNDDRIntSt_EP26_BIT 26
#define USBNDDRIntSt_EP27_MASK 0x8000000
#define USBNDDRIntSt_EP27 0x8000000
#define USBNDDRIntSt_EP27_BIT 27
#define USBNDDRIntSt_EP28_MASK 0x10000000
#define USBNDDRIntSt_EP28 0x10000000
#define USBNDDRIntSt_EP28_BIT 28
#define USBNDDRIntSt_EP29_MASK 0x20000000
#define USBNDDRIntSt_EP29 0x20000000
#define USBNDDRIntSt_EP29_BIT 29
#define USBNDDRIntSt_EP30_MASK 0x40000000
#define USBNDDRIntSt_EP30 0x40000000
#define USBNDDRIntSt_EP30_BIT 30
#define USBNDDRIntSt_EP31_MASK 0x80000000
#define USBNDDRIntSt_EP31 0x80000000
#define USBNDDRIntSt_EP31_BIT 31

#define USBNDDRIntClr (*(volatile unsigned *)0xFFE0C2B0)
#define USBNDDRIntClr_OFFSET 0x2B0
#define USBNDDRIntClr_EP0_MASK 0x1
#define USBNDDRIntClr_EP0 0x1
#define USBNDDRIntClr_EP0_BIT 0
#define USBNDDRIntClr_EP1_MASK 0x2
#define USBNDDRIntClr_EP1 0x2
#define USBNDDRIntClr_EP1_BIT 1
#define USBNDDRIntClr_EP2_MASK 0x4
#define USBNDDRIntClr_EP2 0x4
#define USBNDDRIntClr_EP2_BIT 2
#define USBNDDRIntClr_EP3_MASK 0x8
#define USBNDDRIntClr_EP3 0x8
#define USBNDDRIntClr_EP3_BIT 3
#define USBNDDRIntClr_EP4_MASK 0x10
#define USBNDDRIntClr_EP4 0x10
#define USBNDDRIntClr_EP4_BIT 4
#define USBNDDRIntClr_EP5_MASK 0x20
#define USBNDDRIntClr_EP5 0x20
#define USBNDDRIntClr_EP5_BIT 5
#define USBNDDRIntClr_EP6_MASK 0x40
#define USBNDDRIntClr_EP6 0x40
#define USBNDDRIntClr_EP6_BIT 6
#define USBNDDRIntClr_EP7_MASK 0x80
#define USBNDDRIntClr_EP7 0x80
#define USBNDDRIntClr_EP7_BIT 7
#define USBNDDRIntClr_EP8_MASK 0x100
#define USBNDDRIntClr_EP8 0x100
#define USBNDDRIntClr_EP8_BIT 8
#define USBNDDRIntClr_EP9_MASK 0x200
#define USBNDDRIntClr_EP9 0x200
#define USBNDDRIntClr_EP9_BIT 9
#define USBNDDRIntClr_EP10_MASK 0x400
#define USBNDDRIntClr_EP10 0x400
#define USBNDDRIntClr_EP10_BIT 10
#define USBNDDRIntClr_EP11_MASK 0x800
#define USBNDDRIntClr_EP11 0x800
#define USBNDDRIntClr_EP11_BIT 11
#define USBNDDRIntClr_EP12_MASK 0x1000
#define USBNDDRIntClr_EP12 0x1000
#define USBNDDRIntClr_EP12_BIT 12
#define USBNDDRIntClr_EP13_MASK 0x2000
#define USBNDDRIntClr_EP13 0x2000
#define USBNDDRIntClr_EP13_BIT 13
#define USBNDDRIntClr_EP14_MASK 0x4000
#define USBNDDRIntClr_EP14 0x4000
#define USBNDDRIntClr_EP14_BIT 14
#define USBNDDRIntClr_EP15_MASK 0x8000
#define USBNDDRIntClr_EP15 0x8000
#define USBNDDRIntClr_EP15_BIT 15
#define USBNDDRIntClr_EP16_MASK 0x10000
#define USBNDDRIntClr_EP16 0x10000
#define USBNDDRIntClr_EP16_BIT 16
#define USBNDDRIntClr_EP17_MASK 0x20000
#define USBNDDRIntClr_EP17 0x20000
#define USBNDDRIntClr_EP17_BIT 17
#define USBNDDRIntClr_EP18_MASK 0x40000
#define USBNDDRIntClr_EP18 0x40000
#define USBNDDRIntClr_EP18_BIT 18
#define USBNDDRIntClr_EP19_MASK 0x80000
#define USBNDDRIntClr_EP19 0x80000
#define USBNDDRIntClr_EP19_BIT 19
#define USBNDDRIntClr_EP20_MASK 0x100000
#define USBNDDRIntClr_EP20 0x100000
#define USBNDDRIntClr_EP20_BIT 20
#define USBNDDRIntClr_EP21_MASK 0x200000
#define USBNDDRIntClr_EP21 0x200000
#define USBNDDRIntClr_EP21_BIT 21
#define USBNDDRIntClr_EP22_MASK 0x400000
#define USBNDDRIntClr_EP22 0x400000
#define USBNDDRIntClr_EP22_BIT 22
#define USBNDDRIntClr_EP23_MASK 0x800000
#define USBNDDRIntClr_EP23 0x800000
#define USBNDDRIntClr_EP23_BIT 23
#define USBNDDRIntClr_EP24_MASK 0x1000000
#define USBNDDRIntClr_EP24 0x1000000
#define USBNDDRIntClr_EP24_BIT 24
#define USBNDDRIntClr_EP25_MASK 0x2000000
#define USBNDDRIntClr_EP25 0x2000000
#define USBNDDRIntClr_EP25_BIT 25
#define USBNDDRIntClr_EP26_MASK 0x4000000
#define USBNDDRIntClr_EP26 0x4000000
#define USBNDDRIntClr_EP26_BIT 26
#define USBNDDRIntClr_EP27_MASK 0x8000000
#define USBNDDRIntClr_EP27 0x8000000
#define USBNDDRIntClr_EP27_BIT 27
#define USBNDDRIntClr_EP28_MASK 0x10000000
#define USBNDDRIntClr_EP28 0x10000000
#define USBNDDRIntClr_EP28_BIT 28
#define USBNDDRIntClr_EP29_MASK 0x20000000
#define USBNDDRIntClr_EP29 0x20000000
#define USBNDDRIntClr_EP29_BIT 29
#define USBNDDRIntClr_EP30_MASK 0x40000000
#define USBNDDRIntClr_EP30 0x40000000
#define USBNDDRIntClr_EP30_BIT 30
#define USBNDDRIntClr_EP31_MASK 0x80000000
#define USBNDDRIntClr_EP31 0x80000000
#define USBNDDRIntClr_EP31_BIT 31

#define USBNDDRIntSet (*(volatile unsigned *)0xFFE0C2B4)
#define USBNDDRIntSet_OFFSET 0x2B4
#define USBNDDRIntSet_EP0_MASK 0x1
#define USBNDDRIntSet_EP0 0x1
#define USBNDDRIntSet_EP0_BIT 0
#define USBNDDRIntSet_EP1_MASK 0x2
#define USBNDDRIntSet_EP1 0x2
#define USBNDDRIntSet_EP1_BIT 1
#define USBNDDRIntSet_EP2_MASK 0x4
#define USBNDDRIntSet_EP2 0x4
#define USBNDDRIntSet_EP2_BIT 2
#define USBNDDRIntSet_EP3_MASK 0x8
#define USBNDDRIntSet_EP3 0x8
#define USBNDDRIntSet_EP3_BIT 3
#define USBNDDRIntSet_EP4_MASK 0x10
#define USBNDDRIntSet_EP4 0x10
#define USBNDDRIntSet_EP4_BIT 4
#define USBNDDRIntSet_EP5_MASK 0x20
#define USBNDDRIntSet_EP5 0x20
#define USBNDDRIntSet_EP5_BIT 5
#define USBNDDRIntSet_EP6_MASK 0x40
#define USBNDDRIntSet_EP6 0x40
#define USBNDDRIntSet_EP6_BIT 6
#define USBNDDRIntSet_EP7_MASK 0x80
#define USBNDDRIntSet_EP7 0x80
#define USBNDDRIntSet_EP7_BIT 7
#define USBNDDRIntSet_EP8_MASK 0x100
#define USBNDDRIntSet_EP8 0x100
#define USBNDDRIntSet_EP8_BIT 8
#define USBNDDRIntSet_EP9_MASK 0x200
#define USBNDDRIntSet_EP9 0x200
#define USBNDDRIntSet_EP9_BIT 9
#define USBNDDRIntSet_EP10_MASK 0x400
#define USBNDDRIntSet_EP10 0x400
#define USBNDDRIntSet_EP10_BIT 10
#define USBNDDRIntSet_EP11_MASK 0x800
#define USBNDDRIntSet_EP11 0x800
#define USBNDDRIntSet_EP11_BIT 11
#define USBNDDRIntSet_EP12_MASK 0x1000
#define USBNDDRIntSet_EP12 0x1000
#define USBNDDRIntSet_EP12_BIT 12
#define USBNDDRIntSet_EP13_MASK 0x2000
#define USBNDDRIntSet_EP13 0x2000
#define USBNDDRIntSet_EP13_BIT 13
#define USBNDDRIntSet_EP14_MASK 0x4000
#define USBNDDRIntSet_EP14 0x4000
#define USBNDDRIntSet_EP14_BIT 14
#define USBNDDRIntSet_EP15_MASK 0x8000
#define USBNDDRIntSet_EP15 0x8000
#define USBNDDRIntSet_EP15_BIT 15
#define USBNDDRIntSet_EP16_MASK 0x10000
#define USBNDDRIntSet_EP16 0x10000
#define USBNDDRIntSet_EP16_BIT 16
#define USBNDDRIntSet_EP17_MASK 0x20000
#define USBNDDRIntSet_EP17 0x20000
#define USBNDDRIntSet_EP17_BIT 17
#define USBNDDRIntSet_EP18_MASK 0x40000
#define USBNDDRIntSet_EP18 0x40000
#define USBNDDRIntSet_EP18_BIT 18
#define USBNDDRIntSet_EP19_MASK 0x80000
#define USBNDDRIntSet_EP19 0x80000
#define USBNDDRIntSet_EP19_BIT 19
#define USBNDDRIntSet_EP20_MASK 0x100000
#define USBNDDRIntSet_EP20 0x100000
#define USBNDDRIntSet_EP20_BIT 20
#define USBNDDRIntSet_EP21_MASK 0x200000
#define USBNDDRIntSet_EP21 0x200000
#define USBNDDRIntSet_EP21_BIT 21
#define USBNDDRIntSet_EP22_MASK 0x400000
#define USBNDDRIntSet_EP22 0x400000
#define USBNDDRIntSet_EP22_BIT 22
#define USBNDDRIntSet_EP23_MASK 0x800000
#define USBNDDRIntSet_EP23 0x800000
#define USBNDDRIntSet_EP23_BIT 23
#define USBNDDRIntSet_EP24_MASK 0x1000000
#define USBNDDRIntSet_EP24 0x1000000
#define USBNDDRIntSet_EP24_BIT 24
#define USBNDDRIntSet_EP25_MASK 0x2000000
#define USBNDDRIntSet_EP25 0x2000000
#define USBNDDRIntSet_EP25_BIT 25
#define USBNDDRIntSet_EP26_MASK 0x4000000
#define USBNDDRIntSet_EP26 0x4000000
#define USBNDDRIntSet_EP26_BIT 26
#define USBNDDRIntSet_EP27_MASK 0x8000000
#define USBNDDRIntSet_EP27 0x8000000
#define USBNDDRIntSet_EP27_BIT 27
#define USBNDDRIntSet_EP28_MASK 0x10000000
#define USBNDDRIntSet_EP28 0x10000000
#define USBNDDRIntSet_EP28_BIT 28
#define USBNDDRIntSet_EP29_MASK 0x20000000
#define USBNDDRIntSet_EP29 0x20000000
#define USBNDDRIntSet_EP29_BIT 29
#define USBNDDRIntSet_EP30_MASK 0x40000000
#define USBNDDRIntSet_EP30 0x40000000
#define USBNDDRIntSet_EP30_BIT 30
#define USBNDDRIntSet_EP31_MASK 0x80000000
#define USBNDDRIntSet_EP31 0x80000000
#define USBNDDRIntSet_EP31_BIT 31

#define USBSysErrIntSt (*(volatile unsigned *)0xFFE0C2B8)
#define USBSysErrIntSt_OFFSET 0x2B8
#define USBSysErrIntSt_EP0_MASK 0x1
#define USBSysErrIntSt_EP0 0x1
#define USBSysErrIntSt_EP0_BIT 0
#define USBSysErrIntSt_EP1_MASK 0x2
#define USBSysErrIntSt_EP1 0x2
#define USBSysErrIntSt_EP1_BIT 1
#define USBSysErrIntSt_EP2_MASK 0x4
#define USBSysErrIntSt_EP2 0x4
#define USBSysErrIntSt_EP2_BIT 2
#define USBSysErrIntSt_EP3_MASK 0x8
#define USBSysErrIntSt_EP3 0x8
#define USBSysErrIntSt_EP3_BIT 3
#define USBSysErrIntSt_EP4_MASK 0x10
#define USBSysErrIntSt_EP4 0x10
#define USBSysErrIntSt_EP4_BIT 4
#define USBSysErrIntSt_EP5_MASK 0x20
#define USBSysErrIntSt_EP5 0x20
#define USBSysErrIntSt_EP5_BIT 5
#define USBSysErrIntSt_EP6_MASK 0x40
#define USBSysErrIntSt_EP6 0x40
#define USBSysErrIntSt_EP6_BIT 6
#define USBSysErrIntSt_EP7_MASK 0x80
#define USBSysErrIntSt_EP7 0x80
#define USBSysErrIntSt_EP7_BIT 7
#define USBSysErrIntSt_EP8_MASK 0x100
#define USBSysErrIntSt_EP8 0x100
#define USBSysErrIntSt_EP8_BIT 8
#define USBSysErrIntSt_EP9_MASK 0x200
#define USBSysErrIntSt_EP9 0x200
#define USBSysErrIntSt_EP9_BIT 9
#define USBSysErrIntSt_EP10_MASK 0x400
#define USBSysErrIntSt_EP10 0x400
#define USBSysErrIntSt_EP10_BIT 10
#define USBSysErrIntSt_EP11_MASK 0x800
#define USBSysErrIntSt_EP11 0x800
#define USBSysErrIntSt_EP11_BIT 11
#define USBSysErrIntSt_EP12_MASK 0x1000
#define USBSysErrIntSt_EP12 0x1000
#define USBSysErrIntSt_EP12_BIT 12
#define USBSysErrIntSt_EP13_MASK 0x2000
#define USBSysErrIntSt_EP13 0x2000
#define USBSysErrIntSt_EP13_BIT 13
#define USBSysErrIntSt_EP14_MASK 0x4000
#define USBSysErrIntSt_EP14 0x4000
#define USBSysErrIntSt_EP14_BIT 14
#define USBSysErrIntSt_EP15_MASK 0x8000
#define USBSysErrIntSt_EP15 0x8000
#define USBSysErrIntSt_EP15_BIT 15
#define USBSysErrIntSt_EP16_MASK 0x10000
#define USBSysErrIntSt_EP16 0x10000
#define USBSysErrIntSt_EP16_BIT 16
#define USBSysErrIntSt_EP17_MASK 0x20000
#define USBSysErrIntSt_EP17 0x20000
#define USBSysErrIntSt_EP17_BIT 17
#define USBSysErrIntSt_EP18_MASK 0x40000
#define USBSysErrIntSt_EP18 0x40000
#define USBSysErrIntSt_EP18_BIT 18
#define USBSysErrIntSt_EP19_MASK 0x80000
#define USBSysErrIntSt_EP19 0x80000
#define USBSysErrIntSt_EP19_BIT 19
#define USBSysErrIntSt_EP20_MASK 0x100000
#define USBSysErrIntSt_EP20 0x100000
#define USBSysErrIntSt_EP20_BIT 20
#define USBSysErrIntSt_EP21_MASK 0x200000
#define USBSysErrIntSt_EP21 0x200000
#define USBSysErrIntSt_EP21_BIT 21
#define USBSysErrIntSt_EP22_MASK 0x400000
#define USBSysErrIntSt_EP22 0x400000
#define USBSysErrIntSt_EP22_BIT 22
#define USBSysErrIntSt_EP23_MASK 0x800000
#define USBSysErrIntSt_EP23 0x800000
#define USBSysErrIntSt_EP23_BIT 23
#define USBSysErrIntSt_EP24_MASK 0x1000000
#define USBSysErrIntSt_EP24 0x1000000
#define USBSysErrIntSt_EP24_BIT 24
#define USBSysErrIntSt_EP25_MASK 0x2000000
#define USBSysErrIntSt_EP25 0x2000000
#define USBSysErrIntSt_EP25_BIT 25
#define USBSysErrIntSt_EP26_MASK 0x4000000
#define USBSysErrIntSt_EP26 0x4000000
#define USBSysErrIntSt_EP26_BIT 26
#define USBSysErrIntSt_EP27_MASK 0x8000000
#define USBSysErrIntSt_EP27 0x8000000
#define USBSysErrIntSt_EP27_BIT 27
#define USBSysErrIntSt_EP28_MASK 0x10000000
#define USBSysErrIntSt_EP28 0x10000000
#define USBSysErrIntSt_EP28_BIT 28
#define USBSysErrIntSt_EP29_MASK 0x20000000
#define USBSysErrIntSt_EP29 0x20000000
#define USBSysErrIntSt_EP29_BIT 29
#define USBSysErrIntSt_EP30_MASK 0x40000000
#define USBSysErrIntSt_EP30 0x40000000
#define USBSysErrIntSt_EP30_BIT 30
#define USBSysErrIntSt_EP31_MASK 0x80000000
#define USBSysErrIntSt_EP31 0x80000000
#define USBSysErrIntSt_EP31_BIT 31

#define USBSysErrIntClr (*(volatile unsigned *)0xFFE0C2BC)
#define USBSysErrIntClr_OFFSET 0x2BC
#define USBSysErrIntClr_EP0_MASK 0x1
#define USBSysErrIntClr_EP0 0x1
#define USBSysErrIntClr_EP0_BIT 0
#define USBSysErrIntClr_EP1_MASK 0x2
#define USBSysErrIntClr_EP1 0x2
#define USBSysErrIntClr_EP1_BIT 1
#define USBSysErrIntClr_EP2_MASK 0x4
#define USBSysErrIntClr_EP2 0x4
#define USBSysErrIntClr_EP2_BIT 2
#define USBSysErrIntClr_EP3_MASK 0x8
#define USBSysErrIntClr_EP3 0x8
#define USBSysErrIntClr_EP3_BIT 3
#define USBSysErrIntClr_EP4_MASK 0x10
#define USBSysErrIntClr_EP4 0x10
#define USBSysErrIntClr_EP4_BIT 4
#define USBSysErrIntClr_EP5_MASK 0x20
#define USBSysErrIntClr_EP5 0x20
#define USBSysErrIntClr_EP5_BIT 5
#define USBSysErrIntClr_EP6_MASK 0x40
#define USBSysErrIntClr_EP6 0x40
#define USBSysErrIntClr_EP6_BIT 6
#define USBSysErrIntClr_EP7_MASK 0x80
#define USBSysErrIntClr_EP7 0x80
#define USBSysErrIntClr_EP7_BIT 7
#define USBSysErrIntClr_EP8_MASK 0x100
#define USBSysErrIntClr_EP8 0x100
#define USBSysErrIntClr_EP8_BIT 8
#define USBSysErrIntClr_EP9_MASK 0x200
#define USBSysErrIntClr_EP9 0x200
#define USBSysErrIntClr_EP9_BIT 9
#define USBSysErrIntClr_EP10_MASK 0x400
#define USBSysErrIntClr_EP10 0x400
#define USBSysErrIntClr_EP10_BIT 10
#define USBSysErrIntClr_EP11_MASK 0x800
#define USBSysErrIntClr_EP11 0x800
#define USBSysErrIntClr_EP11_BIT 11
#define USBSysErrIntClr_EP12_MASK 0x1000
#define USBSysErrIntClr_EP12 0x1000
#define USBSysErrIntClr_EP12_BIT 12
#define USBSysErrIntClr_EP13_MASK 0x2000
#define USBSysErrIntClr_EP13 0x2000
#define USBSysErrIntClr_EP13_BIT 13
#define USBSysErrIntClr_EP14_MASK 0x4000
#define USBSysErrIntClr_EP14 0x4000
#define USBSysErrIntClr_EP14_BIT 14
#define USBSysErrIntClr_EP15_MASK 0x8000
#define USBSysErrIntClr_EP15 0x8000
#define USBSysErrIntClr_EP15_BIT 15
#define USBSysErrIntClr_EP16_MASK 0x10000
#define USBSysErrIntClr_EP16 0x10000
#define USBSysErrIntClr_EP16_BIT 16
#define USBSysErrIntClr_EP17_MASK 0x20000
#define USBSysErrIntClr_EP17 0x20000
#define USBSysErrIntClr_EP17_BIT 17
#define USBSysErrIntClr_EP18_MASK 0x40000
#define USBSysErrIntClr_EP18 0x40000
#define USBSysErrIntClr_EP18_BIT 18
#define USBSysErrIntClr_EP19_MASK 0x80000
#define USBSysErrIntClr_EP19 0x80000
#define USBSysErrIntClr_EP19_BIT 19
#define USBSysErrIntClr_EP20_MASK 0x100000
#define USBSysErrIntClr_EP20 0x100000
#define USBSysErrIntClr_EP20_BIT 20
#define USBSysErrIntClr_EP21_MASK 0x200000
#define USBSysErrIntClr_EP21 0x200000
#define USBSysErrIntClr_EP21_BIT 21
#define USBSysErrIntClr_EP22_MASK 0x400000
#define USBSysErrIntClr_EP22 0x400000
#define USBSysErrIntClr_EP22_BIT 22
#define USBSysErrIntClr_EP23_MASK 0x800000
#define USBSysErrIntClr_EP23 0x800000
#define USBSysErrIntClr_EP23_BIT 23
#define USBSysErrIntClr_EP24_MASK 0x1000000
#define USBSysErrIntClr_EP24 0x1000000
#define USBSysErrIntClr_EP24_BIT 24
#define USBSysErrIntClr_EP25_MASK 0x2000000
#define USBSysErrIntClr_EP25 0x2000000
#define USBSysErrIntClr_EP25_BIT 25
#define USBSysErrIntClr_EP26_MASK 0x4000000
#define USBSysErrIntClr_EP26 0x4000000
#define USBSysErrIntClr_EP26_BIT 26
#define USBSysErrIntClr_EP27_MASK 0x8000000
#define USBSysErrIntClr_EP27 0x8000000
#define USBSysErrIntClr_EP27_BIT 27
#define USBSysErrIntClr_EP28_MASK 0x10000000
#define USBSysErrIntClr_EP28 0x10000000
#define USBSysErrIntClr_EP28_BIT 28
#define USBSysErrIntClr_EP29_MASK 0x20000000
#define USBSysErrIntClr_EP29 0x20000000
#define USBSysErrIntClr_EP29_BIT 29
#define USBSysErrIntClr_EP30_MASK 0x40000000
#define USBSysErrIntClr_EP30 0x40000000
#define USBSysErrIntClr_EP30_BIT 30
#define USBSysErrIntClr_EP31_MASK 0x80000000
#define USBSysErrIntClr_EP31 0x80000000
#define USBSysErrIntClr_EP31_BIT 31

#define USBSysErrIntSet (*(volatile unsigned *)0xFFE0C2C0)
#define USBSysErrIntSet_OFFSET 0x2C0
#define USBSysErrIntSet_EP0_MASK 0x1
#define USBSysErrIntSet_EP0 0x1
#define USBSysErrIntSet_EP0_BIT 0
#define USBSysErrIntSet_EP1_MASK 0x2
#define USBSysErrIntSet_EP1 0x2
#define USBSysErrIntSet_EP1_BIT 1
#define USBSysErrIntSet_EP2_MASK 0x4
#define USBSysErrIntSet_EP2 0x4
#define USBSysErrIntSet_EP2_BIT 2
#define USBSysErrIntSet_EP3_MASK 0x8
#define USBSysErrIntSet_EP3 0x8
#define USBSysErrIntSet_EP3_BIT 3
#define USBSysErrIntSet_EP4_MASK 0x10
#define USBSysErrIntSet_EP4 0x10
#define USBSysErrIntSet_EP4_BIT 4
#define USBSysErrIntSet_EP5_MASK 0x20
#define USBSysErrIntSet_EP5 0x20
#define USBSysErrIntSet_EP5_BIT 5
#define USBSysErrIntSet_EP6_MASK 0x40
#define USBSysErrIntSet_EP6 0x40
#define USBSysErrIntSet_EP6_BIT 6
#define USBSysErrIntSet_EP7_MASK 0x80
#define USBSysErrIntSet_EP7 0x80
#define USBSysErrIntSet_EP7_BIT 7
#define USBSysErrIntSet_EP8_MASK 0x100
#define USBSysErrIntSet_EP8 0x100
#define USBSysErrIntSet_EP8_BIT 8
#define USBSysErrIntSet_EP9_MASK 0x200
#define USBSysErrIntSet_EP9 0x200
#define USBSysErrIntSet_EP9_BIT 9
#define USBSysErrIntSet_EP10_MASK 0x400
#define USBSysErrIntSet_EP10 0x400
#define USBSysErrIntSet_EP10_BIT 10
#define USBSysErrIntSet_EP11_MASK 0x800
#define USBSysErrIntSet_EP11 0x800
#define USBSysErrIntSet_EP11_BIT 11
#define USBSysErrIntSet_EP12_MASK 0x1000
#define USBSysErrIntSet_EP12 0x1000
#define USBSysErrIntSet_EP12_BIT 12
#define USBSysErrIntSet_EP13_MASK 0x2000
#define USBSysErrIntSet_EP13 0x2000
#define USBSysErrIntSet_EP13_BIT 13
#define USBSysErrIntSet_EP14_MASK 0x4000
#define USBSysErrIntSet_EP14 0x4000
#define USBSysErrIntSet_EP14_BIT 14
#define USBSysErrIntSet_EP15_MASK 0x8000
#define USBSysErrIntSet_EP15 0x8000
#define USBSysErrIntSet_EP15_BIT 15
#define USBSysErrIntSet_EP16_MASK 0x10000
#define USBSysErrIntSet_EP16 0x10000
#define USBSysErrIntSet_EP16_BIT 16
#define USBSysErrIntSet_EP17_MASK 0x20000
#define USBSysErrIntSet_EP17 0x20000
#define USBSysErrIntSet_EP17_BIT 17
#define USBSysErrIntSet_EP18_MASK 0x40000
#define USBSysErrIntSet_EP18 0x40000
#define USBSysErrIntSet_EP18_BIT 18
#define USBSysErrIntSet_EP19_MASK 0x80000
#define USBSysErrIntSet_EP19 0x80000
#define USBSysErrIntSet_EP19_BIT 19
#define USBSysErrIntSet_EP20_MASK 0x100000
#define USBSysErrIntSet_EP20 0x100000
#define USBSysErrIntSet_EP20_BIT 20
#define USBSysErrIntSet_EP21_MASK 0x200000
#define USBSysErrIntSet_EP21 0x200000
#define USBSysErrIntSet_EP21_BIT 21
#define USBSysErrIntSet_EP22_MASK 0x400000
#define USBSysErrIntSet_EP22 0x400000
#define USBSysErrIntSet_EP22_BIT 22
#define USBSysErrIntSet_EP23_MASK 0x800000
#define USBSysErrIntSet_EP23 0x800000
#define USBSysErrIntSet_EP23_BIT 23
#define USBSysErrIntSet_EP24_MASK 0x1000000
#define USBSysErrIntSet_EP24 0x1000000
#define USBSysErrIntSet_EP24_BIT 24
#define USBSysErrIntSet_EP25_MASK 0x2000000
#define USBSysErrIntSet_EP25 0x2000000
#define USBSysErrIntSet_EP25_BIT 25
#define USBSysErrIntSet_EP26_MASK 0x4000000
#define USBSysErrIntSet_EP26 0x4000000
#define USBSysErrIntSet_EP26_BIT 26
#define USBSysErrIntSet_EP27_MASK 0x8000000
#define USBSysErrIntSet_EP27 0x8000000
#define USBSysErrIntSet_EP27_BIT 27
#define USBSysErrIntSet_EP28_MASK 0x10000000
#define USBSysErrIntSet_EP28 0x10000000
#define USBSysErrIntSet_EP28_BIT 28
#define USBSysErrIntSet_EP29_MASK 0x20000000
#define USBSysErrIntSet_EP29 0x20000000
#define USBSysErrIntSet_EP29_BIT 29
#define USBSysErrIntSet_EP30_MASK 0x40000000
#define USBSysErrIntSet_EP30 0x40000000
#define USBSysErrIntSet_EP30_BIT 30
#define USBSysErrIntSet_EP31_MASK 0x80000000
#define USBSysErrIntSet_EP31 0x80000000
#define USBSysErrIntSet_EP31_BIT 31

#define I2C_RX (*(volatile unsigned *)0xFFE0C300)
#define I2C_RX_OFFSET 0x300
#define I2C_RX_RXData_MASK 0xFF
#define I2C_RX_RXData_BIT 0

#define I2C_TX (*(volatile unsigned *)0xFFE0C300)
#define I2C_TX_OFFSET 0x300
#define I2C_TX_TXData_MASK 0xFF
#define I2C_TX_TXData_BIT 0
#define I2C_TX_START_MASK 0xFF00
#define I2C_TX_START_BIT 8
#define I2C_TX_STOP_MASK 0x1FE00
#define I2C_TX_STOP_BIT 9

#define I2C_STS (*(volatile unsigned *)0xFFE0C304)
#define I2C_STS_OFFSET 0x304
#define I2C_STS_TDI_MASK 0x1
#define I2C_STS_TDI 0x1
#define I2C_STS_TDI_BIT 0
#define I2C_STS_AFI_MASK 0x2
#define I2C_STS_AFI 0x2
#define I2C_STS_AFI_BIT 1
#define I2C_STS_NAI_MASK 0x4
#define I2C_STS_NAI 0x4
#define I2C_STS_NAI_BIT 2
#define I2C_STS_DRMI_MASK 0x8
#define I2C_STS_DRMI 0x8
#define I2C_STS_DRMI_BIT 3
#define I2C_STS_DRSI_MASK 0x10
#define I2C_STS_DRSI 0x10
#define I2C_STS_DRSI_BIT 4
#define I2C_STS_Active_MASK 0x20
#define I2C_STS_Active 0x20
#define I2C_STS_Active_BIT 5
#define I2C_STS_SCL_MASK 0x40
#define I2C_STS_SCL 0x40
#define I2C_STS_SCL_BIT 6
#define I2C_STS_SDA_MASK 0x80
#define I2C_STS_SDA 0x80
#define I2C_STS_SDA_BIT 7
#define I2C_STS_RFF_MASK 0x100
#define I2C_STS_RFF 0x100
#define I2C_STS_RFF_BIT 8
#define I2C_STS_RFE_MASK 0x200
#define I2C_STS_RFE 0x200
#define I2C_STS_RFE_BIT 9
#define I2C_STS_TFF_MASK 0x400
#define I2C_STS_TFF 0x400
#define I2C_STS_TFF_BIT 10
#define I2C_STS_TFE_MASK 0x800
#define I2C_STS_TFE 0x800
#define I2C_STS_TFE_BIT 11

#define I2C_CTL (*(volatile unsigned *)0xFFE0C308)
#define I2C_CTL_OFFSET 0x308
#define I2C_CTL_TDIE_MASK 0x1
#define I2C_CTL_TDIE 0x1
#define I2C_CTL_TDIE_BIT 0
#define I2C_CTL_AFIE_MASK 0x2
#define I2C_CTL_AFIE 0x2
#define I2C_CTL_AFIE_BIT 1
#define I2C_CTL_NAIE_MASK 0x4
#define I2C_CTL_NAIE 0x4
#define I2C_CTL_NAIE_BIT 2
#define I2C_CTL_DRMIE_MASK 0x8
#define I2C_CTL_DRMIE 0x8
#define I2C_CTL_DRMIE_BIT 3
#define I2C_CTL_DRSIE_MASK 0x10
#define I2C_CTL_DRSIE 0x10
#define I2C_CTL_DRSIE_BIT 4
#define I2C_CTL_REFIE_MASK 0x20
#define I2C_CTL_REFIE 0x20
#define I2C_CTL_REFIE_BIT 5
#define I2C_CTL_RFDAIE_MASK 0x40
#define I2C_CTL_RFDAIE 0x40
#define I2C_CTL_RFDAIE_BIT 6
#define I2C_CTL_TFFIE_MASK 0x80
#define I2C_CTL_TFFIE 0x80
#define I2C_CTL_TFFIE_BIT 7
#define I2C_CTL_SRST_MASK 0x100
#define I2C_CTL_SRST 0x100
#define I2C_CTL_SRST_BIT 8

#define I2C_CLKHI (*(volatile unsigned *)0xFFE0C30C)
#define I2C_CLKHI_OFFSET 0x30C
#define I2C_CLKHI_CDHI_MASK 0xFF
#define I2C_CLKHI_CDHI_BIT 0

#define I2C_CLKLO (*(volatile unsigned *)0xFFE0C310)
#define I2C_CLKLO_OFFSET 0x310
#define I2C_CLKLO_CDLO_MASK 0xFF
#define I2C_CLKLO_CDLO_BIT 0

#define USBClkCtrl (*(volatile unsigned *)0xFFE0CFF4)
#define USBClkCtrl_OFFSET 0xFF4
#define USBClkCtrl_DEV_CLK_EN_MASK 0x2
#define USBClkCtrl_DEV_CLK_EN 0x2
#define USBClkCtrl_DEV_CLK_EN_BIT 1
#define USBClkCtrl_PORTSEL_CLK_EN_MASK 0x8
#define USBClkCtrl_PORTSEL_CLK_EN 0x8
#define USBClkCtrl_PORTSEL_CLK_EN_BIT 3
#define USBClkCtrl_AHB_CLK_EN_MASK 0x10
#define USBClkCtrl_AHB_CLK_EN 0x10
#define USBClkCtrl_AHB_CLK_EN_BIT 4

#define USBClkSt (*(volatile unsigned *)0xFFE0CFF8)
#define USBClkSt_OFFSET 0xFF8
#define USBClkSt_DEV_CLK_EN_MASK 0x2
#define USBClkSt_DEV_CLK_EN 0x2
#define USBClkSt_DEV_CLK_EN_BIT 1
#define USBClkSt_PORTSEL_CLK_EN_MASK 0x8
#define USBClkSt_PORTSEL_CLK_EN 0x8
#define USBClkSt_PORTSEL_CLK_EN_BIT 3
#define USBClkSt_AHB_CLK_EN_MASK 0x10
#define USBClkSt_AHB_CLK_EN 0x10
#define USBClkSt_AHB_CLK_EN_BIT 4

#define OTGClkCtrl (*(volatile unsigned *)0xFFE0CFF4)
#define OTGClkCtrl_OFFSET 0xFF4
#define OTGClkCtrl_HOST_CLK_EN_MASK 0x1
#define OTGClkCtrl_HOST_CLK_EN 0x1
#define OTGClkCtrl_HOST_CLK_EN_BIT 0
#define OTGClkCtrl_DEV_CLK_EN_MASK 0x2
#define OTGClkCtrl_DEV_CLK_EN 0x2
#define OTGClkCtrl_DEV_CLK_EN_BIT 1
#define OTGClkCtrl_I2C_CLK_EN_MASK 0x4
#define OTGClkCtrl_I2C_CLK_EN 0x4
#define OTGClkCtrl_I2C_CLK_EN_BIT 2
#define OTGClkCtrl_OTG_CLK_EN_MASK 0x8
#define OTGClkCtrl_OTG_CLK_EN 0x8
#define OTGClkCtrl_OTG_CLK_EN_BIT 3
#define OTGClkCtrl_AHB_CLK_EN_MASK 0x10
#define OTGClkCtrl_AHB_CLK_EN 0x10
#define OTGClkCtrl_AHB_CLK_EN_BIT 4

#define OTGClkSt (*(volatile unsigned *)0xFFE0CFF8)
#define OTGClkSt_OFFSET 0xFF8
#define OTGClkSt_HOST_CLK_EN_MASK 0x1
#define OTGClkSt_HOST_CLK_EN 0x1
#define OTGClkSt_HOST_CLK_EN_BIT 0
#define OTGClkSt_DEV_CLK_EN_MASK 0x2
#define OTGClkSt_DEV_CLK_EN 0x2
#define OTGClkSt_DEV_CLK_EN_BIT 1
#define OTGClkSt_I2C_CLK_EN_MASK 0x4
#define OTGClkSt_I2C_CLK_EN 0x4
#define OTGClkSt_I2C_CLK_EN_BIT 2
#define OTGClkSt_OTG_CLK_EN_MASK 0x8
#define OTGClkSt_OTG_CLK_EN 0x8
#define OTGClkSt_OTG_CLK_EN_BIT 3
#define OTGClkSt_AHB_CLK_EN_MASK 0x10
#define OTGClkSt_AHB_CLK_EN 0x10
#define OTGClkSt_AHB_CLK_EN_BIT 4

#define LCD_BASE 0xFFE10000

#define LCD_TIMH (*(volatile unsigned *)0xFFE10000)
#define LCD_TIMH_OFFSET 0x0
#define LCD_TIMH_PPL_MASK 0xFC
#define LCD_TIMH_PPL_BIT 2
#define LCD_TIMH_HSW_MASK 0xFF00
#define LCD_TIMH_HSW_BIT 8
#define LCD_TIMH_HFP_MASK 0xFF0000
#define LCD_TIMH_HFP_BIT 16
#define LCD_TIMH_HBP_MASK 0xFF000000
#define LCD_TIMH_HBP_BIT 24

#define LCD_TIMV (*(volatile unsigned *)0xFFE10004)
#define LCD_TIMV_OFFSET 0x4
#define LCD_TIMV_LPP_MASK 0x3FF
#define LCD_TIMV_LPP_BIT 0
#define LCD_TIMV_VSW_MASK 0xFC00
#define LCD_TIMV_VSW_BIT 10
#define LCD_TIMV_VFP_MASK 0xFF0000
#define LCD_TIMV_VFP_BIT 16
#define LCD_TIMV_VBP_MASK 0xFF000000
#define LCD_TIMV_VBP_BIT 24

#define LCD_POL (*(volatile unsigned *)0xFFE10008)
#define LCD_POL_OFFSET 0x8
#define LCD_POL_PCD_LO_MASK 0x1F
#define LCD_POL_PCD_LO_BIT 0
#define LCD_POL_CLKSEL_MASK 0x20
#define LCD_POL_CLKSEL 0x20
#define LCD_POL_CLKSEL_BIT 5
#define LCD_POL_ACB_MASK 0x7C0
#define LCD_POL_ACB_BIT 6
#define LCD_POL_IVS_MASK 0x800
#define LCD_POL_IVS 0x800
#define LCD_POL_IVS_BIT 11
#define LCD_POL_IHS_MASK 0x1000
#define LCD_POL_IHS 0x1000
#define LCD_POL_IHS_BIT 12
#define LCD_POL_IPC_MASK 0x2000
#define LCD_POL_IPC 0x2000
#define LCD_POL_IPC_BIT 13
#define LCD_POL_IOE_MASK 0x4000
#define LCD_POL_IOE 0x4000
#define LCD_POL_IOE_BIT 14
#define LCD_POL_CPL_MASK 0x3FF0000
#define LCD_POL_CPL_BIT 16
#define LCD_POL_BCD_MASK 0x4000000
#define LCD_POL_BCD 0x4000000
#define LCD_POL_BCD_BIT 26
#define LCD_POL_PCD_HI_MASK 0xF8000000
#define LCD_POL_PCD_HI_BIT 27

#define LCD_LE (*(volatile unsigned *)0xFFE1000C)
#define LCD_LE_OFFSET 0xC
#define LCD_LE_LED_MASK 0x7F
#define LCD_LE_LED_BIT 0
#define LCD_LE_LEE_MASK 0x10000
#define LCD_LE_LEE 0x10000
#define LCD_LE_LEE_BIT 16

#define LCD_UPBASE (*(volatile unsigned *)0xFFE10010)
#define LCD_UPBASE_OFFSET 0x10
#define LCD_UPBASE_LCDUPBASE_MASK 0xFFFFFFF8
#define LCD_UPBASE_LCDUPBASE_BIT 3

#define LCD_LPBASE (*(volatile unsigned *)0xFFE10014)
#define LCD_LPBASE_OFFSET 0x14
#define LCD_LPBASE_LCDLPBASE_MASK 0xFFFFFFF8
#define LCD_LPBASE_LCDLPBASE_BIT 3

#define LCD_CTRL (*(volatile unsigned *)0xFFE10018)
#define LCD_CTRL_OFFSET 0x18
#define LCD_CTRL_LcdEn_MASK 0x1
#define LCD_CTRL_LcdEn 0x1
#define LCD_CTRL_LcdEn_BIT 0
#define LCD_CTRL_LcdBpp_MASK 0xE
#define LCD_CTRL_LcdBpp_BIT 1
#define LCD_CTRL_LcdBW_MASK 0x10
#define LCD_CTRL_LcdBW 0x10
#define LCD_CTRL_LcdBW_BIT 4
#define LCD_CTRL_LcdTFT_MASK 0x20
#define LCD_CTRL_LcdTFT 0x20
#define LCD_CTRL_LcdTFT_BIT 5
#define LCD_CTRL_LcdMon8_MASK 0x40
#define LCD_CTRL_LcdMon8 0x40
#define LCD_CTRL_LcdMon8_BIT 6
#define LCD_CTRL_LcdDual_MASK 0x80
#define LCD_CTRL_LcdDual 0x80
#define LCD_CTRL_LcdDual_BIT 7
#define LCD_CTRL_BGR_MASK 0x100
#define LCD_CTRL_BGR 0x100
#define LCD_CTRL_BGR_BIT 8
#define LCD_CTRL_BEBO_MASK 0x200
#define LCD_CTRL_BEBO 0x200
#define LCD_CTRL_BEBO_BIT 9
#define LCD_CTRL_BEPO_MASK 0x400
#define LCD_CTRL_BEPO 0x400
#define LCD_CTRL_BEPO_BIT 10
#define LCD_CTRL_LcdPwr_MASK 0x800
#define LCD_CTRL_LcdPwr 0x800
#define LCD_CTRL_LcdPwr_BIT 11
#define LCD_CTRL_LcdVComp_MASK 0x3000
#define LCD_CTRL_LcdVComp_BIT 12
#define LCD_CTRL_WATERMARK_MASK 0x10000
#define LCD_CTRL_WATERMARK 0x10000
#define LCD_CTRL_WATERMARK_BIT 16

#define LCD_INTMSK (*(volatile unsigned *)0xFFE1001C)
#define LCD_INTMSK_OFFSET 0x1C
#define LCD_INTMSK_FUFIM_MASK 0x2
#define LCD_INTMSK_FUFIM 0x2
#define LCD_INTMSK_FUFIM_BIT 1
#define LCD_INTMSK_LNBUIM_MASK 0x4
#define LCD_INTMSK_LNBUIM 0x4
#define LCD_INTMSK_LNBUIM_BIT 2
#define LCD_INTMSK_VCompIM_MASK 0x8
#define LCD_INTMSK_VCompIM 0x8
#define LCD_INTMSK_VCompIM_BIT 3
#define LCD_INTMSK_BERIM_MASK 0x10
#define LCD_INTMSK_BERIM 0x10
#define LCD_INTMSK_BERIM_BIT 4

#define LCD_INTRAW (*(volatile unsigned *)0xFFE10020)
#define LCD_INTRAW_OFFSET 0x20
#define LCD_INTRAW_FUFRIS_MASK 0x2
#define LCD_INTRAW_FUFRIS 0x2
#define LCD_INTRAW_FUFRIS_BIT 1
#define LCD_INTRAW_LNBURIS_MASK 0x4
#define LCD_INTRAW_LNBURIS 0x4
#define LCD_INTRAW_LNBURIS_BIT 2
#define LCD_INTRAW_VCompRIS_MASK 0x8
#define LCD_INTRAW_VCompRIS 0x8
#define LCD_INTRAW_VCompRIS_BIT 3
#define LCD_INTRAW_BERRAW_MASK 0x10
#define LCD_INTRAW_BERRAW 0x10
#define LCD_INTRAW_BERRAW_BIT 4

#define LCD_INTSTAT (*(volatile unsigned *)0xFFE10024)
#define LCD_INTSTAT_OFFSET 0x24
#define LCD_INTSTAT_FUFMIS_MASK 0x2
#define LCD_INTSTAT_FUFMIS 0x2
#define LCD_INTSTAT_FUFMIS_BIT 1
#define LCD_INTSTAT_LNBUMIS_MASK 0x4
#define LCD_INTSTAT_LNBUMIS 0x4
#define LCD_INTSTAT_LNBUMIS_BIT 2
#define LCD_INTSTAT_VCompMIS_MASK 0x8
#define LCD_INTSTAT_VCompMIS 0x8
#define LCD_INTSTAT_VCompMIS_BIT 3
#define LCD_INTSTAT_BERMIS_MASK 0x10
#define LCD_INTSTAT_BERMIS 0x10
#define LCD_INTSTAT_BERMIS_BIT 4

#define LCD_INTCLR (*(volatile unsigned *)0xFFE10028)
#define LCD_INTCLR_OFFSET 0x28
#define LCD_INTCLR_FUFIC_MASK 0x2
#define LCD_INTCLR_FUFIC 0x2
#define LCD_INTCLR_FUFIC_BIT 1
#define LCD_INTCLR_LNBUIC_MASK 0x4
#define LCD_INTCLR_LNBUIC 0x4
#define LCD_INTCLR_LNBUIC_BIT 2
#define LCD_INTCLR_VCompIC_MASK 0x8
#define LCD_INTCLR_VCompIC 0x8
#define LCD_INTCLR_VCompIC_BIT 3
#define LCD_INTCLR_BERIC_MASK 0x10
#define LCD_INTCLR_BERIC 0x10
#define LCD_INTCLR_BERIC_BIT 4

#define LCD_UPCURR (*(volatile unsigned *)0xFFE1002C)
#define LCD_UPCURR_OFFSET 0x2C

#define LCD_LPCURR (*(volatile unsigned *)0xFFE10030)
#define LCD_LPCURR_OFFSET 0x30

#define LCD_PAL0 (*(volatile unsigned long *)0xFFE10200)
#define LCD_PAL0_OFFSET 0x200

#define LCD_PAL1 (*(volatile unsigned long *)0xFFE10204)
#define LCD_PAL1_OFFSET 0x204

#define LCD_PAL2 (*(volatile unsigned long *)0xFFE10208)
#define LCD_PAL2_OFFSET 0x208

#define LCD_PAL3 (*(volatile unsigned long *)0xFFE1020C)
#define LCD_PAL3_OFFSET 0x20C

#define LCD_PAL4 (*(volatile unsigned long *)0xFFE10210)
#define LCD_PAL4_OFFSET 0x210

#define LCD_PAL5 (*(volatile unsigned long *)0xFFE10214)
#define LCD_PAL5_OFFSET 0x214

#define LCD_PAL6 (*(volatile unsigned long *)0xFFE10218)
#define LCD_PAL6_OFFSET 0x218

#define LCD_PAL7 (*(volatile unsigned long *)0xFFE1021C)
#define LCD_PAL7_OFFSET 0x21C

#define LCD_PAL8 (*(volatile unsigned long *)0xFFE10220)
#define LCD_PAL8_OFFSET 0x220

#define LCD_PAL9 (*(volatile unsigned long *)0xFFE10224)
#define LCD_PAL9_OFFSET 0x224

#define LCD_PAL10 (*(volatile unsigned long *)0xFFE10228)
#define LCD_PAL10_OFFSET 0x228

#define LCD_PAL11 (*(volatile unsigned long *)0xFFE1022C)
#define LCD_PAL11_OFFSET 0x22C

#define LCD_PAL12 (*(volatile unsigned long *)0xFFE10230)
#define LCD_PAL12_OFFSET 0x230

#define LCD_PAL13 (*(volatile unsigned long *)0xFFE10234)
#define LCD_PAL13_OFFSET 0x234

#define LCD_PAL14 (*(volatile unsigned long *)0xFFE10238)
#define LCD_PAL14_OFFSET 0x238

#define LCD_PAL15 (*(volatile unsigned long *)0xFFE1023C)
#define LCD_PAL15_OFFSET 0x23C

#define LCD_PAL16 (*(volatile unsigned long *)0xFFE10240)
#define LCD_PAL16_OFFSET 0x240

#define LCD_PAL17 (*(volatile unsigned long *)0xFFE10244)
#define LCD_PAL17_OFFSET 0x244

#define LCD_PAL18 (*(volatile unsigned long *)0xFFE10248)
#define LCD_PAL18_OFFSET 0x248

#define LCD_PAL19 (*(volatile unsigned long *)0xFFE1024C)
#define LCD_PAL19_OFFSET 0x24C

#define LCD_PAL20 (*(volatile unsigned long *)0xFFE10250)
#define LCD_PAL20_OFFSET 0x250

#define LCD_PAL21 (*(volatile unsigned long *)0xFFE10254)
#define LCD_PAL21_OFFSET 0x254

#define LCD_PAL22 (*(volatile unsigned long *)0xFFE10258)
#define LCD_PAL22_OFFSET 0x258

#define LCD_PAL23 (*(volatile unsigned long *)0xFFE1025C)
#define LCD_PAL23_OFFSET 0x25C

#define LCD_PAL24 (*(volatile unsigned long *)0xFFE10260)
#define LCD_PAL24_OFFSET 0x260

#define LCD_PAL25 (*(volatile unsigned long *)0xFFE10264)
#define LCD_PAL25_OFFSET 0x264

#define LCD_PAL26 (*(volatile unsigned long *)0xFFE10268)
#define LCD_PAL26_OFFSET 0x268

#define LCD_PAL27 (*(volatile unsigned long *)0xFFE1026C)
#define LCD_PAL27_OFFSET 0x26C

#define LCD_PAL28 (*(volatile unsigned long *)0xFFE10270)
#define LCD_PAL28_OFFSET 0x270

#define LCD_PAL29 (*(volatile unsigned long *)0xFFE10274)
#define LCD_PAL29_OFFSET 0x274

#define LCD_PAL30 (*(volatile unsigned long *)0xFFE10278)
#define LCD_PAL30_OFFSET 0x278

#define LCD_PAL31 (*(volatile unsigned long *)0xFFE1027C)
#define LCD_PAL31_OFFSET 0x27C

#define LCD_PAL32 (*(volatile unsigned long *)0xFFE10280)
#define LCD_PAL32_OFFSET 0x280

#define LCD_PAL33 (*(volatile unsigned long *)0xFFE10284)
#define LCD_PAL33_OFFSET 0x284

#define LCD_PAL34 (*(volatile unsigned long *)0xFFE10288)
#define LCD_PAL34_OFFSET 0x288

#define LCD_PAL35 (*(volatile unsigned long *)0xFFE1028C)
#define LCD_PAL35_OFFSET 0x28C

#define LCD_PAL36 (*(volatile unsigned long *)0xFFE10290)
#define LCD_PAL36_OFFSET 0x290

#define LCD_PAL37 (*(volatile unsigned long *)0xFFE10294)
#define LCD_PAL37_OFFSET 0x294

#define LCD_PAL38 (*(volatile unsigned long *)0xFFE10298)
#define LCD_PAL38_OFFSET 0x298

#define LCD_PAL39 (*(volatile unsigned long *)0xFFE1029C)
#define LCD_PAL39_OFFSET 0x29C

#define LCD_PAL40 (*(volatile unsigned long *)0xFFE102A0)
#define LCD_PAL40_OFFSET 0x2A0

#define LCD_PAL41 (*(volatile unsigned long *)0xFFE102A4)
#define LCD_PAL41_OFFSET 0x2A4

#define LCD_PAL42 (*(volatile unsigned long *)0xFFE102A8)
#define LCD_PAL42_OFFSET 0x2A8

#define LCD_PAL43 (*(volatile unsigned long *)0xFFE102AC)
#define LCD_PAL43_OFFSET 0x2AC

#define LCD_PAL44 (*(volatile unsigned long *)0xFFE102B0)
#define LCD_PAL44_OFFSET 0x2B0

#define LCD_PAL45 (*(volatile unsigned long *)0xFFE102B4)
#define LCD_PAL45_OFFSET 0x2B4

#define LCD_PAL46 (*(volatile unsigned long *)0xFFE102B8)
#define LCD_PAL46_OFFSET 0x2B8

#define LCD_PAL47 (*(volatile unsigned long *)0xFFE102BC)
#define LCD_PAL47_OFFSET 0x2BC

#define LCD_PAL48 (*(volatile unsigned long *)0xFFE102C0)
#define LCD_PAL48_OFFSET 0x2C0

#define LCD_PAL49 (*(volatile unsigned long *)0xFFE102C4)
#define LCD_PAL49_OFFSET 0x2C4

#define LCD_PAL50 (*(volatile unsigned long *)0xFFE102C8)
#define LCD_PAL50_OFFSET 0x2C8

#define LCD_PAL51 (*(volatile unsigned long *)0xFFE102CC)
#define LCD_PAL51_OFFSET 0x2CC

#define LCD_PAL52 (*(volatile unsigned long *)0xFFE102D0)
#define LCD_PAL52_OFFSET 0x2D0

#define LCD_PAL53 (*(volatile unsigned long *)0xFFE102D4)
#define LCD_PAL53_OFFSET 0x2D4

#define LCD_PAL54 (*(volatile unsigned long *)0xFFE102D8)
#define LCD_PAL54_OFFSET 0x2D8

#define LCD_PAL55 (*(volatile unsigned long *)0xFFE102DC)
#define LCD_PAL55_OFFSET 0x2DC

#define LCD_PAL56 (*(volatile unsigned long *)0xFFE102E0)
#define LCD_PAL56_OFFSET 0x2E0

#define LCD_PAL57 (*(volatile unsigned long *)0xFFE102E4)
#define LCD_PAL57_OFFSET 0x2E4

#define LCD_PAL58 (*(volatile unsigned long *)0xFFE102E8)
#define LCD_PAL58_OFFSET 0x2E8

#define LCD_PAL59 (*(volatile unsigned long *)0xFFE102EC)
#define LCD_PAL59_OFFSET 0x2EC

#define LCD_PAL60 (*(volatile unsigned long *)0xFFE102F0)
#define LCD_PAL60_OFFSET 0x2F0

#define LCD_PAL61 (*(volatile unsigned long *)0xFFE102F4)
#define LCD_PAL61_OFFSET 0x2F4

#define LCD_PAL62 (*(volatile unsigned long *)0xFFE102F8)
#define LCD_PAL62_OFFSET 0x2F8

#define LCD_PAL63 (*(volatile unsigned long *)0xFFE102FC)
#define LCD_PAL63_OFFSET 0x2FC

#define LCD_PAL64 (*(volatile unsigned long *)0xFFE10300)
#define LCD_PAL64_OFFSET 0x300

#define LCD_PAL65 (*(volatile unsigned long *)0xFFE10304)
#define LCD_PAL65_OFFSET 0x304

#define LCD_PAL66 (*(volatile unsigned long *)0xFFE10308)
#define LCD_PAL66_OFFSET 0x308

#define LCD_PAL67 (*(volatile unsigned long *)0xFFE1030C)
#define LCD_PAL67_OFFSET 0x30C

#define LCD_PAL68 (*(volatile unsigned long *)0xFFE10310)
#define LCD_PAL68_OFFSET 0x310

#define LCD_PAL69 (*(volatile unsigned long *)0xFFE10314)
#define LCD_PAL69_OFFSET 0x314

#define LCD_PAL70 (*(volatile unsigned long *)0xFFE10318)
#define LCD_PAL70_OFFSET 0x318

#define LCD_PAL71 (*(volatile unsigned long *)0xFFE1031C)
#define LCD_PAL71_OFFSET 0x31C

#define LCD_PAL72 (*(volatile unsigned long *)0xFFE10320)
#define LCD_PAL72_OFFSET 0x320

#define LCD_PAL73 (*(volatile unsigned long *)0xFFE10324)
#define LCD_PAL73_OFFSET 0x324

#define LCD_PAL74 (*(volatile unsigned long *)0xFFE10328)
#define LCD_PAL74_OFFSET 0x328

#define LCD_PAL75 (*(volatile unsigned long *)0xFFE1032C)
#define LCD_PAL75_OFFSET 0x32C

#define LCD_PAL76 (*(volatile unsigned long *)0xFFE10330)
#define LCD_PAL76_OFFSET 0x330

#define LCD_PAL77 (*(volatile unsigned long *)0xFFE10334)
#define LCD_PAL77_OFFSET 0x334

#define LCD_PAL78 (*(volatile unsigned long *)0xFFE10338)
#define LCD_PAL78_OFFSET 0x338

#define LCD_PAL79 (*(volatile unsigned long *)0xFFE1033C)
#define LCD_PAL79_OFFSET 0x33C

#define LCD_PAL80 (*(volatile unsigned long *)0xFFE10340)
#define LCD_PAL80_OFFSET 0x340

#define LCD_PAL81 (*(volatile unsigned long *)0xFFE10344)
#define LCD_PAL81_OFFSET 0x344

#define LCD_PAL82 (*(volatile unsigned long *)0xFFE10348)
#define LCD_PAL82_OFFSET 0x348

#define LCD_PAL83 (*(volatile unsigned long *)0xFFE1034C)
#define LCD_PAL83_OFFSET 0x34C

#define LCD_PAL84 (*(volatile unsigned long *)0xFFE10350)
#define LCD_PAL84_OFFSET 0x350

#define LCD_PAL85 (*(volatile unsigned long *)0xFFE10354)
#define LCD_PAL85_OFFSET 0x354

#define LCD_PAL86 (*(volatile unsigned long *)0xFFE10358)
#define LCD_PAL86_OFFSET 0x358

#define LCD_PAL87 (*(volatile unsigned long *)0xFFE1035C)
#define LCD_PAL87_OFFSET 0x35C

#define LCD_PAL88 (*(volatile unsigned long *)0xFFE10360)
#define LCD_PAL88_OFFSET 0x360

#define LCD_PAL89 (*(volatile unsigned long *)0xFFE10364)
#define LCD_PAL89_OFFSET 0x364

#define LCD_PAL90 (*(volatile unsigned long *)0xFFE10368)
#define LCD_PAL90_OFFSET 0x368

#define LCD_PAL91 (*(volatile unsigned long *)0xFFE1036C)
#define LCD_PAL91_OFFSET 0x36C

#define LCD_PAL92 (*(volatile unsigned long *)0xFFE10370)
#define LCD_PAL92_OFFSET 0x370

#define LCD_PAL93 (*(volatile unsigned long *)0xFFE10374)
#define LCD_PAL93_OFFSET 0x374

#define LCD_PAL94 (*(volatile unsigned long *)0xFFE10378)
#define LCD_PAL94_OFFSET 0x378

#define LCD_PAL95 (*(volatile unsigned long *)0xFFE1037C)
#define LCD_PAL95_OFFSET 0x37C

#define LCD_PAL96 (*(volatile unsigned long *)0xFFE10380)
#define LCD_PAL96_OFFSET 0x380

#define LCD_PAL97 (*(volatile unsigned long *)0xFFE10384)
#define LCD_PAL97_OFFSET 0x384

#define LCD_PAL98 (*(volatile unsigned long *)0xFFE10388)
#define LCD_PAL98_OFFSET 0x388

#define LCD_PAL99 (*(volatile unsigned long *)0xFFE1038C)
#define LCD_PAL99_OFFSET 0x38C

#define LCD_PAL100 (*(volatile unsigned long *)0xFFE10390)
#define LCD_PAL100_OFFSET 0x390

#define LCD_PAL101 (*(volatile unsigned long *)0xFFE10394)
#define LCD_PAL101_OFFSET 0x394

#define LCD_PAL102 (*(volatile unsigned long *)0xFFE10398)
#define LCD_PAL102_OFFSET 0x398

#define LCD_PAL103 (*(volatile unsigned long *)0xFFE1039C)
#define LCD_PAL103_OFFSET 0x39C

#define LCD_PAL104 (*(volatile unsigned long *)0xFFE103A0)
#define LCD_PAL104_OFFSET 0x3A0

#define LCD_PAL105 (*(volatile unsigned long *)0xFFE103A4)
#define LCD_PAL105_OFFSET 0x3A4

#define LCD_PAL106 (*(volatile unsigned long *)0xFFE103A8)
#define LCD_PAL106_OFFSET 0x3A8

#define LCD_PAL107 (*(volatile unsigned long *)0xFFE103AC)
#define LCD_PAL107_OFFSET 0x3AC

#define LCD_PAL108 (*(volatile unsigned long *)0xFFE103B0)
#define LCD_PAL108_OFFSET 0x3B0

#define LCD_PAL109 (*(volatile unsigned long *)0xFFE103B4)
#define LCD_PAL109_OFFSET 0x3B4

#define LCD_PAL110 (*(volatile unsigned long *)0xFFE103B8)
#define LCD_PAL110_OFFSET 0x3B8

#define LCD_PAL111 (*(volatile unsigned long *)0xFFE103BC)
#define LCD_PAL111_OFFSET 0x3BC

#define LCD_PAL112 (*(volatile unsigned long *)0xFFE103C0)
#define LCD_PAL112_OFFSET 0x3C0

#define LCD_PAL113 (*(volatile unsigned long *)0xFFE103C4)
#define LCD_PAL113_OFFSET 0x3C4

#define LCD_PAL114 (*(volatile unsigned long *)0xFFE103C8)
#define LCD_PAL114_OFFSET 0x3C8

#define LCD_PAL115 (*(volatile unsigned long *)0xFFE103CC)
#define LCD_PAL115_OFFSET 0x3CC

#define LCD_PAL116 (*(volatile unsigned long *)0xFFE103D0)
#define LCD_PAL116_OFFSET 0x3D0

#define LCD_PAL117 (*(volatile unsigned long *)0xFFE103D4)
#define LCD_PAL117_OFFSET 0x3D4

#define LCD_PAL118 (*(volatile unsigned long *)0xFFE103D8)
#define LCD_PAL118_OFFSET 0x3D8

#define LCD_PAL119 (*(volatile unsigned long *)0xFFE103DC)
#define LCD_PAL119_OFFSET 0x3DC

#define LCD_PAL120 (*(volatile unsigned long *)0xFFE103E0)
#define LCD_PAL120_OFFSET 0x3E0

#define LCD_PAL121 (*(volatile unsigned long *)0xFFE103E4)
#define LCD_PAL121_OFFSET 0x3E4

#define LCD_PAL122 (*(volatile unsigned long *)0xFFE103E8)
#define LCD_PAL122_OFFSET 0x3E8

#define LCD_PAL123 (*(volatile unsigned long *)0xFFE103EC)
#define LCD_PAL123_OFFSET 0x3EC

#define LCD_PAL124 (*(volatile unsigned long *)0xFFE103F0)
#define LCD_PAL124_OFFSET 0x3F0

#define LCD_PAL125 (*(volatile unsigned long *)0xFFE103F4)
#define LCD_PAL125_OFFSET 0x3F4

#define LCD_PAL126 (*(volatile unsigned long *)0xFFE103F8)
#define LCD_PAL126_OFFSET 0x3F8

#define LCD_PAL127 (*(volatile unsigned long *)0xFFE103FC)
#define LCD_PAL127_OFFSET 0x3FC

#define CRSR_IMG0 (*(volatile unsigned long *)0xFFE10800)
#define CRSR_IMG0_OFFSET 0x800

#define CRSR_IMG1 (*(volatile unsigned long *)0xFFE10804)
#define CRSR_IMG1_OFFSET 0x804

#define CRSR_IMG2 (*(volatile unsigned long *)0xFFE10808)
#define CRSR_IMG2_OFFSET 0x808

#define CRSR_IMG3 (*(volatile unsigned long *)0xFFE1080C)
#define CRSR_IMG3_OFFSET 0x80C

#define CRSR_IMG4 (*(volatile unsigned long *)0xFFE10810)
#define CRSR_IMG4_OFFSET 0x810

#define CRSR_IMG5 (*(volatile unsigned long *)0xFFE10814)
#define CRSR_IMG5_OFFSET 0x814

#define CRSR_IMG6 (*(volatile unsigned long *)0xFFE10818)
#define CRSR_IMG6_OFFSET 0x818

#define CRSR_IMG7 (*(volatile unsigned long *)0xFFE1081C)
#define CRSR_IMG7_OFFSET 0x81C

#define CRSR_IMG8 (*(volatile unsigned long *)0xFFE10820)
#define CRSR_IMG8_OFFSET 0x820

#define CRSR_IMG9 (*(volatile unsigned long *)0xFFE10824)
#define CRSR_IMG9_OFFSET 0x824

#define CRSR_IMG10 (*(volatile unsigned long *)0xFFE10828)
#define CRSR_IMG10_OFFSET 0x828

#define CRSR_IMG11 (*(volatile unsigned long *)0xFFE1082C)
#define CRSR_IMG11_OFFSET 0x82C

#define CRSR_IMG12 (*(volatile unsigned long *)0xFFE10830)
#define CRSR_IMG12_OFFSET 0x830

#define CRSR_IMG13 (*(volatile unsigned long *)0xFFE10834)
#define CRSR_IMG13_OFFSET 0x834

#define CRSR_IMG14 (*(volatile unsigned long *)0xFFE10838)
#define CRSR_IMG14_OFFSET 0x838

#define CRSR_IMG15 (*(volatile unsigned long *)0xFFE1083C)
#define CRSR_IMG15_OFFSET 0x83C

#define CRSR_IMG16 (*(volatile unsigned long *)0xFFE10840)
#define CRSR_IMG16_OFFSET 0x840

#define CRSR_IMG17 (*(volatile unsigned long *)0xFFE10844)
#define CRSR_IMG17_OFFSET 0x844

#define CRSR_IMG18 (*(volatile unsigned long *)0xFFE10848)
#define CRSR_IMG18_OFFSET 0x848

#define CRSR_IMG19 (*(volatile unsigned long *)0xFFE1084C)
#define CRSR_IMG19_OFFSET 0x84C

#define CRSR_IMG20 (*(volatile unsigned long *)0xFFE10850)
#define CRSR_IMG20_OFFSET 0x850

#define CRSR_IMG21 (*(volatile unsigned long *)0xFFE10854)
#define CRSR_IMG21_OFFSET 0x854

#define CRSR_IMG22 (*(volatile unsigned long *)0xFFE10858)
#define CRSR_IMG22_OFFSET 0x858

#define CRSR_IMG23 (*(volatile unsigned long *)0xFFE1085C)
#define CRSR_IMG23_OFFSET 0x85C

#define CRSR_IMG24 (*(volatile unsigned long *)0xFFE10860)
#define CRSR_IMG24_OFFSET 0x860

#define CRSR_IMG25 (*(volatile unsigned long *)0xFFE10864)
#define CRSR_IMG25_OFFSET 0x864

#define CRSR_IMG26 (*(volatile unsigned long *)0xFFE10868)
#define CRSR_IMG26_OFFSET 0x868

#define CRSR_IMG27 (*(volatile unsigned long *)0xFFE1086C)
#define CRSR_IMG27_OFFSET 0x86C

#define CRSR_IMG28 (*(volatile unsigned long *)0xFFE10870)
#define CRSR_IMG28_OFFSET 0x870

#define CRSR_IMG29 (*(volatile unsigned long *)0xFFE10874)
#define CRSR_IMG29_OFFSET 0x874

#define CRSR_IMG30 (*(volatile unsigned long *)0xFFE10878)
#define CRSR_IMG30_OFFSET 0x878

#define CRSR_IMG31 (*(volatile unsigned long *)0xFFE1087C)
#define CRSR_IMG31_OFFSET 0x87C

#define CRSR_IMG32 (*(volatile unsigned long *)0xFFE10880)
#define CRSR_IMG32_OFFSET 0x880

#define CRSR_IMG33 (*(volatile unsigned long *)0xFFE10884)
#define CRSR_IMG33_OFFSET 0x884

#define CRSR_IMG34 (*(volatile unsigned long *)0xFFE10888)
#define CRSR_IMG34_OFFSET 0x888

#define CRSR_IMG35 (*(volatile unsigned long *)0xFFE1088C)
#define CRSR_IMG35_OFFSET 0x88C

#define CRSR_IMG36 (*(volatile unsigned long *)0xFFE10890)
#define CRSR_IMG36_OFFSET 0x890

#define CRSR_IMG37 (*(volatile unsigned long *)0xFFE10894)
#define CRSR_IMG37_OFFSET 0x894

#define CRSR_IMG38 (*(volatile unsigned long *)0xFFE10898)
#define CRSR_IMG38_OFFSET 0x898

#define CRSR_IMG39 (*(volatile unsigned long *)0xFFE1089C)
#define CRSR_IMG39_OFFSET 0x89C

#define CRSR_IMG40 (*(volatile unsigned long *)0xFFE108A0)
#define CRSR_IMG40_OFFSET 0x8A0

#define CRSR_IMG41 (*(volatile unsigned long *)0xFFE108A4)
#define CRSR_IMG41_OFFSET 0x8A4

#define CRSR_IMG42 (*(volatile unsigned long *)0xFFE108A8)
#define CRSR_IMG42_OFFSET 0x8A8

#define CRSR_IMG43 (*(volatile unsigned long *)0xFFE108AC)
#define CRSR_IMG43_OFFSET 0x8AC

#define CRSR_IMG44 (*(volatile unsigned long *)0xFFE108B0)
#define CRSR_IMG44_OFFSET 0x8B0

#define CRSR_IMG45 (*(volatile unsigned long *)0xFFE108B4)
#define CRSR_IMG45_OFFSET 0x8B4

#define CRSR_IMG46 (*(volatile unsigned long *)0xFFE108B8)
#define CRSR_IMG46_OFFSET 0x8B8

#define CRSR_IMG47 (*(volatile unsigned long *)0xFFE108BC)
#define CRSR_IMG47_OFFSET 0x8BC

#define CRSR_IMG48 (*(volatile unsigned long *)0xFFE108C0)
#define CRSR_IMG48_OFFSET 0x8C0

#define CRSR_IMG49 (*(volatile unsigned long *)0xFFE108C4)
#define CRSR_IMG49_OFFSET 0x8C4

#define CRSR_IMG50 (*(volatile unsigned long *)0xFFE108C8)
#define CRSR_IMG50_OFFSET 0x8C8

#define CRSR_IMG51 (*(volatile unsigned long *)0xFFE108CC)
#define CRSR_IMG51_OFFSET 0x8CC

#define CRSR_IMG52 (*(volatile unsigned long *)0xFFE108D0)
#define CRSR_IMG52_OFFSET 0x8D0

#define CRSR_IMG53 (*(volatile unsigned long *)0xFFE108D4)
#define CRSR_IMG53_OFFSET 0x8D4

#define CRSR_IMG54 (*(volatile unsigned long *)0xFFE108D8)
#define CRSR_IMG54_OFFSET 0x8D8

#define CRSR_IMG55 (*(volatile unsigned long *)0xFFE108DC)
#define CRSR_IMG55_OFFSET 0x8DC

#define CRSR_IMG56 (*(volatile unsigned long *)0xFFE108E0)
#define CRSR_IMG56_OFFSET 0x8E0

#define CRSR_IMG57 (*(volatile unsigned long *)0xFFE108E4)
#define CRSR_IMG57_OFFSET 0x8E4

#define CRSR_IMG58 (*(volatile unsigned long *)0xFFE108E8)
#define CRSR_IMG58_OFFSET 0x8E8

#define CRSR_IMG59 (*(volatile unsigned long *)0xFFE108EC)
#define CRSR_IMG59_OFFSET 0x8EC

#define CRSR_IMG60 (*(volatile unsigned long *)0xFFE108F0)
#define CRSR_IMG60_OFFSET 0x8F0

#define CRSR_IMG61 (*(volatile unsigned long *)0xFFE108F4)
#define CRSR_IMG61_OFFSET 0x8F4

#define CRSR_IMG62 (*(volatile unsigned long *)0xFFE108F8)
#define CRSR_IMG62_OFFSET 0x8F8

#define CRSR_IMG63 (*(volatile unsigned long *)0xFFE108FC)
#define CRSR_IMG63_OFFSET 0x8FC

#define CRSR_IMG64 (*(volatile unsigned long *)0xFFE10900)
#define CRSR_IMG64_OFFSET 0x900

#define CRSR_IMG65 (*(volatile unsigned long *)0xFFE10904)
#define CRSR_IMG65_OFFSET 0x904

#define CRSR_IMG66 (*(volatile unsigned long *)0xFFE10908)
#define CRSR_IMG66_OFFSET 0x908

#define CRSR_IMG67 (*(volatile unsigned long *)0xFFE1090C)
#define CRSR_IMG67_OFFSET 0x90C

#define CRSR_IMG68 (*(volatile unsigned long *)0xFFE10910)
#define CRSR_IMG68_OFFSET 0x910

#define CRSR_IMG69 (*(volatile unsigned long *)0xFFE10914)
#define CRSR_IMG69_OFFSET 0x914

#define CRSR_IMG70 (*(volatile unsigned long *)0xFFE10918)
#define CRSR_IMG70_OFFSET 0x918

#define CRSR_IMG71 (*(volatile unsigned long *)0xFFE1091C)
#define CRSR_IMG71_OFFSET 0x91C

#define CRSR_IMG72 (*(volatile unsigned long *)0xFFE10920)
#define CRSR_IMG72_OFFSET 0x920

#define CRSR_IMG73 (*(volatile unsigned long *)0xFFE10924)
#define CRSR_IMG73_OFFSET 0x924

#define CRSR_IMG74 (*(volatile unsigned long *)0xFFE10928)
#define CRSR_IMG74_OFFSET 0x928

#define CRSR_IMG75 (*(volatile unsigned long *)0xFFE1092C)
#define CRSR_IMG75_OFFSET 0x92C

#define CRSR_IMG76 (*(volatile unsigned long *)0xFFE10930)
#define CRSR_IMG76_OFFSET 0x930

#define CRSR_IMG77 (*(volatile unsigned long *)0xFFE10934)
#define CRSR_IMG77_OFFSET 0x934

#define CRSR_IMG78 (*(volatile unsigned long *)0xFFE10938)
#define CRSR_IMG78_OFFSET 0x938

#define CRSR_IMG79 (*(volatile unsigned long *)0xFFE1093C)
#define CRSR_IMG79_OFFSET 0x93C

#define CRSR_IMG80 (*(volatile unsigned long *)0xFFE10940)
#define CRSR_IMG80_OFFSET 0x940

#define CRSR_IMG81 (*(volatile unsigned long *)0xFFE10944)
#define CRSR_IMG81_OFFSET 0x944

#define CRSR_IMG82 (*(volatile unsigned long *)0xFFE10948)
#define CRSR_IMG82_OFFSET 0x948

#define CRSR_IMG83 (*(volatile unsigned long *)0xFFE1094C)
#define CRSR_IMG83_OFFSET 0x94C

#define CRSR_IMG84 (*(volatile unsigned long *)0xFFE10950)
#define CRSR_IMG84_OFFSET 0x950

#define CRSR_IMG85 (*(volatile unsigned long *)0xFFE10954)
#define CRSR_IMG85_OFFSET 0x954

#define CRSR_IMG86 (*(volatile unsigned long *)0xFFE10958)
#define CRSR_IMG86_OFFSET 0x958

#define CRSR_IMG87 (*(volatile unsigned long *)0xFFE1095C)
#define CRSR_IMG87_OFFSET 0x95C

#define CRSR_IMG88 (*(volatile unsigned long *)0xFFE10960)
#define CRSR_IMG88_OFFSET 0x960

#define CRSR_IMG89 (*(volatile unsigned long *)0xFFE10964)
#define CRSR_IMG89_OFFSET 0x964

#define CRSR_IMG90 (*(volatile unsigned long *)0xFFE10968)
#define CRSR_IMG90_OFFSET 0x968

#define CRSR_IMG91 (*(volatile unsigned long *)0xFFE1096C)
#define CRSR_IMG91_OFFSET 0x96C

#define CRSR_IMG92 (*(volatile unsigned long *)0xFFE10970)
#define CRSR_IMG92_OFFSET 0x970

#define CRSR_IMG93 (*(volatile unsigned long *)0xFFE10974)
#define CRSR_IMG93_OFFSET 0x974

#define CRSR_IMG94 (*(volatile unsigned long *)0xFFE10978)
#define CRSR_IMG94_OFFSET 0x978

#define CRSR_IMG95 (*(volatile unsigned long *)0xFFE1097C)
#define CRSR_IMG95_OFFSET 0x97C

#define CRSR_IMG96 (*(volatile unsigned long *)0xFFE10980)
#define CRSR_IMG96_OFFSET 0x980

#define CRSR_IMG97 (*(volatile unsigned long *)0xFFE10984)
#define CRSR_IMG97_OFFSET 0x984

#define CRSR_IMG98 (*(volatile unsigned long *)0xFFE10988)
#define CRSR_IMG98_OFFSET 0x988

#define CRSR_IMG99 (*(volatile unsigned long *)0xFFE1098C)
#define CRSR_IMG99_OFFSET 0x98C

#define CRSR_IMG100 (*(volatile unsigned long *)0xFFE10990)
#define CRSR_IMG100_OFFSET 0x990

#define CRSR_IMG101 (*(volatile unsigned long *)0xFFE10994)
#define CRSR_IMG101_OFFSET 0x994

#define CRSR_IMG102 (*(volatile unsigned long *)0xFFE10998)
#define CRSR_IMG102_OFFSET 0x998

#define CRSR_IMG103 (*(volatile unsigned long *)0xFFE1099C)
#define CRSR_IMG103_OFFSET 0x99C

#define CRSR_IMG104 (*(volatile unsigned long *)0xFFE109A0)
#define CRSR_IMG104_OFFSET 0x9A0

#define CRSR_IMG105 (*(volatile unsigned long *)0xFFE109A4)
#define CRSR_IMG105_OFFSET 0x9A4

#define CRSR_IMG106 (*(volatile unsigned long *)0xFFE109A8)
#define CRSR_IMG106_OFFSET 0x9A8

#define CRSR_IMG107 (*(volatile unsigned long *)0xFFE109AC)
#define CRSR_IMG107_OFFSET 0x9AC

#define CRSR_IMG108 (*(volatile unsigned long *)0xFFE109B0)
#define CRSR_IMG108_OFFSET 0x9B0

#define CRSR_IMG109 (*(volatile unsigned long *)0xFFE109B4)
#define CRSR_IMG109_OFFSET 0x9B4

#define CRSR_IMG110 (*(volatile unsigned long *)0xFFE109B8)
#define CRSR_IMG110_OFFSET 0x9B8

#define CRSR_IMG111 (*(volatile unsigned long *)0xFFE109BC)
#define CRSR_IMG111_OFFSET 0x9BC

#define CRSR_IMG112 (*(volatile unsigned long *)0xFFE109C0)
#define CRSR_IMG112_OFFSET 0x9C0

#define CRSR_IMG113 (*(volatile unsigned long *)0xFFE109C4)
#define CRSR_IMG113_OFFSET 0x9C4

#define CRSR_IMG114 (*(volatile unsigned long *)0xFFE109C8)
#define CRSR_IMG114_OFFSET 0x9C8

#define CRSR_IMG115 (*(volatile unsigned long *)0xFFE109CC)
#define CRSR_IMG115_OFFSET 0x9CC

#define CRSR_IMG116 (*(volatile unsigned long *)0xFFE109D0)
#define CRSR_IMG116_OFFSET 0x9D0

#define CRSR_IMG117 (*(volatile unsigned long *)0xFFE109D4)
#define CRSR_IMG117_OFFSET 0x9D4

#define CRSR_IMG118 (*(volatile unsigned long *)0xFFE109D8)
#define CRSR_IMG118_OFFSET 0x9D8

#define CRSR_IMG119 (*(volatile unsigned long *)0xFFE109DC)
#define CRSR_IMG119_OFFSET 0x9DC

#define CRSR_IMG120 (*(volatile unsigned long *)0xFFE109E0)
#define CRSR_IMG120_OFFSET 0x9E0

#define CRSR_IMG121 (*(volatile unsigned long *)0xFFE109E4)
#define CRSR_IMG121_OFFSET 0x9E4

#define CRSR_IMG122 (*(volatile unsigned long *)0xFFE109E8)
#define CRSR_IMG122_OFFSET 0x9E8

#define CRSR_IMG123 (*(volatile unsigned long *)0xFFE109EC)
#define CRSR_IMG123_OFFSET 0x9EC

#define CRSR_IMG124 (*(volatile unsigned long *)0xFFE109F0)
#define CRSR_IMG124_OFFSET 0x9F0

#define CRSR_IMG125 (*(volatile unsigned long *)0xFFE109F4)
#define CRSR_IMG125_OFFSET 0x9F4

#define CRSR_IMG126 (*(volatile unsigned long *)0xFFE109F8)
#define CRSR_IMG126_OFFSET 0x9F8

#define CRSR_IMG127 (*(volatile unsigned long *)0xFFE109FC)
#define CRSR_IMG127_OFFSET 0x9FC

#define CRSR_IMG128 (*(volatile unsigned long *)0xFFE10A00)
#define CRSR_IMG128_OFFSET 0xA00

#define CRSR_IMG129 (*(volatile unsigned long *)0xFFE10A04)
#define CRSR_IMG129_OFFSET 0xA04

#define CRSR_IMG130 (*(volatile unsigned long *)0xFFE10A08)
#define CRSR_IMG130_OFFSET 0xA08

#define CRSR_IMG131 (*(volatile unsigned long *)0xFFE10A0C)
#define CRSR_IMG131_OFFSET 0xA0C

#define CRSR_IMG132 (*(volatile unsigned long *)0xFFE10A10)
#define CRSR_IMG132_OFFSET 0xA10

#define CRSR_IMG133 (*(volatile unsigned long *)0xFFE10A14)
#define CRSR_IMG133_OFFSET 0xA14

#define CRSR_IMG134 (*(volatile unsigned long *)0xFFE10A18)
#define CRSR_IMG134_OFFSET 0xA18

#define CRSR_IMG135 (*(volatile unsigned long *)0xFFE10A1C)
#define CRSR_IMG135_OFFSET 0xA1C

#define CRSR_IMG136 (*(volatile unsigned long *)0xFFE10A20)
#define CRSR_IMG136_OFFSET 0xA20

#define CRSR_IMG137 (*(volatile unsigned long *)0xFFE10A24)
#define CRSR_IMG137_OFFSET 0xA24

#define CRSR_IMG138 (*(volatile unsigned long *)0xFFE10A28)
#define CRSR_IMG138_OFFSET 0xA28

#define CRSR_IMG139 (*(volatile unsigned long *)0xFFE10A2C)
#define CRSR_IMG139_OFFSET 0xA2C

#define CRSR_IMG140 (*(volatile unsigned long *)0xFFE10A30)
#define CRSR_IMG140_OFFSET 0xA30

#define CRSR_IMG141 (*(volatile unsigned long *)0xFFE10A34)
#define CRSR_IMG141_OFFSET 0xA34

#define CRSR_IMG142 (*(volatile unsigned long *)0xFFE10A38)
#define CRSR_IMG142_OFFSET 0xA38

#define CRSR_IMG143 (*(volatile unsigned long *)0xFFE10A3C)
#define CRSR_IMG143_OFFSET 0xA3C

#define CRSR_IMG144 (*(volatile unsigned long *)0xFFE10A40)
#define CRSR_IMG144_OFFSET 0xA40

#define CRSR_IMG145 (*(volatile unsigned long *)0xFFE10A44)
#define CRSR_IMG145_OFFSET 0xA44

#define CRSR_IMG146 (*(volatile unsigned long *)0xFFE10A48)
#define CRSR_IMG146_OFFSET 0xA48

#define CRSR_IMG147 (*(volatile unsigned long *)0xFFE10A4C)
#define CRSR_IMG147_OFFSET 0xA4C

#define CRSR_IMG148 (*(volatile unsigned long *)0xFFE10A50)
#define CRSR_IMG148_OFFSET 0xA50

#define CRSR_IMG149 (*(volatile unsigned long *)0xFFE10A54)
#define CRSR_IMG149_OFFSET 0xA54

#define CRSR_IMG150 (*(volatile unsigned long *)0xFFE10A58)
#define CRSR_IMG150_OFFSET 0xA58

#define CRSR_IMG151 (*(volatile unsigned long *)0xFFE10A5C)
#define CRSR_IMG151_OFFSET 0xA5C

#define CRSR_IMG152 (*(volatile unsigned long *)0xFFE10A60)
#define CRSR_IMG152_OFFSET 0xA60

#define CRSR_IMG153 (*(volatile unsigned long *)0xFFE10A64)
#define CRSR_IMG153_OFFSET 0xA64

#define CRSR_IMG154 (*(volatile unsigned long *)0xFFE10A68)
#define CRSR_IMG154_OFFSET 0xA68

#define CRSR_IMG155 (*(volatile unsigned long *)0xFFE10A6C)
#define CRSR_IMG155_OFFSET 0xA6C

#define CRSR_IMG156 (*(volatile unsigned long *)0xFFE10A70)
#define CRSR_IMG156_OFFSET 0xA70

#define CRSR_IMG157 (*(volatile unsigned long *)0xFFE10A74)
#define CRSR_IMG157_OFFSET 0xA74

#define CRSR_IMG158 (*(volatile unsigned long *)0xFFE10A78)
#define CRSR_IMG158_OFFSET 0xA78

#define CRSR_IMG159 (*(volatile unsigned long *)0xFFE10A7C)
#define CRSR_IMG159_OFFSET 0xA7C

#define CRSR_IMG160 (*(volatile unsigned long *)0xFFE10A80)
#define CRSR_IMG160_OFFSET 0xA80

#define CRSR_IMG161 (*(volatile unsigned long *)0xFFE10A84)
#define CRSR_IMG161_OFFSET 0xA84

#define CRSR_IMG162 (*(volatile unsigned long *)0xFFE10A88)
#define CRSR_IMG162_OFFSET 0xA88

#define CRSR_IMG163 (*(volatile unsigned long *)0xFFE10A8C)
#define CRSR_IMG163_OFFSET 0xA8C

#define CRSR_IMG164 (*(volatile unsigned long *)0xFFE10A90)
#define CRSR_IMG164_OFFSET 0xA90

#define CRSR_IMG165 (*(volatile unsigned long *)0xFFE10A94)
#define CRSR_IMG165_OFFSET 0xA94

#define CRSR_IMG166 (*(volatile unsigned long *)0xFFE10A98)
#define CRSR_IMG166_OFFSET 0xA98

#define CRSR_IMG167 (*(volatile unsigned long *)0xFFE10A9C)
#define CRSR_IMG167_OFFSET 0xA9C

#define CRSR_IMG168 (*(volatile unsigned long *)0xFFE10AA0)
#define CRSR_IMG168_OFFSET 0xAA0

#define CRSR_IMG169 (*(volatile unsigned long *)0xFFE10AA4)
#define CRSR_IMG169_OFFSET 0xAA4

#define CRSR_IMG170 (*(volatile unsigned long *)0xFFE10AA8)
#define CRSR_IMG170_OFFSET 0xAA8

#define CRSR_IMG171 (*(volatile unsigned long *)0xFFE10AAC)
#define CRSR_IMG171_OFFSET 0xAAC

#define CRSR_IMG172 (*(volatile unsigned long *)0xFFE10AB0)
#define CRSR_IMG172_OFFSET 0xAB0

#define CRSR_IMG173 (*(volatile unsigned long *)0xFFE10AB4)
#define CRSR_IMG173_OFFSET 0xAB4

#define CRSR_IMG174 (*(volatile unsigned long *)0xFFE10AB8)
#define CRSR_IMG174_OFFSET 0xAB8

#define CRSR_IMG175 (*(volatile unsigned long *)0xFFE10ABC)
#define CRSR_IMG175_OFFSET 0xABC

#define CRSR_IMG176 (*(volatile unsigned long *)0xFFE10AC0)
#define CRSR_IMG176_OFFSET 0xAC0

#define CRSR_IMG177 (*(volatile unsigned long *)0xFFE10AC4)
#define CRSR_IMG177_OFFSET 0xAC4

#define CRSR_IMG178 (*(volatile unsigned long *)0xFFE10AC8)
#define CRSR_IMG178_OFFSET 0xAC8

#define CRSR_IMG179 (*(volatile unsigned long *)0xFFE10ACC)
#define CRSR_IMG179_OFFSET 0xACC

#define CRSR_IMG180 (*(volatile unsigned long *)0xFFE10AD0)
#define CRSR_IMG180_OFFSET 0xAD0

#define CRSR_IMG181 (*(volatile unsigned long *)0xFFE10AD4)
#define CRSR_IMG181_OFFSET 0xAD4

#define CRSR_IMG182 (*(volatile unsigned long *)0xFFE10AD8)
#define CRSR_IMG182_OFFSET 0xAD8

#define CRSR_IMG183 (*(volatile unsigned long *)0xFFE10ADC)
#define CRSR_IMG183_OFFSET 0xADC

#define CRSR_IMG184 (*(volatile unsigned long *)0xFFE10AE0)
#define CRSR_IMG184_OFFSET 0xAE0

#define CRSR_IMG185 (*(volatile unsigned long *)0xFFE10AE4)
#define CRSR_IMG185_OFFSET 0xAE4

#define CRSR_IMG186 (*(volatile unsigned long *)0xFFE10AE8)
#define CRSR_IMG186_OFFSET 0xAE8

#define CRSR_IMG187 (*(volatile unsigned long *)0xFFE10AEC)
#define CRSR_IMG187_OFFSET 0xAEC

#define CRSR_IMG188 (*(volatile unsigned long *)0xFFE10AF0)
#define CRSR_IMG188_OFFSET 0xAF0

#define CRSR_IMG189 (*(volatile unsigned long *)0xFFE10AF4)
#define CRSR_IMG189_OFFSET 0xAF4

#define CRSR_IMG190 (*(volatile unsigned long *)0xFFE10AF8)
#define CRSR_IMG190_OFFSET 0xAF8

#define CRSR_IMG191 (*(volatile unsigned long *)0xFFE10AFC)
#define CRSR_IMG191_OFFSET 0xAFC

#define CRSR_IMG192 (*(volatile unsigned long *)0xFFE10B00)
#define CRSR_IMG192_OFFSET 0xB00

#define CRSR_IMG193 (*(volatile unsigned long *)0xFFE10B04)
#define CRSR_IMG193_OFFSET 0xB04

#define CRSR_IMG194 (*(volatile unsigned long *)0xFFE10B08)
#define CRSR_IMG194_OFFSET 0xB08

#define CRSR_IMG195 (*(volatile unsigned long *)0xFFE10B0C)
#define CRSR_IMG195_OFFSET 0xB0C

#define CRSR_IMG196 (*(volatile unsigned long *)0xFFE10B10)
#define CRSR_IMG196_OFFSET 0xB10

#define CRSR_IMG197 (*(volatile unsigned long *)0xFFE10B14)
#define CRSR_IMG197_OFFSET 0xB14

#define CRSR_IMG198 (*(volatile unsigned long *)0xFFE10B18)
#define CRSR_IMG198_OFFSET 0xB18

#define CRSR_IMG199 (*(volatile unsigned long *)0xFFE10B1C)
#define CRSR_IMG199_OFFSET 0xB1C

#define CRSR_IMG200 (*(volatile unsigned long *)0xFFE10B20)
#define CRSR_IMG200_OFFSET 0xB20

#define CRSR_IMG201 (*(volatile unsigned long *)0xFFE10B24)
#define CRSR_IMG201_OFFSET 0xB24

#define CRSR_IMG202 (*(volatile unsigned long *)0xFFE10B28)
#define CRSR_IMG202_OFFSET 0xB28

#define CRSR_IMG203 (*(volatile unsigned long *)0xFFE10B2C)
#define CRSR_IMG203_OFFSET 0xB2C

#define CRSR_IMG204 (*(volatile unsigned long *)0xFFE10B30)
#define CRSR_IMG204_OFFSET 0xB30

#define CRSR_IMG205 (*(volatile unsigned long *)0xFFE10B34)
#define CRSR_IMG205_OFFSET 0xB34

#define CRSR_IMG206 (*(volatile unsigned long *)0xFFE10B38)
#define CRSR_IMG206_OFFSET 0xB38

#define CRSR_IMG207 (*(volatile unsigned long *)0xFFE10B3C)
#define CRSR_IMG207_OFFSET 0xB3C

#define CRSR_IMG208 (*(volatile unsigned long *)0xFFE10B40)
#define CRSR_IMG208_OFFSET 0xB40

#define CRSR_IMG209 (*(volatile unsigned long *)0xFFE10B44)
#define CRSR_IMG209_OFFSET 0xB44

#define CRSR_IMG210 (*(volatile unsigned long *)0xFFE10B48)
#define CRSR_IMG210_OFFSET 0xB48

#define CRSR_IMG211 (*(volatile unsigned long *)0xFFE10B4C)
#define CRSR_IMG211_OFFSET 0xB4C

#define CRSR_IMG212 (*(volatile unsigned long *)0xFFE10B50)
#define CRSR_IMG212_OFFSET 0xB50

#define CRSR_IMG213 (*(volatile unsigned long *)0xFFE10B54)
#define CRSR_IMG213_OFFSET 0xB54

#define CRSR_IMG214 (*(volatile unsigned long *)0xFFE10B58)
#define CRSR_IMG214_OFFSET 0xB58

#define CRSR_IMG215 (*(volatile unsigned long *)0xFFE10B5C)
#define CRSR_IMG215_OFFSET 0xB5C

#define CRSR_IMG216 (*(volatile unsigned long *)0xFFE10B60)
#define CRSR_IMG216_OFFSET 0xB60

#define CRSR_IMG217 (*(volatile unsigned long *)0xFFE10B64)
#define CRSR_IMG217_OFFSET 0xB64

#define CRSR_IMG218 (*(volatile unsigned long *)0xFFE10B68)
#define CRSR_IMG218_OFFSET 0xB68

#define CRSR_IMG219 (*(volatile unsigned long *)0xFFE10B6C)
#define CRSR_IMG219_OFFSET 0xB6C

#define CRSR_IMG220 (*(volatile unsigned long *)0xFFE10B70)
#define CRSR_IMG220_OFFSET 0xB70

#define CRSR_IMG221 (*(volatile unsigned long *)0xFFE10B74)
#define CRSR_IMG221_OFFSET 0xB74

#define CRSR_IMG222 (*(volatile unsigned long *)0xFFE10B78)
#define CRSR_IMG222_OFFSET 0xB78

#define CRSR_IMG223 (*(volatile unsigned long *)0xFFE10B7C)
#define CRSR_IMG223_OFFSET 0xB7C

#define CRSR_IMG224 (*(volatile unsigned long *)0xFFE10B80)
#define CRSR_IMG224_OFFSET 0xB80

#define CRSR_IMG225 (*(volatile unsigned long *)0xFFE10B84)
#define CRSR_IMG225_OFFSET 0xB84

#define CRSR_IMG226 (*(volatile unsigned long *)0xFFE10B88)
#define CRSR_IMG226_OFFSET 0xB88

#define CRSR_IMG227 (*(volatile unsigned long *)0xFFE10B8C)
#define CRSR_IMG227_OFFSET 0xB8C

#define CRSR_IMG228 (*(volatile unsigned long *)0xFFE10B90)
#define CRSR_IMG228_OFFSET 0xB90

#define CRSR_IMG229 (*(volatile unsigned long *)0xFFE10B94)
#define CRSR_IMG229_OFFSET 0xB94

#define CRSR_IMG230 (*(volatile unsigned long *)0xFFE10B98)
#define CRSR_IMG230_OFFSET 0xB98

#define CRSR_IMG231 (*(volatile unsigned long *)0xFFE10B9C)
#define CRSR_IMG231_OFFSET 0xB9C

#define CRSR_IMG232 (*(volatile unsigned long *)0xFFE10BA0)
#define CRSR_IMG232_OFFSET 0xBA0

#define CRSR_IMG233 (*(volatile unsigned long *)0xFFE10BA4)
#define CRSR_IMG233_OFFSET 0xBA4

#define CRSR_IMG234 (*(volatile unsigned long *)0xFFE10BA8)
#define CRSR_IMG234_OFFSET 0xBA8

#define CRSR_IMG235 (*(volatile unsigned long *)0xFFE10BAC)
#define CRSR_IMG235_OFFSET 0xBAC

#define CRSR_IMG236 (*(volatile unsigned long *)0xFFE10BB0)
#define CRSR_IMG236_OFFSET 0xBB0

#define CRSR_IMG237 (*(volatile unsigned long *)0xFFE10BB4)
#define CRSR_IMG237_OFFSET 0xBB4

#define CRSR_IMG238 (*(volatile unsigned long *)0xFFE10BB8)
#define CRSR_IMG238_OFFSET 0xBB8

#define CRSR_IMG239 (*(volatile unsigned long *)0xFFE10BBC)
#define CRSR_IMG239_OFFSET 0xBBC

#define CRSR_IMG240 (*(volatile unsigned long *)0xFFE10BC0)
#define CRSR_IMG240_OFFSET 0xBC0

#define CRSR_IMG241 (*(volatile unsigned long *)0xFFE10BC4)
#define CRSR_IMG241_OFFSET 0xBC4

#define CRSR_IMG242 (*(volatile unsigned long *)0xFFE10BC8)
#define CRSR_IMG242_OFFSET 0xBC8

#define CRSR_IMG243 (*(volatile unsigned long *)0xFFE10BCC)
#define CRSR_IMG243_OFFSET 0xBCC

#define CRSR_IMG244 (*(volatile unsigned long *)0xFFE10BD0)
#define CRSR_IMG244_OFFSET 0xBD0

#define CRSR_IMG245 (*(volatile unsigned long *)0xFFE10BD4)
#define CRSR_IMG245_OFFSET 0xBD4

#define CRSR_IMG246 (*(volatile unsigned long *)0xFFE10BD8)
#define CRSR_IMG246_OFFSET 0xBD8

#define CRSR_IMG247 (*(volatile unsigned long *)0xFFE10BDC)
#define CRSR_IMG247_OFFSET 0xBDC

#define CRSR_IMG248 (*(volatile unsigned long *)0xFFE10BE0)
#define CRSR_IMG248_OFFSET 0xBE0

#define CRSR_IMG249 (*(volatile unsigned long *)0xFFE10BE4)
#define CRSR_IMG249_OFFSET 0xBE4

#define CRSR_IMG250 (*(volatile unsigned long *)0xFFE10BE8)
#define CRSR_IMG250_OFFSET 0xBE8

#define CRSR_IMG251 (*(volatile unsigned long *)0xFFE10BEC)
#define CRSR_IMG251_OFFSET 0xBEC

#define CRSR_IMG252 (*(volatile unsigned long *)0xFFE10BF0)
#define CRSR_IMG252_OFFSET 0xBF0

#define CRSR_IMG253 (*(volatile unsigned long *)0xFFE10BF4)
#define CRSR_IMG253_OFFSET 0xBF4

#define CRSR_IMG254 (*(volatile unsigned long *)0xFFE10BF8)
#define CRSR_IMG254_OFFSET 0xBF8

#define CRSR_IMG255 (*(volatile unsigned long *)0xFFE10BFC)
#define CRSR_IMG255_OFFSET 0xBFC

#define CRSR_CTRL (*(volatile unsigned *)0xFFE10C00)
#define CRSR_CTRL_OFFSET 0xC00
#define CRSR_CTRL_CrsrOn_MASK 0x1
#define CRSR_CTRL_CrsrOn 0x1
#define CRSR_CTRL_CrsrOn_BIT 0
#define CRSR_CTRL_CrsrNum_MASK 0x30
#define CRSR_CTRL_CrsrNum_BIT 4

#define CRSR_CFG (*(volatile unsigned *)0xFFE10C04)
#define CRSR_CFG_OFFSET 0xC04
#define CRSR_CFG_CrsrSize_MASK 0x1
#define CRSR_CFG_CrsrSize 0x1
#define CRSR_CFG_CrsrSize_BIT 0
#define CRSR_CFG_FrameSync_MASK 0x2
#define CRSR_CFG_FrameSync 0x2
#define CRSR_CFG_FrameSync_BIT 1

#define CRSR_PAL0 (*(volatile unsigned *)0xFFE10C08)
#define CRSR_PAL0_OFFSET 0xC08
#define CRSR_PAL0_Red_MASK 0xFF
#define CRSR_PAL0_Red_BIT 0
#define CRSR_PAL0_Green_MASK 0xFF00
#define CRSR_PAL0_Green_BIT 8
#define CRSR_PAL0_Blue_MASK 0xFF0000
#define CRSR_PAL0_Blue_BIT 16

#define CRSR_PAL1 (*(volatile unsigned *)0xFFE10C0C)
#define CRSR_PAL1_OFFSET 0xC0C
#define CRSR_PAL1_Red_MASK 0xFF
#define CRSR_PAL1_Red_BIT 0
#define CRSR_PAL1_Green_MASK 0xFF00
#define CRSR_PAL1_Green_BIT 8
#define CRSR_PAL1_Blue_MASK 0xFF0000
#define CRSR_PAL1_Blue_BIT 16

#define CRSR_XY (*(volatile unsigned *)0xFFE10C10)
#define CRSR_XY_OFFSET 0xC10
#define CRSR_XY_CrsrX_MASK 0x3FF
#define CRSR_XY_CrsrX_BIT 0
#define CRSR_XY_CrsrY_MASK 0x3FF0000
#define CRSR_XY_CrsrY_BIT 16

#define CRSR_CLIP (*(volatile unsigned *)0xFFE10C14)
#define CRSR_CLIP_OFFSET 0xC14
#define CRSR_CLIP_CrsrClipX_MASK 0x3F
#define CRSR_CLIP_CrsrClipX_BIT 0
#define CRSR_CLIP_CrsrClipY_MASK 0x3F00
#define CRSR_CLIP_CrsrClipY_BIT 8

#define CRSR_INTMSK (*(volatile unsigned *)0xFFE10C20)
#define CRSR_INTMSK_OFFSET 0xC20
#define CRSR_INTMSK_CrsrIM_MASK 0x1
#define CRSR_INTMSK_CrsrIM 0x1
#define CRSR_INTMSK_CrsrIM_BIT 0

#define CRSR_INTCLR (*(volatile unsigned *)0xFFE10C24)
#define CRSR_INTCLR_OFFSET 0xC24
#define CRSR_INTCLR_CrsrIC_MASK 0x1
#define CRSR_INTCLR_CrsrIC 0x1
#define CRSR_INTCLR_CrsrIC_BIT 0

#define CRSR_INTRAW (*(volatile unsigned *)0xFFE10C28)
#define CRSR_INTRAW_OFFSET 0xC28
#define CRSR_INTRAW_CrsrRIS_MASK 0x1
#define CRSR_INTRAW_CrsrRIS 0x1
#define CRSR_INTRAW_CrsrRIS_BIT 0

#define CRSR_INTSTAT (*(volatile unsigned *)0xFFE10C2C)
#define CRSR_INTSTAT_OFFSET 0xC2C
#define CRSR_INTSTAT_CrsrMIS_MASK 0x1
#define CRSR_INTSTAT_CrsrMIS 0x1
#define CRSR_INTSTAT_CrsrMIS_BIT 0

#define VIC_BASE 0xFFFFF000

#define VICIRQStatus (*(volatile unsigned long *)0xFFFFF000)
#define VICIRQStatus_OFFSET 0x0

#define VICFIQStatus (*(volatile unsigned long *)0xFFFFF004)
#define VICFIQStatus_OFFSET 0x4

#define VICRawIntr (*(volatile unsigned long *)0xFFFFF008)
#define VICRawIntr_OFFSET 0x8

#define VICIntSelect (*(volatile unsigned long *)0xFFFFF00C)
#define VICIntSelect_OFFSET 0xC

#define VICIntEnable (*(volatile unsigned long *)0xFFFFF010)
#define VICIntEnable_OFFSET 0x10

#define VICIntEnClr (*(volatile unsigned long *)0xFFFFF014)
#define VICIntEnClr_OFFSET 0x14

#define VICSoftInt (*(volatile unsigned long *)0xFFFFF018)
#define VICSoftInt_OFFSET 0x18

#define VICSoftIntClear (*(volatile unsigned long *)0xFFFFF01C)
#define VICSoftIntClear_OFFSET 0x1C

#define VICProtection (*(volatile unsigned long *)0xFFFFF020)
#define VICProtection_OFFSET 0x20

#define VICSWPriorityMask (*(volatile unsigned long *)0xFFFFF024)
#define VICSWPriorityMask_OFFSET 0x24

#define VICVectAddr0 (*(volatile unsigned long *)0xFFFFF100)
#define VICVectAddr0_OFFSET 0x100

#define VICVectAddr1 (*(volatile unsigned long *)0xFFFFF104)
#define VICVectAddr1_OFFSET 0x104

#define VICVectAddr2 (*(volatile unsigned long *)0xFFFFF108)
#define VICVectAddr2_OFFSET 0x108

#define VICVectAddr3 (*(volatile unsigned long *)0xFFFFF10C)
#define VICVectAddr3_OFFSET 0x10C

#define VICVectAddr4 (*(volatile unsigned long *)0xFFFFF110)
#define VICVectAddr4_OFFSET 0x110

#define VICVectAddr5 (*(volatile unsigned long *)0xFFFFF114)
#define VICVectAddr5_OFFSET 0x114

#define VICVectAddr6 (*(volatile unsigned long *)0xFFFFF118)
#define VICVectAddr6_OFFSET 0x118

#define VICVectAddr7 (*(volatile unsigned long *)0xFFFFF11C)
#define VICVectAddr7_OFFSET 0x11C

#define VICVectAddr8 (*(volatile unsigned long *)0xFFFFF120)
#define VICVectAddr8_OFFSET 0x120

#define VICVectAddr9 (*(volatile unsigned long *)0xFFFFF124)
#define VICVectAddr9_OFFSET 0x124

#define VICVectAddr10 (*(volatile unsigned long *)0xFFFFF128)
#define VICVectAddr10_OFFSET 0x128

#define VICVectAddr11 (*(volatile unsigned long *)0xFFFFF12C)
#define VICVectAddr11_OFFSET 0x12C

#define VICVectAddr12 (*(volatile unsigned long *)0xFFFFF130)
#define VICVectAddr12_OFFSET 0x130

#define VICVectAddr13 (*(volatile unsigned long *)0xFFFFF134)
#define VICVectAddr13_OFFSET 0x134

#define VICVectAddr14 (*(volatile unsigned long *)0xFFFFF138)
#define VICVectAddr14_OFFSET 0x138

#define VICVectAddr15 (*(volatile unsigned long *)0xFFFFF13C)
#define VICVectAddr15_OFFSET 0x13C

#define VICVectAddr16 (*(volatile unsigned long *)0xFFFFF140)
#define VICVectAddr16_OFFSET 0x140

#define VICVectAddr17 (*(volatile unsigned long *)0xFFFFF144)
#define VICVectAddr17_OFFSET 0x144

#define VICVectAddr18 (*(volatile unsigned long *)0xFFFFF148)
#define VICVectAddr18_OFFSET 0x148

#define VICVectAddr19 (*(volatile unsigned long *)0xFFFFF14C)
#define VICVectAddr19_OFFSET 0x14C

#define VICVectAddr20 (*(volatile unsigned long *)0xFFFFF150)
#define VICVectAddr20_OFFSET 0x150

#define VICVectAddr21 (*(volatile unsigned long *)0xFFFFF154)
#define VICVectAddr21_OFFSET 0x154

#define VICVectAddr22 (*(volatile unsigned long *)0xFFFFF158)
#define VICVectAddr22_OFFSET 0x158

#define VICVectAddr23 (*(volatile unsigned long *)0xFFFFF15C)
#define VICVectAddr23_OFFSET 0x15C

#define VICVectAddr24 (*(volatile unsigned long *)0xFFFFF160)
#define VICVectAddr24_OFFSET 0x160

#define VICVectAddr25 (*(volatile unsigned long *)0xFFFFF164)
#define VICVectAddr25_OFFSET 0x164

#define VICVectAddr26 (*(volatile unsigned long *)0xFFFFF168)
#define VICVectAddr26_OFFSET 0x168

#define VICVectAddr27 (*(volatile unsigned long *)0xFFFFF16C)
#define VICVectAddr27_OFFSET 0x16C

#define VICVectAddr28 (*(volatile unsigned long *)0xFFFFF170)
#define VICVectAddr28_OFFSET 0x170

#define VICVectAddr29 (*(volatile unsigned long *)0xFFFFF174)
#define VICVectAddr29_OFFSET 0x174

#define VICVectAddr30 (*(volatile unsigned long *)0xFFFFF178)
#define VICVectAddr30_OFFSET 0x178

#define VICVectAddr31 (*(volatile unsigned long *)0xFFFFF17C)
#define VICVectAddr31_OFFSET 0x17C

#define VICVectPriority0 (*(volatile unsigned long *)0xFFFFF200)
#define VICVectPriority0_OFFSET 0x200

#define VICVectPriority1 (*(volatile unsigned long *)0xFFFFF204)
#define VICVectPriority1_OFFSET 0x204

#define VICVectPriority2 (*(volatile unsigned long *)0xFFFFF208)
#define VICVectPriority2_OFFSET 0x208

#define VICVectPriority3 (*(volatile unsigned long *)0xFFFFF20C)
#define VICVectPriority3_OFFSET 0x20C

#define VICVectPriority4 (*(volatile unsigned long *)0xFFFFF210)
#define VICVectPriority4_OFFSET 0x210

#define VICVectPriority5 (*(volatile unsigned long *)0xFFFFF214)
#define VICVectPriority5_OFFSET 0x214

#define VICVectPriority6 (*(volatile unsigned long *)0xFFFFF218)
#define VICVectPriority6_OFFSET 0x218

#define VICVectPriority7 (*(volatile unsigned long *)0xFFFFF21C)
#define VICVectPriority7_OFFSET 0x21C

#define VICVectPriority8 (*(volatile unsigned long *)0xFFFFF220)
#define VICVectPriority8_OFFSET 0x220

#define VICVectPriority9 (*(volatile unsigned long *)0xFFFFF224)
#define VICVectPriority9_OFFSET 0x224

#define VICVectPriority10 (*(volatile unsigned long *)0xFFFFF228)
#define VICVectPriority10_OFFSET 0x228

#define VICVectPriority11 (*(volatile unsigned long *)0xFFFFF22C)
#define VICVectPriority11_OFFSET 0x22C

#define VICVectPriority12 (*(volatile unsigned long *)0xFFFFF230)
#define VICVectPriority12_OFFSET 0x230

#define VICVectPriority13 (*(volatile unsigned long *)0xFFFFF234)
#define VICVectPriority13_OFFSET 0x234

#define VICVectPriority14 (*(volatile unsigned long *)0xFFFFF238)
#define VICVectPriority14_OFFSET 0x238

#define VICVectPriority15 (*(volatile unsigned long *)0xFFFFF23C)
#define VICVectPriority15_OFFSET 0x23C

#define VICVectPriority16 (*(volatile unsigned long *)0xFFFFF240)
#define VICVectPriority16_OFFSET 0x240

#define VICVectPriority17 (*(volatile unsigned long *)0xFFFFF244)
#define VICVectPriority17_OFFSET 0x244

#define VICVectPriority18 (*(volatile unsigned long *)0xFFFFF248)
#define VICVectPriority18_OFFSET 0x248

#define VICVectPriority19 (*(volatile unsigned long *)0xFFFFF24C)
#define VICVectPriority19_OFFSET 0x24C

#define VICVectPriority20 (*(volatile unsigned long *)0xFFFFF250)
#define VICVectPriority20_OFFSET 0x250

#define VICVectPriority21 (*(volatile unsigned long *)0xFFFFF254)
#define VICVectPriority21_OFFSET 0x254

#define VICVectPriority22 (*(volatile unsigned long *)0xFFFFF258)
#define VICVectPriority22_OFFSET 0x258

#define VICVectPriority23 (*(volatile unsigned long *)0xFFFFF25C)
#define VICVectPriority23_OFFSET 0x25C

#define VICVectPriority24 (*(volatile unsigned long *)0xFFFFF260)
#define VICVectPriority24_OFFSET 0x260

#define VICVectPriority25 (*(volatile unsigned long *)0xFFFFF264)
#define VICVectPriority25_OFFSET 0x264

#define VICVectPriority26 (*(volatile unsigned long *)0xFFFFF268)
#define VICVectPriority26_OFFSET 0x268

#define VICVectPriority27 (*(volatile unsigned long *)0xFFFFF26C)
#define VICVectPriority27_OFFSET 0x26C

#define VICVectPriority28 (*(volatile unsigned long *)0xFFFFF270)
#define VICVectPriority28_OFFSET 0x270

#define VICVectPriority29 (*(volatile unsigned long *)0xFFFFF274)
#define VICVectPriority29_OFFSET 0x274

#define VICVectPriority30 (*(volatile unsigned long *)0xFFFFF278)
#define VICVectPriority30_OFFSET 0x278

#define VICVectPriority31 (*(volatile unsigned long *)0xFFFFF27C)
#define VICVectPriority31_OFFSET 0x27C

#define VICAddress (*(volatile unsigned long *)0xFFFFFF00)
#define VICAddress_OFFSET 0xF00

/* On LPC23xx, the names are USBClkCtrl and USBClkSt; on LPC24xx, the names are
OTG_CLK_CTRL and OTG_CLK_STAT respectively. */
#define USBOTG_CLK_BASE_ADDR	0xFFE0CFF0
#define OTG_CLK_CTRL        (*(volatile unsigned long *)(USBOTG_CLK_BASE_ADDR + 0x04))
#define OTG_CLK_STAT        (*(volatile unsigned long *)(USBOTG_CLK_BASE_ADDR + 0x08))
/* USB OTG Controller */
#define USBOTG_BASE_ADDR	0xFFE0C100
#define OTG_INT_STAT        (*(volatile unsigned long *)(USBOTG_BASE_ADDR + 0x00))
#define OTG_INT_EN          (*(volatile unsigned long *)(USBOTG_BASE_ADDR + 0x04))
#define OTG_INT_SET         (*(volatile unsigned long *)(USBOTG_BASE_ADDR + 0x08))
#define OTG_INT_CLR         (*(volatile unsigned long *)(USBOTG_BASE_ADDR + 0x0C))
/* On LPC23xx, the name is USBPortSel, on LPC24xx, the name is OTG_STAT_CTRL */
#define OTG_STAT_CTRL       (*(volatile unsigned long *)(USBOTG_BASE_ADDR + 0x10))
#define OTG_TIMER           (*(volatile unsigned long *)(USBOTG_BASE_ADDR + 0x14))

#define INTERRUPT_CHANNEL_WDT           0
#define INTERRUPT_CHANNEL_SWI           1
#define INTERRUPT_CHANNEL_DBG_COMM_RX   2
#define INTERRUPT_CHANNEL_DBG_COMM_TX   3
#define INTERRUPT_CHANNEL_TIMER0        4
#define INTERRUPT_CHANNEL_TIMER1        5
#define INTERRUPT_CHANNEL_UART0         6
#define INTERRUPT_CHANNEL_UART1         7
#define INTERRUPT_CHANNEL_PWM0          8
#define INTERRUPT_CHANNEL_PWM1          8
#define INTERRUPT_CHANNEL_I2C0          9
#define INTERRUPT_CHANNEL_SPI           10
#define INTERRUPT_CHANNEL_SSP0          10
#define INTERRUPT_CHANNEL_SSP1          11
#define INTERRUPT_CHANNEL_PLL           12
#define INTERRUPT_CHANNEL_RTC           13
#define INTERRUPT_CHANNEL_EINT0         14
#define INTERRUPT_CHANNEL_EINT1         15
#define INTERRUPT_CHANNEL_EINT2         16
#define INTERRUPT_CHANNEL_LCD           16
#define INTERRUPT_CHANNEL_EINT3         17
#define INTERRUPT_CHANNEL_ADC0          18
#define INTERRUPT_CHANNEL_I2C1          19
#define INTERRUPT_CHANNEL_BOD           20
#define INTERRUPT_CHANNEL_ETHERNET      21
#define INTERRUPT_CHANNEL_USB           22
#define INTERRUPT_CHANNEL_CAN           23
#define INTERRUPT_CHANNEL_SD_MMC        24
#define INTERRUPT_CHANNEL_GP_DMA        25
#define INTERRUPT_CHANNEL_TIMER2        26
#define INTERRUPT_CHANNEL_TIMER3        27
#define INTERRUPT_CHANNEL_UART2         28
#define INTERRUPT_CHANNEL_UART3         29
#define INTERRUPT_CHANNEL_I2C2          30
#define INTERRUPT_CHANNEL_I2S           31


#endif
