// Copyright (c) 2009 Rowley Associates Limited.
//
// This file may be distributed under the terms of the License Agreement
// provided with this software.
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#ifndef LPC1752_h
#define LPC1752_h

#define GPIO_BASE_ADDRESS 0x2009C000

#define FIO0DIR (*(volatile unsigned long *)0x2009C000)
#define FIO0DIR_OFFSET 0x0

#define FIO0DIR0 (*(volatile unsigned char *)0x2009C000)
#define FIO0DIR0_OFFSET 0x0

#define FIO0DIR1 (*(volatile unsigned char *)0x2009C001)
#define FIO0DIR1_OFFSET 0x1

#define FIO0DIR2 (*(volatile unsigned char *)0x2009C002)
#define FIO0DIR2_OFFSET 0x2

#define FIO0DIR3 (*(volatile unsigned char *)0x2009C003)
#define FIO0DIR3_OFFSET 0x3

#define FIO0DIRL (*(volatile unsigned short *)0x2009C000)
#define FIO0DIRL_OFFSET 0x0

#define FIO0DIRU (*(volatile unsigned short *)0x2009C002)
#define FIO0DIRH FIO0DIRU
#define FIO0DIRU_OFFSET 0x2
#define FIO0DIRH_OFFSET FIO0DIRU_OFFSET

#define FIO1DIR (*(volatile unsigned long *)0x2009C020)
#define FIO1DIR_OFFSET 0x20

#define FIO1DIR0 (*(volatile unsigned char *)0x2009C020)
#define FIO1DIR0_OFFSET 0x20

#define FIO1DIR1 (*(volatile unsigned char *)0x2009C021)
#define FIO1DIR1_OFFSET 0x21

#define FIO1DIR2 (*(volatile unsigned char *)0x2009C022)
#define FIO1DIR2_OFFSET 0x22

#define FIO1DIR3 (*(volatile unsigned char *)0x2009C023)
#define FIO1DIR3_OFFSET 0x23

#define FIO1DIRL (*(volatile unsigned short *)0x2009C020)
#define FIO1DIRL_OFFSET 0x20

#define FIO1DIRU (*(volatile unsigned short *)0x2009C022)
#define FIO1DIRH FIO1DIRU
#define FIO1DIRU_OFFSET 0x22
#define FIO1DIRH_OFFSET FIO1DIRU_OFFSET

#define FIO2DIR (*(volatile unsigned long *)0x2009C040)
#define FIO2DIR_OFFSET 0x40

#define FIO2DIR0 (*(volatile unsigned char *)0x2009C040)
#define FIO2DIR0_OFFSET 0x40

#define FIO2DIR1 (*(volatile unsigned char *)0x2009C041)
#define FIO2DIR1_OFFSET 0x41

#define FIO2DIR2 (*(volatile unsigned char *)0x2009C042)
#define FIO2DIR2_OFFSET 0x42

#define FIO2DIR3 (*(volatile unsigned char *)0x2009C043)
#define FIO2DIR3_OFFSET 0x43

#define FIO2DIRL (*(volatile unsigned short *)0x2009C040)
#define FIO2DIRL_OFFSET 0x40

#define FIO2DIRU (*(volatile unsigned short *)0x2009C042)
#define FIO2DIRH FIO2DIRU
#define FIO2DIRU_OFFSET 0x42
#define FIO2DIRH_OFFSET FIO2DIRU_OFFSET

#define FIO3DIR (*(volatile unsigned long *)0x2009C060)
#define FIO3DIR_OFFSET 0x60

#define FIO3DIR0 (*(volatile unsigned char *)0x2009C060)
#define FIO3DIR0_OFFSET 0x60

#define FIO3DIR1 (*(volatile unsigned char *)0x2009C061)
#define FIO3DIR1_OFFSET 0x61

#define FIO3DIR2 (*(volatile unsigned char *)0x2009C062)
#define FIO3DIR2_OFFSET 0x62

#define FIO3DIR3 (*(volatile unsigned char *)0x2009C063)
#define FIO3DIR3_OFFSET 0x63

#define FIO3DIRL (*(volatile unsigned short *)0x2009C060)
#define FIO3DIRL_OFFSET 0x60

#define FIO3DIRU (*(volatile unsigned short *)0x2009C062)
#define FIO3DIRH FIO3DIRU
#define FIO3DIRU_OFFSET 0x62
#define FIO3DIRH_OFFSET FIO3DIRU_OFFSET

#define FIO4DIR (*(volatile unsigned long *)0x2009C080)
#define FIO4DIR_OFFSET 0x80

#define FIO4DIR0 (*(volatile unsigned char *)0x2009C080)
#define FIO4DIR0_OFFSET 0x80

#define FIO4DIR1 (*(volatile unsigned char *)0x2009C081)
#define FIO4DIR1_OFFSET 0x81

#define FIO4DIR2 (*(volatile unsigned char *)0x2009C082)
#define FIO4DIR2_OFFSET 0x82

#define FIO4DIR3 (*(volatile unsigned char *)0x2009C083)
#define FIO4DIR3_OFFSET 0x83

#define FIO4DIRL (*(volatile unsigned short *)0x2009C080)
#define FIO4DIRL_OFFSET 0x80

#define FIO4DIRU (*(volatile unsigned short *)0x2009C082)
#define FIO4DIRH FIO4DIRU
#define FIO4DIRU_OFFSET 0x82
#define FIO4DIRH_OFFSET FIO4DIRU_OFFSET

#define FIO0MASK (*(volatile unsigned long *)0x2009C010)
#define FIO0MASK_OFFSET 0x10

#define FIO0MASK0 (*(volatile unsigned char *)0x2009C010)
#define FIO0MASK0_OFFSET 0x10

#define FIO0MASK1 (*(volatile unsigned char *)0x2009C011)
#define FIO0MASK1_OFFSET 0x11

#define FIO0MASK2 (*(volatile unsigned char *)0x2009C012)
#define FIO0MASK2_OFFSET 0x12

#define FIO0MASK3 (*(volatile unsigned char *)0x2009C013)
#define FIO0MASK3_OFFSET 0x13

#define FIO0MASKL (*(volatile unsigned short *)0x2009C010)
#define FIO0MASKL_OFFSET 0x10

#define FIO0MASKU (*(volatile unsigned short *)0x2009C012)
#define FIO0MASKH FIO0MASKU
#define FIO0MASKU_OFFSET 0x12
#define FIO0MASKH_OFFSET FIO0MASKU_OFFSET

#define FIO1MASK (*(volatile unsigned long *)0x2009C030)
#define FIO1MASK_OFFSET 0x30

#define FIO1MASK0 (*(volatile unsigned char *)0x2009C030)
#define FIO1MASK0_OFFSET 0x30

#define FIO1MASK1 (*(volatile unsigned char *)0x2009C031)
#define FIO1MASK1_OFFSET 0x31

#define FIO1MASK2 (*(volatile unsigned char *)0x2009C032)
#define FIO1MASK2_OFFSET 0x32

#define FIO1MASK3 (*(volatile unsigned char *)0x2009C033)
#define FIO1MASK3_OFFSET 0x33

#define FIO1MASKL (*(volatile unsigned short *)0x2009C030)
#define FIO1MASKL_OFFSET 0x30

#define FIO1MASKU (*(volatile unsigned short *)0x2009C032)
#define FIO1MASKH FIO1MASKU
#define FIO1MASKU_OFFSET 0x32
#define FIO1MASKH_OFFSET FIO1MASKU_OFFSET

#define FIO2MASK (*(volatile unsigned long *)0x2009C050)
#define FIO2MASK_OFFSET 0x50

#define FIO2MASK0 (*(volatile unsigned char *)0x2009C050)
#define FIO2MASK0_OFFSET 0x50

#define FIO2MASK1 (*(volatile unsigned char *)0x2009C051)
#define FIO2MASK1_OFFSET 0x51

#define FIO2MASK2 (*(volatile unsigned char *)0x2009C052)
#define FIO2MASK2_OFFSET 0x52

#define FIO2MASK3 (*(volatile unsigned char *)0x2009C053)
#define FIO2MASK3_OFFSET 0x53

#define FIO2MASKL (*(volatile unsigned short *)0x2009C050)
#define FIO2MASKL_OFFSET 0x50

#define FIO2MASKU (*(volatile unsigned short *)0x2009C052)
#define FIO2MASKH FIO2MASKU
#define FIO2MASKU_OFFSET 0x52
#define FIO2MASKH_OFFSET FIO2MASKU_OFFSET

#define FIO3MASK (*(volatile unsigned long *)0x2009C070)
#define FIO3MASK_OFFSET 0x70

#define FIO3MASK0 (*(volatile unsigned char *)0x2009C070)
#define FIO3MASK0_OFFSET 0x70

#define FIO3MASK1 (*(volatile unsigned char *)0x2009C071)
#define FIO3MASK1_OFFSET 0x71

#define FIO3MASK2 (*(volatile unsigned char *)0x2009C072)
#define FIO3MASK2_OFFSET 0x72

#define FIO3MASK3 (*(volatile unsigned char *)0x2009C073)
#define FIO3MASK3_OFFSET 0x73

#define FIO3MASKL (*(volatile unsigned short *)0x2009C070)
#define FIO3MASKL_OFFSET 0x70

#define FIO3MASKU (*(volatile unsigned short *)0x2009C072)
#define FIO3MASKH FIO3MASKU
#define FIO3MASKU_OFFSET 0x72
#define FIO3MASKH_OFFSET FIO3MASKU_OFFSET

#define FIO4MASK (*(volatile unsigned long *)0x2009C090)
#define FIO4MASK_OFFSET 0x90

#define FIO4MASK0 (*(volatile unsigned char *)0x2009C090)
#define FIO4MASK0_OFFSET 0x90

#define FIO4MASK1 (*(volatile unsigned char *)0x2009C091)
#define FIO4MASK1_OFFSET 0x91

#define FIO4MASK2 (*(volatile unsigned char *)0x2009C092)
#define FIO4MASK2_OFFSET 0x92

#define FIO4MASK3 (*(volatile unsigned char *)0x2009C093)
#define FIO4MASK3_OFFSET 0x93

#define FIO4MASKL (*(volatile unsigned short *)0x2009C090)
#define FIO4MASKL_OFFSET 0x90

#define FIO4MASKU (*(volatile unsigned short *)0x2009C092)
#define FIO4MASKH FIO4MASKU
#define FIO4MASKU_OFFSET 0x92
#define FIO4MASKH_OFFSET FIO4MASKU_OFFSET

#define FIO0PIN (*(volatile unsigned long *)0x2009C014)
#define FIO0PIN_OFFSET 0x14

#define FIO0PIN0 (*(volatile unsigned char *)0x2009C014)
#define FIO0PIN0_OFFSET 0x14

#define FIO0PIN1 (*(volatile unsigned char *)0x2009C015)
#define FIO0PIN1_OFFSET 0x15

#define FIO0PIN2 (*(volatile unsigned char *)0x2009C016)
#define FIO0PIN2_OFFSET 0x16

#define FIO0PIN3 (*(volatile unsigned char *)0x2009C017)
#define FIO0PIN3_OFFSET 0x17

#define FIO0PINL (*(volatile unsigned short *)0x2009C014)
#define FIO0PINL_OFFSET 0x14

#define FIO0PINU (*(volatile unsigned short *)0x2009C016)
#define FIO0PINH FIO0PINU
#define FIO0PINU_OFFSET 0x16
#define FIO0PINH_OFFSET FIO0PINU_OFFSET

#define FIO1PIN (*(volatile unsigned long *)0x2009C034)
#define FIO1PIN_OFFSET 0x34

#define FIO1PIN0 (*(volatile unsigned char *)0x2009C034)
#define FIO1PIN0_OFFSET 0x34

#define FIO1PIN1 (*(volatile unsigned char *)0x2009C035)
#define FIO1PIN1_OFFSET 0x35

#define FIO1PIN2 (*(volatile unsigned char *)0x2009C036)
#define FIO1PIN2_OFFSET 0x36

#define FIO1PIN3 (*(volatile unsigned char *)0x2009C037)
#define FIO1PIN3_OFFSET 0x37

#define FIO1PINL (*(volatile unsigned short *)0x2009C034)
#define FIO1PINL_OFFSET 0x34

#define FIO1PINU (*(volatile unsigned short *)0x2009C036)
#define FIO1PINH FIO1PINU
#define FIO1PINU_OFFSET 0x36
#define FIO1PINH_OFFSET FIO1PINU_OFFSET

#define FIO2PIN (*(volatile unsigned long *)0x2009C054)
#define FIO2PIN_OFFSET 0x54

#define FIO2PIN0 (*(volatile unsigned char *)0x2009C054)
#define FIO2PIN0_OFFSET 0x54

#define FIO2PIN1 (*(volatile unsigned char *)0x2009C055)
#define FIO2PIN1_OFFSET 0x55

#define FIO2PIN2 (*(volatile unsigned char *)0x2009C056)
#define FIO2PIN2_OFFSET 0x56

#define FIO2PIN3 (*(volatile unsigned char *)0x2009C057)
#define FIO2PIN3_OFFSET 0x57

#define FIO2PINL (*(volatile unsigned short *)0x2009C054)
#define FIO2PINL_OFFSET 0x54

#define FIO2PINU (*(volatile unsigned short *)0x2009C056)
#define FIO2PINH FIO2PINU
#define FIO2PINU_OFFSET 0x56
#define FIO2PINH_OFFSET FIO2PINU_OFFSET

#define FIO3PIN (*(volatile unsigned long *)0x2009C074)
#define FIO3PIN_OFFSET 0x74

#define FIO3PIN0 (*(volatile unsigned char *)0x2009C074)
#define FIO3PIN0_OFFSET 0x74

#define FIO3PIN1 (*(volatile unsigned char *)0x2009C075)
#define FIO3PIN1_OFFSET 0x75

#define FIO3PIN2 (*(volatile unsigned char *)0x2009C076)
#define FIO3PIN2_OFFSET 0x76

#define FIO3PIN3 (*(volatile unsigned char *)0x2009C077)
#define FIO3PIN3_OFFSET 0x77

#define FIO3PINL (*(volatile unsigned short *)0x2009C074)
#define FIO3PINL_OFFSET 0x74

#define FIO3PINU (*(volatile unsigned short *)0x2009C076)
#define FIO3PINH FIO3PINU
#define FIO3PINU_OFFSET 0x76
#define FIO3PINH_OFFSET FIO3PINU_OFFSET

#define FIO4PIN (*(volatile unsigned long *)0x2009C094)
#define FIO4PIN_OFFSET 0x94

#define FIO4PIN0 (*(volatile unsigned char *)0x2009C094)
#define FIO4PIN0_OFFSET 0x94

#define FIO4PIN1 (*(volatile unsigned char *)0x2009C095)
#define FIO4PIN1_OFFSET 0x95

#define FIO4PIN2 (*(volatile unsigned char *)0x2009C096)
#define FIO4PIN2_OFFSET 0x96

#define FIO4PIN3 (*(volatile unsigned char *)0x2009C097)
#define FIO4PIN3_OFFSET 0x97

#define FIO4PINL (*(volatile unsigned short *)0x2009C094)
#define FIO4PINL_OFFSET 0x94

#define FIO4PINU (*(volatile unsigned short *)0x2009C096)
#define FIO4PINH FIO4PINU
#define FIO4PINU_OFFSET 0x96
#define FIO4PINH_OFFSET FIO4PINU_OFFSET

#define FIO0SET (*(volatile unsigned long *)0x2009C018)
#define FIO0SET_OFFSET 0x18

#define FIO0SET0 (*(volatile unsigned char *)0x2009C018)
#define FIO0SET0_OFFSET 0x18

#define FIO0SET1 (*(volatile unsigned char *)0x2009C019)
#define FIO0SET1_OFFSET 0x19

#define FIO0SET2 (*(volatile unsigned char *)0x2009C01A)
#define FIO0SET2_OFFSET 0x1A

#define FIO0SET3 (*(volatile unsigned char *)0x2009C01B)
#define FIO0SET3_OFFSET 0x1B

#define FIO0SETL (*(volatile unsigned short *)0x2009C018)
#define FIO0SETL_OFFSET 0x18

#define FIO0SETU (*(volatile unsigned short *)0x2009C01A)
#define FIO0SETH FIO0SETU
#define FIO0SETU_OFFSET 0x1A
#define FIO0SETH_OFFSET FIO0SETU_OFFSET

#define FIO1SET (*(volatile unsigned long *)0x2009C038)
#define FIO1SET_OFFSET 0x38

#define FIO1SET0 (*(volatile unsigned char *)0x2009C038)
#define FIO1SET0_OFFSET 0x38

#define FIO1SET1 (*(volatile unsigned char *)0x2009C039)
#define FIO1SET1_OFFSET 0x39

#define FIO1SET2 (*(volatile unsigned char *)0x2009C03A)
#define FIO1SET2_OFFSET 0x3A

#define FIO1SET3 (*(volatile unsigned char *)0x2009C03B)
#define FIO1SET3_OFFSET 0x3B

#define FIO1SETL (*(volatile unsigned short *)0x2009C038)
#define FIO1SETL_OFFSET 0x38

#define FIO1SETU (*(volatile unsigned short *)0x2009C03A)
#define FIO1SETH FIO1SETU
#define FIO1SETU_OFFSET 0x3A
#define FIO1SETH_OFFSET FIO1SETU_OFFSET

#define FIO2SET (*(volatile unsigned long *)0x2009C058)
#define FIO2SET_OFFSET 0x58

#define FIO2SET0 (*(volatile unsigned char *)0x2009C058)
#define FIO2SET0_OFFSET 0x58

#define FIO2SET1 (*(volatile unsigned char *)0x2009C059)
#define FIO2SET1_OFFSET 0x59

#define FIO2SET2 (*(volatile unsigned char *)0x2009C05A)
#define FIO2SET2_OFFSET 0x5A

#define FIO2SET3 (*(volatile unsigned char *)0x2009C05B)
#define FIO2SET3_OFFSET 0x5B

#define FIO2SETL (*(volatile unsigned short *)0x2009C058)
#define FIO2SETL_OFFSET 0x58

#define FIO2SETU (*(volatile unsigned short *)0x2009C05A)
#define FIO2SETH FIO2SETU
#define FIO2SETU_OFFSET 0x5A
#define FIO2SETH_OFFSET FIO2SETU_OFFSET

#define FIO3SET (*(volatile unsigned long *)0x2009C078)
#define FIO3SET_OFFSET 0x78

#define FIO3SET0 (*(volatile unsigned char *)0x2009C078)
#define FIO3SET0_OFFSET 0x78

#define FIO3SET1 (*(volatile unsigned char *)0x2009C079)
#define FIO3SET1_OFFSET 0x79

#define FIO3SET2 (*(volatile unsigned char *)0x2009C07A)
#define FIO3SET2_OFFSET 0x7A

#define FIO3SET3 (*(volatile unsigned char *)0x2009C07B)
#define FIO3SET3_OFFSET 0x7B

#define FIO3SETL (*(volatile unsigned short *)0x2009C078)
#define FIO3SETL_OFFSET 0x78

#define FIO3SETU (*(volatile unsigned short *)0x2009C07A)
#define FIO3SETH FIO3SETU
#define FIO3SETU_OFFSET 0x7A
#define FIO3SETH_OFFSET FIO3SETU_OFFSET

#define FIO4SET (*(volatile unsigned long *)0x2009C098)
#define FIO4SET_OFFSET 0x98

#define FIO4SET0 (*(volatile unsigned char *)0x2009C098)
#define FIO4SET0_OFFSET 0x98

#define FIO4SET1 (*(volatile unsigned char *)0x2009C099)
#define FIO4SET1_OFFSET 0x99

#define FIO4SET2 (*(volatile unsigned char *)0x2009C09A)
#define FIO4SET2_OFFSET 0x9A

#define FIO4SET3 (*(volatile unsigned char *)0x2009C09B)
#define FIO4SET3_OFFSET 0x9B

#define FIO4SETL (*(volatile unsigned short *)0x2009C098)
#define FIO4SETL_OFFSET 0x98

#define FIO4SETU (*(volatile unsigned short *)0x2009C09A)
#define FIO4SETH FIO4SETU
#define FIO4SETU_OFFSET 0x9A
#define FIO4SETH_OFFSET FIO4SETU_OFFSET

#define FIO0CLR (*(volatile unsigned long *)0x2009C01C)
#define FIO0CLR_OFFSET 0x1C

#define FIO0CLR0 (*(volatile unsigned char *)0x2009C01C)
#define FIO0CLR0_OFFSET 0x1C

#define FIO0CLR1 (*(volatile unsigned char *)0x2009C01D)
#define FIO0CLR1_OFFSET 0x1D

#define FIO0CLR2 (*(volatile unsigned char *)0x2009C01E)
#define FIO0CLR2_OFFSET 0x1E

#define FIO0CLR3 (*(volatile unsigned char *)0x2009C01F)
#define FIO0CLR3_OFFSET 0x1F

#define FIO0CLRL (*(volatile unsigned short *)0x2009C01C)
#define FIO0CLRL_OFFSET 0x1C

#define FIO0CLRU (*(volatile unsigned short *)0x2009C01E)
#define FIO0CLRH FIO0CLRU
#define FIO0CLRU_OFFSET 0x1E
#define FIO0CLRH_OFFSET FIO0CLRU_OFFSET

#define FIO1CLR (*(volatile unsigned long *)0x2009C03C)
#define FIO1CLR_OFFSET 0x3C

#define FIO1CLR0 (*(volatile unsigned char *)0x2009C03C)
#define FIO1CLR0_OFFSET 0x3C

#define FIO1CLR1 (*(volatile unsigned char *)0x2009C03D)
#define FIO1CLR1_OFFSET 0x3D

#define FIO1CLR2 (*(volatile unsigned char *)0x2009C03E)
#define FIO1CLR2_OFFSET 0x3E

#define FIO1CLR3 (*(volatile unsigned char *)0x2009C03F)
#define FIO1CLR3_OFFSET 0x3F

#define FIO1CLRL (*(volatile unsigned short *)0x2009C03C)
#define FIO1CLRL_OFFSET 0x3C

#define FIO1CLRU (*(volatile unsigned short *)0x2009C03E)
#define FIO1CLRH FIO1CLRU
#define FIO1CLRU_OFFSET 0x3E
#define FIO1CLRH_OFFSET FIO1CLRU_OFFSET

#define FIO2CLR (*(volatile unsigned long *)0x2009C05C)
#define FIO2CLR_OFFSET 0x5C

#define FIO2CLR0 (*(volatile unsigned char *)0x2009C05C)
#define FIO2CLR0_OFFSET 0x5C

#define FIO2CLR1 (*(volatile unsigned char *)0x2009C05D)
#define FIO2CLR1_OFFSET 0x5D

#define FIO2CLR2 (*(volatile unsigned char *)0x2009C05E)
#define FIO2CLR2_OFFSET 0x5E

#define FIO2CLR3 (*(volatile unsigned char *)0x2009C05F)
#define FIO2CLR3_OFFSET 0x5F

#define FIO2CLRL (*(volatile unsigned short *)0x2009C05C)
#define FIO2CLRL_OFFSET 0x5C

#define FIO2CLRU (*(volatile unsigned short *)0x2009C05E)
#define FIO2CLRH FIO2CLRU
#define FIO2CLRU_OFFSET 0x5E
#define FIO2CLRH_OFFSET FIO2CLRU_OFFSET

#define FIO3CLR (*(volatile unsigned long *)0x2009C07C)
#define FIO3CLR_OFFSET 0x7C

#define FIO3CLR0 (*(volatile unsigned char *)0x2009C07C)
#define FIO3CLR0_OFFSET 0x7C

#define FIO3CLR1 (*(volatile unsigned char *)0x2009C07D)
#define FIO3CLR1_OFFSET 0x7D

#define FIO3CLR2 (*(volatile unsigned char *)0x2009C07E)
#define FIO3CLR2_OFFSET 0x7E

#define FIO3CLR3 (*(volatile unsigned char *)0x2009C07F)
#define FIO3CLR3_OFFSET 0x7F

#define FIO3CLRL (*(volatile unsigned short *)0x2009C07C)
#define FIO3CLRL_OFFSET 0x7C

#define FIO3CLRU (*(volatile unsigned short *)0x2009C07E)
#define FIO3CLRH FIO3CLRU
#define FIO3CLRU_OFFSET 0x7E
#define FIO3CLRH_OFFSET FIO3CLRU_OFFSET

#define FIO4CLR (*(volatile unsigned long *)0x2009C09C)
#define FIO4CLR_OFFSET 0x9C

#define FIO4CLR0 (*(volatile unsigned char *)0x2009C09C)
#define FIO4CLR0_OFFSET 0x9C

#define FIO4CLR1 (*(volatile unsigned char *)0x2009C09D)
#define FIO4CLR1_OFFSET 0x9D

#define FIO4CLR2 (*(volatile unsigned char *)0x2009C09E)
#define FIO4CLR2_OFFSET 0x9E

#define FIO4CLR3 (*(volatile unsigned char *)0x2009C09F)
#define FIO4CLR3_OFFSET 0x9F

#define FIO4CLRL (*(volatile unsigned short *)0x2009C09C)
#define FIO4CLRL_OFFSET 0x9C

#define FIO4CLRU (*(volatile unsigned short *)0x2009C09E)
#define FIO4CLRH FIO4CLRU
#define FIO4CLRU_OFFSET 0x9E
#define FIO4CLRH_OFFSET FIO4CLRU_OFFSET

#define WDT_BASE_ADDRESS 0x40000000

#define WDMOD (*(volatile unsigned long *)0x40000000)
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

#define WDTC (*(volatile unsigned long *)0x40000004)
#define WDTC_OFFSET 0x4

#define WDFEED (*(volatile unsigned long *)0x40000008)
#define WDFEED_OFFSET 0x8

#define WDTV (*(volatile unsigned long *)0x4000000C)
#define WDTV_OFFSET 0xC

#define WDCLKSEL (*(volatile unsigned long *)0x40000010)
#define WDCLKSEL_OFFSET 0x10
#define WDCLKSEL_WDSEL_MASK 0x3
#define WDCLKSEL_WDSEL_BIT 0
#define WDCLKSEL_WDLOCK_MASK 0x80000000
#define WDCLKSEL_WDLOCK 0x80000000
#define WDCLKSEL_WDLOCK_BIT 31

#define TIMER0_BASE_ADDRESS 0x40004000

#define T0IR (*(volatile unsigned char *)0x40004000)
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

#define T0TCR (*(volatile unsigned char *)0x40004004)
#define T0TCR_OFFSET 0x4
#define T0TCR_Counter_Enable_MASK 0x1
#define T0TCR_Counter_Enable 0x1
#define T0TCR_Counter_Enable_BIT 0
#define T0TCR_Counter_Reset_MASK 0x2
#define T0TCR_Counter_Reset 0x2
#define T0TCR_Counter_Reset_BIT 1

#define T0TC (*(volatile unsigned long *)0x40004008)
#define T0TC_OFFSET 0x8

#define T0PR (*(volatile unsigned long *)0x4000400C)
#define T0PR_OFFSET 0xC

#define T0PC (*(volatile unsigned long *)0x40004010)
#define T0PC_OFFSET 0x10

#define T0MCR (*(volatile unsigned short *)0x40004014)
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

#define T0MR0 (*(volatile unsigned long *)0x40004018)
#define T0MR0_OFFSET 0x18

#define T0MR1 (*(volatile unsigned long *)0x4000401C)
#define T0MR1_OFFSET 0x1C

#define T0MR2 (*(volatile unsigned long *)0x40004020)
#define T0MR2_OFFSET 0x20

#define T0MR3 (*(volatile unsigned long *)0x40004024)
#define T0MR3_OFFSET 0x24

#define T0CCR (*(volatile unsigned short *)0x40004028)
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

#define T0CR0 (*(volatile unsigned long *)0x4000402C)
#define T0CR0_OFFSET 0x2C

#define T0CR1 (*(volatile unsigned long *)0x40004030)
#define T0CR1_OFFSET 0x30

#define T0EMR (*(volatile unsigned short *)0x4000403C)
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

#define TIMER1_BASE_ADDRESS 0x40008000

#define T1IR (*(volatile unsigned char *)0x40008000)
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

#define T1TCR (*(volatile unsigned char *)0x40008004)
#define T1TCR_OFFSET 0x4
#define T1TCR_Counter_Enable_MASK 0x1
#define T1TCR_Counter_Enable 0x1
#define T1TCR_Counter_Enable_BIT 0
#define T1TCR_Counter_Reset_MASK 0x2
#define T1TCR_Counter_Reset 0x2
#define T1TCR_Counter_Reset_BIT 1

#define T1TC (*(volatile unsigned long *)0x40008008)
#define T1TC_OFFSET 0x8

#define T1PR (*(volatile unsigned long *)0x4000800C)
#define T1PR_OFFSET 0xC

#define T1PC (*(volatile unsigned long *)0x40008010)
#define T1PC_OFFSET 0x10

#define T1MCR (*(volatile unsigned short *)0x40008014)
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

#define T1MR0 (*(volatile unsigned long *)0x40008018)
#define T1MR0_OFFSET 0x18

#define T1MR1 (*(volatile unsigned long *)0x4000801C)
#define T1MR1_OFFSET 0x1C

#define T1MR2 (*(volatile unsigned long *)0x40008020)
#define T1MR2_OFFSET 0x20

#define T1MR3 (*(volatile unsigned long *)0x40008024)
#define T1MR3_OFFSET 0x24

#define T1CCR (*(volatile unsigned short *)0x40008028)
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

#define T1CR0 (*(volatile unsigned long *)0x4000802C)
#define T1CR0_OFFSET 0x2C

#define T1CR1 (*(volatile unsigned long *)0x40008030)
#define T1CR1_OFFSET 0x30

#define T1EMR (*(volatile unsigned short *)0x4000803C)
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

#define UART0_BASE_ADDRESS 0x4000C000

#define U0RBR (*(volatile unsigned char *)0x4000C000)
#define U0RBR_OFFSET 0x0

#define U0THR (*(volatile unsigned char *)0x4000C000)
#define U0THR_OFFSET 0x0

#define U0DLL (*(volatile unsigned char *)0x4000C000)
#define U0DLL_OFFSET 0x0

#define U0DLM (*(volatile unsigned char *)0x4000C004)
#define U0DLM_OFFSET 0x4

#define U0IER (*(volatile unsigned long *)0x4000C004)
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

#define U0IIR (*(volatile unsigned long *)0x4000C008)
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

#define U0FCR (*(volatile unsigned char *)0x4000C008)
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
#define U0FCR_DMA_Mode_Select_MASK 0x8
#define U0FCR_DMA_Mode_Select 0x8
#define U0FCR_DMA_Mode_Select_BIT 3
#define U0FCR_Rx_Trigger_Level_Select_MASK 0xC0
#define U0FCR_Rx_Trigger_Level_Select_BIT 6

#define U0LCR (*(volatile unsigned char *)0x4000C00C)
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

#define U0LSR (*(volatile unsigned char *)0x4000C014)
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

#define U0SCR (*(volatile unsigned char *)0x4000C01C)
#define U0SCR_OFFSET 0x1C

#define U0ACR (*(volatile unsigned long *)0x4000C020)
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

#define U0FDR (*(volatile unsigned long *)0x4000C028)
#define U0FDR_OFFSET 0x28
#define U0FDR_DIVADDVAL_MASK 0xF
#define U0FDR_DIVADDVAL_BIT 0
#define U0FDR_MULVAL_MASK 0xF0
#define U0FDR_MULVAL_BIT 4

#define U0TER (*(volatile unsigned char *)0x4000C030)
#define U0TER_OFFSET 0x30
#define U0TER_TXEN_MASK 0x80
#define U0TER_TXEN 0x80
#define U0TER_TXEN_BIT 7

#define U0FIFOLVL (*(volatile unsigned char *)0x4000C058)
#define U0FIFOLVL_OFFSET 0x58
#define U0FIFOLVL_RXFIFILVL_MASK 0xF
#define U0FIFOLVL_RXFIFILVL_BIT 0
#define U0FIFOLVL_RXFIFOLVL_MASK 0xF00
#define U0FIFOLVL_RXFIFOLVL_BIT 8

#define UART1_BASE_ADDRESS 0x40010000

#define U1RBR (*(volatile unsigned char *)0x40010000)
#define U1RBR_OFFSET 0x0

#define U1THR (*(volatile unsigned char *)0x40010000)
#define U1THR_OFFSET 0x0

#define U1DLL (*(volatile unsigned char *)0x40010000)
#define U1DLL_OFFSET 0x0

#define U1DLM (*(volatile unsigned char *)0x40010004)
#define U1DLM_OFFSET 0x4

#define U1IER (*(volatile unsigned long *)0x40010004)
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

#define U1IIR (*(volatile unsigned long *)0x40010008)
#define U1IIR_OFFSET 0x8
#define U1IIR_IntStatus_MASK 0x1
#define U1IIR_IntStatus 0x1
#define U1IIR_IntStatus_BIT 0
#define U1IIR_IntId_MASK 0xE
#define U1IIR_IntId_BIT 1
#define U1IIR_FIFO_Enable_MASK 0xC0
#define U1IIR_FIFO_Enable_BIT 6
#define U1IIR_ABEOInt_MASK 0x100
#define U1IIR_ABEOInt 0x100
#define U1IIR_ABEOInt_BIT 8
#define U1IIR_ABTOInt_MASK 0x200
#define U1IIR_ABTOInt 0x200
#define U1IIR_ABTOInt_BIT 9

#define U1FCR (*(volatile unsigned char *)0x40010008)
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
#define U1FCR_DMA_Mode_Select_MASK 0x8
#define U1FCR_DMA_Mode_Select 0x8
#define U1FCR_DMA_Mode_Select_BIT 3
#define U1FCR_Rx_Trigger_Level_Select_MASK 0xC0
#define U1FCR_Rx_Trigger_Level_Select_BIT 6

#define U1LCR (*(volatile unsigned char *)0x4001000C)
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

#define U1MCR (*(volatile unsigned char *)0x40010010)
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

#define U1LSR (*(volatile unsigned char *)0x40010014)
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

#define U1MSR (*(volatile unsigned char *)0x40010018)
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

#define U1SCR (*(volatile unsigned char *)0x4001001C)
#define U1SCR_OFFSET 0x1C

#define U1ACR (*(volatile unsigned long *)0x40010020)
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

#define U1FDR (*(volatile unsigned long *)0x40010028)
#define U1FDR_OFFSET 0x28
#define U1FDR_DIVADDVAL_MASK 0xF
#define U1FDR_DIVADDVAL_BIT 0
#define U1FDR_MULVAL_MASK 0xF0
#define U1FDR_MULVAL_BIT 4

#define U1TER (*(volatile unsigned char *)0x40010030)
#define U1TER_OFFSET 0x30
#define U1TER_TXEN_MASK 0x80
#define U1TER_TXEN 0x80
#define U1TER_TXEN_BIT 7

#define U1RS485CTRL (*(volatile unsigned long *)0x4001004C)
#define U1RS485CTRL_OFFSET 0x4C
#define U1RS485CTRL_NMMEN_MASK 0x1
#define U1RS485CTRL_NMMEN 0x1
#define U1RS485CTRL_NMMEN_BIT 0
#define U1RS485CTRL_RXDIS_MASK 0x2
#define U1RS485CTRL_RXDIS 0x2
#define U1RS485CTRL_RXDIS_BIT 1
#define U1RS485CTRL_AADEN_MASK 0x4
#define U1RS485CTRL_AADEN 0x4
#define U1RS485CTRL_AADEN_BIT 2
#define U1RS485CTRL_SEL_MASK 0x8
#define U1RS485CTRL_SEL 0x8
#define U1RS485CTRL_SEL_BIT 3
#define U1RS485CTRL_DCTRL_MASK 0x10
#define U1RS485CTRL_DCTRL 0x10
#define U1RS485CTRL_DCTRL_BIT 4
#define U1RS485CTRL_OINV_MASK 0x20
#define U1RS485CTRL_OINV 0x20
#define U1RS485CTRL_OINV_BIT 5

#define U1RS485ADRMATCH (*(volatile unsigned long *)0x4001004C)
#define U1RS485ADRMATCH_OFFSET 0x4C

#define U1RS485DLY (*(volatile unsigned long *)0x4001004C)
#define U1RS485DLY_OFFSET 0x4C

#define U1FIFOLVL (*(volatile unsigned char *)0x40010058)
#define U1FIFOLVL_OFFSET 0x58
#define U1FIFOLVL_RXFIFILVL_MASK 0xF
#define U1FIFOLVL_RXFIFILVL_BIT 0
#define U1FIFOLVL_RXFIFOLVL_MASK 0xF00
#define U1FIFOLVL_RXFIFOLVL_BIT 8

#define PWM1_BASE_ADDRESS 0x40018000

#define PWM1IR (*(volatile unsigned long *)0x40018000)
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
#define PWM1IR_PWMCAP0_Interrupt_MASK 0x10
#define PWM1IR_PWMCAP0_Interrupt 0x10
#define PWM1IR_PWMCAP0_Interrupt_BIT 4
#define PWM1IR_PWMCAP1_Interrupt_MASK 0x20
#define PWM1IR_PWMCAP1_Interrupt 0x20
#define PWM1IR_PWMCAP1_Interrupt_BIT 5
#define PWM1IR_PWMMR4_Interrupt_MASK 0x100
#define PWM1IR_PWMMR4_Interrupt 0x100
#define PWM1IR_PWMMR4_Interrupt_BIT 8
#define PWM1IR_PWMMR5_Interrupt_MASK 0x200
#define PWM1IR_PWMMR5_Interrupt 0x200
#define PWM1IR_PWMMR5_Interrupt_BIT 9
#define PWM1IR_PWMMR6_Interrupt_MASK 0x400
#define PWM1IR_PWMMR6_Interrupt 0x400
#define PWM1IR_PWMMR6_Interrupt_BIT 10

#define PWM1TCR (*(volatile unsigned long *)0x40018004)
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

#define PWM1TC (*(volatile unsigned long *)0x40018008)
#define PWM1TC_OFFSET 0x8

#define PWM1PR (*(volatile unsigned long *)0x4001800C)
#define PWM1PR_OFFSET 0xC

#define PWM1PC (*(volatile unsigned long *)0x40018010)
#define PWM1PC_OFFSET 0x10

#define PWM1MCR (*(volatile unsigned long *)0x40018014)
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

#define PWM1MR0 (*(volatile unsigned long *)0x40018018)
#define PWM1MR0_OFFSET 0x18

#define PWM1MR1 (*(volatile unsigned long *)0x4001801C)
#define PWM1MR1_OFFSET 0x1C

#define PWM1MR2 (*(volatile unsigned long *)0x40018020)
#define PWM1MR2_OFFSET 0x20

#define PWM1MR3 (*(volatile unsigned long *)0x40018024)
#define PWM1MR3_OFFSET 0x24

#define PWM1CCR (*(volatile unsigned long *)0x40018028)
#define PWM1CCR_OFFSET 0x28
#define PWM1CCR_Capture_on_CAPn_0_rising_edge_MASK 0x1
#define PWM1CCR_Capture_on_CAPn_0_rising_edge 0x1
#define PWM1CCR_Capture_on_CAPn_0_rising_edge_BIT 0
#define PWM1CCR_Capture_on_CAPn_0_falling_edge_MASK 0x2
#define PWM1CCR_Capture_on_CAPn_0_falling_edge 0x2
#define PWM1CCR_Capture_on_CAPn_0_falling_edge_BIT 1
#define PWM1CCR_Interrupt_on_CAPn_0_event_MASK 0x4
#define PWM1CCR_Interrupt_on_CAPn_0_event 0x4
#define PWM1CCR_Interrupt_on_CAPn_0_event_BIT 2
#define PWM1CCR_Capture_on_CAPn_1_rising_edge_MASK 0x8
#define PWM1CCR_Capture_on_CAPn_1_rising_edge 0x8
#define PWM1CCR_Capture_on_CAPn_1_rising_edge_BIT 3
#define PWM1CCR_Capture_on_CAPn_1_falling_edge_MASK 0x10
#define PWM1CCR_Capture_on_CAPn_1_falling_edge 0x10
#define PWM1CCR_Capture_on_CAPn_1_falling_edge_BIT 4
#define PWM1CCR_Interrupt_on_CAPn_1_event_MASK 0x20
#define PWM1CCR_Interrupt_on_CAPn_1_event 0x20
#define PWM1CCR_Interrupt_on_CAPn_1_event_BIT 5

#define PWM1CR0 (*(volatile unsigned long *)0x4001802C)
#define PWM1CR0_OFFSET 0x2C

#define PWM1CR1 (*(volatile unsigned long *)0x40018030)
#define PWM1CR1_OFFSET 0x30

#define PWM1CR2 (*(volatile unsigned long *)0x40018034)
#define PWM1CR2_OFFSET 0x34

#define PWM1CR3 (*(volatile unsigned long *)0x40018038)
#define PWM1CR3_OFFSET 0x38

#define PWM1MR4 (*(volatile unsigned long *)0x40018040)
#define PWM1MR4_OFFSET 0x40

#define PWM1MR5 (*(volatile unsigned long *)0x40018044)
#define PWM1MR5_OFFSET 0x44

#define PWM1MR6 (*(volatile unsigned long *)0x40018048)
#define PWM1MR6_OFFSET 0x48

#define PWM1PCR (*(volatile unsigned long *)0x4001804C)
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

#define PWM1LER (*(volatile unsigned long *)0x40018050)
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

#define PWM1CTCR (*(volatile unsigned long *)0x40018070)
#define PWM1CTCR_OFFSET 0x70
#define PWM1CTCR_Counter_Timer_Mode_MASK 0x3
#define PWM1CTCR_Counter_Timer_Mode_BIT 0
#define PWM1CTCR_Count_Input_Select_MASK 0xC
#define PWM1CTCR_Count_Input_Select_BIT 2

#define I2C0_BASE_ADDRESS 0x4001C000

#define I2C0CONSET (*(volatile unsigned char *)0x4001C000)
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

#define I2C0STAT (*(volatile unsigned char *)0x4001C004)
#define I2C0STAT_OFFSET 0x4
#define I2C0STAT_Status_MASK 0xF8
#define I2C0STAT_Status_BIT 3

#define I2C0DAT (*(volatile unsigned char *)0x4001C008)
#define I2C0DAT_OFFSET 0x8

#define I2C0ADR0 (*(volatile unsigned char *)0x4001C00C)
#define I2C0ADR0_OFFSET 0xC
#define I2C0ADR0_GC_MASK 0x1
#define I2C0ADR0_GC 0x1
#define I2C0ADR0_GC_BIT 0
#define I2C0ADR0_Address_MASK 0xFE
#define I2C0ADR0_Address_BIT 1

#define I2C0SCLH (*(volatile unsigned short *)0x4001C010)
#define I2C0SCLH_OFFSET 0x10

#define I2C0SCLL (*(volatile unsigned short *)0x4001C014)
#define I2C0SCLL_OFFSET 0x14

#define I2C0CONCLR (*(volatile unsigned char *)0x4001C018)
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

#define I2C0MMCTRL (*(volatile unsigned char *)0x4001C01C)
#define I2C0MMCTRL_OFFSET 0x1C
#define I2C0MMCTRL_MM_ENA_MASK 0x1
#define I2C0MMCTRL_MM_ENA 0x1
#define I2C0MMCTRL_MM_ENA_BIT 0
#define I2C0MMCTRL_ENA_SCL_MASK 0x2
#define I2C0MMCTRL_ENA_SCL 0x2
#define I2C0MMCTRL_ENA_SCL_BIT 1
#define I2C0MMCTRL_MATCH_ALL_MASK 0x8
#define I2C0MMCTRL_MATCH_ALL 0x8
#define I2C0MMCTRL_MATCH_ALL_BIT 3

#define I2C0ADR1 (*(volatile unsigned char *)0x4001C020)
#define I2C0ADR1_OFFSET 0x20
#define I2C0ADR1_GC_MASK 0x1
#define I2C0ADR1_GC 0x1
#define I2C0ADR1_GC_BIT 0
#define I2C0ADR1_Address_MASK 0xFE
#define I2C0ADR1_Address_BIT 1

#define I2C0ADR2 (*(volatile unsigned char *)0x4001C024)
#define I2C0ADR2_OFFSET 0x24
#define I2C0ADR2_GC_MASK 0x1
#define I2C0ADR2_GC 0x1
#define I2C0ADR2_GC_BIT 0
#define I2C0ADR2_Address_MASK 0xFE
#define I2C0ADR2_Address_BIT 1

#define I2C0ADR3 (*(volatile unsigned char *)0x4001C028)
#define I2C0ADR3_OFFSET 0x28
#define I2C0ADR3_GC_MASK 0x1
#define I2C0ADR3_GC 0x1
#define I2C0ADR3_GC_BIT 0
#define I2C0ADR3_Address_MASK 0xFE
#define I2C0ADR3_Address_BIT 1

#define I2C0DATA_BUFFER (*(volatile unsigned char *)0x4001C02C)
#define I2C0DATA_BUFFER_OFFSET 0x2C

#define I2C0MASK0 (*(volatile unsigned long *)0x4001C030)
#define I2C0MASK0_OFFSET 0x30
#define I2C0MASK0_MASK_MASK 0xFE
#define I2C0MASK0_MASK_BIT 1

#define I2C0MASK1 (*(volatile unsigned long *)0x4001C034)
#define I2C0MASK1_OFFSET 0x34
#define I2C0MASK1_MASK_MASK 0xFE
#define I2C0MASK1_MASK_BIT 1

#define I2C0MASK2 (*(volatile unsigned long *)0x4001C038)
#define I2C0MASK2_OFFSET 0x38
#define I2C0MASK2_MASK_MASK 0xFE
#define I2C0MASK2_MASK_BIT 1

#define I2C0MASK3 (*(volatile unsigned long *)0x4001C03C)
#define I2C0MASK3_OFFSET 0x3C
#define I2C0MASK3_MASK_MASK 0xFE
#define I2C0MASK3_MASK_BIT 1

#define SPI0_BASE_ADDRESS 0x40020000

#define S0SPCR (*(volatile unsigned short *)0x40020000)
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

#define S0SPSR (*(volatile unsigned char *)0x40020004)
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

#define S0SPDR (*(volatile unsigned short *)0x40020008)
#define S0SPDR_OFFSET 0x8

#define S0SPCCR (*(volatile unsigned char *)0x4002000C)
#define S0SPCCR_OFFSET 0xC

#define S0SPINT (*(volatile unsigned char *)0x4002001C)
#define S0SPINT_OFFSET 0x1C

#define RTC_BASE_ADDRESS 0x40024000

#define ILR (*(volatile unsigned char *)0x40024000)
#define ILR_OFFSET 0x0
#define ILR_RTCCIF_MASK 0x1
#define ILR_RTCCIF 0x1
#define ILR_RTCCIF_BIT 0
#define ILR_RTCALF_MASK 0x2
#define ILR_RTCALF 0x2
#define ILR_RTCALF_BIT 1

#define CCR (*(volatile unsigned char *)0x40024008)
#define CCR_OFFSET 0x8
#define CCR_CLKEN_MASK 0x1
#define CCR_CLKEN 0x1
#define CCR_CLKEN_BIT 0
#define CCR_CTCRST_MASK 0x2
#define CCR_CTCRST 0x2
#define CCR_CTCRST_BIT 1
#define CCR_CCALEN_MASK 0x10
#define CCR_CCALEN 0x10
#define CCR_CCALEN_BIT 4

#define CIIR (*(volatile unsigned char *)0x4002400C)
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

#define AMR (*(volatile unsigned char *)0x40024010)
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

#define CTIME0 (*(volatile unsigned long *)0x40024014)
#define CTIME0_OFFSET 0x14
#define CTIME0_Seconds_MASK 0x3F
#define CTIME0_Seconds_BIT 0
#define CTIME0_Minutes_MASK 0x3F00
#define CTIME0_Minutes_BIT 8
#define CTIME0_Hours_MASK 0x1F0000
#define CTIME0_Hours_BIT 16
#define CTIME0_Day_of_Week_MASK 0x7000000
#define CTIME0_Day_of_Week_BIT 24

#define CTIME1 (*(volatile unsigned long *)0x40024018)
#define CTIME1_OFFSET 0x18
#define CTIME1_Day_of_Month_MASK 0x1F
#define CTIME1_Day_of_Month_BIT 0
#define CTIME1_Month_MASK 0xF00
#define CTIME1_Month_BIT 8
#define CTIME1_Year_MASK 0xFFF0000
#define CTIME1_Year_BIT 16

#define CTIME2 (*(volatile unsigned long *)0x4002401C)
#define CTIME2_OFFSET 0x1C
#define CTIME2_Day_of_Year_MASK 0xFFF
#define CTIME2_Day_of_Year_BIT 0

#define SEC (*(volatile unsigned long *)0x40024020)
#define SEC_OFFSET 0x20

#define MIN (*(volatile unsigned long *)0x40024024)
#define MIN_OFFSET 0x24

#define HOUR (*(volatile unsigned long *)0x40024028)
#define HOUR_OFFSET 0x28

#define DOM (*(volatile unsigned long *)0x4002402C)
#define DOM_OFFSET 0x2C

#define DOW (*(volatile unsigned long *)0x40024030)
#define DOW_OFFSET 0x30

#define DOY (*(volatile unsigned long *)0x40024034)
#define DOY_OFFSET 0x34

#define MONTH (*(volatile unsigned long *)0x40024038)
#define MONTH_OFFSET 0x38

#define YEAR (*(volatile unsigned long *)0x4002403C)
#define YEAR_OFFSET 0x3C

#define CALIBRATION (*(volatile unsigned long *)0x40024040)
#define CALIBRATION_OFFSET 0x40
#define CALIBRATION_CALVAL_MASK 0x1FFFF
#define CALIBRATION_CALVAL_BIT 0
#define CALIBRATION_CALDIR_MASK 0x20000
#define CALIBRATION_CALDIR 0x20000
#define CALIBRATION_CALDIR_BIT 17

#define GPREG0 (*(volatile unsigned long *)0x40024044)
#define GPREG0_OFFSET 0x44

#define GPREG1 (*(volatile unsigned long *)0x40024048)
#define GPREG1_OFFSET 0x48

#define GPREG2 (*(volatile unsigned long *)0x4002404C)
#define GPREG2_OFFSET 0x4C

#define GPREG3 (*(volatile unsigned long *)0x40024050)
#define GPREG3_OFFSET 0x50

#define GPREG4 (*(volatile unsigned long *)0x40024054)
#define GPREG4_OFFSET 0x54

#define RTC_AUXEN (*(volatile unsigned char *)0x40024058)
#define RTC_AUXEN_OFFSET 0x58
#define RTC_AUXEN_RTC_OSCFEN_MASK 0x10
#define RTC_AUXEN_RTC_OSCFEN 0x10
#define RTC_AUXEN_RTC_OSCFEN_BIT 4

#define RTC_AUX (*(volatile unsigned char *)0x4002405C)
#define RTC_AUX_OFFSET 0x5C
#define RTC_AUX_RTC_OSCF_MASK 0x10
#define RTC_AUX_RTC_OSCF 0x10
#define RTC_AUX_RTC_OSCF_BIT 4

#define ALSEC (*(volatile unsigned long *)0x40024060)
#define ALSEC_OFFSET 0x60

#define ALMIN (*(volatile unsigned long *)0x40024064)
#define ALMIN_OFFSET 0x64

#define ALHOUR (*(volatile unsigned long *)0x40024068)
#define ALHOUR_OFFSET 0x68

#define ALDOM (*(volatile unsigned long *)0x4002406C)
#define ALDOM_OFFSET 0x6C

#define ALDOW (*(volatile unsigned long *)0x40024070)
#define ALDOW_OFFSET 0x70

#define ALDOY (*(volatile unsigned long *)0x40024074)
#define ALDOY_OFFSET 0x74

#define ALMON (*(volatile unsigned long *)0x40024078)
#define ALMON_OFFSET 0x78

#define ALYEAR (*(volatile unsigned long *)0x4002407C)
#define ALYEAR_OFFSET 0x7C

#define GPIO_INT_BASE_ADDRESS 0x40028000

#define IOIntStatus (*(volatile unsigned long *)0x40028080)
#define IOIntStatus_OFFSET 0x80
#define IOIntStatus_P0Int_MASK 0x1
#define IOIntStatus_P0Int 0x1
#define IOIntStatus_P0Int_BIT 0
#define IOIntStatus_P2Int_MASK 0x4
#define IOIntStatus_P2Int 0x4
#define IOIntStatus_P2Int_BIT 2

#define IO0IntStatR (*(volatile unsigned long *)0x40028084)
#define IO0IntStatR_OFFSET 0x84

#define IO0IntStatF (*(volatile unsigned long *)0x40028088)
#define IO0IntStatF_OFFSET 0x88

#define IO0IntClr (*(volatile unsigned long *)0x4002808C)
#define IO0IntClr_OFFSET 0x8C

#define IO0IntEnR (*(volatile unsigned long *)0x40028090)
#define IO0IntEnR_OFFSET 0x90

#define IO0IntEnF (*(volatile unsigned long *)0x40028094)
#define IO0IntEnF_OFFSET 0x94

#define IO2IntStatR (*(volatile unsigned long *)0x400280A4)
#define IO2IntStatR_OFFSET 0xA4

#define IO2IntStatF (*(volatile unsigned long *)0x400280A8)
#define IO2IntStatF_OFFSET 0xA8

#define IO2IntClr (*(volatile unsigned long *)0x400280AC)
#define IO2IntClr_OFFSET 0xAC

#define IO2IntEnR (*(volatile unsigned long *)0x400280B0)
#define IO2IntEnR_OFFSET 0xB0

#define IO2IntEnF (*(volatile unsigned long *)0x400280B4)
#define IO2IntEnF_OFFSET 0xB4

#define PCB_BASE_ADDRESS 0x4002C000

#define PINSEL0 (*(volatile unsigned long *)0x4002C000)
#define PINSEL0_OFFSET 0x0
#define PINSEL0_P0_0_MASK 0x3
#define PINSEL0_P0_0_BIT 0
#define PINSEL0_P0_1_MASK 0xC
#define PINSEL0_P0_1_BIT 2
#define PINSEL0_P0_2_MASK 0x30
#define PINSEL0_P0_2_BIT 4
#define PINSEL0_P0_3_MASK 0xC0
#define PINSEL0_P0_3_BIT 6
#define PINSEL0_P0_4_MASK 0x300
#define PINSEL0_P0_4_BIT 8
#define PINSEL0_P0_5_MASK 0xC00
#define PINSEL0_P0_5_BIT 10
#define PINSEL0_P0_6_MASK 0x3000
#define PINSEL0_P0_6_BIT 12
#define PINSEL0_P0_7_MASK 0xC000
#define PINSEL0_P0_7_BIT 14
#define PINSEL0_P0_8_MASK 0x30000
#define PINSEL0_P0_8_BIT 16
#define PINSEL0_P0_9_MASK 0xC0000
#define PINSEL0_P0_9_BIT 18
#define PINSEL0_P0_10_MASK 0x300000
#define PINSEL0_P0_10_BIT 20
#define PINSEL0_P0_11_MASK 0xC00000
#define PINSEL0_P0_11_BIT 22
#define PINSEL0_P0_15_MASK 0xC0000000
#define PINSEL0_P0_15_BIT 30

#define PINSEL1 (*(volatile unsigned long *)0x4002C004)
#define PINSEL1_OFFSET 0x4
#define PINSEL1_P0_16_MASK 0x3
#define PINSEL1_P0_16_BIT 0
#define PINSEL1_P0_17_MASK 0xC
#define PINSEL1_P0_17_BIT 2
#define PINSEL1_P0_18_MASK 0x30
#define PINSEL1_P0_18_BIT 4
#define PINSEL1_P0_19_MASK 0xC0
#define PINSEL1_P0_19_BIT 6
#define PINSEL1_P0_20_MASK 0x300
#define PINSEL1_P0_20_BIT 8
#define PINSEL1_P0_21_MASK 0xC00
#define PINSEL1_P0_21_BIT 10
#define PINSEL1_P0_22_MASK 0x3000
#define PINSEL1_P0_22_BIT 12
#define PINSEL1_P0_23_MASK 0xC000
#define PINSEL1_P0_23_BIT 14
#define PINSEL1_P0_24_MASK 0x30000
#define PINSEL1_P0_24_BIT 16
#define PINSEL1_P0_25_MASK 0xC0000
#define PINSEL1_P0_25_BIT 18
#define PINSEL1_P0_26_MASK 0x300000
#define PINSEL1_P0_26_BIT 20
#define PINSEL1_P0_27_MASK 0xC00000
#define PINSEL1_P0_27_BIT 22
#define PINSEL1_P0_28_MASK 0x3000000
#define PINSEL1_P0_28_BIT 24
#define PINSEL1_P0_29_MASK 0xC000000
#define PINSEL1_P0_29_BIT 26
#define PINSEL1_P0_30_MASK 0x30000000
#define PINSEL1_P0_30_BIT 28

#define PINSEL2 (*(volatile unsigned long *)0x4002C008)
#define PINSEL2_OFFSET 0x8
#define PINSEL2_P1_0_MASK 0x3
#define PINSEL2_P1_0_BIT 0
#define PINSEL2_P1_1_MASK 0xC
#define PINSEL2_P1_1_BIT 2
#define PINSEL2_P1_4_MASK 0x300
#define PINSEL2_P1_4_BIT 8
#define PINSEL2_P1_8_MASK 0x30000
#define PINSEL2_P1_8_BIT 16
#define PINSEL2_P1_9_MASK 0xC0000
#define PINSEL2_P1_9_BIT 18
#define PINSEL2_P1_10_MASK 0x300000
#define PINSEL2_P1_10_BIT 20
#define PINSEL2_P1_14_MASK 0x30000000
#define PINSEL2_P1_14_BIT 28
#define PINSEL2_P1_15_MASK 0xC0000000
#define PINSEL2_P1_15_BIT 30

#define PINSEL3 (*(volatile unsigned long *)0x4002C00C)
#define PINSEL3_OFFSET 0xC
#define PINSEL3_P1_16_MASK 0x3
#define PINSEL3_P1_16_BIT 0
#define PINSEL3_P1_17_MASK 0xC
#define PINSEL3_P1_17_BIT 2
#define PINSEL3_P1_18_MASK 0x30
#define PINSEL3_P1_18_BIT 4
#define PINSEL3_P1_19_MASK 0xC0
#define PINSEL3_P1_19_BIT 6
#define PINSEL3_P1_20_MASK 0x300
#define PINSEL3_P1_20_BIT 8
#define PINSEL3_P1_21_MASK 0xC00
#define PINSEL3_P1_21_BIT 10
#define PINSEL3_P1_22_MASK 0x3000
#define PINSEL3_P1_22_BIT 12
#define PINSEL3_P1_23_MASK 0xC000
#define PINSEL3_P1_23_BIT 14
#define PINSEL3_P1_24_MASK 0x30000
#define PINSEL3_P1_24_BIT 16
#define PINSEL3_P1_25_MASK 0xC0000
#define PINSEL3_P1_25_BIT 18
#define PINSEL3_P1_26_MASK 0x300000
#define PINSEL3_P1_26_BIT 20
#define PINSEL3_P1_27_MASK 0xC00000
#define PINSEL3_P1_27_BIT 22
#define PINSEL3_P1_28_MASK 0x3000000
#define PINSEL3_P1_28_BIT 24
#define PINSEL3_P1_29_MASK 0xC000000
#define PINSEL3_P1_29_BIT 26
#define PINSEL3_P1_30_MASK 0x30000000
#define PINSEL3_P1_30_BIT 28
#define PINSEL3_P1_31_MASK 0xC0000000
#define PINSEL3_P1_31_BIT 30

#define PINSEL4 (*(volatile unsigned long *)0x4002C010)
#define PINSEL4_OFFSET 0x10
#define PINSEL4_P2_0_MASK 0x3
#define PINSEL4_P2_0_BIT 0
#define PINSEL4_P2_1_MASK 0xC
#define PINSEL4_P2_1_BIT 2
#define PINSEL4_P2_2_MASK 0x30
#define PINSEL4_P2_2_BIT 4
#define PINSEL4_P2_3_MASK 0xC0
#define PINSEL4_P2_3_BIT 6
#define PINSEL4_P2_4_MASK 0x300
#define PINSEL4_P2_4_BIT 8
#define PINSEL4_P2_5_MASK 0xC00
#define PINSEL4_P2_5_BIT 10
#define PINSEL4_P2_6_MASK 0x3000
#define PINSEL4_P2_6_BIT 12
#define PINSEL4_P2_7_MASK 0xC000
#define PINSEL4_P2_7_BIT 14
#define PINSEL4_P2_8_MASK 0x30000
#define PINSEL4_P2_8_BIT 16
#define PINSEL4_P2_9_MASK 0xC0000
#define PINSEL4_P2_9_BIT 18
#define PINSEL4_P2_10_MASK 0x300000
#define PINSEL4_P2_10_BIT 20
#define PINSEL4_P2_11_MASK 0xC00000
#define PINSEL4_P2_11_BIT 22
#define PINSEL4_P2_12_MASK 0x3000000
#define PINSEL4_P2_12_BIT 24
#define PINSEL4_P2_13_MASK 0xC000000
#define PINSEL4_P2_13_BIT 26

#define PINSEL5 (*(volatile unsigned long *)0x4002C014)
#define PINSEL5_OFFSET 0x14

#define PINSEL6 (*(volatile unsigned long *)0x4002C018)
#define PINSEL6_OFFSET 0x18

#define PINSEL7 (*(volatile unsigned long *)0x4002C01C)
#define PINSEL7_OFFSET 0x1C
#define PINSEL7_P3_25_MASK 0xC0000
#define PINSEL7_P3_25_BIT 18
#define PINSEL7_P3_26_MASK 0x300000
#define PINSEL7_P3_26_BIT 20

#define PINSEL8 (*(volatile unsigned long *)0x4002C020)
#define PINSEL8_OFFSET 0x20

#define PINSEL9 (*(volatile unsigned long *)0x4002C024)
#define PINSEL9_OFFSET 0x24
#define PINSEL9_P4_28_MASK 0x3000000
#define PINSEL9_P4_28_BIT 24
#define PINSEL9_P4_29_MASK 0xC000000
#define PINSEL9_P4_29_BIT 26

#define PINSEL10 (*(volatile unsigned long *)0x4002C028)
#define PINSEL10_OFFSET 0x28
#define PINSEL10_GPIO_TRACE_MASK 0x8
#define PINSEL10_GPIO_TRACE 0x8
#define PINSEL10_GPIO_TRACE_BIT 3

#define PINMODE0 (*(volatile unsigned long *)0x4002C040)
#define PINMODE0_OFFSET 0x40

#define PINMODE1 (*(volatile unsigned long *)0x4002C044)
#define PINMODE1_OFFSET 0x44

#define PINMODE2 (*(volatile unsigned long *)0x4002C048)
#define PINMODE2_OFFSET 0x48

#define PINMODE3 (*(volatile unsigned long *)0x4002C04C)
#define PINMODE3_OFFSET 0x4C

#define PINMODE4 (*(volatile unsigned long *)0x4002C050)
#define PINMODE4_OFFSET 0x50

#define PINMODE5 (*(volatile unsigned long *)0x4002C054)
#define PINMODE5_OFFSET 0x54

#define PINMODE6 (*(volatile unsigned long *)0x4002C058)
#define PINMODE6_OFFSET 0x58

#define PINMODE7 (*(volatile unsigned long *)0x4002C05C)
#define PINMODE7_OFFSET 0x5C

#define PINMODE8 (*(volatile unsigned long *)0x4002C060)
#define PINMODE8_OFFSET 0x60

#define PINMODE9 (*(volatile unsigned long *)0x4002C064)
#define PINMODE9_OFFSET 0x64

#define PINMODE_OD0 (*(volatile unsigned long *)0x4002C068)
#define PINMODE_OD0_OFFSET 0x68
#define PINMODE_OD0_P0_0OD_MASK 0x1
#define PINMODE_OD0_P0_0OD 0x1
#define PINMODE_OD0_P0_0OD_BIT 0
#define PINMODE_OD0_P0_1OD_MASK 0x2
#define PINMODE_OD0_P0_1OD 0x2
#define PINMODE_OD0_P0_1OD_BIT 1
#define PINMODE_OD0_P0_2OD_MASK 0x4
#define PINMODE_OD0_P0_2OD 0x4
#define PINMODE_OD0_P0_2OD_BIT 2
#define PINMODE_OD0_P0_3OD_MASK 0x8
#define PINMODE_OD0_P0_3OD 0x8
#define PINMODE_OD0_P0_3OD_BIT 3
#define PINMODE_OD0_P0_4OD_MASK 0x10
#define PINMODE_OD0_P0_4OD 0x10
#define PINMODE_OD0_P0_4OD_BIT 4
#define PINMODE_OD0_P0_5OD_MASK 0x20
#define PINMODE_OD0_P0_5OD 0x20
#define PINMODE_OD0_P0_5OD_BIT 5
#define PINMODE_OD0_P0_6OD_MASK 0x40
#define PINMODE_OD0_P0_6OD 0x40
#define PINMODE_OD0_P0_6OD_BIT 6
#define PINMODE_OD0_P0_7OD_MASK 0x80
#define PINMODE_OD0_P0_7OD 0x80
#define PINMODE_OD0_P0_7OD_BIT 7
#define PINMODE_OD0_P0_8OD_MASK 0x100
#define PINMODE_OD0_P0_8OD 0x100
#define PINMODE_OD0_P0_8OD_BIT 8
#define PINMODE_OD0_P0_9OD_MASK 0x200
#define PINMODE_OD0_P0_9OD 0x200
#define PINMODE_OD0_P0_9OD_BIT 9
#define PINMODE_OD0_P0_10OD_MASK 0x400
#define PINMODE_OD0_P0_10OD 0x400
#define PINMODE_OD0_P0_10OD_BIT 10
#define PINMODE_OD0_P0_11OD_MASK 0x800
#define PINMODE_OD0_P0_11OD 0x800
#define PINMODE_OD0_P0_11OD_BIT 11
#define PINMODE_OD0_P0_12OD_MASK 0x1000
#define PINMODE_OD0_P0_12OD 0x1000
#define PINMODE_OD0_P0_12OD_BIT 12
#define PINMODE_OD0_P0_13OD_MASK 0x2000
#define PINMODE_OD0_P0_13OD 0x2000
#define PINMODE_OD0_P0_13OD_BIT 13
#define PINMODE_OD0_P0_14OD_MASK 0x4000
#define PINMODE_OD0_P0_14OD 0x4000
#define PINMODE_OD0_P0_14OD_BIT 14
#define PINMODE_OD0_P0_15OD_MASK 0x8000
#define PINMODE_OD0_P0_15OD 0x8000
#define PINMODE_OD0_P0_15OD_BIT 15
#define PINMODE_OD0_P0_16OD_MASK 0x10000
#define PINMODE_OD0_P0_16OD 0x10000
#define PINMODE_OD0_P0_16OD_BIT 16
#define PINMODE_OD0_P0_17OD_MASK 0x20000
#define PINMODE_OD0_P0_17OD 0x20000
#define PINMODE_OD0_P0_17OD_BIT 17
#define PINMODE_OD0_P0_18OD_MASK 0x40000
#define PINMODE_OD0_P0_18OD 0x40000
#define PINMODE_OD0_P0_18OD_BIT 18
#define PINMODE_OD0_P0_19OD_MASK 0x80000
#define PINMODE_OD0_P0_19OD 0x80000
#define PINMODE_OD0_P0_19OD_BIT 19
#define PINMODE_OD0_P0_20OD_MASK 0x100000
#define PINMODE_OD0_P0_20OD 0x100000
#define PINMODE_OD0_P0_20OD_BIT 20
#define PINMODE_OD0_P0_21OD_MASK 0x200000
#define PINMODE_OD0_P0_21OD 0x200000
#define PINMODE_OD0_P0_21OD_BIT 21
#define PINMODE_OD0_P0_22OD_MASK 0x400000
#define PINMODE_OD0_P0_22OD 0x400000
#define PINMODE_OD0_P0_22OD_BIT 22
#define PINMODE_OD0_P0_23OD_MASK 0x800000
#define PINMODE_OD0_P0_23OD 0x800000
#define PINMODE_OD0_P0_23OD_BIT 23
#define PINMODE_OD0_P0_24OD_MASK 0x1000000
#define PINMODE_OD0_P0_24OD 0x1000000
#define PINMODE_OD0_P0_24OD_BIT 24
#define PINMODE_OD0_P0_25OD_MASK 0x2000000
#define PINMODE_OD0_P0_25OD 0x2000000
#define PINMODE_OD0_P0_25OD_BIT 25
#define PINMODE_OD0_P0_26OD_MASK 0x4000000
#define PINMODE_OD0_P0_26OD 0x4000000
#define PINMODE_OD0_P0_26OD_BIT 26
#define PINMODE_OD0_P0_27OD_MASK 0x8000000
#define PINMODE_OD0_P0_27OD 0x8000000
#define PINMODE_OD0_P0_27OD_BIT 27
#define PINMODE_OD0_P0_28OD_MASK 0x10000000
#define PINMODE_OD0_P0_28OD 0x10000000
#define PINMODE_OD0_P0_28OD_BIT 28
#define PINMODE_OD0_P0_29OD_MASK 0x20000000
#define PINMODE_OD0_P0_29OD 0x20000000
#define PINMODE_OD0_P0_29OD_BIT 29
#define PINMODE_OD0_P0_30OD_MASK 0x40000000
#define PINMODE_OD0_P0_30OD 0x40000000
#define PINMODE_OD0_P0_30OD_BIT 30
#define PINMODE_OD0_P0_31OD_MASK 0x80000000
#define PINMODE_OD0_P0_31OD 0x80000000
#define PINMODE_OD0_P0_31OD_BIT 31

#define PINMODE_OD1 (*(volatile unsigned long *)0x4002C06C)
#define PINMODE_OD1_OFFSET 0x6C
#define PINMODE_OD1_P1_0OD_MASK 0x1
#define PINMODE_OD1_P1_0OD 0x1
#define PINMODE_OD1_P1_0OD_BIT 0
#define PINMODE_OD1_P1_1OD_MASK 0x2
#define PINMODE_OD1_P1_1OD 0x2
#define PINMODE_OD1_P1_1OD_BIT 1
#define PINMODE_OD1_P1_2OD_MASK 0x4
#define PINMODE_OD1_P1_2OD 0x4
#define PINMODE_OD1_P1_2OD_BIT 2
#define PINMODE_OD1_P1_3OD_MASK 0x8
#define PINMODE_OD1_P1_3OD 0x8
#define PINMODE_OD1_P1_3OD_BIT 3
#define PINMODE_OD1_P1_4OD_MASK 0x10
#define PINMODE_OD1_P1_4OD 0x10
#define PINMODE_OD1_P1_4OD_BIT 4
#define PINMODE_OD1_P1_5OD_MASK 0x20
#define PINMODE_OD1_P1_5OD 0x20
#define PINMODE_OD1_P1_5OD_BIT 5
#define PINMODE_OD1_P1_6OD_MASK 0x40
#define PINMODE_OD1_P1_6OD 0x40
#define PINMODE_OD1_P1_6OD_BIT 6
#define PINMODE_OD1_P1_7OD_MASK 0x80
#define PINMODE_OD1_P1_7OD 0x80
#define PINMODE_OD1_P1_7OD_BIT 7
#define PINMODE_OD1_P1_8OD_MASK 0x100
#define PINMODE_OD1_P1_8OD 0x100
#define PINMODE_OD1_P1_8OD_BIT 8
#define PINMODE_OD1_P1_9OD_MASK 0x200
#define PINMODE_OD1_P1_9OD 0x200
#define PINMODE_OD1_P1_9OD_BIT 9
#define PINMODE_OD1_P1_10OD_MASK 0x400
#define PINMODE_OD1_P1_10OD 0x400
#define PINMODE_OD1_P1_10OD_BIT 10
#define PINMODE_OD1_P1_11OD_MASK 0x800
#define PINMODE_OD1_P1_11OD 0x800
#define PINMODE_OD1_P1_11OD_BIT 11
#define PINMODE_OD1_P1_12OD_MASK 0x1000
#define PINMODE_OD1_P1_12OD 0x1000
#define PINMODE_OD1_P1_12OD_BIT 12
#define PINMODE_OD1_P1_13OD_MASK 0x2000
#define PINMODE_OD1_P1_13OD 0x2000
#define PINMODE_OD1_P1_13OD_BIT 13
#define PINMODE_OD1_P1_14OD_MASK 0x4000
#define PINMODE_OD1_P1_14OD 0x4000
#define PINMODE_OD1_P1_14OD_BIT 14
#define PINMODE_OD1_P1_15OD_MASK 0x8000
#define PINMODE_OD1_P1_15OD 0x8000
#define PINMODE_OD1_P1_15OD_BIT 15
#define PINMODE_OD1_P1_16OD_MASK 0x10000
#define PINMODE_OD1_P1_16OD 0x10000
#define PINMODE_OD1_P1_16OD_BIT 16
#define PINMODE_OD1_P1_17OD_MASK 0x20000
#define PINMODE_OD1_P1_17OD 0x20000
#define PINMODE_OD1_P1_17OD_BIT 17
#define PINMODE_OD1_P1_18OD_MASK 0x40000
#define PINMODE_OD1_P1_18OD 0x40000
#define PINMODE_OD1_P1_18OD_BIT 18
#define PINMODE_OD1_P1_19OD_MASK 0x80000
#define PINMODE_OD1_P1_19OD 0x80000
#define PINMODE_OD1_P1_19OD_BIT 19
#define PINMODE_OD1_P1_20OD_MASK 0x100000
#define PINMODE_OD1_P1_20OD 0x100000
#define PINMODE_OD1_P1_20OD_BIT 20
#define PINMODE_OD1_P1_21OD_MASK 0x200000
#define PINMODE_OD1_P1_21OD 0x200000
#define PINMODE_OD1_P1_21OD_BIT 21
#define PINMODE_OD1_P1_22OD_MASK 0x400000
#define PINMODE_OD1_P1_22OD 0x400000
#define PINMODE_OD1_P1_22OD_BIT 22
#define PINMODE_OD1_P1_23OD_MASK 0x800000
#define PINMODE_OD1_P1_23OD 0x800000
#define PINMODE_OD1_P1_23OD_BIT 23
#define PINMODE_OD1_P1_24OD_MASK 0x1000000
#define PINMODE_OD1_P1_24OD 0x1000000
#define PINMODE_OD1_P1_24OD_BIT 24
#define PINMODE_OD1_P1_25OD_MASK 0x2000000
#define PINMODE_OD1_P1_25OD 0x2000000
#define PINMODE_OD1_P1_25OD_BIT 25
#define PINMODE_OD1_P1_26OD_MASK 0x4000000
#define PINMODE_OD1_P1_26OD 0x4000000
#define PINMODE_OD1_P1_26OD_BIT 26
#define PINMODE_OD1_P1_27OD_MASK 0x8000000
#define PINMODE_OD1_P1_27OD 0x8000000
#define PINMODE_OD1_P1_27OD_BIT 27
#define PINMODE_OD1_P1_28OD_MASK 0x10000000
#define PINMODE_OD1_P1_28OD 0x10000000
#define PINMODE_OD1_P1_28OD_BIT 28
#define PINMODE_OD1_P1_29OD_MASK 0x20000000
#define PINMODE_OD1_P1_29OD 0x20000000
#define PINMODE_OD1_P1_29OD_BIT 29
#define PINMODE_OD1_P1_30OD_MASK 0x40000000
#define PINMODE_OD1_P1_30OD 0x40000000
#define PINMODE_OD1_P1_30OD_BIT 30
#define PINMODE_OD1_P1_31OD_MASK 0x80000000
#define PINMODE_OD1_P1_31OD 0x80000000
#define PINMODE_OD1_P1_31OD_BIT 31

#define PINMODE_OD2 (*(volatile unsigned long *)0x4002C070)
#define PINMODE_OD2_OFFSET 0x70
#define PINMODE_OD2_P2_0OD_MASK 0x1
#define PINMODE_OD2_P2_0OD 0x1
#define PINMODE_OD2_P2_0OD_BIT 0
#define PINMODE_OD2_P2_1OD_MASK 0x2
#define PINMODE_OD2_P2_1OD 0x2
#define PINMODE_OD2_P2_1OD_BIT 1
#define PINMODE_OD2_P2_2OD_MASK 0x4
#define PINMODE_OD2_P2_2OD 0x4
#define PINMODE_OD2_P2_2OD_BIT 2
#define PINMODE_OD2_P2_3OD_MASK 0x8
#define PINMODE_OD2_P2_3OD 0x8
#define PINMODE_OD2_P2_3OD_BIT 3
#define PINMODE_OD2_P2_4OD_MASK 0x10
#define PINMODE_OD2_P2_4OD 0x10
#define PINMODE_OD2_P2_4OD_BIT 4
#define PINMODE_OD2_P2_5OD_MASK 0x20
#define PINMODE_OD2_P2_5OD 0x20
#define PINMODE_OD2_P2_5OD_BIT 5
#define PINMODE_OD2_P2_6OD_MASK 0x40
#define PINMODE_OD2_P2_6OD 0x40
#define PINMODE_OD2_P2_6OD_BIT 6
#define PINMODE_OD2_P2_7OD_MASK 0x80
#define PINMODE_OD2_P2_7OD 0x80
#define PINMODE_OD2_P2_7OD_BIT 7
#define PINMODE_OD2_P2_8OD_MASK 0x100
#define PINMODE_OD2_P2_8OD 0x100
#define PINMODE_OD2_P2_8OD_BIT 8
#define PINMODE_OD2_P2_9OD_MASK 0x200
#define PINMODE_OD2_P2_9OD 0x200
#define PINMODE_OD2_P2_9OD_BIT 9
#define PINMODE_OD2_P2_10OD_MASK 0x400
#define PINMODE_OD2_P2_10OD 0x400
#define PINMODE_OD2_P2_10OD_BIT 10
#define PINMODE_OD2_P2_11OD_MASK 0x800
#define PINMODE_OD2_P2_11OD 0x800
#define PINMODE_OD2_P2_11OD_BIT 11
#define PINMODE_OD2_P2_12OD_MASK 0x1000
#define PINMODE_OD2_P2_12OD 0x1000
#define PINMODE_OD2_P2_12OD_BIT 12
#define PINMODE_OD2_P2_13OD_MASK 0x2000
#define PINMODE_OD2_P2_13OD 0x2000
#define PINMODE_OD2_P2_13OD_BIT 13
#define PINMODE_OD2_P2_14OD_MASK 0x4000
#define PINMODE_OD2_P2_14OD 0x4000
#define PINMODE_OD2_P2_14OD_BIT 14
#define PINMODE_OD2_P2_15OD_MASK 0x8000
#define PINMODE_OD2_P2_15OD 0x8000
#define PINMODE_OD2_P2_15OD_BIT 15
#define PINMODE_OD2_P2_16OD_MASK 0x10000
#define PINMODE_OD2_P2_16OD 0x10000
#define PINMODE_OD2_P2_16OD_BIT 16
#define PINMODE_OD2_P2_17OD_MASK 0x20000
#define PINMODE_OD2_P2_17OD 0x20000
#define PINMODE_OD2_P2_17OD_BIT 17
#define PINMODE_OD2_P2_18OD_MASK 0x40000
#define PINMODE_OD2_P2_18OD 0x40000
#define PINMODE_OD2_P2_18OD_BIT 18
#define PINMODE_OD2_P2_19OD_MASK 0x80000
#define PINMODE_OD2_P2_19OD 0x80000
#define PINMODE_OD2_P2_19OD_BIT 19
#define PINMODE_OD2_P2_20OD_MASK 0x100000
#define PINMODE_OD2_P2_20OD 0x100000
#define PINMODE_OD2_P2_20OD_BIT 20
#define PINMODE_OD2_P2_21OD_MASK 0x200000
#define PINMODE_OD2_P2_21OD 0x200000
#define PINMODE_OD2_P2_21OD_BIT 21
#define PINMODE_OD2_P2_22OD_MASK 0x400000
#define PINMODE_OD2_P2_22OD 0x400000
#define PINMODE_OD2_P2_22OD_BIT 22
#define PINMODE_OD2_P2_23OD_MASK 0x800000
#define PINMODE_OD2_P2_23OD 0x800000
#define PINMODE_OD2_P2_23OD_BIT 23
#define PINMODE_OD2_P2_24OD_MASK 0x1000000
#define PINMODE_OD2_P2_24OD 0x1000000
#define PINMODE_OD2_P2_24OD_BIT 24
#define PINMODE_OD2_P2_25OD_MASK 0x2000000
#define PINMODE_OD2_P2_25OD 0x2000000
#define PINMODE_OD2_P2_25OD_BIT 25
#define PINMODE_OD2_P2_26OD_MASK 0x4000000
#define PINMODE_OD2_P2_26OD 0x4000000
#define PINMODE_OD2_P2_26OD_BIT 26
#define PINMODE_OD2_P2_27OD_MASK 0x8000000
#define PINMODE_OD2_P2_27OD 0x8000000
#define PINMODE_OD2_P2_27OD_BIT 27
#define PINMODE_OD2_P2_28OD_MASK 0x10000000
#define PINMODE_OD2_P2_28OD 0x10000000
#define PINMODE_OD2_P2_28OD_BIT 28
#define PINMODE_OD2_P2_29OD_MASK 0x20000000
#define PINMODE_OD2_P2_29OD 0x20000000
#define PINMODE_OD2_P2_29OD_BIT 29
#define PINMODE_OD2_P2_30OD_MASK 0x40000000
#define PINMODE_OD2_P2_30OD 0x40000000
#define PINMODE_OD2_P2_30OD_BIT 30
#define PINMODE_OD2_P2_31OD_MASK 0x80000000
#define PINMODE_OD2_P2_31OD 0x80000000
#define PINMODE_OD2_P2_31OD_BIT 31

#define PINMODE_OD3 (*(volatile unsigned long *)0x4002C074)
#define PINMODE_OD3_OFFSET 0x74
#define PINMODE_OD3_P3_0OD_MASK 0x1
#define PINMODE_OD3_P3_0OD 0x1
#define PINMODE_OD3_P3_0OD_BIT 0
#define PINMODE_OD3_P3_1OD_MASK 0x2
#define PINMODE_OD3_P3_1OD 0x2
#define PINMODE_OD3_P3_1OD_BIT 1
#define PINMODE_OD3_P3_2OD_MASK 0x4
#define PINMODE_OD3_P3_2OD 0x4
#define PINMODE_OD3_P3_2OD_BIT 2
#define PINMODE_OD3_P3_3OD_MASK 0x8
#define PINMODE_OD3_P3_3OD 0x8
#define PINMODE_OD3_P3_3OD_BIT 3
#define PINMODE_OD3_P3_4OD_MASK 0x10
#define PINMODE_OD3_P3_4OD 0x10
#define PINMODE_OD3_P3_4OD_BIT 4
#define PINMODE_OD3_P3_5OD_MASK 0x20
#define PINMODE_OD3_P3_5OD 0x20
#define PINMODE_OD3_P3_5OD_BIT 5
#define PINMODE_OD3_P3_6OD_MASK 0x40
#define PINMODE_OD3_P3_6OD 0x40
#define PINMODE_OD3_P3_6OD_BIT 6
#define PINMODE_OD3_P3_7OD_MASK 0x80
#define PINMODE_OD3_P3_7OD 0x80
#define PINMODE_OD3_P3_7OD_BIT 7
#define PINMODE_OD3_P3_8OD_MASK 0x100
#define PINMODE_OD3_P3_8OD 0x100
#define PINMODE_OD3_P3_8OD_BIT 8
#define PINMODE_OD3_P3_9OD_MASK 0x200
#define PINMODE_OD3_P3_9OD 0x200
#define PINMODE_OD3_P3_9OD_BIT 9
#define PINMODE_OD3_P3_10OD_MASK 0x400
#define PINMODE_OD3_P3_10OD 0x400
#define PINMODE_OD3_P3_10OD_BIT 10
#define PINMODE_OD3_P3_11OD_MASK 0x800
#define PINMODE_OD3_P3_11OD 0x800
#define PINMODE_OD3_P3_11OD_BIT 11
#define PINMODE_OD3_P3_12OD_MASK 0x1000
#define PINMODE_OD3_P3_12OD 0x1000
#define PINMODE_OD3_P3_12OD_BIT 12
#define PINMODE_OD3_P3_13OD_MASK 0x2000
#define PINMODE_OD3_P3_13OD 0x2000
#define PINMODE_OD3_P3_13OD_BIT 13
#define PINMODE_OD3_P3_14OD_MASK 0x4000
#define PINMODE_OD3_P3_14OD 0x4000
#define PINMODE_OD3_P3_14OD_BIT 14
#define PINMODE_OD3_P3_15OD_MASK 0x8000
#define PINMODE_OD3_P3_15OD 0x8000
#define PINMODE_OD3_P3_15OD_BIT 15
#define PINMODE_OD3_P3_16OD_MASK 0x10000
#define PINMODE_OD3_P3_16OD 0x10000
#define PINMODE_OD3_P3_16OD_BIT 16
#define PINMODE_OD3_P3_17OD_MASK 0x20000
#define PINMODE_OD3_P3_17OD 0x20000
#define PINMODE_OD3_P3_17OD_BIT 17
#define PINMODE_OD3_P3_18OD_MASK 0x40000
#define PINMODE_OD3_P3_18OD 0x40000
#define PINMODE_OD3_P3_18OD_BIT 18
#define PINMODE_OD3_P3_19OD_MASK 0x80000
#define PINMODE_OD3_P3_19OD 0x80000
#define PINMODE_OD3_P3_19OD_BIT 19
#define PINMODE_OD3_P3_20OD_MASK 0x100000
#define PINMODE_OD3_P3_20OD 0x100000
#define PINMODE_OD3_P3_20OD_BIT 20
#define PINMODE_OD3_P3_21OD_MASK 0x200000
#define PINMODE_OD3_P3_21OD 0x200000
#define PINMODE_OD3_P3_21OD_BIT 21
#define PINMODE_OD3_P3_22OD_MASK 0x400000
#define PINMODE_OD3_P3_22OD 0x400000
#define PINMODE_OD3_P3_22OD_BIT 22
#define PINMODE_OD3_P3_23OD_MASK 0x800000
#define PINMODE_OD3_P3_23OD 0x800000
#define PINMODE_OD3_P3_23OD_BIT 23
#define PINMODE_OD3_P3_24OD_MASK 0x1000000
#define PINMODE_OD3_P3_24OD 0x1000000
#define PINMODE_OD3_P3_24OD_BIT 24
#define PINMODE_OD3_P3_25OD_MASK 0x2000000
#define PINMODE_OD3_P3_25OD 0x2000000
#define PINMODE_OD3_P3_25OD_BIT 25
#define PINMODE_OD3_P3_26OD_MASK 0x4000000
#define PINMODE_OD3_P3_26OD 0x4000000
#define PINMODE_OD3_P3_26OD_BIT 26
#define PINMODE_OD3_P3_27OD_MASK 0x8000000
#define PINMODE_OD3_P3_27OD 0x8000000
#define PINMODE_OD3_P3_27OD_BIT 27
#define PINMODE_OD3_P3_28OD_MASK 0x10000000
#define PINMODE_OD3_P3_28OD 0x10000000
#define PINMODE_OD3_P3_28OD_BIT 28
#define PINMODE_OD3_P3_29OD_MASK 0x20000000
#define PINMODE_OD3_P3_29OD 0x20000000
#define PINMODE_OD3_P3_29OD_BIT 29
#define PINMODE_OD3_P3_30OD_MASK 0x40000000
#define PINMODE_OD3_P3_30OD 0x40000000
#define PINMODE_OD3_P3_30OD_BIT 30
#define PINMODE_OD3_P3_31OD_MASK 0x80000000
#define PINMODE_OD3_P3_31OD 0x80000000
#define PINMODE_OD3_P3_31OD_BIT 31

#define PINMODE_OD5 (*(volatile unsigned long *)0x4002C078)
#define PINMODE_OD5_OFFSET 0x78
#define PINMODE_OD5_P4_0OD_MASK 0x1
#define PINMODE_OD5_P4_0OD 0x1
#define PINMODE_OD5_P4_0OD_BIT 0
#define PINMODE_OD5_P4_1OD_MASK 0x2
#define PINMODE_OD5_P4_1OD 0x2
#define PINMODE_OD5_P4_1OD_BIT 1
#define PINMODE_OD5_P4_2OD_MASK 0x4
#define PINMODE_OD5_P4_2OD 0x4
#define PINMODE_OD5_P4_2OD_BIT 2
#define PINMODE_OD5_P4_3OD_MASK 0x8
#define PINMODE_OD5_P4_3OD 0x8
#define PINMODE_OD5_P4_3OD_BIT 3
#define PINMODE_OD5_P4_4OD_MASK 0x10
#define PINMODE_OD5_P4_4OD 0x10
#define PINMODE_OD5_P4_4OD_BIT 4
#define PINMODE_OD5_P4_5OD_MASK 0x20
#define PINMODE_OD5_P4_5OD 0x20
#define PINMODE_OD5_P4_5OD_BIT 5
#define PINMODE_OD5_P4_6OD_MASK 0x40
#define PINMODE_OD5_P4_6OD 0x40
#define PINMODE_OD5_P4_6OD_BIT 6
#define PINMODE_OD5_P4_7OD_MASK 0x80
#define PINMODE_OD5_P4_7OD 0x80
#define PINMODE_OD5_P4_7OD_BIT 7
#define PINMODE_OD5_P4_8OD_MASK 0x100
#define PINMODE_OD5_P4_8OD 0x100
#define PINMODE_OD5_P4_8OD_BIT 8
#define PINMODE_OD5_P4_9OD_MASK 0x200
#define PINMODE_OD5_P4_9OD 0x200
#define PINMODE_OD5_P4_9OD_BIT 9
#define PINMODE_OD5_P4_10OD_MASK 0x400
#define PINMODE_OD5_P4_10OD 0x400
#define PINMODE_OD5_P4_10OD_BIT 10
#define PINMODE_OD5_P4_11OD_MASK 0x800
#define PINMODE_OD5_P4_11OD 0x800
#define PINMODE_OD5_P4_11OD_BIT 11
#define PINMODE_OD5_P4_12OD_MASK 0x1000
#define PINMODE_OD5_P4_12OD 0x1000
#define PINMODE_OD5_P4_12OD_BIT 12
#define PINMODE_OD5_P4_13OD_MASK 0x2000
#define PINMODE_OD5_P4_13OD 0x2000
#define PINMODE_OD5_P4_13OD_BIT 13
#define PINMODE_OD5_P4_14OD_MASK 0x4000
#define PINMODE_OD5_P4_14OD 0x4000
#define PINMODE_OD5_P4_14OD_BIT 14
#define PINMODE_OD5_P4_15OD_MASK 0x8000
#define PINMODE_OD5_P4_15OD 0x8000
#define PINMODE_OD5_P4_15OD_BIT 15
#define PINMODE_OD5_P4_16OD_MASK 0x10000
#define PINMODE_OD5_P4_16OD 0x10000
#define PINMODE_OD5_P4_16OD_BIT 16
#define PINMODE_OD5_P4_17OD_MASK 0x20000
#define PINMODE_OD5_P4_17OD 0x20000
#define PINMODE_OD5_P4_17OD_BIT 17
#define PINMODE_OD5_P4_18OD_MASK 0x40000
#define PINMODE_OD5_P4_18OD 0x40000
#define PINMODE_OD5_P4_18OD_BIT 18
#define PINMODE_OD5_P4_19OD_MASK 0x80000
#define PINMODE_OD5_P4_19OD 0x80000
#define PINMODE_OD5_P4_19OD_BIT 19
#define PINMODE_OD5_P4_20OD_MASK 0x100000
#define PINMODE_OD5_P4_20OD 0x100000
#define PINMODE_OD5_P4_20OD_BIT 20
#define PINMODE_OD5_P4_21OD_MASK 0x200000
#define PINMODE_OD5_P4_21OD 0x200000
#define PINMODE_OD5_P4_21OD_BIT 21
#define PINMODE_OD5_P4_22OD_MASK 0x400000
#define PINMODE_OD5_P4_22OD 0x400000
#define PINMODE_OD5_P4_22OD_BIT 22
#define PINMODE_OD5_P4_23OD_MASK 0x800000
#define PINMODE_OD5_P4_23OD 0x800000
#define PINMODE_OD5_P4_23OD_BIT 23
#define PINMODE_OD5_P4_24OD_MASK 0x1000000
#define PINMODE_OD5_P4_24OD 0x1000000
#define PINMODE_OD5_P4_24OD_BIT 24
#define PINMODE_OD5_P4_25OD_MASK 0x2000000
#define PINMODE_OD5_P4_25OD 0x2000000
#define PINMODE_OD5_P4_25OD_BIT 25
#define PINMODE_OD5_P4_26OD_MASK 0x4000000
#define PINMODE_OD5_P4_26OD 0x4000000
#define PINMODE_OD5_P4_26OD_BIT 26
#define PINMODE_OD5_P4_27OD_MASK 0x8000000
#define PINMODE_OD5_P4_27OD 0x8000000
#define PINMODE_OD5_P4_27OD_BIT 27
#define PINMODE_OD5_P4_28OD_MASK 0x10000000
#define PINMODE_OD5_P4_28OD 0x10000000
#define PINMODE_OD5_P4_28OD_BIT 28
#define PINMODE_OD5_P4_29OD_MASK 0x20000000
#define PINMODE_OD5_P4_29OD 0x20000000
#define PINMODE_OD5_P4_29OD_BIT 29
#define PINMODE_OD5_P4_30OD_MASK 0x40000000
#define PINMODE_OD5_P4_30OD 0x40000000
#define PINMODE_OD5_P4_30OD_BIT 30
#define PINMODE_OD5_P4_31OD_MASK 0x80000000
#define PINMODE_OD5_P4_31OD 0x80000000
#define PINMODE_OD5_P4_31OD_BIT 31

#define I2CPADCFG (*(volatile unsigned long *)0x4002C07C)
#define I2CPADCFG_OFFSET 0x7C
#define I2CPADCFG_SDADRV0_MASK 0x1
#define I2CPADCFG_SDADRV0 0x1
#define I2CPADCFG_SDADRV0_BIT 0
#define I2CPADCFG_SDAI2C0_MASK 0x2
#define I2CPADCFG_SDAI2C0 0x2
#define I2CPADCFG_SDAI2C0_BIT 1
#define I2CPADCFG_SCLDRV0_MASK 0x4
#define I2CPADCFG_SCLDRV0 0x4
#define I2CPADCFG_SCLDRV0_BIT 2
#define I2CPADCFG_SCLI2C0_MASK 0x8
#define I2CPADCFG_SCLI2C0 0x8
#define I2CPADCFG_SCLI2C0_BIT 3

#define SSP1_BASE_ADDRESS 0x40030000

#define SSP1CR0 (*(volatile unsigned long *)0x40030000)
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

#define SSP1CR1 (*(volatile unsigned long *)0x40030004)
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
#define SSP1CR1_LBM_MASK 0x1
#define SSP1CR1_LBM 0x1
#define SSP1CR1_LBM_BIT 0

#define SSP1DR (*(volatile unsigned long *)0x40030008)
#define SSP1DR_OFFSET 0x8

#define SSP1SR (*(volatile unsigned long *)0x4003000C)
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

#define SSP1CPSR (*(volatile unsigned long *)0x40030010)
#define SSP1CPSR_OFFSET 0x10
#define SSP1CPSR_CPSDVSR_MASK 0xFF
#define SSP1CPSR_CPSDVSR_BIT 0

#define SSP1IMSC (*(volatile unsigned long *)0x40030014)
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

#define SSP1RIS (*(volatile unsigned long *)0x40030018)
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

#define SSP1MIS (*(volatile unsigned long *)0x4003001C)
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

#define SSP1ICR (*(volatile unsigned long *)0x40030020)
#define SSP1ICR_OFFSET 0x20
#define SSP1ICR_RTIC_MASK 0x2
#define SSP1ICR_RTIC 0x2
#define SSP1ICR_RTIC_BIT 1
#define SSP1ICR_RORIC_MASK 0x1
#define SSP1ICR_RORIC 0x1
#define SSP1ICR_RORIC_BIT 0

#define SSP1DMACR (*(volatile unsigned long *)0x40030024)
#define SSP1DMACR_OFFSET 0x24
#define SSP1DMACR_RXDMAE_MASK 0x1
#define SSP1DMACR_RXDMAE 0x1
#define SSP1DMACR_RXDMAE_BIT 0
#define SSP1DMACR_TXDMAE_MASK 0x2
#define SSP1DMACR_TXDMAE 0x2
#define SSP1DMACR_TXDMAE_BIT 1

#define AD_BASE_ADDRESS 0x40034000

#define ADCR (*(volatile unsigned *)0x40034000)
#define ADCR_OFFSET 0x0
#define ADCR_SEL_MASK 0xFF
#define ADCR_SEL_BIT 0
#define ADCR_CLKDIV_MASK 0xFF00
#define ADCR_CLKDIV_BIT 8
#define ADCR_BURST_MASK 0x10000
#define ADCR_BURST 0x10000
#define ADCR_BURST_BIT 16
#define ADCR_PDN_MASK 0x200000
#define ADCR_PDN 0x200000
#define ADCR_PDN_BIT 21
#define ADCR_START_MASK 0x7000000
#define ADCR_START_BIT 24
#define ADCR_EDGE_MASK 0x8000000
#define ADCR_EDGE 0x8000000
#define ADCR_EDGE_BIT 27

#define ADGDR (*(volatile unsigned *)0x40034004)
#define ADGDR_OFFSET 0x4
#define ADGDR_RESULT_MASK 0xFFF0
#define ADGDR_RESULT_BIT 4
#define ADGDR_CHN_MASK 0x7000000
#define ADGDR_CHN_BIT 24
#define ADGDR_OVERUN_MASK 0x40000000
#define ADGDR_OVERUN 0x40000000
#define ADGDR_OVERUN_BIT 30
#define ADGDR_DONE_MASK 0x80000000
#define ADGDR_DONE 0x80000000
#define ADGDR_DONE_BIT 31

#define ADINTEN (*(volatile unsigned *)0x4003400C)
#define ADINTEN_OFFSET 0xC
#define ADINTEN_ADINTEN0_MASK 0x1
#define ADINTEN_ADINTEN0 0x1
#define ADINTEN_ADINTEN0_BIT 0
#define ADINTEN_ADINTEN1_MASK 0x2
#define ADINTEN_ADINTEN1 0x2
#define ADINTEN_ADINTEN1_BIT 1
#define ADINTEN_ADINTEN2_MASK 0x4
#define ADINTEN_ADINTEN2 0x4
#define ADINTEN_ADINTEN2_BIT 2
#define ADINTEN_ADINTEN3_MASK 0x8
#define ADINTEN_ADINTEN3 0x8
#define ADINTEN_ADINTEN3_BIT 3
#define ADINTEN_ADINTEN4_MASK 0x10
#define ADINTEN_ADINTEN4 0x10
#define ADINTEN_ADINTEN4_BIT 4
#define ADINTEN_ADINTEN5_MASK 0x20
#define ADINTEN_ADINTEN5 0x20
#define ADINTEN_ADINTEN5_BIT 5
#define ADINTEN_ADINTEN6_MASK 0x40
#define ADINTEN_ADINTEN6 0x40
#define ADINTEN_ADINTEN6_BIT 6
#define ADINTEN_ADINTEN7_MASK 0x80
#define ADINTEN_ADINTEN7 0x80
#define ADINTEN_ADINTEN7_BIT 7
#define ADINTEN_ADGINTEN_MASK 0x100
#define ADINTEN_ADGINTEN 0x100
#define ADINTEN_ADGINTEN_BIT 8

#define ADDR0 (*(volatile unsigned *)0x40034010)
#define ADDR0_OFFSET 0x10
#define ADDR0_RESULT_MASK 0xFFF0
#define ADDR0_RESULT_BIT 4
#define ADDR0_OVERRUN_MASK 0x40000000
#define ADDR0_OVERRUN 0x40000000
#define ADDR0_OVERRUN_BIT 30
#define ADDR0_DONE_MASK 0x80000000
#define ADDR0_DONE 0x80000000
#define ADDR0_DONE_BIT 31

#define ADDR1 (*(volatile unsigned *)0x40034014)
#define ADDR1_OFFSET 0x14
#define ADDR1_RESULT_MASK 0xFFF0
#define ADDR1_RESULT_BIT 4
#define ADDR1_OVERRUN_MASK 0x40000000
#define ADDR1_OVERRUN 0x40000000
#define ADDR1_OVERRUN_BIT 30
#define ADDR1_DONE_MASK 0x80000000
#define ADDR1_DONE 0x80000000
#define ADDR1_DONE_BIT 31

#define ADDR2 (*(volatile unsigned *)0x40034018)
#define ADDR2_OFFSET 0x18
#define ADDR2_RESULT_MASK 0xFFF0
#define ADDR2_RESULT_BIT 4
#define ADDR2_OVERRUN_MASK 0x40000000
#define ADDR2_OVERRUN 0x40000000
#define ADDR2_OVERRUN_BIT 30
#define ADDR2_DONE_MASK 0x80000000
#define ADDR2_DONE 0x80000000
#define ADDR2_DONE_BIT 31

#define ADDR3 (*(volatile unsigned *)0x4003401C)
#define ADDR3_OFFSET 0x1C
#define ADDR3_RESULT_MASK 0xFFF0
#define ADDR3_RESULT_BIT 4
#define ADDR3_OVERRUN_MASK 0x40000000
#define ADDR3_OVERRUN 0x40000000
#define ADDR3_OVERRUN_BIT 30
#define ADDR3_DONE_MASK 0x80000000
#define ADDR3_DONE 0x80000000
#define ADDR3_DONE_BIT 31

#define ADDR4 (*(volatile unsigned *)0x40034020)
#define ADDR4_OFFSET 0x20
#define ADDR4_RESULT_MASK 0xFFF0
#define ADDR4_RESULT_BIT 4
#define ADDR4_OVERRUN_MASK 0x40000000
#define ADDR4_OVERRUN 0x40000000
#define ADDR4_OVERRUN_BIT 30
#define ADDR4_DONE_MASK 0x80000000
#define ADDR4_DONE 0x80000000
#define ADDR4_DONE_BIT 31

#define ADDR5 (*(volatile unsigned *)0x40034024)
#define ADDR5_OFFSET 0x24
#define ADDR5_RESULT_MASK 0xFFF0
#define ADDR5_RESULT_BIT 4
#define ADDR5_OVERRUN_MASK 0x40000000
#define ADDR5_OVERRUN 0x40000000
#define ADDR5_OVERRUN_BIT 30
#define ADDR5_DONE_MASK 0x80000000
#define ADDR5_DONE 0x80000000
#define ADDR5_DONE_BIT 31

#define ADDR6 (*(volatile unsigned *)0x40034028)
#define ADDR6_OFFSET 0x28
#define ADDR6_RESULT_MASK 0xFFF0
#define ADDR6_RESULT_BIT 4
#define ADDR6_OVERRUN_MASK 0x40000000
#define ADDR6_OVERRUN 0x40000000
#define ADDR6_OVERRUN_BIT 30
#define ADDR6_DONE_MASK 0x80000000
#define ADDR6_DONE 0x80000000
#define ADDR6_DONE_BIT 31

#define ADDR7 (*(volatile unsigned *)0x4003402C)
#define ADDR7_OFFSET 0x2C
#define ADDR7_RESULT_MASK 0xFFF0
#define ADDR7_RESULT_BIT 4
#define ADDR7_OVERRUN_MASK 0x40000000
#define ADDR7_OVERRUN 0x40000000
#define ADDR7_OVERRUN_BIT 30
#define ADDR7_DONE_MASK 0x80000000
#define ADDR7_DONE 0x80000000
#define ADDR7_DONE_BIT 31

#define ADSTAT (*(volatile unsigned *)0x40034030)
#define ADSTAT_OFFSET 0x30
#define ADSTAT_DONE0_MASK 0x1
#define ADSTAT_DONE0 0x1
#define ADSTAT_DONE0_BIT 0
#define ADSTAT_DONE1_MASK 0x2
#define ADSTAT_DONE1 0x2
#define ADSTAT_DONE1_BIT 1
#define ADSTAT_DONE2_MASK 0x4
#define ADSTAT_DONE2 0x4
#define ADSTAT_DONE2_BIT 2
#define ADSTAT_DONE3_MASK 0x8
#define ADSTAT_DONE3 0x8
#define ADSTAT_DONE3_BIT 3
#define ADSTAT_DONE4_MASK 0x10
#define ADSTAT_DONE4 0x10
#define ADSTAT_DONE4_BIT 4
#define ADSTAT_DONE5_MASK 0x20
#define ADSTAT_DONE5 0x20
#define ADSTAT_DONE5_BIT 5
#define ADSTAT_DONE6_MASK 0x40
#define ADSTAT_DONE6 0x40
#define ADSTAT_DONE6_BIT 6
#define ADSTAT_DONE7_MASK 0x80
#define ADSTAT_DONE7 0x80
#define ADSTAT_DONE7_BIT 7
#define ADSTAT_OVERRUN0_MASK 0x100
#define ADSTAT_OVERRUN0 0x100
#define ADSTAT_OVERRUN0_BIT 8
#define ADSTAT_OVERRUN1_MASK 0x200
#define ADSTAT_OVERRUN1 0x200
#define ADSTAT_OVERRUN1_BIT 9
#define ADSTAT_OVERRUN2_MASK 0x400
#define ADSTAT_OVERRUN2 0x400
#define ADSTAT_OVERRUN2_BIT 10
#define ADSTAT_OVERRUN3_MASK 0x800
#define ADSTAT_OVERRUN3 0x800
#define ADSTAT_OVERRUN3_BIT 11
#define ADSTAT_OVERRUN4_MASK 0x1000
#define ADSTAT_OVERRUN4 0x1000
#define ADSTAT_OVERRUN4_BIT 12
#define ADSTAT_OVERRUN5_MASK 0x2000
#define ADSTAT_OVERRUN5 0x2000
#define ADSTAT_OVERRUN5_BIT 13
#define ADSTAT_OVERRUN6_MASK 0x4000
#define ADSTAT_OVERRUN6 0x4000
#define ADSTAT_OVERRUN6_BIT 14
#define ADSTAT_OVERRUN7_MASK 0x8000
#define ADSTAT_OVERRUN7 0x8000
#define ADSTAT_OVERRUN7_BIT 15
#define ADSTAT_ADINT_MASK 0x10000
#define ADSTAT_ADINT 0x10000
#define ADSTAT_ADINT_BIT 16

#define ADTRIM (*(volatile unsigned *)0x40034034)
#define ADTRIM_OFFSET 0x34
#define ADTRIM_ADCOFFS_MASK 0xF0
#define ADTRIM_ADCOFFS_BIT 4
#define ADTRIM_TRIM_MASK 0xF00
#define ADTRIM_TRIM_BIT 8

#define CAN_BASE_ADDRESS 0x40038000

#define AFMR (*(volatile unsigned long *)0x4003C000)
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

#define SFF_sa (*(volatile unsigned long *)0x4003C004)
#define SFF_sa_OFFSET 0x4004

#define SFF_GRP_sa (*(volatile unsigned long *)0x4003C008)
#define SFF_GRP_sa_OFFSET 0x4008

#define EFF_sa (*(volatile unsigned long *)0x4003C00C)
#define EFF_sa_OFFSET 0x400C

#define EFF_GRP_sa (*(volatile unsigned long *)0x4003C010)
#define EFF_GRP_sa_OFFSET 0x4010

#define ENDofTable (*(volatile unsigned long *)0x4003C014)
#define ENDofTable_OFFSET 0x4014

#define LUTerrAd (*(volatile unsigned long *)0x4003C018)
#define LUTerrAd_OFFSET 0x4018

#define LUTerr (*(volatile unsigned long *)0x4003C01C)
#define LUTerr_OFFSET 0x401C

#define FCANIE (*(volatile unsigned long *)0x4003C020)
#define FCANIE_OFFSET 0x4020
#define FCANIE_FCANIE_MASK 0x1
#define FCANIE_FCANIE 0x1
#define FCANIE_FCANIE_BIT 0

#define FCANIC0 (*(volatile unsigned long *)0x4003C024)
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

#define FCANIC1 (*(volatile unsigned long *)0x4003C028)
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

#define CANTxSR (*(volatile unsigned long *)0x40040000)
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

#define CANRxSR (*(volatile unsigned long *)0x40040004)
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

#define CANMSR (*(volatile unsigned long *)0x40040008)
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

#define CAN1_BASE_ADDRESS 0x40044000

#define C1MOD (*(volatile unsigned long *)0x40044000)
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

#define C1CMR (*(volatile unsigned long *)0x40044004)
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

#define C1GSR (*(volatile unsigned long *)0x40044008)
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

#define C1ICR (*(volatile unsigned long *)0x4004400C)
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

#define C1IER (*(volatile unsigned long *)0x40044010)
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

#define C1BTR (*(volatile unsigned long *)0x40044014)
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

#define C1EWL (*(volatile unsigned long *)0x40044018)
#define CAN1EWL C1EWL
#define C1EWL_OFFSET 0x18
#define CAN1EWL_OFFSET C1EWL_OFFSET
#define C1EWL_EWL_MASK 0xFF
#define CAN1EWL_EWL_MASK C1EWL_EWL_MASK
#define C1EWL_EWL_BIT 0
#define CAN1EWL_EWL_BIT C1EWL_EWL_BIT

#define C1SR (*(volatile unsigned long *)0x4004401C)
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

#define C1RFS (*(volatile unsigned long *)0x40044020)
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

#define C1RID (*(volatile unsigned long *)0x40044024)
#define CAN1RID C1RID
#define C1RID_OFFSET 0x24
#define CAN1RID_OFFSET C1RID_OFFSET
#define C1RID_ID_MASK 0x7FF
#define CAN1RID_ID_MASK C1RID_ID_MASK
#define C1RID_ID_BIT 0
#define CAN1RID_ID_BIT C1RID_ID_BIT

#define C1RDA (*(volatile unsigned long *)0x40044028)
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

#define C1RDB (*(volatile unsigned long *)0x4004402C)
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

#define C1TFI1 (*(volatile unsigned long *)0x40044030)
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

#define C1TID1 (*(volatile unsigned long *)0x40044034)
#define CAN1TID1 C1TID1
#define C1TID1_OFFSET 0x34
#define CAN1TID1_OFFSET C1TID1_OFFSET
#define C1TID1_ID_MASK 0x7FF
#define CAN1TID1_ID_MASK C1TID1_ID_MASK
#define C1TID1_ID_BIT 0
#define CAN1TID1_ID_BIT C1TID1_ID_BIT

#define C1TDA1 (*(volatile unsigned long *)0x40044038)
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

#define C1TDB1 (*(volatile unsigned long *)0x4004403C)
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

#define C1TFI2 (*(volatile unsigned long *)0x40044040)
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

#define C1TID2 (*(volatile unsigned long *)0x40044044)
#define CAN1TID2 C1TID2
#define C1TID2_OFFSET 0x44
#define CAN1TID2_OFFSET C1TID2_OFFSET
#define C1TID2_ID_MASK 0x7FF
#define CAN1TID2_ID_MASK C1TID2_ID_MASK
#define C1TID2_ID_BIT 0
#define CAN1TID2_ID_BIT C1TID2_ID_BIT

#define C1TDA2 (*(volatile unsigned long *)0x40044048)
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

#define C1TDB2 (*(volatile unsigned long *)0x4004404C)
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

#define C1TFI3 (*(volatile unsigned long *)0x40044050)
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

#define C1TID3 (*(volatile unsigned long *)0x40044054)
#define CAN1TID3 C1TID3
#define C1TID3_OFFSET 0x54
#define CAN1TID3_OFFSET C1TID3_OFFSET
#define C1TID3_ID_MASK 0x7FF
#define CAN1TID3_ID_MASK C1TID3_ID_MASK
#define C1TID3_ID_BIT 0
#define CAN1TID3_ID_BIT C1TID3_ID_BIT

#define C1TDA3 (*(volatile unsigned long *)0x40044058)
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

#define C1TDB3 (*(volatile unsigned long *)0x4004405C)
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

#define CAN2_BASE_ADDRESS 0x40048000

#define C2MOD (*(volatile unsigned long *)0x40048000)
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

#define C2CMR (*(volatile unsigned long *)0x40048004)
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

#define C2GSR (*(volatile unsigned long *)0x40048008)
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

#define C2ICR (*(volatile unsigned long *)0x4004800C)
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

#define C2IER (*(volatile unsigned long *)0x40048010)
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

#define C2BTR (*(volatile unsigned long *)0x40048014)
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

#define C2EWL (*(volatile unsigned long *)0x40048018)
#define CAN2EWL C2EWL
#define C2EWL_OFFSET 0x18
#define CAN2EWL_OFFSET C2EWL_OFFSET
#define C2EWL_EWL_MASK 0xFF
#define CAN2EWL_EWL_MASK C2EWL_EWL_MASK
#define C2EWL_EWL_BIT 0
#define CAN2EWL_EWL_BIT C2EWL_EWL_BIT

#define C2SR (*(volatile unsigned long *)0x4004801C)
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

#define C2RFS (*(volatile unsigned long *)0x40048020)
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

#define C2RID (*(volatile unsigned long *)0x40048024)
#define CAN2RID C2RID
#define C2RID_OFFSET 0x24
#define CAN2RID_OFFSET C2RID_OFFSET
#define C2RID_ID_MASK 0x7FF
#define CAN2RID_ID_MASK C2RID_ID_MASK
#define C2RID_ID_BIT 0
#define CAN2RID_ID_BIT C2RID_ID_BIT

#define C2RDA (*(volatile unsigned long *)0x40048028)
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

#define C2RDB (*(volatile unsigned long *)0x4004802C)
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

#define C2TFI1 (*(volatile unsigned long *)0x40048030)
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

#define C2TID1 (*(volatile unsigned long *)0x40048034)
#define CAN2TID1 C2TID1
#define C2TID1_OFFSET 0x34
#define CAN2TID1_OFFSET C2TID1_OFFSET
#define C2TID1_ID_MASK 0x7FF
#define CAN2TID1_ID_MASK C2TID1_ID_MASK
#define C2TID1_ID_BIT 0
#define CAN2TID1_ID_BIT C2TID1_ID_BIT

#define C2TDA1 (*(volatile unsigned long *)0x40048038)
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

#define C2TDB1 (*(volatile unsigned long *)0x4004803C)
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

#define C2TFI2 (*(volatile unsigned long *)0x40048040)
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

#define C2TID2 (*(volatile unsigned long *)0x40048044)
#define CAN2TID2 C2TID2
#define C2TID2_OFFSET 0x44
#define CAN2TID2_OFFSET C2TID2_OFFSET
#define C2TID2_ID_MASK 0x7FF
#define CAN2TID2_ID_MASK C2TID2_ID_MASK
#define C2TID2_ID_BIT 0
#define CAN2TID2_ID_BIT C2TID2_ID_BIT

#define C2TDA2 (*(volatile unsigned long *)0x40048048)
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

#define C2TDB2 (*(volatile unsigned long *)0x4004804C)
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

#define C2TFI3 (*(volatile unsigned long *)0x40048050)
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

#define C2TID3 (*(volatile unsigned long *)0x40048054)
#define CAN2TID3 C2TID3
#define C2TID3_OFFSET 0x54
#define CAN2TID3_OFFSET C2TID3_OFFSET
#define C2TID3_ID_MASK 0x7FF
#define CAN2TID3_ID_MASK C2TID3_ID_MASK
#define C2TID3_ID_BIT 0
#define CAN2TID3_ID_BIT C2TID3_ID_BIT

#define C2TDA3 (*(volatile unsigned long *)0x40048058)
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

#define C2TDB3 (*(volatile unsigned long *)0x4004805C)
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

#define I2C1_BASE_ADDRESS 0x4005C000

#define I2C1CONSET (*(volatile unsigned char *)0x4005C000)
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

#define I2C1STAT (*(volatile unsigned char *)0x4005C004)
#define I2C1STAT_OFFSET 0x4
#define I2C1STAT_Status_MASK 0xF8
#define I2C1STAT_Status_BIT 3

#define I2C1DAT (*(volatile unsigned char *)0x4005C008)
#define I2C1DAT_OFFSET 0x8

#define I2C1ADR0 (*(volatile unsigned char *)0x4005C00C)
#define I2C1ADR0_OFFSET 0xC
#define I2C1ADR0_GC_MASK 0x1
#define I2C1ADR0_GC 0x1
#define I2C1ADR0_GC_BIT 0
#define I2C1ADR0_Address_MASK 0xFE
#define I2C1ADR0_Address_BIT 1

#define I2C1SCLH (*(volatile unsigned short *)0x4005C010)
#define I2C1SCLH_OFFSET 0x10

#define I2C1SCLL (*(volatile unsigned short *)0x4005C014)
#define I2C1SCLL_OFFSET 0x14

#define I2C1CONCLR (*(volatile unsigned char *)0x4005C018)
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

#define I2C1MMCTRL (*(volatile unsigned char *)0x4005C01C)
#define I2C1MMCTRL_OFFSET 0x1C
#define I2C1MMCTRL_MM_ENA_MASK 0x1
#define I2C1MMCTRL_MM_ENA 0x1
#define I2C1MMCTRL_MM_ENA_BIT 0
#define I2C1MMCTRL_ENA_SCL_MASK 0x2
#define I2C1MMCTRL_ENA_SCL 0x2
#define I2C1MMCTRL_ENA_SCL_BIT 1
#define I2C1MMCTRL_MATCH_ALL_MASK 0x8
#define I2C1MMCTRL_MATCH_ALL 0x8
#define I2C1MMCTRL_MATCH_ALL_BIT 3

#define I2C1ADR1 (*(volatile unsigned char *)0x4005C020)
#define I2C1ADR1_OFFSET 0x20
#define I2C1ADR1_GC_MASK 0x1
#define I2C1ADR1_GC 0x1
#define I2C1ADR1_GC_BIT 0
#define I2C1ADR1_Address_MASK 0xFE
#define I2C1ADR1_Address_BIT 1

#define I2C1ADR2 (*(volatile unsigned char *)0x4005C024)
#define I2C1ADR2_OFFSET 0x24
#define I2C1ADR2_GC_MASK 0x1
#define I2C1ADR2_GC 0x1
#define I2C1ADR2_GC_BIT 0
#define I2C1ADR2_Address_MASK 0xFE
#define I2C1ADR2_Address_BIT 1

#define I2C1ADR3 (*(volatile unsigned char *)0x4005C028)
#define I2C1ADR3_OFFSET 0x28
#define I2C1ADR3_GC_MASK 0x1
#define I2C1ADR3_GC 0x1
#define I2C1ADR3_GC_BIT 0
#define I2C1ADR3_Address_MASK 0xFE
#define I2C1ADR3_Address_BIT 1

#define I2C1DATA_BUFFER (*(volatile unsigned char *)0x4005C02C)
#define I2C1DATA_BUFFER_OFFSET 0x2C

#define I2C1MASK0 (*(volatile unsigned long *)0x4005C030)
#define I2C1MASK0_OFFSET 0x30
#define I2C1MASK0_MASK_MASK 0xFE
#define I2C1MASK0_MASK_BIT 1

#define I2C1MASK1 (*(volatile unsigned long *)0x4005C034)
#define I2C1MASK1_OFFSET 0x34
#define I2C1MASK1_MASK_MASK 0xFE
#define I2C1MASK1_MASK_BIT 1

#define I2C1MASK2 (*(volatile unsigned long *)0x4005C038)
#define I2C1MASK2_OFFSET 0x38
#define I2C1MASK2_MASK_MASK 0xFE
#define I2C1MASK2_MASK_BIT 1

#define I2C1MASK3 (*(volatile unsigned long *)0x4005C03C)
#define I2C1MASK3_OFFSET 0x3C
#define I2C1MASK3_MASK_MASK 0xFE
#define I2C1MASK3_MASK_BIT 1

#define SSP0_BASE_ADDRESS 0x40088000

#define SSP0CR0 (*(volatile unsigned long *)0x40088000)
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

#define SSP0CR1 (*(volatile unsigned long *)0x40088004)
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
#define SSP0CR1_LBM_MASK 0x1
#define SSP0CR1_LBM 0x1
#define SSP0CR1_LBM_BIT 0

#define SSP0DR (*(volatile unsigned long *)0x40088008)
#define SSP0DR_OFFSET 0x8

#define SSP0SR (*(volatile unsigned long *)0x4008800C)
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

#define SSP0CPSR (*(volatile unsigned long *)0x40088010)
#define SSP0CPSR_OFFSET 0x10
#define SSP0CPSR_CPSDVSR_MASK 0xFF
#define SSP0CPSR_CPSDVSR_BIT 0

#define SSP0IMSC (*(volatile unsigned long *)0x40088014)
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

#define SSP0RIS (*(volatile unsigned long *)0x40088018)
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

#define SSP0MIS (*(volatile unsigned long *)0x4008801C)
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

#define SSP0ICR (*(volatile unsigned long *)0x40088020)
#define SSP0ICR_OFFSET 0x20
#define SSP0ICR_RTIC_MASK 0x2
#define SSP0ICR_RTIC 0x2
#define SSP0ICR_RTIC_BIT 1
#define SSP0ICR_RORIC_MASK 0x1
#define SSP0ICR_RORIC 0x1
#define SSP0ICR_RORIC_BIT 0

#define SSP0DMACR (*(volatile unsigned long *)0x40088024)
#define SSP0DMACR_OFFSET 0x24
#define SSP0DMACR_RXDMAE_MASK 0x1
#define SSP0DMACR_RXDMAE 0x1
#define SSP0DMACR_RXDMAE_BIT 0
#define SSP0DMACR_TXDMAE_MASK 0x2
#define SSP0DMACR_TXDMAE 0x2
#define SSP0DMACR_TXDMAE_BIT 1

#define TIMER2_BASE_ADDRESS 0x40090000

#define T2IR (*(volatile unsigned char *)0x40090000)
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

#define T2TCR (*(volatile unsigned char *)0x40090004)
#define T2TCR_OFFSET 0x4
#define T2TCR_Counter_Enable_MASK 0x1
#define T2TCR_Counter_Enable 0x1
#define T2TCR_Counter_Enable_BIT 0
#define T2TCR_Counter_Reset_MASK 0x2
#define T2TCR_Counter_Reset 0x2
#define T2TCR_Counter_Reset_BIT 1

#define T2TC (*(volatile unsigned long *)0x40090008)
#define T2TC_OFFSET 0x8

#define T2PR (*(volatile unsigned long *)0x4009000C)
#define T2PR_OFFSET 0xC

#define T2PC (*(volatile unsigned long *)0x40090010)
#define T2PC_OFFSET 0x10

#define T2MCR (*(volatile unsigned short *)0x40090014)
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

#define T2MR0 (*(volatile unsigned long *)0x40090018)
#define T2MR0_OFFSET 0x18

#define T2MR1 (*(volatile unsigned long *)0x4009001C)
#define T2MR1_OFFSET 0x1C

#define T2MR2 (*(volatile unsigned long *)0x40090020)
#define T2MR2_OFFSET 0x20

#define T2MR3 (*(volatile unsigned long *)0x40090024)
#define T2MR3_OFFSET 0x24

#define T2CCR (*(volatile unsigned short *)0x40090028)
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

#define T2CR0 (*(volatile unsigned long *)0x4009002C)
#define T2CR0_OFFSET 0x2C

#define T2CR1 (*(volatile unsigned long *)0x40090030)
#define T2CR1_OFFSET 0x30

#define T2EMR (*(volatile unsigned short *)0x4009003C)
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

#define TIMER3_BASE_ADDRESS 0x40094000

#define T3IR (*(volatile unsigned char *)0x40094000)
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

#define T3TCR (*(volatile unsigned char *)0x40094004)
#define T3TCR_OFFSET 0x4
#define T3TCR_Counter_Enable_MASK 0x1
#define T3TCR_Counter_Enable 0x1
#define T3TCR_Counter_Enable_BIT 0
#define T3TCR_Counter_Reset_MASK 0x2
#define T3TCR_Counter_Reset 0x2
#define T3TCR_Counter_Reset_BIT 1

#define T3TC (*(volatile unsigned long *)0x40094008)
#define T3TC_OFFSET 0x8

#define T3PR (*(volatile unsigned long *)0x4009400C)
#define T3PR_OFFSET 0xC

#define T3PC (*(volatile unsigned long *)0x40094010)
#define T3PC_OFFSET 0x10

#define T3MCR (*(volatile unsigned short *)0x40094014)
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

#define T3MR0 (*(volatile unsigned long *)0x40094018)
#define T3MR0_OFFSET 0x18

#define T3MR1 (*(volatile unsigned long *)0x4009401C)
#define T3MR1_OFFSET 0x1C

#define T3MR2 (*(volatile unsigned long *)0x40094020)
#define T3MR2_OFFSET 0x20

#define T3MR3 (*(volatile unsigned long *)0x40094024)
#define T3MR3_OFFSET 0x24

#define T3CCR (*(volatile unsigned short *)0x40094028)
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

#define T3CR0 (*(volatile unsigned long *)0x4009402C)
#define T3CR0_OFFSET 0x2C

#define T3CR1 (*(volatile unsigned long *)0x40094030)
#define T3CR1_OFFSET 0x30

#define T3EMR (*(volatile unsigned short *)0x4009403C)
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

#define UART2_BASE_ADDRESS 0x40098000

#define U2RBR (*(volatile unsigned char *)0x40098000)
#define U2RBR_OFFSET 0x0

#define U2THR (*(volatile unsigned char *)0x40098000)
#define U2THR_OFFSET 0x0

#define U2DLL (*(volatile unsigned char *)0x40098000)
#define U2DLL_OFFSET 0x0

#define U2DLM (*(volatile unsigned char *)0x40098004)
#define U2DLM_OFFSET 0x4

#define U2IER (*(volatile unsigned long *)0x40098004)
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

#define U2IIR (*(volatile unsigned long *)0x40098008)
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

#define U2FCR (*(volatile unsigned char *)0x40098008)
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
#define U2FCR_DMA_Mode_Select_MASK 0x8
#define U2FCR_DMA_Mode_Select 0x8
#define U2FCR_DMA_Mode_Select_BIT 3
#define U2FCR_Rx_Trigger_Level_Select_MASK 0xC0
#define U2FCR_Rx_Trigger_Level_Select_BIT 6

#define U2LCR (*(volatile unsigned char *)0x4009800C)
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

#define U2LSR (*(volatile unsigned char *)0x40098014)
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

#define U2SCR (*(volatile unsigned char *)0x4009801C)
#define U2SCR_OFFSET 0x1C

#define U2ACR (*(volatile unsigned long *)0x40098020)
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

#define U2FDR (*(volatile unsigned long *)0x40098028)
#define U2FDR_OFFSET 0x28
#define U2FDR_DIVADDVAL_MASK 0xF
#define U2FDR_DIVADDVAL_BIT 0
#define U2FDR_MULVAL_MASK 0xF0
#define U2FDR_MULVAL_BIT 4

#define U2TER (*(volatile unsigned char *)0x40098030)
#define U2TER_OFFSET 0x30
#define U2TER_TXEN_MASK 0x80
#define U2TER_TXEN 0x80
#define U2TER_TXEN_BIT 7

#define U2FIFOLVL (*(volatile unsigned char *)0x40098058)
#define U2FIFOLVL_OFFSET 0x58
#define U2FIFOLVL_RXFIFILVL_MASK 0xF
#define U2FIFOLVL_RXFIFILVL_BIT 0
#define U2FIFOLVL_RXFIFOLVL_MASK 0xF00
#define U2FIFOLVL_RXFIFOLVL_BIT 8

#define UART3_BASE_ADDRESS 0x4009C000

#define U3RBR (*(volatile unsigned char *)0x4009C000)
#define U3RBR_OFFSET 0x0

#define U3THR (*(volatile unsigned char *)0x4009C000)
#define U3THR_OFFSET 0x0

#define U3DLL (*(volatile unsigned char *)0x4009C000)
#define U3DLL_OFFSET 0x0

#define U3DLM (*(volatile unsigned char *)0x4009C004)
#define U3DLM_OFFSET 0x4

#define U3IER (*(volatile unsigned long *)0x4009C004)
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

#define U3IIR (*(volatile unsigned long *)0x4009C008)
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

#define U3FCR (*(volatile unsigned char *)0x4009C008)
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
#define U3FCR_DMA_Mode_Select_MASK 0x8
#define U3FCR_DMA_Mode_Select 0x8
#define U3FCR_DMA_Mode_Select_BIT 3
#define U3FCR_Rx_Trigger_Level_Select_MASK 0xC0
#define U3FCR_Rx_Trigger_Level_Select_BIT 6

#define U3LCR (*(volatile unsigned char *)0x4009C00C)
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

#define U3LSR (*(volatile unsigned char *)0x4009C014)
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

#define U3SCR (*(volatile unsigned char *)0x4009C01C)
#define U3SCR_OFFSET 0x1C

#define U3ACR (*(volatile unsigned long *)0x4009C020)
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

#define U3ICR (*(volatile unsigned long *)0x4009C024)
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

#define U3FDR (*(volatile unsigned long *)0x4009C028)
#define U3FDR_OFFSET 0x28
#define U3FDR_DIVADDVAL_MASK 0xF
#define U3FDR_DIVADDVAL_BIT 0
#define U3FDR_MULVAL_MASK 0xF0
#define U3FDR_MULVAL_BIT 4

#define U3TER (*(volatile unsigned char *)0x4009C030)
#define U3TER_OFFSET 0x30
#define U3TER_TXEN_MASK 0x80
#define U3TER_TXEN 0x80
#define U3TER_TXEN_BIT 7

#define U3FIFOLVL (*(volatile unsigned char *)0x4009C058)
#define U3FIFOLVL_OFFSET 0x58
#define U3FIFOLVL_RXFIFILVL_MASK 0xF
#define U3FIFOLVL_RXFIFILVL_BIT 0
#define U3FIFOLVL_RXFIFOLVL_MASK 0xF00
#define U3FIFOLVL_RXFIFOLVL_BIT 8

#define I2C2_BASE_ADDRESS 0x400A0000

#define I2C2CONSET (*(volatile unsigned char *)0x400A0000)
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

#define I2C2STAT (*(volatile unsigned char *)0x400A0004)
#define I2C2STAT_OFFSET 0x4
#define I2C2STAT_Status_MASK 0xF8
#define I2C2STAT_Status_BIT 3

#define I2C2DAT (*(volatile unsigned char *)0x400A0008)
#define I2C2DAT_OFFSET 0x8

#define I2C2ADR0 (*(volatile unsigned char *)0x400A000C)
#define I2C2ADR0_OFFSET 0xC
#define I2C2ADR0_GC_MASK 0x1
#define I2C2ADR0_GC 0x1
#define I2C2ADR0_GC_BIT 0
#define I2C2ADR0_Address_MASK 0xFE
#define I2C2ADR0_Address_BIT 1

#define I2C2SCLH (*(volatile unsigned short *)0x400A0010)
#define I2C2SCLH_OFFSET 0x10

#define I2C2SCLL (*(volatile unsigned short *)0x400A0014)
#define I2C2SCLL_OFFSET 0x14

#define I2C2CONCLR (*(volatile unsigned char *)0x400A0018)
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

#define I2C2MMCTRL (*(volatile unsigned char *)0x400A001C)
#define I2C2MMCTRL_OFFSET 0x1C
#define I2C2MMCTRL_MM_ENA_MASK 0x1
#define I2C2MMCTRL_MM_ENA 0x1
#define I2C2MMCTRL_MM_ENA_BIT 0
#define I2C2MMCTRL_ENA_SCL_MASK 0x2
#define I2C2MMCTRL_ENA_SCL 0x2
#define I2C2MMCTRL_ENA_SCL_BIT 1
#define I2C2MMCTRL_MATCH_ALL_MASK 0x8
#define I2C2MMCTRL_MATCH_ALL 0x8
#define I2C2MMCTRL_MATCH_ALL_BIT 3

#define I2C2ADR1 (*(volatile unsigned char *)0x400A0020)
#define I2C2ADR1_OFFSET 0x20
#define I2C2ADR1_GC_MASK 0x1
#define I2C2ADR1_GC 0x1
#define I2C2ADR1_GC_BIT 0
#define I2C2ADR1_Address_MASK 0xFE
#define I2C2ADR1_Address_BIT 1

#define I2C2ADR2 (*(volatile unsigned char *)0x400A0024)
#define I2C2ADR2_OFFSET 0x24
#define I2C2ADR2_GC_MASK 0x1
#define I2C2ADR2_GC 0x1
#define I2C2ADR2_GC_BIT 0
#define I2C2ADR2_Address_MASK 0xFE
#define I2C2ADR2_Address_BIT 1

#define I2C2ADR3 (*(volatile unsigned char *)0x400A0028)
#define I2C2ADR3_OFFSET 0x28
#define I2C2ADR3_GC_MASK 0x1
#define I2C2ADR3_GC 0x1
#define I2C2ADR3_GC_BIT 0
#define I2C2ADR3_Address_MASK 0xFE
#define I2C2ADR3_Address_BIT 1

#define I2C2DATA_BUFFER (*(volatile unsigned char *)0x400A002C)
#define I2C2DATA_BUFFER_OFFSET 0x2C

#define I2C2MASK0 (*(volatile unsigned long *)0x400A0030)
#define I2C2MASK0_OFFSET 0x30
#define I2C2MASK0_MASK_MASK 0xFE
#define I2C2MASK0_MASK_BIT 1

#define I2C2MASK1 (*(volatile unsigned long *)0x400A0034)
#define I2C2MASK1_OFFSET 0x34
#define I2C2MASK1_MASK_MASK 0xFE
#define I2C2MASK1_MASK_BIT 1

#define I2C2MASK2 (*(volatile unsigned long *)0x400A0038)
#define I2C2MASK2_OFFSET 0x38
#define I2C2MASK2_MASK_MASK 0xFE
#define I2C2MASK2_MASK_BIT 1

#define I2C2MASK3 (*(volatile unsigned long *)0x400A003C)
#define I2C2MASK3_OFFSET 0x3C
#define I2C2MASK3_MASK_MASK 0xFE
#define I2C2MASK3_MASK_BIT 1

#define RIT_BASE_ADDRESS 0x400B0000

#define RICOMPVAL (*(volatile unsigned long *)0x400B0000)
#define RICOMPVAL_OFFSET 0x0

#define RIMASK (*(volatile unsigned long *)0x400B0004)
#define RIMASK_OFFSET 0x4

#define RICTRL (*(volatile unsigned long *)0x400B0008)
#define RICTRL_OFFSET 0x8
#define RICTRL_RITINT_MASK 0x1
#define RICTRL_RITINT 0x1
#define RICTRL_RITINT_BIT 0
#define RICTRL_RITENCLR_MASK 0x2
#define RICTRL_RITENCLR 0x2
#define RICTRL_RITENCLR_BIT 1
#define RICTRL_RITENBR_MASK 0x4
#define RICTRL_RITENBR 0x4
#define RICTRL_RITENBR_BIT 2
#define RICTRL_RITEN_MASK 0x8
#define RICTRL_RITEN 0x8
#define RICTRL_RITEN_BIT 3

#define RICOUNTER (*(volatile unsigned long *)0x400B000C)
#define RICOUNTER_OFFSET 0xC

#define MCPWM_BASE_ADDRESS 0x400B8000

#define MCCON (*(volatile unsigned long *)0x400B8000)
#define MCCON_OFFSET 0x0
#define MCCON_RUN0_MASK 0x1
#define MCCON_RUN0 0x1
#define MCCON_RUN0_BIT 0
#define MCCON_CENTER0_MASK 0x2
#define MCCON_CENTER0 0x2
#define MCCON_CENTER0_BIT 1
#define MCCON_POLA0_MASK 0x4
#define MCCON_POLA0 0x4
#define MCCON_POLA0_BIT 2
#define MCCON_DTE0_MASK 0x8
#define MCCON_DTE0 0x8
#define MCCON_DTE0_BIT 3
#define MCCON_DISUP0_MASK 0x10
#define MCCON_DISUP0 0x10
#define MCCON_DISUP0_BIT 4
#define MCCON_RUN1_MASK 0x100
#define MCCON_RUN1 0x100
#define MCCON_RUN1_BIT 8
#define MCCON_CENTER1_MASK 0x200
#define MCCON_CENTER1 0x200
#define MCCON_CENTER1_BIT 9
#define MCCON_POLA1_MASK 0x400
#define MCCON_POLA1 0x400
#define MCCON_POLA1_BIT 10
#define MCCON_DTE1_MASK 0x800
#define MCCON_DTE1 0x800
#define MCCON_DTE1_BIT 11
#define MCCON_DISUP1_MASK 0x1000
#define MCCON_DISUP1 0x1000
#define MCCON_DISUP1_BIT 12
#define MCCON_RUN2_MASK 0x10000
#define MCCON_RUN2 0x10000
#define MCCON_RUN2_BIT 16
#define MCCON_CENTER2_MASK 0x20000
#define MCCON_CENTER2 0x20000
#define MCCON_CENTER2_BIT 17
#define MCCON_POLA2_MASK 0x40000
#define MCCON_POLA2 0x40000
#define MCCON_POLA2_BIT 18
#define MCCON_DTE2_MASK 0x80000
#define MCCON_DTE2 0x80000
#define MCCON_DTE2_BIT 19
#define MCCON_DISUP2_MASK 0x100000
#define MCCON_DISUP2 0x100000
#define MCCON_DISUP2_BIT 20
#define MCCON_INVBDC_MASK 0x20000000
#define MCCON_INVBDC 0x20000000
#define MCCON_INVBDC_BIT 29
#define MCCON_ACMODE_MASK 0x40000000
#define MCCON_ACMODE 0x40000000
#define MCCON_ACMODE_BIT 30
#define MCCON_DCMODE_MASK 0x80000000
#define MCCON_DCMODE 0x80000000
#define MCCON_DCMODE_BIT 31

#define MCCON_SET (*(volatile unsigned long *)0x400B8004)
#define MCCON_SET_OFFSET 0x4
#define MCCON_SET_RUN0_MASK 0x1
#define MCCON_SET_RUN0 0x1
#define MCCON_SET_RUN0_BIT 0
#define MCCON_SET_CENTER0_MASK 0x2
#define MCCON_SET_CENTER0 0x2
#define MCCON_SET_CENTER0_BIT 1
#define MCCON_SET_POLA0_MASK 0x4
#define MCCON_SET_POLA0 0x4
#define MCCON_SET_POLA0_BIT 2
#define MCCON_SET_DTE0_MASK 0x8
#define MCCON_SET_DTE0 0x8
#define MCCON_SET_DTE0_BIT 3
#define MCCON_SET_DISUP0_MASK 0x10
#define MCCON_SET_DISUP0 0x10
#define MCCON_SET_DISUP0_BIT 4
#define MCCON_SET_RUN1_MASK 0x100
#define MCCON_SET_RUN1 0x100
#define MCCON_SET_RUN1_BIT 8
#define MCCON_SET_CENTER1_MASK 0x200
#define MCCON_SET_CENTER1 0x200
#define MCCON_SET_CENTER1_BIT 9
#define MCCON_SET_POLA1_MASK 0x400
#define MCCON_SET_POLA1 0x400
#define MCCON_SET_POLA1_BIT 10
#define MCCON_SET_DTE1_MASK 0x800
#define MCCON_SET_DTE1 0x800
#define MCCON_SET_DTE1_BIT 11
#define MCCON_SET_DISUP1_MASK 0x1000
#define MCCON_SET_DISUP1 0x1000
#define MCCON_SET_DISUP1_BIT 12
#define MCCON_SET_RUN2_MASK 0x10000
#define MCCON_SET_RUN2 0x10000
#define MCCON_SET_RUN2_BIT 16
#define MCCON_SET_CENTER2_MASK 0x20000
#define MCCON_SET_CENTER2 0x20000
#define MCCON_SET_CENTER2_BIT 17
#define MCCON_SET_POLA2_MASK 0x40000
#define MCCON_SET_POLA2 0x40000
#define MCCON_SET_POLA2_BIT 18
#define MCCON_SET_DTE2_MASK 0x80000
#define MCCON_SET_DTE2 0x80000
#define MCCON_SET_DTE2_BIT 19
#define MCCON_SET_DISUP2_MASK 0x100000
#define MCCON_SET_DISUP2 0x100000
#define MCCON_SET_DISUP2_BIT 20
#define MCCON_SET_INVBDC_MASK 0x20000000
#define MCCON_SET_INVBDC 0x20000000
#define MCCON_SET_INVBDC_BIT 29
#define MCCON_SET_ACMODE_MASK 0x40000000
#define MCCON_SET_ACMODE 0x40000000
#define MCCON_SET_ACMODE_BIT 30
#define MCCON_SET_DCMODE_MASK 0x80000000
#define MCCON_SET_DCMODE 0x80000000
#define MCCON_SET_DCMODE_BIT 31

#define MCCON_CLR (*(volatile unsigned long *)0x400B8008)
#define MCCON_CLR_OFFSET 0x8
#define MCCON_CLR_RUN0_MASK 0x1
#define MCCON_CLR_RUN0 0x1
#define MCCON_CLR_RUN0_BIT 0
#define MCCON_CLR_CENTER0_MASK 0x2
#define MCCON_CLR_CENTER0 0x2
#define MCCON_CLR_CENTER0_BIT 1
#define MCCON_CLR_POLA0_MASK 0x4
#define MCCON_CLR_POLA0 0x4
#define MCCON_CLR_POLA0_BIT 2
#define MCCON_CLR_DTE0_MASK 0x8
#define MCCON_CLR_DTE0 0x8
#define MCCON_CLR_DTE0_BIT 3
#define MCCON_CLR_DISUP0_MASK 0x10
#define MCCON_CLR_DISUP0 0x10
#define MCCON_CLR_DISUP0_BIT 4
#define MCCON_CLR_RUN1_MASK 0x100
#define MCCON_CLR_RUN1 0x100
#define MCCON_CLR_RUN1_BIT 8
#define MCCON_CLR_CENTER1_MASK 0x200
#define MCCON_CLR_CENTER1 0x200
#define MCCON_CLR_CENTER1_BIT 9
#define MCCON_CLR_POLA1_MASK 0x400
#define MCCON_CLR_POLA1 0x400
#define MCCON_CLR_POLA1_BIT 10
#define MCCON_CLR_DTE1_MASK 0x800
#define MCCON_CLR_DTE1 0x800
#define MCCON_CLR_DTE1_BIT 11
#define MCCON_CLR_DISUP1_MASK 0x1000
#define MCCON_CLR_DISUP1 0x1000
#define MCCON_CLR_DISUP1_BIT 12
#define MCCON_CLR_RUN2_MASK 0x10000
#define MCCON_CLR_RUN2 0x10000
#define MCCON_CLR_RUN2_BIT 16
#define MCCON_CLR_CENTER2_MASK 0x20000
#define MCCON_CLR_CENTER2 0x20000
#define MCCON_CLR_CENTER2_BIT 17
#define MCCON_CLR_POLA2_MASK 0x40000
#define MCCON_CLR_POLA2 0x40000
#define MCCON_CLR_POLA2_BIT 18
#define MCCON_CLR_DTE2_MASK 0x80000
#define MCCON_CLR_DTE2 0x80000
#define MCCON_CLR_DTE2_BIT 19
#define MCCON_CLR_DISUP2_MASK 0x100000
#define MCCON_CLR_DISUP2 0x100000
#define MCCON_CLR_DISUP2_BIT 20
#define MCCON_CLR_INVBDC_MASK 0x20000000
#define MCCON_CLR_INVBDC 0x20000000
#define MCCON_CLR_INVBDC_BIT 29
#define MCCON_CLR_ACMODE_MASK 0x40000000
#define MCCON_CLR_ACMODE 0x40000000
#define MCCON_CLR_ACMODE_BIT 30
#define MCCON_CLR_DCMODE_MASK 0x80000000
#define MCCON_CLR_DCMODE 0x80000000
#define MCCON_CLR_DCMODE_BIT 31

#define MCCAPCON (*(volatile unsigned long *)0x400B800C)
#define MCCAPCON_OFFSET 0xC
#define MCCAPCON_CAP0MCI0_RE_MASK 0x1
#define MCCAPCON_CAP0MCI0_RE 0x1
#define MCCAPCON_CAP0MCI0_RE_BIT 0
#define MCCAPCON_CAP0MCI0_FE_MASK 0x2
#define MCCAPCON_CAP0MCI0_FE 0x2
#define MCCAPCON_CAP0MCI0_FE_BIT 1
#define MCCAPCON_CAP0MCI1_RE_MASK 0x4
#define MCCAPCON_CAP0MCI1_RE 0x4
#define MCCAPCON_CAP0MCI1_RE_BIT 2
#define MCCAPCON_CAP0MCI1_FE_MASK 0x8
#define MCCAPCON_CAP0MCI1_FE 0x8
#define MCCAPCON_CAP0MCI1_FE_BIT 3
#define MCCAPCON_CAP0MCI2_RE_MASK 0x10
#define MCCAPCON_CAP0MCI2_RE 0x10
#define MCCAPCON_CAP0MCI2_RE_BIT 4
#define MCCAPCON_CAP0MCI2_FE_MASK 0x20
#define MCCAPCON_CAP0MCI2_FE 0x20
#define MCCAPCON_CAP0MCI2_FE_BIT 5
#define MCCAPCON_CAP1MCI0_RE_MASK 0x40
#define MCCAPCON_CAP1MCI0_RE 0x40
#define MCCAPCON_CAP1MCI0_RE_BIT 6
#define MCCAPCON_CAP1MCI0_FE_MASK 0x80
#define MCCAPCON_CAP1MCI0_FE 0x80
#define MCCAPCON_CAP1MCI0_FE_BIT 7
#define MCCAPCON_CAP1MCI1_RE_MASK 0x100
#define MCCAPCON_CAP1MCI1_RE 0x100
#define MCCAPCON_CAP1MCI1_RE_BIT 8
#define MCCAPCON_CAP1MCI1_FE_MASK 0x200
#define MCCAPCON_CAP1MCI1_FE 0x200
#define MCCAPCON_CAP1MCI1_FE_BIT 9
#define MCCAPCON_CAP1MCI2_RE_MASK 0x400
#define MCCAPCON_CAP1MCI2_RE 0x400
#define MCCAPCON_CAP1MCI2_RE_BIT 10
#define MCCAPCON_CAP1MCI2_FE_MASK 0x800
#define MCCAPCON_CAP1MCI2_FE 0x800
#define MCCAPCON_CAP1MCI2_FE_BIT 11
#define MCCAPCON_CAP2MCI0_RE_MASK 0x1000
#define MCCAPCON_CAP2MCI0_RE 0x1000
#define MCCAPCON_CAP2MCI0_RE_BIT 12
#define MCCAPCON_CAP2MCI0_FE_MASK 0x2000
#define MCCAPCON_CAP2MCI0_FE 0x2000
#define MCCAPCON_CAP2MCI0_FE_BIT 13
#define MCCAPCON_CAP2MCI1_RE_MASK 0x4000
#define MCCAPCON_CAP2MCI1_RE 0x4000
#define MCCAPCON_CAP2MCI1_RE_BIT 14
#define MCCAPCON_CAP2MCI1_FE_MASK 0x8000
#define MCCAPCON_CAP2MCI1_FE 0x8000
#define MCCAPCON_CAP2MCI1_FE_BIT 15
#define MCCAPCON_CAP2MCI2_RE_MASK 0x10000
#define MCCAPCON_CAP2MCI2_RE 0x10000
#define MCCAPCON_CAP2MCI2_RE_BIT 16
#define MCCAPCON_CAP2MCI2_FE_MASK 0x20000
#define MCCAPCON_CAP2MCI2_FE 0x20000
#define MCCAPCON_CAP2MCI2_FE_BIT 17
#define MCCAPCON_RT0_MASK 0x40000
#define MCCAPCON_RT0 0x40000
#define MCCAPCON_RT0_BIT 18
#define MCCAPCON_RT1_MASK 0x80000
#define MCCAPCON_RT1 0x80000
#define MCCAPCON_RT1_BIT 19
#define MCCAPCON_RT2_MASK 0x100000
#define MCCAPCON_RT2 0x100000
#define MCCAPCON_RT2_BIT 20
#define MCCAPCON_HNFCAP0_MASK 0x200000
#define MCCAPCON_HNFCAP0 0x200000
#define MCCAPCON_HNFCAP0_BIT 21
#define MCCAPCON_HNFCAP1_MASK 0x400000
#define MCCAPCON_HNFCAP1 0x400000
#define MCCAPCON_HNFCAP1_BIT 22
#define MCCAPCON_HNFCAP2_MASK 0x800000
#define MCCAPCON_HNFCAP2 0x800000
#define MCCAPCON_HNFCAP2_BIT 23

#define MCCAPCON_SET (*(volatile unsigned long *)0x400B8010)
#define MCCAPCON_SET_OFFSET 0x10
#define MCCAPCON_SET_CAP0MCI0_RE_MASK 0x1
#define MCCAPCON_SET_CAP0MCI0_RE 0x1
#define MCCAPCON_SET_CAP0MCI0_RE_BIT 0
#define MCCAPCON_SET_CAP0MCI0_FE_MASK 0x2
#define MCCAPCON_SET_CAP0MCI0_FE 0x2
#define MCCAPCON_SET_CAP0MCI0_FE_BIT 1
#define MCCAPCON_SET_CAP0MCI1_RE_MASK 0x4
#define MCCAPCON_SET_CAP0MCI1_RE 0x4
#define MCCAPCON_SET_CAP0MCI1_RE_BIT 2
#define MCCAPCON_SET_CAP0MCI1_FE_MASK 0x8
#define MCCAPCON_SET_CAP0MCI1_FE 0x8
#define MCCAPCON_SET_CAP0MCI1_FE_BIT 3
#define MCCAPCON_SET_CAP0MCI2_RE_MASK 0x10
#define MCCAPCON_SET_CAP0MCI2_RE 0x10
#define MCCAPCON_SET_CAP0MCI2_RE_BIT 4
#define MCCAPCON_SET_CAP0MCI2_FE_MASK 0x20
#define MCCAPCON_SET_CAP0MCI2_FE 0x20
#define MCCAPCON_SET_CAP0MCI2_FE_BIT 5
#define MCCAPCON_SET_CAP1MCI0_RE_MASK 0x40
#define MCCAPCON_SET_CAP1MCI0_RE 0x40
#define MCCAPCON_SET_CAP1MCI0_RE_BIT 6
#define MCCAPCON_SET_CAP1MCI0_FE_MASK 0x80
#define MCCAPCON_SET_CAP1MCI0_FE 0x80
#define MCCAPCON_SET_CAP1MCI0_FE_BIT 7
#define MCCAPCON_SET_CAP1MCI1_RE_MASK 0x100
#define MCCAPCON_SET_CAP1MCI1_RE 0x100
#define MCCAPCON_SET_CAP1MCI1_RE_BIT 8
#define MCCAPCON_SET_CAP1MCI1_FE_MASK 0x200
#define MCCAPCON_SET_CAP1MCI1_FE 0x200
#define MCCAPCON_SET_CAP1MCI1_FE_BIT 9
#define MCCAPCON_SET_CAP1MCI2_RE_MASK 0x400
#define MCCAPCON_SET_CAP1MCI2_RE 0x400
#define MCCAPCON_SET_CAP1MCI2_RE_BIT 10
#define MCCAPCON_SET_CAP1MCI2_FE_MASK 0x800
#define MCCAPCON_SET_CAP1MCI2_FE 0x800
#define MCCAPCON_SET_CAP1MCI2_FE_BIT 11
#define MCCAPCON_SET_CAP2MCI0_RE_MASK 0x1000
#define MCCAPCON_SET_CAP2MCI0_RE 0x1000
#define MCCAPCON_SET_CAP2MCI0_RE_BIT 12
#define MCCAPCON_SET_CAP2MCI0_FE_MASK 0x2000
#define MCCAPCON_SET_CAP2MCI0_FE 0x2000
#define MCCAPCON_SET_CAP2MCI0_FE_BIT 13
#define MCCAPCON_SET_CAP2MCI1_RE_MASK 0x4000
#define MCCAPCON_SET_CAP2MCI1_RE 0x4000
#define MCCAPCON_SET_CAP2MCI1_RE_BIT 14
#define MCCAPCON_SET_CAP2MCI1_FE_MASK 0x8000
#define MCCAPCON_SET_CAP2MCI1_FE 0x8000
#define MCCAPCON_SET_CAP2MCI1_FE_BIT 15
#define MCCAPCON_SET_CAP2MCI2_RE_MASK 0x10000
#define MCCAPCON_SET_CAP2MCI2_RE 0x10000
#define MCCAPCON_SET_CAP2MCI2_RE_BIT 16
#define MCCAPCON_SET_CAP2MCI2_FE_MASK 0x20000
#define MCCAPCON_SET_CAP2MCI2_FE 0x20000
#define MCCAPCON_SET_CAP2MCI2_FE_BIT 17
#define MCCAPCON_SET_RT0_MASK 0x40000
#define MCCAPCON_SET_RT0 0x40000
#define MCCAPCON_SET_RT0_BIT 18
#define MCCAPCON_SET_RT1_MASK 0x80000
#define MCCAPCON_SET_RT1 0x80000
#define MCCAPCON_SET_RT1_BIT 19
#define MCCAPCON_SET_RT2_MASK 0x100000
#define MCCAPCON_SET_RT2 0x100000
#define MCCAPCON_SET_RT2_BIT 20
#define MCCAPCON_SET_HNFCAP0_MASK 0x200000
#define MCCAPCON_SET_HNFCAP0 0x200000
#define MCCAPCON_SET_HNFCAP0_BIT 21
#define MCCAPCON_SET_HNFCAP1_MASK 0x400000
#define MCCAPCON_SET_HNFCAP1 0x400000
#define MCCAPCON_SET_HNFCAP1_BIT 22
#define MCCAPCON_SET_HNFCAP2_MASK 0x800000
#define MCCAPCON_SET_HNFCAP2 0x800000
#define MCCAPCON_SET_HNFCAP2_BIT 23

#define MCCAPCON_CLR (*(volatile unsigned long *)0x400B8014)
#define MCCAPCON_CLR_OFFSET 0x14
#define MCCAPCON_CLR_CAP0MCI0_RE_MASK 0x1
#define MCCAPCON_CLR_CAP0MCI0_RE 0x1
#define MCCAPCON_CLR_CAP0MCI0_RE_BIT 0
#define MCCAPCON_CLR_CAP0MCI0_FE_MASK 0x2
#define MCCAPCON_CLR_CAP0MCI0_FE 0x2
#define MCCAPCON_CLR_CAP0MCI0_FE_BIT 1
#define MCCAPCON_CLR_CAP0MCI1_RE_MASK 0x4
#define MCCAPCON_CLR_CAP0MCI1_RE 0x4
#define MCCAPCON_CLR_CAP0MCI1_RE_BIT 2
#define MCCAPCON_CLR_CAP0MCI1_FE_MASK 0x8
#define MCCAPCON_CLR_CAP0MCI1_FE 0x8
#define MCCAPCON_CLR_CAP0MCI1_FE_BIT 3
#define MCCAPCON_CLR_CAP0MCI2_RE_MASK 0x10
#define MCCAPCON_CLR_CAP0MCI2_RE 0x10
#define MCCAPCON_CLR_CAP0MCI2_RE_BIT 4
#define MCCAPCON_CLR_CAP0MCI2_FE_MASK 0x20
#define MCCAPCON_CLR_CAP0MCI2_FE 0x20
#define MCCAPCON_CLR_CAP0MCI2_FE_BIT 5
#define MCCAPCON_CLR_CAP1MCI0_RE_MASK 0x40
#define MCCAPCON_CLR_CAP1MCI0_RE 0x40
#define MCCAPCON_CLR_CAP1MCI0_RE_BIT 6
#define MCCAPCON_CLR_CAP1MCI0_FE_MASK 0x80
#define MCCAPCON_CLR_CAP1MCI0_FE 0x80
#define MCCAPCON_CLR_CAP1MCI0_FE_BIT 7
#define MCCAPCON_CLR_CAP1MCI1_RE_MASK 0x100
#define MCCAPCON_CLR_CAP1MCI1_RE 0x100
#define MCCAPCON_CLR_CAP1MCI1_RE_BIT 8
#define MCCAPCON_CLR_CAP1MCI1_FE_MASK 0x200
#define MCCAPCON_CLR_CAP1MCI1_FE 0x200
#define MCCAPCON_CLR_CAP1MCI1_FE_BIT 9
#define MCCAPCON_CLR_CAP1MCI2_RE_MASK 0x400
#define MCCAPCON_CLR_CAP1MCI2_RE 0x400
#define MCCAPCON_CLR_CAP1MCI2_RE_BIT 10
#define MCCAPCON_CLR_CAP1MCI2_FE_MASK 0x800
#define MCCAPCON_CLR_CAP1MCI2_FE 0x800
#define MCCAPCON_CLR_CAP1MCI2_FE_BIT 11
#define MCCAPCON_CLR_CAP2MCI0_RE_MASK 0x1000
#define MCCAPCON_CLR_CAP2MCI0_RE 0x1000
#define MCCAPCON_CLR_CAP2MCI0_RE_BIT 12
#define MCCAPCON_CLR_CAP2MCI0_FE_MASK 0x2000
#define MCCAPCON_CLR_CAP2MCI0_FE 0x2000
#define MCCAPCON_CLR_CAP2MCI0_FE_BIT 13
#define MCCAPCON_CLR_CAP2MCI1_RE_MASK 0x4000
#define MCCAPCON_CLR_CAP2MCI1_RE 0x4000
#define MCCAPCON_CLR_CAP2MCI1_RE_BIT 14
#define MCCAPCON_CLR_CAP2MCI1_FE_MASK 0x8000
#define MCCAPCON_CLR_CAP2MCI1_FE 0x8000
#define MCCAPCON_CLR_CAP2MCI1_FE_BIT 15
#define MCCAPCON_CLR_CAP2MCI2_RE_MASK 0x10000
#define MCCAPCON_CLR_CAP2MCI2_RE 0x10000
#define MCCAPCON_CLR_CAP2MCI2_RE_BIT 16
#define MCCAPCON_CLR_CAP2MCI2_FE_MASK 0x20000
#define MCCAPCON_CLR_CAP2MCI2_FE 0x20000
#define MCCAPCON_CLR_CAP2MCI2_FE_BIT 17
#define MCCAPCON_CLR_RT0_MASK 0x40000
#define MCCAPCON_CLR_RT0 0x40000
#define MCCAPCON_CLR_RT0_BIT 18
#define MCCAPCON_CLR_RT1_MASK 0x80000
#define MCCAPCON_CLR_RT1 0x80000
#define MCCAPCON_CLR_RT1_BIT 19
#define MCCAPCON_CLR_RT2_MASK 0x100000
#define MCCAPCON_CLR_RT2 0x100000
#define MCCAPCON_CLR_RT2_BIT 20
#define MCCAPCON_CLR_HNFCAP0_MASK 0x200000
#define MCCAPCON_CLR_HNFCAP0 0x200000
#define MCCAPCON_CLR_HNFCAP0_BIT 21
#define MCCAPCON_CLR_HNFCAP1_MASK 0x400000
#define MCCAPCON_CLR_HNFCAP1 0x400000
#define MCCAPCON_CLR_HNFCAP1_BIT 22
#define MCCAPCON_CLR_HNFCAP2_MASK 0x800000
#define MCCAPCON_CLR_HNFCAP2 0x800000
#define MCCAPCON_CLR_HNFCAP2_BIT 23

#define MCTC0 (*(volatile unsigned long *)0x400B8018)
#define MCTC0_OFFSET 0x18

#define MCTC1 (*(volatile unsigned long *)0x400B801C)
#define MCTC1_OFFSET 0x1C

#define MCTC2 (*(volatile unsigned long *)0x400B8020)
#define MCTC2_OFFSET 0x20

#define MCLIM0 (*(volatile unsigned long *)0x400B8024)
#define MCLIM0_OFFSET 0x24

#define MCLIM1 (*(volatile unsigned long *)0x400B8028)
#define MCLIM1_OFFSET 0x28

#define MCLIM2 (*(volatile unsigned long *)0x400B802C)
#define MCLIM2_OFFSET 0x2C

#define MCMAT0 (*(volatile unsigned long *)0x400B8030)
#define MCMAT0_OFFSET 0x30

#define MCMAT1 (*(volatile unsigned long *)0x400B8034)
#define MCMAT1_OFFSET 0x34

#define MCMAT2 (*(volatile unsigned long *)0x400B8038)
#define MCMAT2_OFFSET 0x38

#define MCDT (*(volatile unsigned long *)0x400B803C)
#define MCDT_OFFSET 0x3C
#define MCDT_DT0_MASK 0x3FF
#define MCDT_DT0_BIT 0
#define MCDT_DT1_MASK 0xFFC00
#define MCDT_DT1_BIT 10
#define MCDT_DT2_MASK 0x3FF00000
#define MCDT_DT2_BIT 20

#define MCCP (*(volatile unsigned long *)0x400B8040)
#define MCCP_OFFSET 0x40
#define MCCP_CCPA0_MASK 0x1
#define MCCP_CCPA0 0x1
#define MCCP_CCPA0_BIT 0
#define MCCP_CCPB0_MASK 0x2
#define MCCP_CCPB0 0x2
#define MCCP_CCPB0_BIT 1
#define MCCP_CCPA1_MASK 0x4
#define MCCP_CCPA1 0x4
#define MCCP_CCPA1_BIT 2
#define MCCP_CCPB1_MASK 0x8
#define MCCP_CCPB1 0x8
#define MCCP_CCPB1_BIT 3
#define MCCP_CCPA2_MASK 0x10
#define MCCP_CCPA2 0x10
#define MCCP_CCPA2_BIT 4
#define MCCP_CCPB2_MASK 0x20
#define MCCP_CCPB2 0x20
#define MCCP_CCPB2_BIT 5

#define MCCAP0 (*(volatile unsigned long *)0x400B8044)
#define MCCAP0_OFFSET 0x44

#define MCCAP1 (*(volatile unsigned long *)0x400B8048)
#define MCCAP1_OFFSET 0x48

#define MCCAP2 (*(volatile unsigned long *)0x400B804C)
#define MCCAP2_OFFSET 0x4C

#define MCINTEN (*(volatile unsigned long *)0x400B8050)
#define MCINTEN_OFFSET 0x50
#define MCINTEN_ILIM0_MASK 0x1
#define MCINTEN_ILIM0 0x1
#define MCINTEN_ILIM0_BIT 0
#define MCINTEN_IMAT0_MASK 0x2
#define MCINTEN_IMAT0 0x2
#define MCINTEN_IMAT0_BIT 1
#define MCINTEN_ICAP0_MASK 0x4
#define MCINTEN_ICAP0 0x4
#define MCINTEN_ICAP0_BIT 2
#define MCINTEN_ILIM1_MASK 0x10
#define MCINTEN_ILIM1 0x10
#define MCINTEN_ILIM1_BIT 4
#define MCINTEN_IMAT1_MASK 0x20
#define MCINTEN_IMAT1 0x20
#define MCINTEN_IMAT1_BIT 5
#define MCINTEN_ICAP1_MASK 0x40
#define MCINTEN_ICAP1 0x40
#define MCINTEN_ICAP1_BIT 6
#define MCINTEN_ILIM2_MASK 0x100
#define MCINTEN_ILIM2 0x100
#define MCINTEN_ILIM2_BIT 8
#define MCINTEN_IMAT2_MASK 0x200
#define MCINTEN_IMAT2 0x200
#define MCINTEN_IMAT2_BIT 9
#define MCINTEN_ICAP2_MASK 0x400
#define MCINTEN_ICAP2 0x400
#define MCINTEN_ICAP2_BIT 10
#define MCINTEN_ABORT_MASK 0x8000
#define MCINTEN_ABORT 0x8000
#define MCINTEN_ABORT_BIT 15

#define MCINTEN_SET (*(volatile unsigned long *)0x400B8054)
#define MCINTEN_SET_OFFSET 0x54
#define MCINTEN_SET_ILIM0_MASK 0x1
#define MCINTEN_SET_ILIM0 0x1
#define MCINTEN_SET_ILIM0_BIT 0
#define MCINTEN_SET_IMAT0_MASK 0x2
#define MCINTEN_SET_IMAT0 0x2
#define MCINTEN_SET_IMAT0_BIT 1
#define MCINTEN_SET_ICAP0_MASK 0x4
#define MCINTEN_SET_ICAP0 0x4
#define MCINTEN_SET_ICAP0_BIT 2
#define MCINTEN_SET_ILIM1_MASK 0x10
#define MCINTEN_SET_ILIM1 0x10
#define MCINTEN_SET_ILIM1_BIT 4
#define MCINTEN_SET_IMAT1_MASK 0x20
#define MCINTEN_SET_IMAT1 0x20
#define MCINTEN_SET_IMAT1_BIT 5
#define MCINTEN_SET_ICAP1_MASK 0x40
#define MCINTEN_SET_ICAP1 0x40
#define MCINTEN_SET_ICAP1_BIT 6
#define MCINTEN_SET_ILIM2_MASK 0x100
#define MCINTEN_SET_ILIM2 0x100
#define MCINTEN_SET_ILIM2_BIT 8
#define MCINTEN_SET_IMAT2_MASK 0x200
#define MCINTEN_SET_IMAT2 0x200
#define MCINTEN_SET_IMAT2_BIT 9
#define MCINTEN_SET_ICAP2_MASK 0x400
#define MCINTEN_SET_ICAP2 0x400
#define MCINTEN_SET_ICAP2_BIT 10
#define MCINTEN_SET_ABORT_MASK 0x8000
#define MCINTEN_SET_ABORT 0x8000
#define MCINTEN_SET_ABORT_BIT 15

#define MCINTEN_CLR (*(volatile unsigned long *)0x400B8058)
#define MCINTEN_CLR_OFFSET 0x58
#define MCINTEN_CLR_ILIM0_MASK 0x1
#define MCINTEN_CLR_ILIM0 0x1
#define MCINTEN_CLR_ILIM0_BIT 0
#define MCINTEN_CLR_IMAT0_MASK 0x2
#define MCINTEN_CLR_IMAT0 0x2
#define MCINTEN_CLR_IMAT0_BIT 1
#define MCINTEN_CLR_ICAP0_MASK 0x4
#define MCINTEN_CLR_ICAP0 0x4
#define MCINTEN_CLR_ICAP0_BIT 2
#define MCINTEN_CLR_ILIM1_MASK 0x10
#define MCINTEN_CLR_ILIM1 0x10
#define MCINTEN_CLR_ILIM1_BIT 4
#define MCINTEN_CLR_IMAT1_MASK 0x20
#define MCINTEN_CLR_IMAT1 0x20
#define MCINTEN_CLR_IMAT1_BIT 5
#define MCINTEN_CLR_ICAP1_MASK 0x40
#define MCINTEN_CLR_ICAP1 0x40
#define MCINTEN_CLR_ICAP1_BIT 6
#define MCINTEN_CLR_ILIM2_MASK 0x100
#define MCINTEN_CLR_ILIM2 0x100
#define MCINTEN_CLR_ILIM2_BIT 8
#define MCINTEN_CLR_IMAT2_MASK 0x200
#define MCINTEN_CLR_IMAT2 0x200
#define MCINTEN_CLR_IMAT2_BIT 9
#define MCINTEN_CLR_ICAP2_MASK 0x400
#define MCINTEN_CLR_ICAP2 0x400
#define MCINTEN_CLR_ICAP2_BIT 10
#define MCINTEN_CLR_ABORT_MASK 0x8000
#define MCINTEN_CLR_ABORT 0x8000
#define MCINTEN_CLR_ABORT_BIT 15

#define MCCNTCON (*(volatile unsigned long *)0x400B805C)
#define MCCNTCON_OFFSET 0x5C
#define MCCNTCON_TC0MCI0_RE_MASK 0x1
#define MCCNTCON_TC0MCI0_RE 0x1
#define MCCNTCON_TC0MCI0_RE_BIT 0
#define MCCNTCON_TC0MCI0_FE_MASK 0x2
#define MCCNTCON_TC0MCI0_FE 0x2
#define MCCNTCON_TC0MCI0_FE_BIT 1
#define MCCNTCON_TC0MCI1_RE_MASK 0x4
#define MCCNTCON_TC0MCI1_RE 0x4
#define MCCNTCON_TC0MCI1_RE_BIT 2
#define MCCNTCON_TC0MCI1_FE_MASK 0x8
#define MCCNTCON_TC0MCI1_FE 0x8
#define MCCNTCON_TC0MCI1_FE_BIT 3
#define MCCNTCON_TC0MCI2_RE_MASK 0x10
#define MCCNTCON_TC0MCI2_RE 0x10
#define MCCNTCON_TC0MCI2_RE_BIT 4
#define MCCNTCON_TC0MCI2_FE_MASK 0x20
#define MCCNTCON_TC0MCI2_FE 0x20
#define MCCNTCON_TC0MCI2_FE_BIT 5
#define MCCNTCON_TC1MCI0_RE_MASK 0x40
#define MCCNTCON_TC1MCI0_RE 0x40
#define MCCNTCON_TC1MCI0_RE_BIT 6
#define MCCNTCON_TC1MCI0_FE_MASK 0x80
#define MCCNTCON_TC1MCI0_FE 0x80
#define MCCNTCON_TC1MCI0_FE_BIT 7
#define MCCNTCON_TC1MCI1_RE_MASK 0x100
#define MCCNTCON_TC1MCI1_RE 0x100
#define MCCNTCON_TC1MCI1_RE_BIT 8
#define MCCNTCON_TC1MCI1_FE_MASK 0x200
#define MCCNTCON_TC1MCI1_FE 0x200
#define MCCNTCON_TC1MCI1_FE_BIT 9
#define MCCNTCON_TC1MCI2_RE_MASK 0x400
#define MCCNTCON_TC1MCI2_RE 0x400
#define MCCNTCON_TC1MCI2_RE_BIT 10
#define MCCNTCON_TC1MCI2_FE_MASK 0x800
#define MCCNTCON_TC1MCI2_FE 0x800
#define MCCNTCON_TC1MCI2_FE_BIT 11
#define MCCNTCON_TC2MCI0_RE_MASK 0x1000
#define MCCNTCON_TC2MCI0_RE 0x1000
#define MCCNTCON_TC2MCI0_RE_BIT 12
#define MCCNTCON_TC2MCI0_FE_MASK 0x2000
#define MCCNTCON_TC2MCI0_FE 0x2000
#define MCCNTCON_TC2MCI0_FE_BIT 13
#define MCCNTCON_TC2MCI1_RE_MASK 0x4000
#define MCCNTCON_TC2MCI1_RE 0x4000
#define MCCNTCON_TC2MCI1_RE_BIT 14
#define MCCNTCON_TC2MCI1_FE_MASK 0x8000
#define MCCNTCON_TC2MCI1_FE 0x8000
#define MCCNTCON_TC2MCI1_FE_BIT 15
#define MCCNTCON_TC2MCI2_RE_MASK 0x10000
#define MCCNTCON_TC2MCI2_RE 0x10000
#define MCCNTCON_TC2MCI2_RE_BIT 16
#define MCCNTCON_TC2MCI2_FE_MASK 0x20000
#define MCCNTCON_TC2MCI2_FE 0x20000
#define MCCNTCON_TC2MCI2_FE_BIT 17
#define MCCNTCON_CNTR0_MASK 0x20000000
#define MCCNTCON_CNTR0 0x20000000
#define MCCNTCON_CNTR0_BIT 29
#define MCCNTCON_CNTR1_MASK 0x40000000
#define MCCNTCON_CNTR1 0x40000000
#define MCCNTCON_CNTR1_BIT 30
#define MCCNTCON_CNTR2_MASK 0x80000000
#define MCCNTCON_CNTR2 0x80000000
#define MCCNTCON_CNTR2_BIT 31

#define MCCNTCON_SET (*(volatile unsigned long *)0x400B8060)
#define MCCNTCON_SET_OFFSET 0x60
#define MCCNTCON_SET_TC0MCI0_RE_MASK 0x1
#define MCCNTCON_SET_TC0MCI0_RE 0x1
#define MCCNTCON_SET_TC0MCI0_RE_BIT 0
#define MCCNTCON_SET_TC0MCI0_FE_MASK 0x2
#define MCCNTCON_SET_TC0MCI0_FE 0x2
#define MCCNTCON_SET_TC0MCI0_FE_BIT 1
#define MCCNTCON_SET_TC0MCI1_RE_MASK 0x4
#define MCCNTCON_SET_TC0MCI1_RE 0x4
#define MCCNTCON_SET_TC0MCI1_RE_BIT 2
#define MCCNTCON_SET_TC0MCI1_FE_MASK 0x8
#define MCCNTCON_SET_TC0MCI1_FE 0x8
#define MCCNTCON_SET_TC0MCI1_FE_BIT 3
#define MCCNTCON_SET_TC0MCI2_RE_MASK 0x10
#define MCCNTCON_SET_TC0MCI2_RE 0x10
#define MCCNTCON_SET_TC0MCI2_RE_BIT 4
#define MCCNTCON_SET_TC0MCI2_FE_MASK 0x20
#define MCCNTCON_SET_TC0MCI2_FE 0x20
#define MCCNTCON_SET_TC0MCI2_FE_BIT 5
#define MCCNTCON_SET_TC1MCI0_RE_MASK 0x40
#define MCCNTCON_SET_TC1MCI0_RE 0x40
#define MCCNTCON_SET_TC1MCI0_RE_BIT 6
#define MCCNTCON_SET_TC1MCI0_FE_MASK 0x80
#define MCCNTCON_SET_TC1MCI0_FE 0x80
#define MCCNTCON_SET_TC1MCI0_FE_BIT 7
#define MCCNTCON_SET_TC1MCI1_RE_MASK 0x100
#define MCCNTCON_SET_TC1MCI1_RE 0x100
#define MCCNTCON_SET_TC1MCI1_RE_BIT 8
#define MCCNTCON_SET_TC1MCI1_FE_MASK 0x200
#define MCCNTCON_SET_TC1MCI1_FE 0x200
#define MCCNTCON_SET_TC1MCI1_FE_BIT 9
#define MCCNTCON_SET_TC1MCI2_RE_MASK 0x400
#define MCCNTCON_SET_TC1MCI2_RE 0x400
#define MCCNTCON_SET_TC1MCI2_RE_BIT 10
#define MCCNTCON_SET_TC1MCI2_FE_MASK 0x800
#define MCCNTCON_SET_TC1MCI2_FE 0x800
#define MCCNTCON_SET_TC1MCI2_FE_BIT 11
#define MCCNTCON_SET_TC2MCI0_RE_MASK 0x1000
#define MCCNTCON_SET_TC2MCI0_RE 0x1000
#define MCCNTCON_SET_TC2MCI0_RE_BIT 12
#define MCCNTCON_SET_TC2MCI0_FE_MASK 0x2000
#define MCCNTCON_SET_TC2MCI0_FE 0x2000
#define MCCNTCON_SET_TC2MCI0_FE_BIT 13
#define MCCNTCON_SET_TC2MCI1_RE_MASK 0x4000
#define MCCNTCON_SET_TC2MCI1_RE 0x4000
#define MCCNTCON_SET_TC2MCI1_RE_BIT 14
#define MCCNTCON_SET_TC2MCI1_FE_MASK 0x8000
#define MCCNTCON_SET_TC2MCI1_FE 0x8000
#define MCCNTCON_SET_TC2MCI1_FE_BIT 15
#define MCCNTCON_SET_TC2MCI2_RE_MASK 0x10000
#define MCCNTCON_SET_TC2MCI2_RE 0x10000
#define MCCNTCON_SET_TC2MCI2_RE_BIT 16
#define MCCNTCON_SET_TC2MCI2_FE_MASK 0x20000
#define MCCNTCON_SET_TC2MCI2_FE 0x20000
#define MCCNTCON_SET_TC2MCI2_FE_BIT 17
#define MCCNTCON_SET_CNTR0_MASK 0x20000000
#define MCCNTCON_SET_CNTR0 0x20000000
#define MCCNTCON_SET_CNTR0_BIT 29
#define MCCNTCON_SET_CNTR1_MASK 0x40000000
#define MCCNTCON_SET_CNTR1 0x40000000
#define MCCNTCON_SET_CNTR1_BIT 30
#define MCCNTCON_SET_CNTR2_MASK 0x80000000
#define MCCNTCON_SET_CNTR2 0x80000000
#define MCCNTCON_SET_CNTR2_BIT 31

#define MCCNTCON_CLR (*(volatile unsigned long *)0x400B8064)
#define MCCNTCON_CLR_OFFSET 0x64
#define MCCNTCON_CLR_TC0MCI0_RE_MASK 0x1
#define MCCNTCON_CLR_TC0MCI0_RE 0x1
#define MCCNTCON_CLR_TC0MCI0_RE_BIT 0
#define MCCNTCON_CLR_TC0MCI0_FE_MASK 0x2
#define MCCNTCON_CLR_TC0MCI0_FE 0x2
#define MCCNTCON_CLR_TC0MCI0_FE_BIT 1
#define MCCNTCON_CLR_TC0MCI1_RE_MASK 0x4
#define MCCNTCON_CLR_TC0MCI1_RE 0x4
#define MCCNTCON_CLR_TC0MCI1_RE_BIT 2
#define MCCNTCON_CLR_TC0MCI1_FE_MASK 0x8
#define MCCNTCON_CLR_TC0MCI1_FE 0x8
#define MCCNTCON_CLR_TC0MCI1_FE_BIT 3
#define MCCNTCON_CLR_TC0MCI2_RE_MASK 0x10
#define MCCNTCON_CLR_TC0MCI2_RE 0x10
#define MCCNTCON_CLR_TC0MCI2_RE_BIT 4
#define MCCNTCON_CLR_TC0MCI2_FE_MASK 0x20
#define MCCNTCON_CLR_TC0MCI2_FE 0x20
#define MCCNTCON_CLR_TC0MCI2_FE_BIT 5
#define MCCNTCON_CLR_TC1MCI0_RE_MASK 0x40
#define MCCNTCON_CLR_TC1MCI0_RE 0x40
#define MCCNTCON_CLR_TC1MCI0_RE_BIT 6
#define MCCNTCON_CLR_TC1MCI0_FE_MASK 0x80
#define MCCNTCON_CLR_TC1MCI0_FE 0x80
#define MCCNTCON_CLR_TC1MCI0_FE_BIT 7
#define MCCNTCON_CLR_TC1MCI1_RE_MASK 0x100
#define MCCNTCON_CLR_TC1MCI1_RE 0x100
#define MCCNTCON_CLR_TC1MCI1_RE_BIT 8
#define MCCNTCON_CLR_TC1MCI1_FE_MASK 0x200
#define MCCNTCON_CLR_TC1MCI1_FE 0x200
#define MCCNTCON_CLR_TC1MCI1_FE_BIT 9
#define MCCNTCON_CLR_TC1MCI2_RE_MASK 0x400
#define MCCNTCON_CLR_TC1MCI2_RE 0x400
#define MCCNTCON_CLR_TC1MCI2_RE_BIT 10
#define MCCNTCON_CLR_TC1MCI2_FE_MASK 0x800
#define MCCNTCON_CLR_TC1MCI2_FE 0x800
#define MCCNTCON_CLR_TC1MCI2_FE_BIT 11
#define MCCNTCON_CLR_TC2MCI0_RE_MASK 0x1000
#define MCCNTCON_CLR_TC2MCI0_RE 0x1000
#define MCCNTCON_CLR_TC2MCI0_RE_BIT 12
#define MCCNTCON_CLR_TC2MCI0_FE_MASK 0x2000
#define MCCNTCON_CLR_TC2MCI0_FE 0x2000
#define MCCNTCON_CLR_TC2MCI0_FE_BIT 13
#define MCCNTCON_CLR_TC2MCI1_RE_MASK 0x4000
#define MCCNTCON_CLR_TC2MCI1_RE 0x4000
#define MCCNTCON_CLR_TC2MCI1_RE_BIT 14
#define MCCNTCON_CLR_TC2MCI1_FE_MASK 0x8000
#define MCCNTCON_CLR_TC2MCI1_FE 0x8000
#define MCCNTCON_CLR_TC2MCI1_FE_BIT 15
#define MCCNTCON_CLR_TC2MCI2_RE_MASK 0x10000
#define MCCNTCON_CLR_TC2MCI2_RE 0x10000
#define MCCNTCON_CLR_TC2MCI2_RE_BIT 16
#define MCCNTCON_CLR_TC2MCI2_FE_MASK 0x20000
#define MCCNTCON_CLR_TC2MCI2_FE 0x20000
#define MCCNTCON_CLR_TC2MCI2_FE_BIT 17
#define MCCNTCON_CLR_CNTR0_MASK 0x20000000
#define MCCNTCON_CLR_CNTR0 0x20000000
#define MCCNTCON_CLR_CNTR0_BIT 29
#define MCCNTCON_CLR_CNTR1_MASK 0x40000000
#define MCCNTCON_CLR_CNTR1 0x40000000
#define MCCNTCON_CLR_CNTR1_BIT 30
#define MCCNTCON_CLR_CNTR2_MASK 0x80000000
#define MCCNTCON_CLR_CNTR2 0x80000000
#define MCCNTCON_CLR_CNTR2_BIT 31

#define MCINTF (*(volatile unsigned long *)0x400B8068)
#define MCINTF_OFFSET 0x68
#define MCINTF_ILIM0_MASK 0x1
#define MCINTF_ILIM0 0x1
#define MCINTF_ILIM0_BIT 0
#define MCINTF_IMAT0_MASK 0x2
#define MCINTF_IMAT0 0x2
#define MCINTF_IMAT0_BIT 1
#define MCINTF_ICAP0_MASK 0x4
#define MCINTF_ICAP0 0x4
#define MCINTF_ICAP0_BIT 2
#define MCINTF_ILIM1_MASK 0x10
#define MCINTF_ILIM1 0x10
#define MCINTF_ILIM1_BIT 4
#define MCINTF_IMAT1_MASK 0x20
#define MCINTF_IMAT1 0x20
#define MCINTF_IMAT1_BIT 5
#define MCINTF_ICAP1_MASK 0x40
#define MCINTF_ICAP1 0x40
#define MCINTF_ICAP1_BIT 6
#define MCINTF_ILIM2_MASK 0x100
#define MCINTF_ILIM2 0x100
#define MCINTF_ILIM2_BIT 8
#define MCINTF_IMAT2_MASK 0x200
#define MCINTF_IMAT2 0x200
#define MCINTF_IMAT2_BIT 9
#define MCINTF_ICAP2_MASK 0x400
#define MCINTF_ICAP2 0x400
#define MCINTF_ICAP2_BIT 10
#define MCINTF_ABORT_MASK 0x8000
#define MCINTF_ABORT 0x8000
#define MCINTF_ABORT_BIT 15

#define MCINTF_SET (*(volatile unsigned long *)0x400B806C)
#define MCINTF_SET_OFFSET 0x6C
#define MCINTF_SET_ILIM0_MASK 0x1
#define MCINTF_SET_ILIM0 0x1
#define MCINTF_SET_ILIM0_BIT 0
#define MCINTF_SET_IMAT0_MASK 0x2
#define MCINTF_SET_IMAT0 0x2
#define MCINTF_SET_IMAT0_BIT 1
#define MCINTF_SET_ICAP0_MASK 0x4
#define MCINTF_SET_ICAP0 0x4
#define MCINTF_SET_ICAP0_BIT 2
#define MCINTF_SET_ILIM1_MASK 0x10
#define MCINTF_SET_ILIM1 0x10
#define MCINTF_SET_ILIM1_BIT 4
#define MCINTF_SET_IMAT1_MASK 0x20
#define MCINTF_SET_IMAT1 0x20
#define MCINTF_SET_IMAT1_BIT 5
#define MCINTF_SET_ICAP1_MASK 0x40
#define MCINTF_SET_ICAP1 0x40
#define MCINTF_SET_ICAP1_BIT 6
#define MCINTF_SET_ILIM2_MASK 0x100
#define MCINTF_SET_ILIM2 0x100
#define MCINTF_SET_ILIM2_BIT 8
#define MCINTF_SET_IMAT2_MASK 0x200
#define MCINTF_SET_IMAT2 0x200
#define MCINTF_SET_IMAT2_BIT 9
#define MCINTF_SET_ICAP2_MASK 0x400
#define MCINTF_SET_ICAP2 0x400
#define MCINTF_SET_ICAP2_BIT 10
#define MCINTF_SET_ABORT_MASK 0x8000
#define MCINTF_SET_ABORT 0x8000
#define MCINTF_SET_ABORT_BIT 15

#define MCINTF_CLR (*(volatile unsigned long *)0x400B8070)
#define MCINTF_CLR_OFFSET 0x70
#define MCINTF_CLR_ILIM0_MASK 0x1
#define MCINTF_CLR_ILIM0 0x1
#define MCINTF_CLR_ILIM0_BIT 0
#define MCINTF_CLR_IMAT0_MASK 0x2
#define MCINTF_CLR_IMAT0 0x2
#define MCINTF_CLR_IMAT0_BIT 1
#define MCINTF_CLR_ICAP0_MASK 0x4
#define MCINTF_CLR_ICAP0 0x4
#define MCINTF_CLR_ICAP0_BIT 2
#define MCINTF_CLR_ILIM1_MASK 0x10
#define MCINTF_CLR_ILIM1 0x10
#define MCINTF_CLR_ILIM1_BIT 4
#define MCINTF_CLR_IMAT1_MASK 0x20
#define MCINTF_CLR_IMAT1 0x20
#define MCINTF_CLR_IMAT1_BIT 5
#define MCINTF_CLR_ICAP1_MASK 0x40
#define MCINTF_CLR_ICAP1 0x40
#define MCINTF_CLR_ICAP1_BIT 6
#define MCINTF_CLR_ILIM2_MASK 0x100
#define MCINTF_CLR_ILIM2 0x100
#define MCINTF_CLR_ILIM2_BIT 8
#define MCINTF_CLR_IMAT2_MASK 0x200
#define MCINTF_CLR_IMAT2 0x200
#define MCINTF_CLR_IMAT2_BIT 9
#define MCINTF_CLR_ICAP2_MASK 0x400
#define MCINTF_CLR_ICAP2 0x400
#define MCINTF_CLR_ICAP2_BIT 10
#define MCINTF_CLR_ABORT_MASK 0x8000
#define MCINTF_CLR_ABORT 0x8000
#define MCINTF_CLR_ABORT_BIT 15

#define MCCAP_CLR (*(volatile unsigned long *)0x400B8074)
#define MCCAP_CLR_OFFSET 0x74
#define MCCAP_CLR_CAP_CLR0_MASK 0x1
#define MCCAP_CLR_CAP_CLR0 0x1
#define MCCAP_CLR_CAP_CLR0_BIT 0
#define MCCAP_CLR_CAP_CLR1_MASK 0x2
#define MCCAP_CLR_CAP_CLR1 0x2
#define MCCAP_CLR_CAP_CLR1_BIT 1
#define MCCAP_CLR_CAP_CLR2_MASK 0x4
#define MCCAP_CLR_CAP_CLR2 0x4
#define MCCAP_CLR_CAP_CLR2_BIT 2

#define QEI_BASE_ADDRESS 0x400BC000

#define QEICON (*(volatile unsigned long *)0x400BC000)
#define QEICON_OFFSET 0x0
#define QEICON_RESP_MASK 0x1
#define QEICON_RESP 0x1
#define QEICON_RESP_BIT 0
#define QEICON_RESPI_MASK 0x2
#define QEICON_RESPI 0x2
#define QEICON_RESPI_BIT 1
#define QEICON_RESV_MASK 0x4
#define QEICON_RESV 0x4
#define QEICON_RESV_BIT 2
#define QEICON_RESi_MASK 0x8
#define QEICON_RESi 0x8
#define QEICON_RESi_BIT 3

#define QEISTAT (*(volatile unsigned long *)0x400BC004)
#define QEISTAT_OFFSET 0x4
#define QEISTAT_DIR_MASK 0x1
#define QEISTAT_DIR 0x1
#define QEISTAT_DIR_BIT 0

#define QEICONF (*(volatile unsigned long *)0x400BC008)
#define QEICONF_OFFSET 0x8
#define QEICONF_DIRINV_MASK 0x1
#define QEICONF_DIRINV 0x1
#define QEICONF_DIRINV_BIT 0
#define QEICONF_SIGMODE_MASK 0x2
#define QEICONF_SIGMODE 0x2
#define QEICONF_SIGMODE_BIT 1
#define QEICONF_CAPMODE_MASK 0x4
#define QEICONF_CAPMODE 0x4
#define QEICONF_CAPMODE_BIT 2
#define QEICONF_INVINX_MASK 0x8
#define QEICONF_INVINX 0x8
#define QEICONF_INVINX_BIT 3

#define QEIPOS (*(volatile unsigned long *)0x400BC00C)
#define QEIPOS_OFFSET 0xC

#define QEIMAXPOS (*(volatile unsigned long *)0x400BC010)
#define QEIMAXPOS_OFFSET 0x10

#define CMPOS0 (*(volatile unsigned long *)0x400BC014)
#define CMPOS0_OFFSET 0x14

#define CMPOS1 (*(volatile unsigned long *)0x400BC018)
#define CMPOS1_OFFSET 0x18

#define CMPOS2 (*(volatile unsigned long *)0x400BC01C)
#define CMPOS2_OFFSET 0x1C

#define INXCNT (*(volatile unsigned long *)0x400BC020)
#define INXCNT_OFFSET 0x20

#define INXCMP (*(volatile unsigned long *)0x400BC024)
#define INXCMP_OFFSET 0x24

#define QEILOAD (*(volatile unsigned long *)0x400BC028)
#define QEILOAD_OFFSET 0x28

#define QEITIME (*(volatile unsigned long *)0x400BC02C)
#define QEITIME_OFFSET 0x2C

#define QEIVEL (*(volatile unsigned long *)0x400BC030)
#define QEIVEL_OFFSET 0x30

#define QEICAP (*(volatile unsigned long *)0x400BC034)
#define QEICAP_OFFSET 0x34

#define VELCOMP (*(volatile unsigned long *)0x400BC038)
#define VELCOMP_OFFSET 0x38

#define FILTER (*(volatile unsigned long *)0x400BC03C)
#define FILTER_OFFSET 0x3C

#define QEIIEC (*(volatile unsigned long *)0x400BCFD8)
#define QEIIEC_OFFSET 0xFD8
#define QEIIEC_INX_EN_MASK 0x1
#define QEIIEC_INX_EN 0x1
#define QEIIEC_INX_EN_BIT 0
#define QEIIEC_TIM_EN_MASK 0x2
#define QEIIEC_TIM_EN 0x2
#define QEIIEC_TIM_EN_BIT 1
#define QEIIEC_VELC_EN_MASK 0x4
#define QEIIEC_VELC_EN 0x4
#define QEIIEC_VELC_EN_BIT 2
#define QEIIEC_DIR_EN_MASK 0x8
#define QEIIEC_DIR_EN 0x8
#define QEIIEC_DIR_EN_BIT 3
#define QEIIEC_ERR_EN_MASK 0x10
#define QEIIEC_ERR_EN 0x10
#define QEIIEC_ERR_EN_BIT 4
#define QEIIEC_ENCLK_EN_MASK 0x20
#define QEIIEC_ENCLK_EN 0x20
#define QEIIEC_ENCLK_EN_BIT 5
#define QEIIEC_POS0_EN_MASK 0x40
#define QEIIEC_POS0_EN 0x40
#define QEIIEC_POS0_EN_BIT 6
#define QEIIEC_POS1_EN_MASK 0x80
#define QEIIEC_POS1_EN 0x80
#define QEIIEC_POS1_EN_BIT 7
#define QEIIEC_POS2_EN_MASK 0x100
#define QEIIEC_POS2_EN 0x100
#define QEIIEC_POS2_EN_BIT 8
#define QEIIEC_REV_Int_MASK 0x200
#define QEIIEC_REV_Int 0x200
#define QEIIEC_REV_Int_BIT 9
#define QEIIEC_POS0REV_Int_MASK 0x400
#define QEIIEC_POS0REV_Int 0x400
#define QEIIEC_POS0REV_Int_BIT 10
#define QEIIEC_POS1REV_Int_MASK 0x800
#define QEIIEC_POS1REV_Int 0x800
#define QEIIEC_POS1REV_Int_BIT 11
#define QEIIEC_POS2REV_Int_MASK 0x1000
#define QEIIEC_POS2REV_Int 0x1000
#define QEIIEC_POS2REV_Int_BIT 12

#define QEIIES (*(volatile unsigned long *)0x400BCFDC)
#define QEIIES_OFFSET 0xFDC
#define QEIIES_INX_EN_MASK 0x1
#define QEIIES_INX_EN 0x1
#define QEIIES_INX_EN_BIT 0
#define QEIIES_TIM_EN_MASK 0x2
#define QEIIES_TIM_EN 0x2
#define QEIIES_TIM_EN_BIT 1
#define QEIIES_VELC_EN_MASK 0x4
#define QEIIES_VELC_EN 0x4
#define QEIIES_VELC_EN_BIT 2
#define QEIIES_DIR_EN_MASK 0x8
#define QEIIES_DIR_EN 0x8
#define QEIIES_DIR_EN_BIT 3
#define QEIIES_ERR_EN_MASK 0x10
#define QEIIES_ERR_EN 0x10
#define QEIIES_ERR_EN_BIT 4
#define QEIIES_ENCLK_EN_MASK 0x20
#define QEIIES_ENCLK_EN 0x20
#define QEIIES_ENCLK_EN_BIT 5
#define QEIIES_POS0_EN_MASK 0x40
#define QEIIES_POS0_EN 0x40
#define QEIIES_POS0_EN_BIT 6
#define QEIIES_POS1_EN_MASK 0x80
#define QEIIES_POS1_EN 0x80
#define QEIIES_POS1_EN_BIT 7
#define QEIIES_POS2_EN_MASK 0x100
#define QEIIES_POS2_EN 0x100
#define QEIIES_POS2_EN_BIT 8
#define QEIIES_REV_Int_MASK 0x200
#define QEIIES_REV_Int 0x200
#define QEIIES_REV_Int_BIT 9
#define QEIIES_POS0REV_Int_MASK 0x400
#define QEIIES_POS0REV_Int 0x400
#define QEIIES_POS0REV_Int_BIT 10
#define QEIIES_POS1REV_Int_MASK 0x800
#define QEIIES_POS1REV_Int 0x800
#define QEIIES_POS1REV_Int_BIT 11
#define QEIIES_POS2REV_Int_MASK 0x1000
#define QEIIES_POS2REV_Int 0x1000
#define QEIIES_POS2REV_Int_BIT 12

#define QEIINTSTAT (*(volatile unsigned long *)0x400BCFE0)
#define QEIINTSTAT_OFFSET 0xFE0
#define QEIINTSTAT_INX_Int_MASK 0x1
#define QEIINTSTAT_INX_Int 0x1
#define QEIINTSTAT_INX_Int_BIT 0
#define QEIINTSTAT_TIM_Int_MASK 0x2
#define QEIINTSTAT_TIM_Int 0x2
#define QEIINTSTAT_TIM_Int_BIT 1
#define QEIINTSTAT_VELC_Int_MASK 0x4
#define QEIINTSTAT_VELC_Int 0x4
#define QEIINTSTAT_VELC_Int_BIT 2
#define QEIINTSTAT_DIR_Int_MASK 0x8
#define QEIINTSTAT_DIR_Int 0x8
#define QEIINTSTAT_DIR_Int_BIT 3
#define QEIINTSTAT_ERR_Int_MASK 0x10
#define QEIINTSTAT_ERR_Int 0x10
#define QEIINTSTAT_ERR_Int_BIT 4
#define QEIINTSTAT_ENCLK_Int_MASK 0x20
#define QEIINTSTAT_ENCLK_Int 0x20
#define QEIINTSTAT_ENCLK_Int_BIT 5
#define QEIINTSTAT_POS0_Int_MASK 0x40
#define QEIINTSTAT_POS0_Int 0x40
#define QEIINTSTAT_POS0_Int_BIT 6
#define QEIINTSTAT_POS1_Int_MASK 0x80
#define QEIINTSTAT_POS1_Int 0x80
#define QEIINTSTAT_POS1_Int_BIT 7
#define QEIINTSTAT_POS2_Int_MASK 0x100
#define QEIINTSTAT_POS2_Int 0x100
#define QEIINTSTAT_POS2_Int_BIT 8
#define QEIINTSTAT_REV_Int_MASK 0x200
#define QEIINTSTAT_REV_Int 0x200
#define QEIINTSTAT_REV_Int_BIT 9
#define QEIINTSTAT_POS0REV_Int_MASK 0x400
#define QEIINTSTAT_POS0REV_Int 0x400
#define QEIINTSTAT_POS0REV_Int_BIT 10
#define QEIINTSTAT_POS1REV_Int_MASK 0x800
#define QEIINTSTAT_POS1REV_Int 0x800
#define QEIINTSTAT_POS1REV_Int_BIT 11
#define QEIINTSTAT_POS2REV_Int_MASK 0x1000
#define QEIINTSTAT_POS2REV_Int 0x1000
#define QEIINTSTAT_POS2REV_Int_BIT 12

#define QEIIE (*(volatile unsigned long *)0x400BCFE4)
#define QEIIE_OFFSET 0xFE4
#define QEIIE_INX_Int_MASK 0x1
#define QEIIE_INX_Int 0x1
#define QEIIE_INX_Int_BIT 0
#define QEIIE_TIM_Int_MASK 0x2
#define QEIIE_TIM_Int 0x2
#define QEIIE_TIM_Int_BIT 1
#define QEIIE_VELC_Int_MASK 0x4
#define QEIIE_VELC_Int 0x4
#define QEIIE_VELC_Int_BIT 2
#define QEIIE_DIR_Int_MASK 0x8
#define QEIIE_DIR_Int 0x8
#define QEIIE_DIR_Int_BIT 3
#define QEIIE_ERR_Int_MASK 0x10
#define QEIIE_ERR_Int 0x10
#define QEIIE_ERR_Int_BIT 4
#define QEIIE_ENCLK_Int_MASK 0x20
#define QEIIE_ENCLK_Int 0x20
#define QEIIE_ENCLK_Int_BIT 5
#define QEIIE_POS0_Int_MASK 0x40
#define QEIIE_POS0_Int 0x40
#define QEIIE_POS0_Int_BIT 6
#define QEIIE_POS1_Int_MASK 0x80
#define QEIIE_POS1_Int 0x80
#define QEIIE_POS1_Int_BIT 7
#define QEIIE_POS2_Int_MASK 0x100
#define QEIIE_POS2_Int 0x100
#define QEIIE_POS2_Int_BIT 8
#define QEIIE_REV_Int_MASK 0x200
#define QEIIE_REV_Int 0x200
#define QEIIE_REV_Int_BIT 9
#define QEIIE_POS0REV_Int_MASK 0x400
#define QEIIE_POS0REV_Int 0x400
#define QEIIE_POS0REV_Int_BIT 10
#define QEIIE_POS1REV_Int_MASK 0x800
#define QEIIE_POS1REV_Int 0x800
#define QEIIE_POS1REV_Int_BIT 11
#define QEIIE_POS2REV_Int_MASK 0x1000
#define QEIIE_POS2REV_Int 0x1000
#define QEIIE_POS2REV_Int_BIT 12

#define QEICLR (*(volatile unsigned long *)0x400BCFE8)
#define QEICLR_OFFSET 0xFE8
#define QEICLR_INX_Int_MASK 0x1
#define QEICLR_INX_Int 0x1
#define QEICLR_INX_Int_BIT 0
#define QEICLR_TIM_Int_MASK 0x2
#define QEICLR_TIM_Int 0x2
#define QEICLR_TIM_Int_BIT 1
#define QEICLR_VELC_Int_MASK 0x4
#define QEICLR_VELC_Int 0x4
#define QEICLR_VELC_Int_BIT 2
#define QEICLR_DIR_Int_MASK 0x8
#define QEICLR_DIR_Int 0x8
#define QEICLR_DIR_Int_BIT 3
#define QEICLR_ERR_Int_MASK 0x10
#define QEICLR_ERR_Int 0x10
#define QEICLR_ERR_Int_BIT 4
#define QEICLR_ENCLK_Int_MASK 0x20
#define QEICLR_ENCLK_Int 0x20
#define QEICLR_ENCLK_Int_BIT 5
#define QEICLR_POS0_Int_MASK 0x40
#define QEICLR_POS0_Int 0x40
#define QEICLR_POS0_Int_BIT 6
#define QEICLR_POS1_Int_MASK 0x80
#define QEICLR_POS1_Int 0x80
#define QEICLR_POS1_Int_BIT 7
#define QEICLR_POS2_Int_MASK 0x100
#define QEICLR_POS2_Int 0x100
#define QEICLR_POS2_Int_BIT 8
#define QEICLR_REV_Int_MASK 0x200
#define QEICLR_REV_Int 0x200
#define QEICLR_REV_Int_BIT 9
#define QEICLR_POS0REV_Int_MASK 0x400
#define QEICLR_POS0REV_Int 0x400
#define QEICLR_POS0REV_Int_BIT 10
#define QEICLR_POS1REV_Int_MASK 0x800
#define QEICLR_POS1REV_Int 0x800
#define QEICLR_POS1REV_Int_BIT 11
#define QEICLR_POS2REV_Int_MASK 0x1000
#define QEICLR_POS2REV_Int 0x1000
#define QEICLR_POS2REV_Int_BIT 12

#define QEISET (*(volatile unsigned long *)0x400BCFEC)
#define QEISET_OFFSET 0xFEC
#define QEISET_INX_Int_MASK 0x1
#define QEISET_INX_Int 0x1
#define QEISET_INX_Int_BIT 0
#define QEISET_TIM_Int_MASK 0x2
#define QEISET_TIM_Int 0x2
#define QEISET_TIM_Int_BIT 1
#define QEISET_VELC_Int_MASK 0x4
#define QEISET_VELC_Int 0x4
#define QEISET_VELC_Int_BIT 2
#define QEISET_DIR_Int_MASK 0x8
#define QEISET_DIR_Int 0x8
#define QEISET_DIR_Int_BIT 3
#define QEISET_ERR_Int_MASK 0x10
#define QEISET_ERR_Int 0x10
#define QEISET_ERR_Int_BIT 4
#define QEISET_ENCLK_Int_MASK 0x20
#define QEISET_ENCLK_Int 0x20
#define QEISET_ENCLK_Int_BIT 5
#define QEISET_POS0_Int_MASK 0x40
#define QEISET_POS0_Int 0x40
#define QEISET_POS0_Int_BIT 6
#define QEISET_POS1_Int_MASK 0x80
#define QEISET_POS1_Int 0x80
#define QEISET_POS1_Int_BIT 7
#define QEISET_POS2_Int_MASK 0x100
#define QEISET_POS2_Int 0x100
#define QEISET_POS2_Int_BIT 8
#define QEISET_REV_Int_MASK 0x200
#define QEISET_REV_Int 0x200
#define QEISET_REV_Int_BIT 9
#define QEISET_POS0REV_Int_MASK 0x400
#define QEISET_POS0REV_Int 0x400
#define QEISET_POS0REV_Int_BIT 10
#define QEISET_POS1REV_Int_MASK 0x800
#define QEISET_POS1REV_Int 0x800
#define QEISET_POS1REV_Int_BIT 11
#define QEISET_POS2REV_Int_MASK 0x1000
#define QEISET_POS2REV_Int 0x1000
#define QEISET_POS2REV_Int_BIT 12

#define SC_BASE_ADDRESS 0x400FC000

#define FLASHCFG (*(volatile unsigned long *)0x400FC000)
#define FLASHCFG_OFFSET 0x0
#define FLASHCFG_FLASHTIM_MASK 0xF000
#define FLASHCFG_FLASHTIM_BIT 12

#define CLKSRCSEL (*(volatile unsigned long *)0x400FC10C)
#define CLKSRCSEL_OFFSET 0x10C
#define CLKSRCSEL_CLKSRC_MASK 0x3
#define CLKSRCSEL_CLKSRC_BIT 0

#define PLL0CON (*(volatile unsigned long *)0x400FC080)
#define PLL0CON_OFFSET 0x80
#define PLL0CON_PLLE0_MASK 0x1
#define PLL0CON_PLLE0 0x1
#define PLL0CON_PLLE0_BIT 0
#define PLL0CON_PLLC0_MASK 0x2
#define PLL0CON_PLLC0 0x2
#define PLL0CON_PLLC0_BIT 1

#define PLL0CFG (*(volatile unsigned long *)0x400FC084)
#define PLL0CFG_OFFSET 0x84
#define PLL0CFG_MSEL0_MASK 0x7FFF
#define PLL0CFG_MSEL0_BIT 0
#define PLL0CFG_NSEL0_MASK 0xFF0000
#define PLL0CFG_NSEL0_BIT 16

#define PLL0STAT (*(volatile unsigned long *)0x400FC088)
#define PLL0STAT_OFFSET 0x88
#define PLL0STAT_MSEL0_MASK 0x7FFF
#define PLL0STAT_MSEL0_BIT 0
#define PLL0STAT_NSEL0_MASK 0xFF0000
#define PLL0STAT_NSEL0_BIT 16
#define PLL0STAT_PLLE0_STAT_MASK 0x1000000
#define PLL0STAT_PLLE0_STAT 0x1000000
#define PLL0STAT_PLLE0_STAT_BIT 24
#define PLL0STAT_PLLC0_STAT_MASK 0x2000000
#define PLL0STAT_PLLC0_STAT 0x2000000
#define PLL0STAT_PLLC0_STAT_BIT 25
#define PLL0STAT_PLOCK0_MASK 0x4000000
#define PLL0STAT_PLOCK0 0x4000000
#define PLL0STAT_PLOCK0_BIT 26

#define PLL0FEED (*(volatile unsigned long *)0x400FC08C)
#define PLL0FEED_OFFSET 0x8C

#define PLL1CON (*(volatile unsigned long *)0x400FC0A0)
#define PLL1CON_OFFSET 0xA0
#define PLL1CON_PLLE1_MASK 0x1
#define PLL1CON_PLLE1 0x1
#define PLL1CON_PLLE1_BIT 0
#define PLL1CON_PLLC1_MASK 0x2
#define PLL1CON_PLLC1 0x2
#define PLL1CON_PLLC1_BIT 1

#define PLL1CFG (*(volatile unsigned long *)0x400FC0A4)
#define PLL1CFG_OFFSET 0xA4
#define PLL1CFG_MSEL1_MASK 0x1F
#define PLL1CFG_MSEL1_BIT 0
#define PLL1CFG_PSEL1_MASK 0x60
#define PLL1CFG_PSEL1_BIT 5

#define PLL1STAT (*(volatile unsigned long *)0x400FC0A8)
#define PLL1STAT_OFFSET 0xA8
#define PLL1STAT_MSEL1_MASK 0x1F
#define PLL1STAT_MSEL1_BIT 0
#define PLL1STAT_PSEL1_MASK 0x60
#define PLL1STAT_PSEL1_BIT 5
#define PLL1STAT_PLLE1_STAT_MASK 0x100
#define PLL1STAT_PLLE1_STAT 0x100
#define PLL1STAT_PLLE1_STAT_BIT 8
#define PLL1STAT_PLLC1_STAT_MASK 0x200
#define PLL1STAT_PLLC1_STAT 0x200
#define PLL1STAT_PLLC1_STAT_BIT 9
#define PLL1STAT_PLOCK1_MASK 0x400
#define PLL1STAT_PLOCK1 0x400
#define PLL1STAT_PLOCK1_BIT 10

#define PLL1FEED (*(volatile unsigned long *)0x400FC0AC)
#define PLL1FEED_OFFSET 0xAC

#define CCLKCFG (*(volatile unsigned long *)0x400FC104)
#define CCLKCFG_OFFSET 0x104
#define CCLKCFG_CCLKSEL_MASK 0xFF
#define CCLKCFG_CCLKSEL_BIT 0

#define USBCLKCFG (*(volatile unsigned long *)0x400FC108)
#define USBCLKCFG_OFFSET 0x108
#define USBCLKCFG_USBSEL_MASK 0xF
#define USBCLKCFG_USBSEL_BIT 0

#define IRCTRIM (*(volatile unsigned long *)0x400FC1A4)
#define IRCTRIM_OFFSET 0x1A4
#define IRCTRIM_IRCtrim_MASK 0xFF
#define IRCTRIM_IRCtrim_BIT 0

#define PCLKSEL0 (*(volatile unsigned long *)0x400FC1A8)
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
#define PCLKSEL0_PCLK_PWM1_MASK 0x3000
#define PCLKSEL0_PCLK_PWM1_BIT 12
#define PCLKSEL0_PCLK_I2C0_MASK 0xC000
#define PCLKSEL0_PCLK_I2C0_BIT 14
#define PCLKSEL0_PCLK_SPI_MASK 0x30000
#define PCLKSEL0_PCLK_SPI_BIT 16
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

#define PCLKSEL1 (*(volatile unsigned long *)0x400FC1AC)
#define PCLKSEL1_OFFSET 0x1AC
#define PCLKSEL1_PCLK_QEI_MASK 0x3
#define PCLKSEL1_PCLK_QEI_BIT 0
#define PCLKSEL1_PCLK_GPIOINT_MASK 0xC
#define PCLKSEL1_PCLK_GPIOINT_BIT 2
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
#define PCLKSEL1_PCLK_RIT_MASK 0xC000000
#define PCLKSEL1_PCLK_RIT_BIT 26
#define PCLKSEL1_PCLK_SYSCON_MASK 0x30000000
#define PCLKSEL1_PCLK_SYSCON_BIT 28
#define PCLKSEL1_PCLK_MC_MASK 0xC0000000
#define PCLKSEL1_PCLK_MC_BIT 30

#define PCON (*(volatile unsigned long *)0x400FC0C0)
#define PCON_OFFSET 0xC0
#define PCON_PM0_MASK 0x1
#define PCON_PM0 0x1
#define PCON_PM0_BIT 0
#define PCON_PM1_MASK 0x2
#define PCON_PM1 0x2
#define PCON_PM1_BIT 1
#define PCON_BODRPM_MASK 0x4
#define PCON_BODRPM 0x4
#define PCON_BODRPM_BIT 2
#define PCON_BOGD_MASK 0x8
#define PCON_BOGD 0x8
#define PCON_BOGD_BIT 3
#define PCON_BORD_MASK 0x10
#define PCON_BORD 0x10
#define PCON_BORD_BIT 4
#define PCON_SMFLAG_MASK 0x100
#define PCON_SMFLAG 0x100
#define PCON_SMFLAG_BIT 8
#define PCON_DSFLAG_MASK 0x200
#define PCON_DSFLAG 0x200
#define PCON_DSFLAG_BIT 9
#define PCON_PDFLAG_MASK 0x400
#define PCON_PDFLAG 0x400
#define PCON_PDFLAG_BIT 10
#define PCON_DPDFLAG_MASK 0x800
#define PCON_DPDFLAG 0x800
#define PCON_DPDFLAG_BIT 11

#define PCONP (*(volatile unsigned long *)0x400FC0C4)
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
#define PCONP_PWM1_MASK 0x40
#define PCONP_PWM1 0x40
#define PCONP_PWM1_BIT 6
#define PCONP_PCI2C0_MASK 0x80
#define PCONP_PCI2C0 0x80
#define PCONP_PCI2C0_BIT 7
#define PCONP_PCSPI_MASK 0x100
#define PCONP_PCSPI 0x100
#define PCONP_PCSPI_BIT 8
#define PCONP_PCRTC_MASK 0x200
#define PCONP_PCRTC 0x200
#define PCONP_PCRTC_BIT 9
#define PCONP_PCSSP_MASK 0x400
#define PCONP_PCSSP 0x400
#define PCONP_PCSSP_BIT 10
#define PCONP_PCAD_MASK 0x1000
#define PCONP_PCAD 0x1000
#define PCONP_PCAD_BIT 12
#define PCONP_PCCAN1_MASK 0x2000
#define PCONP_PCCAN1 0x2000
#define PCONP_PCCAN1_BIT 13
#define PCONP_PCCAN2_MASK 0x4000
#define PCONP_PCCAN2 0x4000
#define PCONP_PCCAN2_BIT 14
#define PCONP_PCGPIO_MASK 0x8000
#define PCONP_PCGPIO 0x8000
#define PCONP_PCGPIO_BIT 15
#define PCONP_PCRIT_MASK 0x10000
#define PCONP_PCRIT 0x10000
#define PCONP_PCRIT_BIT 16
#define PCONP_PCMC_MASK 0x20000
#define PCONP_PCMC 0x20000
#define PCONP_PCMC_BIT 17
#define PCONP_PCQEI_MASK 0x40000
#define PCONP_PCQEI 0x40000
#define PCONP_PCQEI_BIT 18
#define PCONP_PCI2C1_MASK 0x80000
#define PCONP_PCI2C1 0x80000
#define PCONP_PCI2C1_BIT 19
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
#define PCONP_PCI2S_MASK 0x8000000
#define PCONP_PCI2S 0x8000000
#define PCONP_PCI2S_BIT 27
#define PCONP_PCGPDMA_MASK 0x20000000
#define PCONP_PCGPDMA 0x20000000
#define PCONP_PCGPDMA_BIT 29
#define PCONP_PCENET_MASK 0x40000000
#define PCONP_PCENET 0x40000000
#define PCONP_PCENET_BIT 30
#define PCONP_PCUSB_MASK 0x80000000
#define PCONP_PCUSB 0x80000000
#define PCONP_PCUSB_BIT 31

#define CLKOUTCFG (*(volatile unsigned long *)0x400FC1C8)
#define CLKOUTCFG_OFFSET 0x1C8
#define CLKOUTCFG_CLKOUTSEL_MASK 0xF
#define CLKOUTCFG_CLKOUTSEL_BIT 0
#define CLKOUTCFG_CLKOUTDIV_MASK 0xF0
#define CLKOUTCFG_CLKOUTDIV_BIT 4
#define CLKOUTCFG_CLKOUT_EN_MASK 0x100
#define CLKOUTCFG_CLKOUT_EN 0x100
#define CLKOUTCFG_CLKOUT_EN_BIT 8
#define CLKOUTCFG_CLKOUT_ACT_MASK 0x200
#define CLKOUTCFG_CLKOUT_ACT 0x200
#define CLKOUTCFG_CLKOUT_ACT_BIT 9

#define CANSLEEPCLR (*(volatile unsigned long *)0x400FC110)
#define CANSLEEPCLR_OFFSET 0x110
#define CANSLEEPCLR_CAN1SLEEP_MASK 0x2
#define CANSLEEPCLR_CAN1SLEEP 0x2
#define CANSLEEPCLR_CAN1SLEEP_BIT 1
#define CANSLEEPCLR_CAN2SLEEP_MASK 0x4
#define CANSLEEPCLR_CAN2SLEEP 0x4
#define CANSLEEPCLR_CAN2SLEEP_BIT 2

#define CANWAKEFLAGS (*(volatile unsigned long *)0x400FC114)
#define CANWAKEFLAGS_OFFSET 0x114
#define CANWAKEFLAGS_CAN1WAKE_MASK 0x2
#define CANWAKEFLAGS_CAN1WAKE 0x2
#define CANWAKEFLAGS_CAN1WAKE_BIT 1
#define CANWAKEFLAGS_CAN2WAKE_MASK 0x4
#define CANWAKEFLAGS_CAN2WAKE 0x4
#define CANWAKEFLAGS_CAN2WAKE_BIT 2

#define EXTINT (*(volatile unsigned char *)0x400FC140)
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

#define EXTMODE (*(volatile unsigned char *)0x400FC148)
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

#define EXTPOLAR (*(volatile unsigned char *)0x400FC14C)
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

#define RSID (*(volatile unsigned char *)0x400FC180)
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

#define SCS (*(volatile unsigned char *)0x400FC1A0)
#define SCS_OFFSET 0x1A0
#define SCS_OSCRANGE_MASK 0x10
#define SCS_OSCRANGE 0x10
#define SCS_OSCRANGE_BIT 4
#define SCS_OSCEN_MASK 0x20
#define SCS_OSCEN 0x20
#define SCS_OSCEN_BIT 5
#define SCS_OSCSTAT_MASK 0x40
#define SCS_OSCSTAT 0x40
#define SCS_OSCSTAT_BIT 6

#define USBIntSt (*(volatile unsigned long *)0x400FC1C0)
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
#define USBIntSt_USB_NEED_CLK_MASK 0x100
#define USBIntSt_USB_NEED_CLK 0x100
#define USBIntSt_USB_NEED_CLK_BIT 8
#define USBIntSt_EN_USB_INTS_MASK 0x80000000
#define USBIntSt_EN_USB_INTS 0x80000000
#define USBIntSt_EN_USB_INTS_BIT 31

#define GPDMA_BASE_ADDRESS 0x50004000

#define DMACIntStat (*(volatile unsigned *)0x50004000)
#define DMACIntStat_OFFSET 0x0
#define DMACIntStat_IntStat_MASK 0xFF
#define DMACIntStat_IntStat_BIT 0

#define DMACIntTCStat (*(volatile unsigned *)0x50004004)
#define DMACIntTCStat_OFFSET 0x4
#define DMACIntTCStat_IntTCStat_MASK 0xFF
#define DMACIntTCStat_IntTCStat_BIT 0

#define DMACIntTCClear (*(volatile unsigned *)0x50004008)
#define DMACIntTCClear_OFFSET 0x8
#define DMACIntTCClear_IntTCClear_MASK 0xFF
#define DMACIntTCClear_IntTCClear_BIT 0

#define DMACIntErrStat (*(volatile unsigned *)0x5000400C)
#define DMACIntErrStat_OFFSET 0xC
#define DMACIntErrStat_IntErrStat_MASK 0xFF
#define DMACIntErrStat_IntErrStat_BIT 0

#define DMACIntErrClr (*(volatile unsigned *)0x50004010)
#define DMACIntErrClr_OFFSET 0x10
#define DMACIntErrClr_IntErrClr_MASK 0xFF
#define DMACIntErrClr_IntErrClr_BIT 0

#define DMACRawIntTCStat (*(volatile unsigned *)0x50004014)
#define DMACRawIntTCStat_OFFSET 0x14
#define DMACRawIntTCStat_RawIntTCStat_MASK 0xFF
#define DMACRawIntTCStat_RawIntTCStat_BIT 0

#define DMACRawIntErrStat (*(volatile unsigned *)0x50004018)
#define DMACRawIntErrStat_OFFSET 0x18
#define DMACRawIntErrStat_RawIntErrStat_MASK 0xFF
#define DMACRawIntErrStat_RawIntErrStat_BIT 0

#define DMACEnbldChns (*(volatile unsigned *)0x5000401C)
#define DMACEnbldChns_OFFSET 0x1C
#define DMACEnbldChns_EnabledChannels_MASK 0xFF
#define DMACEnbldChns_EnabledChannels_BIT 0

#define DMACSoftBReq (*(volatile unsigned *)0x50004020)
#define DMACSoftBReq_OFFSET 0x20
#define DMACSoftBReq_SoftBReq_MASK 0xFFFF
#define DMACSoftBReq_SoftBReq_BIT 0

#define DMACSoftSReq (*(volatile unsigned *)0x50004024)
#define DMACSoftSReq_OFFSET 0x24
#define DMACSoftSReq_SoftSReq_MASK 0xFFFF
#define DMACSoftSReq_SoftSReq_BIT 0

#define DMACSoftLBReq (*(volatile unsigned *)0x50004028)
#define DMACSoftLBReq_OFFSET 0x28
#define DMACSoftLBReq_SoftLBReq_MASK 0xFFFF
#define DMACSoftLBReq_SoftLBReq_BIT 0

#define DMACSoftLSReq (*(volatile unsigned *)0x5000402C)
#define DMACSoftLSReq_OFFSET 0x2C
#define DMACSoftLSReq_SoftLSReq_MASK 0xFFFF
#define DMACSoftLSReq_SoftLSReq_BIT 0

#define DMACConfig (*(volatile unsigned *)0x50004030)
#define DMACConfig_OFFSET 0x30
#define DMACConfig_E_MASK 0x1
#define DMACConfig_E 0x1
#define DMACConfig_E_BIT 0
#define DMACConfig_M_MASK 0x2
#define DMACConfig_M 0x2
#define DMACConfig_M_BIT 1

#define DMACSync (*(volatile unsigned *)0x50004034)
#define DMACSync_OFFSET 0x34
#define DMACSync_DMACSync_MASK 0xFFFF
#define DMACSync_DMACSync_BIT 0

#define DMACC0SrcAddr (*(volatile unsigned *)0x50004100)
#define DMACC0SrcAddr_OFFSET 0x100

#define DMACC1SrcAddr (*(volatile unsigned *)0x50004120)
#define DMACC1SrcAddr_OFFSET 0x120

#define DMACC2SrcAddr (*(volatile unsigned *)0x50004140)
#define DMACC2SrcAddr_OFFSET 0x140

#define DMACC3SrcAddr (*(volatile unsigned *)0x50004160)
#define DMACC3SrcAddr_OFFSET 0x160

#define DMACC4SrcAddr (*(volatile unsigned *)0x50004180)
#define DMACC4SrcAddr_OFFSET 0x180

#define DMACC5SrcAddr (*(volatile unsigned *)0x500041A0)
#define DMACC5SrcAddr_OFFSET 0x1A0

#define DMACC6SrcAddr (*(volatile unsigned *)0x500041C0)
#define DMACC6SrcAddr_OFFSET 0x1C0

#define DMACC7SrcAddr (*(volatile unsigned *)0x500041E0)
#define DMACC7SrcAddr_OFFSET 0x1E0

#define DMACC0DestAddr (*(volatile unsigned *)0x50004104)
#define DMACC0DestAddr_OFFSET 0x104

#define DMACC1DestAddr (*(volatile unsigned *)0x50004124)
#define DMACC1DestAddr_OFFSET 0x124

#define DMACC2DestAddr (*(volatile unsigned *)0x50004144)
#define DMACC2DestAddr_OFFSET 0x144

#define DMACC3DestAddr (*(volatile unsigned *)0x50004164)
#define DMACC3DestAddr_OFFSET 0x164

#define DMACC4DestAddr (*(volatile unsigned *)0x50004184)
#define DMACC4DestAddr_OFFSET 0x184

#define DMACC5DestAddr (*(volatile unsigned *)0x500041A4)
#define DMACC5DestAddr_OFFSET 0x1A4

#define DMACC6DestAddr (*(volatile unsigned *)0x500041C4)
#define DMACC6DestAddr_OFFSET 0x1C4

#define DMACC7DestAddr (*(volatile unsigned *)0x500041E4)
#define DMACC7DestAddr_OFFSET 0x1E4

#define DMACC0LLI (*(volatile unsigned *)0x50004108)
#define DMACC0LLI_OFFSET 0x108
#define DMACC0LLI_LLI_MASK 0xFFFFFFFC
#define DMACC0LLI_LLI_BIT 2

#define DMACC1LLI (*(volatile unsigned *)0x50004128)
#define DMACC1LLI_OFFSET 0x128
#define DMACC1LLI_LLI_MASK 0xFFFFFFFC
#define DMACC1LLI_LLI_BIT 2

#define DMACC2LLI (*(volatile unsigned *)0x50004148)
#define DMACC2LLI_OFFSET 0x148
#define DMACC2LLI_LLI_MASK 0xFFFFFFFC
#define DMACC2LLI_LLI_BIT 2

#define DMACC3LLI (*(volatile unsigned *)0x50004168)
#define DMACC3LLI_OFFSET 0x168
#define DMACC3LLI_LLI_MASK 0xFFFFFFFC
#define DMACC3LLI_LLI_BIT 2

#define DMACC4LLI (*(volatile unsigned *)0x50004188)
#define DMACC4LLI_OFFSET 0x188
#define DMACC4LLI_LLI_MASK 0xFFFFFFFC
#define DMACC4LLI_LLI_BIT 2

#define DMACC5LLI (*(volatile unsigned *)0x500041A8)
#define DMACC5LLI_OFFSET 0x1A8
#define DMACC5LLI_LLI_MASK 0xFFFFFFFC
#define DMACC5LLI_LLI_BIT 2

#define DMACC6LLI (*(volatile unsigned *)0x500041C8)
#define DMACC6LLI_OFFSET 0x1C8
#define DMACC6LLI_LLI_MASK 0xFFFFFFFC
#define DMACC6LLI_LLI_BIT 2

#define DMACC7LLI (*(volatile unsigned *)0x500041E8)
#define DMACC7LLI_OFFSET 0x1E8
#define DMACC7LLI_LLI_MASK 0xFFFFFFFC
#define DMACC7LLI_LLI_BIT 2

#define DMACC0Control (*(volatile unsigned *)0x5000410C)
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
#define DMACC0Control_Prot1_MASK 0x10000000
#define DMACC0Control_Prot1 0x10000000
#define DMACC0Control_Prot1_BIT 28
#define DMACC0Control_Prot2_MASK 0x20000000
#define DMACC0Control_Prot2 0x20000000
#define DMACC0Control_Prot2_BIT 29
#define DMACC0Control_Prot3_MASK 0x40000000
#define DMACC0Control_Prot3 0x40000000
#define DMACC0Control_Prot3_BIT 30
#define DMACC0Control_I_MASK 0x80000000
#define DMACC0Control_I 0x80000000
#define DMACC0Control_I_BIT 31

#define DMACC1Control (*(volatile unsigned *)0x5000412C)
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
#define DMACC1Control_Prot1_MASK 0x10000000
#define DMACC1Control_Prot1 0x10000000
#define DMACC1Control_Prot1_BIT 28
#define DMACC1Control_Prot2_MASK 0x20000000
#define DMACC1Control_Prot2 0x20000000
#define DMACC1Control_Prot2_BIT 29
#define DMACC1Control_Prot3_MASK 0x40000000
#define DMACC1Control_Prot3 0x40000000
#define DMACC1Control_Prot3_BIT 30
#define DMACC1Control_I_MASK 0x80000000
#define DMACC1Control_I 0x80000000
#define DMACC1Control_I_BIT 31

#define DMACC2Control (*(volatile unsigned *)0x5000414C)
#define DMACC2Control_OFFSET 0x14C
#define DMACC2Control_TransferSize_MASK 0xFFF
#define DMACC2Control_TransferSize_BIT 0
#define DMACC2Control_SBSize_MASK 0x7000
#define DMACC2Control_SBSize_BIT 12
#define DMACC2Control_DBSize_MASK 0x38000
#define DMACC2Control_DBSize_BIT 15
#define DMACC2Control_SWidth_MASK 0x1C0000
#define DMACC2Control_SWidth_BIT 18
#define DMACC2Control_DWidth_MASK 0xE00000
#define DMACC2Control_DWidth_BIT 21
#define DMACC2Control_SI_MASK 0x4000000
#define DMACC2Control_SI 0x4000000
#define DMACC2Control_SI_BIT 26
#define DMACC2Control_DI_MASK 0x8000000
#define DMACC2Control_DI 0x8000000
#define DMACC2Control_DI_BIT 27
#define DMACC2Control_Prot1_MASK 0x10000000
#define DMACC2Control_Prot1 0x10000000
#define DMACC2Control_Prot1_BIT 28
#define DMACC2Control_Prot2_MASK 0x20000000
#define DMACC2Control_Prot2 0x20000000
#define DMACC2Control_Prot2_BIT 29
#define DMACC2Control_Prot3_MASK 0x40000000
#define DMACC2Control_Prot3 0x40000000
#define DMACC2Control_Prot3_BIT 30
#define DMACC2Control_I_MASK 0x80000000
#define DMACC2Control_I 0x80000000
#define DMACC2Control_I_BIT 31

#define DMACC3Control (*(volatile unsigned *)0x5000416C)
#define DMACC3Control_OFFSET 0x16C
#define DMACC3Control_TransferSize_MASK 0xFFF
#define DMACC3Control_TransferSize_BIT 0
#define DMACC3Control_SBSize_MASK 0x7000
#define DMACC3Control_SBSize_BIT 12
#define DMACC3Control_DBSize_MASK 0x38000
#define DMACC3Control_DBSize_BIT 15
#define DMACC3Control_SWidth_MASK 0x1C0000
#define DMACC3Control_SWidth_BIT 18
#define DMACC3Control_DWidth_MASK 0xE00000
#define DMACC3Control_DWidth_BIT 21
#define DMACC3Control_SI_MASK 0x4000000
#define DMACC3Control_SI 0x4000000
#define DMACC3Control_SI_BIT 26
#define DMACC3Control_DI_MASK 0x8000000
#define DMACC3Control_DI 0x8000000
#define DMACC3Control_DI_BIT 27
#define DMACC3Control_Prot1_MASK 0x10000000
#define DMACC3Control_Prot1 0x10000000
#define DMACC3Control_Prot1_BIT 28
#define DMACC3Control_Prot2_MASK 0x20000000
#define DMACC3Control_Prot2 0x20000000
#define DMACC3Control_Prot2_BIT 29
#define DMACC3Control_Prot3_MASK 0x40000000
#define DMACC3Control_Prot3 0x40000000
#define DMACC3Control_Prot3_BIT 30
#define DMACC3Control_I_MASK 0x80000000
#define DMACC3Control_I 0x80000000
#define DMACC3Control_I_BIT 31

#define DMACC4Control (*(volatile unsigned *)0x5000418C)
#define DMACC4Control_OFFSET 0x18C
#define DMACC4Control_TransferSize_MASK 0xFFF
#define DMACC4Control_TransferSize_BIT 0
#define DMACC4Control_SBSize_MASK 0x7000
#define DMACC4Control_SBSize_BIT 12
#define DMACC4Control_DBSize_MASK 0x38000
#define DMACC4Control_DBSize_BIT 15
#define DMACC4Control_SWidth_MASK 0x1C0000
#define DMACC4Control_SWidth_BIT 18
#define DMACC4Control_DWidth_MASK 0xE00000
#define DMACC4Control_DWidth_BIT 21
#define DMACC4Control_SI_MASK 0x4000000
#define DMACC4Control_SI 0x4000000
#define DMACC4Control_SI_BIT 26
#define DMACC4Control_DI_MASK 0x8000000
#define DMACC4Control_DI 0x8000000
#define DMACC4Control_DI_BIT 27
#define DMACC4Control_Prot1_MASK 0x10000000
#define DMACC4Control_Prot1 0x10000000
#define DMACC4Control_Prot1_BIT 28
#define DMACC4Control_Prot2_MASK 0x20000000
#define DMACC4Control_Prot2 0x20000000
#define DMACC4Control_Prot2_BIT 29
#define DMACC4Control_Prot3_MASK 0x40000000
#define DMACC4Control_Prot3 0x40000000
#define DMACC4Control_Prot3_BIT 30
#define DMACC4Control_I_MASK 0x80000000
#define DMACC4Control_I 0x80000000
#define DMACC4Control_I_BIT 31

#define DMACC5Control (*(volatile unsigned *)0x500041AC)
#define DMACC5Control_OFFSET 0x1AC
#define DMACC5Control_TransferSize_MASK 0xFFF
#define DMACC5Control_TransferSize_BIT 0
#define DMACC5Control_SBSize_MASK 0x7000
#define DMACC5Control_SBSize_BIT 12
#define DMACC5Control_DBSize_MASK 0x38000
#define DMACC5Control_DBSize_BIT 15
#define DMACC5Control_SWidth_MASK 0x1C0000
#define DMACC5Control_SWidth_BIT 18
#define DMACC5Control_DWidth_MASK 0xE00000
#define DMACC5Control_DWidth_BIT 21
#define DMACC5Control_SI_MASK 0x4000000
#define DMACC5Control_SI 0x4000000
#define DMACC5Control_SI_BIT 26
#define DMACC5Control_DI_MASK 0x8000000
#define DMACC5Control_DI 0x8000000
#define DMACC5Control_DI_BIT 27
#define DMACC5Control_Prot1_MASK 0x10000000
#define DMACC5Control_Prot1 0x10000000
#define DMACC5Control_Prot1_BIT 28
#define DMACC5Control_Prot2_MASK 0x20000000
#define DMACC5Control_Prot2 0x20000000
#define DMACC5Control_Prot2_BIT 29
#define DMACC5Control_Prot3_MASK 0x40000000
#define DMACC5Control_Prot3 0x40000000
#define DMACC5Control_Prot3_BIT 30
#define DMACC5Control_I_MASK 0x80000000
#define DMACC5Control_I 0x80000000
#define DMACC5Control_I_BIT 31

#define DMACC6Control (*(volatile unsigned *)0x500041CC)
#define DMACC6Control_OFFSET 0x1CC
#define DMACC6Control_TransferSize_MASK 0xFFF
#define DMACC6Control_TransferSize_BIT 0
#define DMACC6Control_SBSize_MASK 0x7000
#define DMACC6Control_SBSize_BIT 12
#define DMACC6Control_DBSize_MASK 0x38000
#define DMACC6Control_DBSize_BIT 15
#define DMACC6Control_SWidth_MASK 0x1C0000
#define DMACC6Control_SWidth_BIT 18
#define DMACC6Control_DWidth_MASK 0xE00000
#define DMACC6Control_DWidth_BIT 21
#define DMACC6Control_SI_MASK 0x4000000
#define DMACC6Control_SI 0x4000000
#define DMACC6Control_SI_BIT 26
#define DMACC6Control_DI_MASK 0x8000000
#define DMACC6Control_DI 0x8000000
#define DMACC6Control_DI_BIT 27
#define DMACC6Control_Prot1_MASK 0x10000000
#define DMACC6Control_Prot1 0x10000000
#define DMACC6Control_Prot1_BIT 28
#define DMACC6Control_Prot2_MASK 0x20000000
#define DMACC6Control_Prot2 0x20000000
#define DMACC6Control_Prot2_BIT 29
#define DMACC6Control_Prot3_MASK 0x40000000
#define DMACC6Control_Prot3 0x40000000
#define DMACC6Control_Prot3_BIT 30
#define DMACC6Control_I_MASK 0x80000000
#define DMACC6Control_I 0x80000000
#define DMACC6Control_I_BIT 31

#define DMACC7Control (*(volatile unsigned *)0x500041EC)
#define DMACC7Control_OFFSET 0x1EC
#define DMACC7Control_TransferSize_MASK 0xFFF
#define DMACC7Control_TransferSize_BIT 0
#define DMACC7Control_SBSize_MASK 0x7000
#define DMACC7Control_SBSize_BIT 12
#define DMACC7Control_DBSize_MASK 0x38000
#define DMACC7Control_DBSize_BIT 15
#define DMACC7Control_SWidth_MASK 0x1C0000
#define DMACC7Control_SWidth_BIT 18
#define DMACC7Control_DWidth_MASK 0xE00000
#define DMACC7Control_DWidth_BIT 21
#define DMACC7Control_SI_MASK 0x4000000
#define DMACC7Control_SI 0x4000000
#define DMACC7Control_SI_BIT 26
#define DMACC7Control_DI_MASK 0x8000000
#define DMACC7Control_DI 0x8000000
#define DMACC7Control_DI_BIT 27
#define DMACC7Control_Prot1_MASK 0x10000000
#define DMACC7Control_Prot1 0x10000000
#define DMACC7Control_Prot1_BIT 28
#define DMACC7Control_Prot2_MASK 0x20000000
#define DMACC7Control_Prot2 0x20000000
#define DMACC7Control_Prot2_BIT 29
#define DMACC7Control_Prot3_MASK 0x40000000
#define DMACC7Control_Prot3 0x40000000
#define DMACC7Control_Prot3_BIT 30
#define DMACC7Control_I_MASK 0x80000000
#define DMACC7Control_I 0x80000000
#define DMACC7Control_I_BIT 31

#define DMACC0Config (*(volatile unsigned *)0x50004110)
#define DMACC0Config_OFFSET 0x110
#define DMACC0Config_E_MASK 0x1
#define DMACC0Config_E 0x1
#define DMACC0Config_E_BIT 0
#define DMACC0Config_SrcPeripheral_MASK 0x3E
#define DMACC0Config_SrcPeripheral_BIT 1
#define DMACC0Config_DestPeripheral_MASK 0x7C0
#define DMACC0Config_DestPeripheral_BIT 6
#define DMACC0Config_TransferType_MASK 0x3800
#define DMACC0Config_TransferType_BIT 11
#define DMACC0Config_IE_MASK 0x4000
#define DMACC0Config_IE 0x4000
#define DMACC0Config_IE_BIT 14
#define DMACC0Config_ITC_MASK 0x8000
#define DMACC0Config_ITC 0x8000
#define DMACC0Config_ITC_BIT 15
#define DMACC0Config_L_MASK 0x10000
#define DMACC0Config_L 0x10000
#define DMACC0Config_L_BIT 16
#define DMACC0Config_A_MASK 0x20000
#define DMACC0Config_A 0x20000
#define DMACC0Config_A_BIT 17
#define DMACC0Config_H_MASK 0x40000
#define DMACC0Config_H 0x40000
#define DMACC0Config_H_BIT 18

#define DMACC1Config (*(volatile unsigned *)0x50004130)
#define DMACC1Config_OFFSET 0x130
#define DMACC1Config_E_MASK 0x1
#define DMACC1Config_E 0x1
#define DMACC1Config_E_BIT 0
#define DMACC1Config_SrcPeripheral_MASK 0x3E
#define DMACC1Config_SrcPeripheral_BIT 1
#define DMACC1Config_DestPeripheral_MASK 0x7C0
#define DMACC1Config_DestPeripheral_BIT 6
#define DMACC1Config_TransferType_MASK 0x3800
#define DMACC1Config_TransferType_BIT 11
#define DMACC1Config_IE_MASK 0x4000
#define DMACC1Config_IE 0x4000
#define DMACC1Config_IE_BIT 14
#define DMACC1Config_ITC_MASK 0x8000
#define DMACC1Config_ITC 0x8000
#define DMACC1Config_ITC_BIT 15
#define DMACC1Config_L_MASK 0x10000
#define DMACC1Config_L 0x10000
#define DMACC1Config_L_BIT 16
#define DMACC1Config_A_MASK 0x20000
#define DMACC1Config_A 0x20000
#define DMACC1Config_A_BIT 17
#define DMACC1Config_H_MASK 0x40000
#define DMACC1Config_H 0x40000
#define DMACC1Config_H_BIT 18

#define DMACC2Config (*(volatile unsigned *)0x50004150)
#define DMACC2Config_OFFSET 0x150
#define DMACC2Config_E_MASK 0x1
#define DMACC2Config_E 0x1
#define DMACC2Config_E_BIT 0
#define DMACC2Config_SrcPeripheral_MASK 0x3E
#define DMACC2Config_SrcPeripheral_BIT 1
#define DMACC2Config_DestPeripheral_MASK 0x7C0
#define DMACC2Config_DestPeripheral_BIT 6
#define DMACC2Config_TransferType_MASK 0x3800
#define DMACC2Config_TransferType_BIT 11
#define DMACC2Config_IE_MASK 0x4000
#define DMACC2Config_IE 0x4000
#define DMACC2Config_IE_BIT 14
#define DMACC2Config_ITC_MASK 0x8000
#define DMACC2Config_ITC 0x8000
#define DMACC2Config_ITC_BIT 15
#define DMACC2Config_L_MASK 0x10000
#define DMACC2Config_L 0x10000
#define DMACC2Config_L_BIT 16
#define DMACC2Config_A_MASK 0x20000
#define DMACC2Config_A 0x20000
#define DMACC2Config_A_BIT 17
#define DMACC2Config_H_MASK 0x40000
#define DMACC2Config_H 0x40000
#define DMACC2Config_H_BIT 18

#define DMACC3Config (*(volatile unsigned *)0x50004170)
#define DMACC3Config_OFFSET 0x170
#define DMACC3Config_E_MASK 0x1
#define DMACC3Config_E 0x1
#define DMACC3Config_E_BIT 0
#define DMACC3Config_SrcPeripheral_MASK 0x3E
#define DMACC3Config_SrcPeripheral_BIT 1
#define DMACC3Config_DestPeripheral_MASK 0x7C0
#define DMACC3Config_DestPeripheral_BIT 6
#define DMACC3Config_TransferType_MASK 0x3800
#define DMACC3Config_TransferType_BIT 11
#define DMACC3Config_IE_MASK 0x4000
#define DMACC3Config_IE 0x4000
#define DMACC3Config_IE_BIT 14
#define DMACC3Config_ITC_MASK 0x8000
#define DMACC3Config_ITC 0x8000
#define DMACC3Config_ITC_BIT 15
#define DMACC3Config_L_MASK 0x10000
#define DMACC3Config_L 0x10000
#define DMACC3Config_L_BIT 16
#define DMACC3Config_A_MASK 0x20000
#define DMACC3Config_A 0x20000
#define DMACC3Config_A_BIT 17
#define DMACC3Config_H_MASK 0x40000
#define DMACC3Config_H 0x40000
#define DMACC3Config_H_BIT 18

#define DMACC4Config (*(volatile unsigned *)0x50004190)
#define DMACC4Config_OFFSET 0x190
#define DMACC4Config_E_MASK 0x1
#define DMACC4Config_E 0x1
#define DMACC4Config_E_BIT 0
#define DMACC4Config_SrcPeripheral_MASK 0x3E
#define DMACC4Config_SrcPeripheral_BIT 1
#define DMACC4Config_DestPeripheral_MASK 0x7C0
#define DMACC4Config_DestPeripheral_BIT 6
#define DMACC4Config_TransferType_MASK 0x3800
#define DMACC4Config_TransferType_BIT 11
#define DMACC4Config_IE_MASK 0x4000
#define DMACC4Config_IE 0x4000
#define DMACC4Config_IE_BIT 14
#define DMACC4Config_ITC_MASK 0x8000
#define DMACC4Config_ITC 0x8000
#define DMACC4Config_ITC_BIT 15
#define DMACC4Config_L_MASK 0x10000
#define DMACC4Config_L 0x10000
#define DMACC4Config_L_BIT 16
#define DMACC4Config_A_MASK 0x20000
#define DMACC4Config_A 0x20000
#define DMACC4Config_A_BIT 17
#define DMACC4Config_H_MASK 0x40000
#define DMACC4Config_H 0x40000
#define DMACC4Config_H_BIT 18

#define DMACC5Config (*(volatile unsigned *)0x500041B0)
#define DMACC5Config_OFFSET 0x1B0
#define DMACC5Config_E_MASK 0x1
#define DMACC5Config_E 0x1
#define DMACC5Config_E_BIT 0
#define DMACC5Config_SrcPeripheral_MASK 0x3E
#define DMACC5Config_SrcPeripheral_BIT 1
#define DMACC5Config_DestPeripheral_MASK 0x7C0
#define DMACC5Config_DestPeripheral_BIT 6
#define DMACC5Config_TransferType_MASK 0x3800
#define DMACC5Config_TransferType_BIT 11
#define DMACC5Config_IE_MASK 0x4000
#define DMACC5Config_IE 0x4000
#define DMACC5Config_IE_BIT 14
#define DMACC5Config_ITC_MASK 0x8000
#define DMACC5Config_ITC 0x8000
#define DMACC5Config_ITC_BIT 15
#define DMACC5Config_L_MASK 0x10000
#define DMACC5Config_L 0x10000
#define DMACC5Config_L_BIT 16
#define DMACC5Config_A_MASK 0x20000
#define DMACC5Config_A 0x20000
#define DMACC5Config_A_BIT 17
#define DMACC5Config_H_MASK 0x40000
#define DMACC5Config_H 0x40000
#define DMACC5Config_H_BIT 18

#define DMACC6Config (*(volatile unsigned *)0x500041D0)
#define DMACC6Config_OFFSET 0x1D0
#define DMACC6Config_E_MASK 0x1
#define DMACC6Config_E 0x1
#define DMACC6Config_E_BIT 0
#define DMACC6Config_SrcPeripheral_MASK 0x3E
#define DMACC6Config_SrcPeripheral_BIT 1
#define DMACC6Config_DestPeripheral_MASK 0x7C0
#define DMACC6Config_DestPeripheral_BIT 6
#define DMACC6Config_TransferType_MASK 0x3800
#define DMACC6Config_TransferType_BIT 11
#define DMACC6Config_IE_MASK 0x4000
#define DMACC6Config_IE 0x4000
#define DMACC6Config_IE_BIT 14
#define DMACC6Config_ITC_MASK 0x8000
#define DMACC6Config_ITC 0x8000
#define DMACC6Config_ITC_BIT 15
#define DMACC6Config_L_MASK 0x10000
#define DMACC6Config_L 0x10000
#define DMACC6Config_L_BIT 16
#define DMACC6Config_A_MASK 0x20000
#define DMACC6Config_A 0x20000
#define DMACC6Config_A_BIT 17
#define DMACC6Config_H_MASK 0x40000
#define DMACC6Config_H 0x40000
#define DMACC6Config_H_BIT 18

#define DMACC7Config (*(volatile unsigned *)0x500041F0)
#define DMACC7Config_OFFSET 0x1F0
#define DMACC7Config_E_MASK 0x1
#define DMACC7Config_E 0x1
#define DMACC7Config_E_BIT 0
#define DMACC7Config_SrcPeripheral_MASK 0x3E
#define DMACC7Config_SrcPeripheral_BIT 1
#define DMACC7Config_DestPeripheral_MASK 0x7C0
#define DMACC7Config_DestPeripheral_BIT 6
#define DMACC7Config_TransferType_MASK 0x3800
#define DMACC7Config_TransferType_BIT 11
#define DMACC7Config_IE_MASK 0x4000
#define DMACC7Config_IE 0x4000
#define DMACC7Config_IE_BIT 14
#define DMACC7Config_ITC_MASK 0x8000
#define DMACC7Config_ITC 0x8000
#define DMACC7Config_ITC_BIT 15
#define DMACC7Config_L_MASK 0x10000
#define DMACC7Config_L 0x10000
#define DMACC7Config_L_BIT 16
#define DMACC7Config_A_MASK 0x20000
#define DMACC7Config_A 0x20000
#define DMACC7Config_A_BIT 17
#define DMACC7Config_H_MASK 0x40000
#define DMACC7Config_H 0x40000
#define DMACC7Config_H_BIT 18

#define USB_BASE_ADDRESS 0x5000C000

#define USBPortSel (*(volatile unsigned *)0x5000C110)
#define USBPortSel_OFFSET 0x110

#define USBDevIntSt (*(volatile unsigned *)0x5000C200)
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

#define USBDevIntEn (*(volatile unsigned *)0x5000C204)
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

#define USBDevIntClr (*(volatile unsigned *)0x5000C208)
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

#define USBDevIntSet (*(volatile unsigned *)0x5000C20C)
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

#define USBCmdCode (*(volatile unsigned *)0x5000C210)
#define USBCmdCode_OFFSET 0x210
#define USBCmdCode_CMD_PHASE_MASK 0xFF00
#define USBCmdCode_CMD_PHASE_BIT 8
#define USBCmdCode_CMD_CODE_MASK 0x7F0000
#define USBCmdCode_CMD_CODE_BIT 16
#define USBCmdCode_CMD_WDATA_MASK 0x7F0000
#define USBCmdCode_CMD_WDATA_BIT 16

#define USBCmdData (*(volatile unsigned *)0x5000C214)
#define USBCmdData_OFFSET 0x214
#define USBCmdData_CMD_RDATA_MASK 0xFF
#define USBCmdData_CMD_RDATA_BIT 0

#define USBRxData (*(volatile unsigned *)0x5000C218)
#define USBRxData_OFFSET 0x218

#define USBTxData (*(volatile unsigned *)0x5000C21C)
#define USBTxData_OFFSET 0x21C

#define USBRxPLen (*(volatile unsigned *)0x5000C220)
#define USBRxPLen_OFFSET 0x220
#define USBRxPLen_PKT_LNGTH_MASK 0x3FF
#define USBRxPLen_PKT_LNGTH_BIT 0
#define USBRxPLen_DV_MASK 0x400
#define USBRxPLen_DV 0x400
#define USBRxPLen_DV_BIT 10
#define USBRxPLen_PKT_RDY_MASK 0x800
#define USBRxPLen_PKT_RDY 0x800
#define USBRxPLen_PKT_RDY_BIT 11

#define USBTxPLen (*(volatile unsigned *)0x5000C224)
#define USBTxPLen_OFFSET 0x224
#define USBTxPLen_PKT_LNGTH_MASK 0x3FF
#define USBTxPLen_PKT_LNGTH_BIT 0

#define USBCtrl (*(volatile unsigned *)0x5000C228)
#define USBCtrl_OFFSET 0x228
#define USBCtrl_RD_EN_MASK 0x1
#define USBCtrl_RD_EN 0x1
#define USBCtrl_RD_EN_BIT 0
#define USBCtrl_WR_EN_MASK 0x2
#define USBCtrl_WR_EN 0x2
#define USBCtrl_WR_EN_BIT 1
#define USBCtrl_LOG_ENDPOINT_MASK 0x3C
#define USBCtrl_LOG_ENDPOINT_BIT 2

#define USBDevIntPri (*(volatile unsigned *)0x5000C22C)
#define USBDevIntPri_OFFSET 0x22C
#define USBDevIntPri_FRAME_MASK 0x1
#define USBDevIntPri_FRAME 0x1
#define USBDevIntPri_FRAME_BIT 0
#define USBDevIntPri_EP_FAST_MASK 0x2
#define USBDevIntPri_EP_FAST 0x2
#define USBDevIntPri_EP_FAST_BIT 1

#define USBEpIntSt (*(volatile unsigned *)0x5000C230)
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

#define USBEpIntEn (*(volatile unsigned *)0x5000C234)
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

#define USBEpIntClr (*(volatile unsigned *)0x5000C238)
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

#define USBEpIntSet (*(volatile unsigned *)0x5000C23C)
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

#define USBEpIntPri (*(volatile unsigned *)0x5000C240)
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

#define USBReEp (*(volatile unsigned *)0x5000C244)
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

#define USBEpInd (*(volatile unsigned *)0x5000C248)
#define USBEpInd_OFFSET 0x248
#define USBEpInd_PHY_EP_MASK 0x1F
#define USBEpInd_PHY_EP_BIT 0

#define USBEpMaxPSize (*(volatile unsigned *)0x5000C24C)
#define USBEpMaxPSize_OFFSET 0x24C
#define USBEpMaxPSize_MPS_MASK 0x3FF
#define USBEpMaxPSize_MPS_BIT 0

#define USBDMARSt (*(volatile unsigned *)0x5000C250)
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

#define USBDMARClr (*(volatile unsigned *)0x5000C254)
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

#define USBDMARSet (*(volatile unsigned *)0x5000C258)
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

#define USBUDCAH (*(volatile unsigned *)0x5000C280)
#define USBUDCAH_OFFSET 0x280
#define USBUDCAH_UDCA_ADDR_MASK 0xFFFFFF80
#define USBUDCAH_UDCA_ADDR_BIT 7

#define USBEpDMASt (*(volatile unsigned *)0x5000C284)
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

#define USBEpDMAEn (*(volatile unsigned *)0x5000C288)
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

#define USBEpDMADis (*(volatile unsigned *)0x5000C28C)
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

#define USBDMAIntSt (*(volatile unsigned *)0x5000C290)
#define USBDMAIntSt_OFFSET 0x290
#define USBDMAIntSt_EOT_MASK 0x1
#define USBDMAIntSt_EOT 0x1
#define USBDMAIntSt_EOT_BIT 0
#define USBDMAIntSt_NDDR_MASK 0x2
#define USBDMAIntSt_NDDR 0x2
#define USBDMAIntSt_NDDR_BIT 1
#define USBDMAIntSt_ERR_MASK 0x4
#define USBDMAIntSt_ERR 0x4
#define USBDMAIntSt_ERR_BIT 2

#define USBDMAIntEn (*(volatile unsigned *)0x5000C294)
#define USBDMAIntEn_OFFSET 0x294
#define USBDMAIntEn_EOT_MASK 0x1
#define USBDMAIntEn_EOT 0x1
#define USBDMAIntEn_EOT_BIT 0
#define USBDMAIntEn_NDDR_MASK 0x2
#define USBDMAIntEn_NDDR 0x2
#define USBDMAIntEn_NDDR_BIT 1
#define USBDMAIntEn_ERR_MASK 0x4
#define USBDMAIntEn_ERR 0x4
#define USBDMAIntEn_ERR_BIT 2

#define USBEoTIntSt (*(volatile unsigned *)0x5000C2A0)
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

#define USBEoTIntClr (*(volatile unsigned *)0x5000C2A4)
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

#define USBEoTIntSet (*(volatile unsigned *)0x5000C2A8)
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

#define USBNDDRIntSt (*(volatile unsigned *)0x5000C2AC)
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

#define USBNDDRIntClr (*(volatile unsigned *)0x5000C2B0)
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

#define USBNDDRIntSet (*(volatile unsigned *)0x5000C2B4)
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

#define USBSysErrIntSt (*(volatile unsigned *)0x5000C2B8)
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

#define USBSysErrIntClr (*(volatile unsigned *)0x5000C2BC)
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

#define USBSysErrIntSet (*(volatile unsigned *)0x5000C2C0)
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

#define USBClkCtrl (*(volatile unsigned *)0x5000CFF4)
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

#define USBClkSt (*(volatile unsigned *)0x5000CFF8)
#define USBClkSt_OFFSET 0xFF8
#define USBClkSt_DEV_CLK_ON_MASK 0x2
#define USBClkSt_DEV_CLK_ON 0x2
#define USBClkSt_DEV_CLK_ON_BIT 1
#define USBClkSt_PORTSEL_CLK_ON_MASK 0x8
#define USBClkSt_PORTSEL_CLK_ON 0x8
#define USBClkSt_PORTSEL_CLK_ON_BIT 3
#define USBClkSt_AHB_CLK_ON_MASK 0x10
#define USBClkSt_AHB_CLK_ON 0x10
#define USBClkSt_AHB_CLK_ON_BIT 4

#define InterruptType_BASE_ADDRESS 0xE000E000

#define InterruptType_ICTR (*(volatile unsigned *)0xE000E004)
#define Interrupt_Control_Type InterruptType_ICTR
#define InterruptType_ICTR_OFFSET 0x4
#define Interrupt_Control_Type_OFFSET InterruptType_ICTR_OFFSET
#define InterruptType_ICTR_INTLINESNUM_MASK 0x1F
#define Interrupt_Control_Type_INTLINESNUM_MASK InterruptType_ICTR_INTLINESNUM_MASK
#define InterruptType_ICTR_INTLINESNUM_BIT 0
#define Interrupt_Control_Type_INTLINESNUM_BIT InterruptType_ICTR_INTLINESNUM_BIT

#define SysTick_BASE_ADDRESS 0xE000E010

#define SysTick_CTRL (*(volatile unsigned *)0xE000E010)
#define SysTick_Control_And_Status SysTick_CTRL
#define SysTick_CTRL_OFFSET 0x0
#define SysTick_Control_And_Status_OFFSET SysTick_CTRL_OFFSET
#define SysTick_CTRL_COUNTFLAG_MASK 0x10000
#define SysTick_Control_And_Status_COUNTFLAG_MASK SysTick_CTRL_COUNTFLAG_MASK
#define SysTick_CTRL_COUNTFLAG 0x10000
#define SysTick_Control_And_Status_COUNTFLAG SysTick_CTRL_COUNTFLAG
#define SysTick_CTRL_COUNTFLAG_BIT 16
#define SysTick_Control_And_Status_COUNTFLAG_BIT SysTick_CTRL_COUNTFLAG_BIT
#define SysTick_CTRL_CLKSOURCE_MASK 0x4
#define SysTick_Control_And_Status_CLKSOURCE_MASK SysTick_CTRL_CLKSOURCE_MASK
#define SysTick_CTRL_CLKSOURCE 0x4
#define SysTick_Control_And_Status_CLKSOURCE SysTick_CTRL_CLKSOURCE
#define SysTick_CTRL_CLKSOURCE_BIT 2
#define SysTick_Control_And_Status_CLKSOURCE_BIT SysTick_CTRL_CLKSOURCE_BIT
#define SysTick_CTRL_TICKINT_MASK 0x2
#define SysTick_Control_And_Status_TICKINT_MASK SysTick_CTRL_TICKINT_MASK
#define SysTick_CTRL_TICKINT 0x2
#define SysTick_Control_And_Status_TICKINT SysTick_CTRL_TICKINT
#define SysTick_CTRL_TICKINT_BIT 1
#define SysTick_Control_And_Status_TICKINT_BIT SysTick_CTRL_TICKINT_BIT
#define SysTick_CTRL_ENABLE_MASK 0x1
#define SysTick_Control_And_Status_ENABLE_MASK SysTick_CTRL_ENABLE_MASK
#define SysTick_CTRL_ENABLE 0x1
#define SysTick_Control_And_Status_ENABLE SysTick_CTRL_ENABLE
#define SysTick_CTRL_ENABLE_BIT 0
#define SysTick_Control_And_Status_ENABLE_BIT SysTick_CTRL_ENABLE_BIT

#define SysTick_LOAD (*(volatile unsigned *)0xE000E014)
#define SysTick_Reload_Value SysTick_LOAD
#define SysTick_LOAD_OFFSET 0x4
#define SysTick_Reload_Value_OFFSET SysTick_LOAD_OFFSET
#define SysTick_LOAD_RELOAD_MASK 0xFFFFFF
#define SysTick_Reload_Value_RELOAD_MASK SysTick_LOAD_RELOAD_MASK
#define SysTick_LOAD_RELOAD_BIT 0
#define SysTick_Reload_Value_RELOAD_BIT SysTick_LOAD_RELOAD_BIT

#define SysTick_VAL (*(volatile unsigned *)0xE000E018)
#define SysTick_Current_Value SysTick_VAL
#define SysTick_VAL_OFFSET 0x8
#define SysTick_Current_Value_OFFSET SysTick_VAL_OFFSET
#define SysTick_VAL_CURRENT_MASK 0xFFFFFF
#define SysTick_Current_Value_CURRENT_MASK SysTick_VAL_CURRENT_MASK
#define SysTick_VAL_CURRENT_BIT 0
#define SysTick_Current_Value_CURRENT_BIT SysTick_VAL_CURRENT_BIT

#define SysTick_CALIB (*(volatile unsigned *)0xE000E01C)
#define SysTick_Calibration_Value SysTick_CALIB
#define SysTick_CALIB_OFFSET 0xC
#define SysTick_Calibration_Value_OFFSET SysTick_CALIB_OFFSET
#define SysTick_CALIB_NOREF_MASK 0x80000000
#define SysTick_Calibration_Value_NOREF_MASK SysTick_CALIB_NOREF_MASK
#define SysTick_CALIB_NOREF 0x80000000
#define SysTick_Calibration_Value_NOREF SysTick_CALIB_NOREF
#define SysTick_CALIB_NOREF_BIT 31
#define SysTick_Calibration_Value_NOREF_BIT SysTick_CALIB_NOREF_BIT
#define SysTick_CALIB_SKEW_MASK 0x40000000
#define SysTick_Calibration_Value_SKEW_MASK SysTick_CALIB_SKEW_MASK
#define SysTick_CALIB_SKEW 0x40000000
#define SysTick_Calibration_Value_SKEW SysTick_CALIB_SKEW
#define SysTick_CALIB_SKEW_BIT 30
#define SysTick_Calibration_Value_SKEW_BIT SysTick_CALIB_SKEW_BIT
#define SysTick_CALIB_TENMS_MASK 0xFFFFFF
#define SysTick_Calibration_Value_TENMS_MASK SysTick_CALIB_TENMS_MASK
#define SysTick_CALIB_TENMS_BIT 0
#define SysTick_Calibration_Value_TENMS_BIT SysTick_CALIB_TENMS_BIT

#define NVIC_BASE_ADDRESS 0xE000E100

#define NVIC_ISER0 (*(volatile unsigned *)0xE000E100)
#define Irq_0_to_31_Set_Enable NVIC_ISER0
#define NVIC_ISER0_OFFSET 0x0
#define Irq_0_to_31_Set_Enable_OFFSET NVIC_ISER0_OFFSET

#define NVIC_ISER1 (*(volatile unsigned *)0xE000E104)
#define Irq_32_to_63_Set_Enable NVIC_ISER1
#define NVIC_ISER1_OFFSET 0x4
#define Irq_32_to_63_Set_Enable_OFFSET NVIC_ISER1_OFFSET

#define NVIC_ISER2 (*(volatile unsigned *)0xE000E108)
#define Irq_64_to_95_Set_Enable NVIC_ISER2
#define NVIC_ISER2_OFFSET 0x8
#define Irq_64_to_95_Set_Enable_OFFSET NVIC_ISER2_OFFSET

#define NVIC_ISER3 (*(volatile unsigned *)0xE000E10C)
#define Irq_96_to_127_Set_Enable NVIC_ISER3
#define NVIC_ISER3_OFFSET 0xC
#define Irq_96_to_127_Set_Enable_OFFSET NVIC_ISER3_OFFSET

#define NVIC_ICER0 (*(volatile unsigned *)0xE000E180)
#define Irq_0_to_31_Clear_Enable NVIC_ICER0
#define NVIC_ICER0_OFFSET 0x80
#define Irq_0_to_31_Clear_Enable_OFFSET NVIC_ICER0_OFFSET

#define NVIC_ICER1 (*(volatile unsigned *)0xE000E184)
#define Irq_32_to_63_Clear_Enable NVIC_ICER1
#define NVIC_ICER1_OFFSET 0x84
#define Irq_32_to_63_Clear_Enable_OFFSET NVIC_ICER1_OFFSET

#define NVIC_ICER2 (*(volatile unsigned *)0xE000E188)
#define Irq_64_to_95_Clear_Enable NVIC_ICER2
#define NVIC_ICER2_OFFSET 0x88
#define Irq_64_to_95_Clear_Enable_OFFSET NVIC_ICER2_OFFSET

#define NVIC_ICER3 (*(volatile unsigned *)0xE000E18C)
#define Irq_96_to_127_Clear_Enable NVIC_ICER3
#define NVIC_ICER3_OFFSET 0x8C
#define Irq_96_to_127_Clear_Enable_OFFSET NVIC_ICER3_OFFSET

#define NVIC_ISPR0 (*(volatile unsigned *)0xE000E200)
#define Irq_0_to_31_Set_Pending NVIC_ISPR0
#define NVIC_ISPR0_OFFSET 0x100
#define Irq_0_to_31_Set_Pending_OFFSET NVIC_ISPR0_OFFSET

#define NVIC_ISPR1 (*(volatile unsigned *)0xE000E204)
#define Irq_32_to_63_Set_Pending NVIC_ISPR1
#define NVIC_ISPR1_OFFSET 0x104
#define Irq_32_to_63_Set_Pending_OFFSET NVIC_ISPR1_OFFSET

#define NVIC_ISPR2 (*(volatile unsigned *)0xE000E208)
#define Irq_64_to_95_Set_Pending NVIC_ISPR2
#define NVIC_ISPR2_OFFSET 0x108
#define Irq_64_to_95_Set_Pending_OFFSET NVIC_ISPR2_OFFSET

#define NVIC_ISPR3 (*(volatile unsigned *)0xE000E20C)
#define Irq_96_to_127_Set_Pending NVIC_ISPR3
#define NVIC_ISPR3_OFFSET 0x10C
#define Irq_96_to_127_Set_Pending_OFFSET NVIC_ISPR3_OFFSET

#define NVIC_ICPR0 (*(volatile unsigned *)0xE000E280)
#define Irq_0_to_31_Clear_Pending NVIC_ICPR0
#define NVIC_ICPR0_OFFSET 0x180
#define Irq_0_to_31_Clear_Pending_OFFSET NVIC_ICPR0_OFFSET

#define NVIC_ICPR1 (*(volatile unsigned *)0xE000E284)
#define Irq_32_to_63_Clear_Pending NVIC_ICPR1
#define NVIC_ICPR1_OFFSET 0x184
#define Irq_32_to_63_Clear_Pending_OFFSET NVIC_ICPR1_OFFSET

#define NVIC_ICPR2 (*(volatile unsigned *)0xE000E288)
#define Irq_64_to_95_Clear_Pending NVIC_ICPR2
#define NVIC_ICPR2_OFFSET 0x188
#define Irq_64_to_95_Clear_Pending_OFFSET NVIC_ICPR2_OFFSET

#define NVIC_ICPR3 (*(volatile unsigned *)0xE000E28C)
#define Irq_96_to_127_Clear_Pending NVIC_ICPR3
#define NVIC_ICPR3_OFFSET 0x18C
#define Irq_96_to_127_Clear_Pending_OFFSET NVIC_ICPR3_OFFSET

#define NVIC_IABR0 (*(volatile unsigned *)0xE000E300)
#define Irq_0_to_31_Active_Bit NVIC_IABR0
#define NVIC_IABR0_OFFSET 0x200
#define Irq_0_to_31_Active_Bit_OFFSET NVIC_IABR0_OFFSET

#define NVIC_IABR1 (*(volatile unsigned *)0xE000E304)
#define Irq_32_to_63_Active_Bit NVIC_IABR1
#define NVIC_IABR1_OFFSET 0x204
#define Irq_32_to_63_Active_Bit_OFFSET NVIC_IABR1_OFFSET

#define NVIC_IABR2 (*(volatile unsigned *)0xE000E308)
#define Irq_64_to_95_Active_Bit NVIC_IABR2
#define NVIC_IABR2_OFFSET 0x208
#define Irq_64_to_95_Active_Bit_OFFSET NVIC_IABR2_OFFSET

#define NVIC_IABR3 (*(volatile unsigned *)0xE000E30C)
#define Irq_96_to_127_Active_Bit NVIC_IABR3
#define NVIC_IABR3_OFFSET 0x20C
#define Irq_96_to_127_Active_Bit_OFFSET NVIC_IABR3_OFFSET

#define NVIC_IPR0 (*(volatile unsigned *)0xE000E400)
#define Irq_0_to_3_Priority NVIC_IPR0
#define NVIC_IPR0_OFFSET 0x300
#define Irq_0_to_3_Priority_OFFSET NVIC_IPR0_OFFSET
#define NVIC_IPR0_PRI_0_MASK 0xFF
#define Irq_0_to_3_Priority_PRI_0_MASK NVIC_IPR0_PRI_0_MASK
#define NVIC_IPR0_PRI_0_BIT 0
#define Irq_0_to_3_Priority_PRI_0_BIT NVIC_IPR0_PRI_0_BIT
#define NVIC_IPR0_PRI_1_MASK 0xFF00
#define Irq_0_to_3_Priority_PRI_1_MASK NVIC_IPR0_PRI_1_MASK
#define NVIC_IPR0_PRI_1_BIT 8
#define Irq_0_to_3_Priority_PRI_1_BIT NVIC_IPR0_PRI_1_BIT
#define NVIC_IPR0_PRI_2_MASK 0xFF0000
#define Irq_0_to_3_Priority_PRI_2_MASK NVIC_IPR0_PRI_2_MASK
#define NVIC_IPR0_PRI_2_BIT 16
#define Irq_0_to_3_Priority_PRI_2_BIT NVIC_IPR0_PRI_2_BIT
#define NVIC_IPR0_PRI_3_MASK 0xFF000000
#define Irq_0_to_3_Priority_PRI_3_MASK NVIC_IPR0_PRI_3_MASK
#define NVIC_IPR0_PRI_3_BIT 24
#define Irq_0_to_3_Priority_PRI_3_BIT NVIC_IPR0_PRI_3_BIT

#define NVIC_IPR1 (*(volatile unsigned *)0xE000E404)
#define Irq_4_to_7_Priority NVIC_IPR1
#define NVIC_IPR1_OFFSET 0x304
#define Irq_4_to_7_Priority_OFFSET NVIC_IPR1_OFFSET
#define NVIC_IPR1_PRI_4_MASK 0xFF
#define Irq_4_to_7_Priority_PRI_4_MASK NVIC_IPR1_PRI_4_MASK
#define NVIC_IPR1_PRI_4_BIT 0
#define Irq_4_to_7_Priority_PRI_4_BIT NVIC_IPR1_PRI_4_BIT
#define NVIC_IPR1_PRI_5_MASK 0xFF00
#define Irq_4_to_7_Priority_PRI_5_MASK NVIC_IPR1_PRI_5_MASK
#define NVIC_IPR1_PRI_5_BIT 8
#define Irq_4_to_7_Priority_PRI_5_BIT NVIC_IPR1_PRI_5_BIT
#define NVIC_IPR1_PRI_6_MASK 0xFF0000
#define Irq_4_to_7_Priority_PRI_6_MASK NVIC_IPR1_PRI_6_MASK
#define NVIC_IPR1_PRI_6_BIT 16
#define Irq_4_to_7_Priority_PRI_6_BIT NVIC_IPR1_PRI_6_BIT
#define NVIC_IPR1_PRI_7_MASK 0xFF000000
#define Irq_4_to_7_Priority_PRI_7_MASK NVIC_IPR1_PRI_7_MASK
#define NVIC_IPR1_PRI_7_BIT 24
#define Irq_4_to_7_Priority_PRI_7_BIT NVIC_IPR1_PRI_7_BIT

#define NVIC_IPR2 (*(volatile unsigned *)0xE000E408)
#define Irq_8_to_11_Priority NVIC_IPR2
#define NVIC_IPR2_OFFSET 0x308
#define Irq_8_to_11_Priority_OFFSET NVIC_IPR2_OFFSET
#define NVIC_IPR2_PRI_8_MASK 0xFF
#define Irq_8_to_11_Priority_PRI_8_MASK NVIC_IPR2_PRI_8_MASK
#define NVIC_IPR2_PRI_8_BIT 0
#define Irq_8_to_11_Priority_PRI_8_BIT NVIC_IPR2_PRI_8_BIT
#define NVIC_IPR2_PRI_9_MASK 0xFF00
#define Irq_8_to_11_Priority_PRI_9_MASK NVIC_IPR2_PRI_9_MASK
#define NVIC_IPR2_PRI_9_BIT 8
#define Irq_8_to_11_Priority_PRI_9_BIT NVIC_IPR2_PRI_9_BIT
#define NVIC_IPR2_PRI_10_MASK 0xFF0000
#define Irq_8_to_11_Priority_PRI_10_MASK NVIC_IPR2_PRI_10_MASK
#define NVIC_IPR2_PRI_10_BIT 16
#define Irq_8_to_11_Priority_PRI_10_BIT NVIC_IPR2_PRI_10_BIT
#define NVIC_IPR2_PRI_11_MASK 0xFF000000
#define Irq_8_to_11_Priority_PRI_11_MASK NVIC_IPR2_PRI_11_MASK
#define NVIC_IPR2_PRI_11_BIT 24
#define Irq_8_to_11_Priority_PRI_11_BIT NVIC_IPR2_PRI_11_BIT

#define NVIC_IPR3 (*(volatile unsigned *)0xE000E40C)
#define Irq_12_to_15_Priority NVIC_IPR3
#define NVIC_IPR3_OFFSET 0x30C
#define Irq_12_to_15_Priority_OFFSET NVIC_IPR3_OFFSET
#define NVIC_IPR3_PRI_12_MASK 0xFF
#define Irq_12_to_15_Priority_PRI_12_MASK NVIC_IPR3_PRI_12_MASK
#define NVIC_IPR3_PRI_12_BIT 0
#define Irq_12_to_15_Priority_PRI_12_BIT NVIC_IPR3_PRI_12_BIT
#define NVIC_IPR3_PRI_13_MASK 0xFF00
#define Irq_12_to_15_Priority_PRI_13_MASK NVIC_IPR3_PRI_13_MASK
#define NVIC_IPR3_PRI_13_BIT 8
#define Irq_12_to_15_Priority_PRI_13_BIT NVIC_IPR3_PRI_13_BIT
#define NVIC_IPR3_PRI_14_MASK 0xFF0000
#define Irq_12_to_15_Priority_PRI_14_MASK NVIC_IPR3_PRI_14_MASK
#define NVIC_IPR3_PRI_14_BIT 16
#define Irq_12_to_15_Priority_PRI_14_BIT NVIC_IPR3_PRI_14_BIT
#define NVIC_IPR3_PRI_15_MASK 0xFF000000
#define Irq_12_to_15_Priority_PRI_15_MASK NVIC_IPR3_PRI_15_MASK
#define NVIC_IPR3_PRI_15_BIT 24
#define Irq_12_to_15_Priority_PRI_15_BIT NVIC_IPR3_PRI_15_BIT

#define NVIC_IPR4 (*(volatile unsigned *)0xE000E410)
#define Irq_16_to_19_Priority NVIC_IPR4
#define NVIC_IPR4_OFFSET 0x310
#define Irq_16_to_19_Priority_OFFSET NVIC_IPR4_OFFSET
#define NVIC_IPR4_PRI_16_MASK 0xFF
#define Irq_16_to_19_Priority_PRI_16_MASK NVIC_IPR4_PRI_16_MASK
#define NVIC_IPR4_PRI_16_BIT 0
#define Irq_16_to_19_Priority_PRI_16_BIT NVIC_IPR4_PRI_16_BIT
#define NVIC_IPR4_PRI_17_MASK 0xFF00
#define Irq_16_to_19_Priority_PRI_17_MASK NVIC_IPR4_PRI_17_MASK
#define NVIC_IPR4_PRI_17_BIT 8
#define Irq_16_to_19_Priority_PRI_17_BIT NVIC_IPR4_PRI_17_BIT
#define NVIC_IPR4_PRI_18_MASK 0xFF0000
#define Irq_16_to_19_Priority_PRI_18_MASK NVIC_IPR4_PRI_18_MASK
#define NVIC_IPR4_PRI_18_BIT 16
#define Irq_16_to_19_Priority_PRI_18_BIT NVIC_IPR4_PRI_18_BIT
#define NVIC_IPR4_PRI_19_MASK 0xFF000000
#define Irq_16_to_19_Priority_PRI_19_MASK NVIC_IPR4_PRI_19_MASK
#define NVIC_IPR4_PRI_19_BIT 24
#define Irq_16_to_19_Priority_PRI_19_BIT NVIC_IPR4_PRI_19_BIT

#define NVIC_IPR5 (*(volatile unsigned *)0xE000E414)
#define Irq_20_to_23_Priority NVIC_IPR5
#define NVIC_IPR5_OFFSET 0x314
#define Irq_20_to_23_Priority_OFFSET NVIC_IPR5_OFFSET
#define NVIC_IPR5_PRI_20_MASK 0xFF
#define Irq_20_to_23_Priority_PRI_20_MASK NVIC_IPR5_PRI_20_MASK
#define NVIC_IPR5_PRI_20_BIT 0
#define Irq_20_to_23_Priority_PRI_20_BIT NVIC_IPR5_PRI_20_BIT
#define NVIC_IPR5_PRI_21_MASK 0xFF00
#define Irq_20_to_23_Priority_PRI_21_MASK NVIC_IPR5_PRI_21_MASK
#define NVIC_IPR5_PRI_21_BIT 8
#define Irq_20_to_23_Priority_PRI_21_BIT NVIC_IPR5_PRI_21_BIT
#define NVIC_IPR5_PRI_22_MASK 0xFF0000
#define Irq_20_to_23_Priority_PRI_22_MASK NVIC_IPR5_PRI_22_MASK
#define NVIC_IPR5_PRI_22_BIT 16
#define Irq_20_to_23_Priority_PRI_22_BIT NVIC_IPR5_PRI_22_BIT
#define NVIC_IPR5_PRI_23_MASK 0xFF000000
#define Irq_20_to_23_Priority_PRI_23_MASK NVIC_IPR5_PRI_23_MASK
#define NVIC_IPR5_PRI_23_BIT 24
#define Irq_20_to_23_Priority_PRI_23_BIT NVIC_IPR5_PRI_23_BIT

#define NVIC_IPR6 (*(volatile unsigned *)0xE000E418)
#define Irq_24_to_27_Priority NVIC_IPR6
#define NVIC_IPR6_OFFSET 0x318
#define Irq_24_to_27_Priority_OFFSET NVIC_IPR6_OFFSET
#define NVIC_IPR6_PRI_24_MASK 0xFF
#define Irq_24_to_27_Priority_PRI_24_MASK NVIC_IPR6_PRI_24_MASK
#define NVIC_IPR6_PRI_24_BIT 0
#define Irq_24_to_27_Priority_PRI_24_BIT NVIC_IPR6_PRI_24_BIT
#define NVIC_IPR6_PRI_25_MASK 0xFF00
#define Irq_24_to_27_Priority_PRI_25_MASK NVIC_IPR6_PRI_25_MASK
#define NVIC_IPR6_PRI_25_BIT 8
#define Irq_24_to_27_Priority_PRI_25_BIT NVIC_IPR6_PRI_25_BIT
#define NVIC_IPR6_PRI_26_MASK 0xFF0000
#define Irq_24_to_27_Priority_PRI_26_MASK NVIC_IPR6_PRI_26_MASK
#define NVIC_IPR6_PRI_26_BIT 16
#define Irq_24_to_27_Priority_PRI_26_BIT NVIC_IPR6_PRI_26_BIT
#define NVIC_IPR6_PRI_27_MASK 0xFF000000
#define Irq_24_to_27_Priority_PRI_27_MASK NVIC_IPR6_PRI_27_MASK
#define NVIC_IPR6_PRI_27_BIT 24
#define Irq_24_to_27_Priority_PRI_27_BIT NVIC_IPR6_PRI_27_BIT

#define NVIC_IPR7 (*(volatile unsigned *)0xE000E41C)
#define Irq_28_to_31_Priority NVIC_IPR7
#define NVIC_IPR7_OFFSET 0x31C
#define Irq_28_to_31_Priority_OFFSET NVIC_IPR7_OFFSET
#define NVIC_IPR7_PRI_28_MASK 0xFF
#define Irq_28_to_31_Priority_PRI_28_MASK NVIC_IPR7_PRI_28_MASK
#define NVIC_IPR7_PRI_28_BIT 0
#define Irq_28_to_31_Priority_PRI_28_BIT NVIC_IPR7_PRI_28_BIT
#define NVIC_IPR7_PRI_29_MASK 0xFF00
#define Irq_28_to_31_Priority_PRI_29_MASK NVIC_IPR7_PRI_29_MASK
#define NVIC_IPR7_PRI_29_BIT 8
#define Irq_28_to_31_Priority_PRI_29_BIT NVIC_IPR7_PRI_29_BIT
#define NVIC_IPR7_PRI_30_MASK 0xFF0000
#define Irq_28_to_31_Priority_PRI_30_MASK NVIC_IPR7_PRI_30_MASK
#define NVIC_IPR7_PRI_30_BIT 16
#define Irq_28_to_31_Priority_PRI_30_BIT NVIC_IPR7_PRI_30_BIT
#define NVIC_IPR7_PRI_31_MASK 0xFF000000
#define Irq_28_to_31_Priority_PRI_31_MASK NVIC_IPR7_PRI_31_MASK
#define NVIC_IPR7_PRI_31_BIT 24
#define Irq_28_to_31_Priority_PRI_31_BIT NVIC_IPR7_PRI_31_BIT

#define NVIC_IPR8 (*(volatile unsigned *)0xE000E420)
#define Irq_32_to_35_Priority NVIC_IPR8
#define NVIC_IPR8_OFFSET 0x320
#define Irq_32_to_35_Priority_OFFSET NVIC_IPR8_OFFSET
#define NVIC_IPR8_PRI_32_MASK 0xFF
#define Irq_32_to_35_Priority_PRI_32_MASK NVIC_IPR8_PRI_32_MASK
#define NVIC_IPR8_PRI_32_BIT 0
#define Irq_32_to_35_Priority_PRI_32_BIT NVIC_IPR8_PRI_32_BIT
#define NVIC_IPR8_PRI_33_MASK 0xFF00
#define Irq_32_to_35_Priority_PRI_33_MASK NVIC_IPR8_PRI_33_MASK
#define NVIC_IPR8_PRI_33_BIT 8
#define Irq_32_to_35_Priority_PRI_33_BIT NVIC_IPR8_PRI_33_BIT
#define NVIC_IPR8_PRI_34_MASK 0xFF0000
#define Irq_32_to_35_Priority_PRI_34_MASK NVIC_IPR8_PRI_34_MASK
#define NVIC_IPR8_PRI_34_BIT 16
#define Irq_32_to_35_Priority_PRI_34_BIT NVIC_IPR8_PRI_34_BIT
#define NVIC_IPR8_PRI_35_MASK 0xFF000000
#define Irq_32_to_35_Priority_PRI_35_MASK NVIC_IPR8_PRI_35_MASK
#define NVIC_IPR8_PRI_35_BIT 24
#define Irq_32_to_35_Priority_PRI_35_BIT NVIC_IPR8_PRI_35_BIT

#define NVIC_IPR9 (*(volatile unsigned *)0xE000E424)
#define Irq_36_to_39_Priority NVIC_IPR9
#define NVIC_IPR9_OFFSET 0x324
#define Irq_36_to_39_Priority_OFFSET NVIC_IPR9_OFFSET
#define NVIC_IPR9_PRI_36_MASK 0xFF
#define Irq_36_to_39_Priority_PRI_36_MASK NVIC_IPR9_PRI_36_MASK
#define NVIC_IPR9_PRI_36_BIT 0
#define Irq_36_to_39_Priority_PRI_36_BIT NVIC_IPR9_PRI_36_BIT
#define NVIC_IPR9_PRI_37_MASK 0xFF00
#define Irq_36_to_39_Priority_PRI_37_MASK NVIC_IPR9_PRI_37_MASK
#define NVIC_IPR9_PRI_37_BIT 8
#define Irq_36_to_39_Priority_PRI_37_BIT NVIC_IPR9_PRI_37_BIT
#define NVIC_IPR9_PRI_38_MASK 0xFF0000
#define Irq_36_to_39_Priority_PRI_38_MASK NVIC_IPR9_PRI_38_MASK
#define NVIC_IPR9_PRI_38_BIT 16
#define Irq_36_to_39_Priority_PRI_38_BIT NVIC_IPR9_PRI_38_BIT
#define NVIC_IPR9_PRI_39_MASK 0xFF000000
#define Irq_36_to_39_Priority_PRI_39_MASK NVIC_IPR9_PRI_39_MASK
#define NVIC_IPR9_PRI_39_BIT 24
#define Irq_36_to_39_Priority_PRI_39_BIT NVIC_IPR9_PRI_39_BIT

#define NVIC_IPR10 (*(volatile unsigned *)0xE000E428)
#define Irq_40_to_43_Priority NVIC_IPR10
#define NVIC_IPR10_OFFSET 0x328
#define Irq_40_to_43_Priority_OFFSET NVIC_IPR10_OFFSET
#define NVIC_IPR10_PRI_40_MASK 0xFF
#define Irq_40_to_43_Priority_PRI_40_MASK NVIC_IPR10_PRI_40_MASK
#define NVIC_IPR10_PRI_40_BIT 0
#define Irq_40_to_43_Priority_PRI_40_BIT NVIC_IPR10_PRI_40_BIT
#define NVIC_IPR10_PRI_41_MASK 0xFF00
#define Irq_40_to_43_Priority_PRI_41_MASK NVIC_IPR10_PRI_41_MASK
#define NVIC_IPR10_PRI_41_BIT 8
#define Irq_40_to_43_Priority_PRI_41_BIT NVIC_IPR10_PRI_41_BIT
#define NVIC_IPR10_PRI_42_MASK 0xFF0000
#define Irq_40_to_43_Priority_PRI_42_MASK NVIC_IPR10_PRI_42_MASK
#define NVIC_IPR10_PRI_42_BIT 16
#define Irq_40_to_43_Priority_PRI_42_BIT NVIC_IPR10_PRI_42_BIT
#define NVIC_IPR10_PRI_43_MASK 0xFF000000
#define Irq_40_to_43_Priority_PRI_43_MASK NVIC_IPR10_PRI_43_MASK
#define NVIC_IPR10_PRI_43_BIT 24
#define Irq_40_to_43_Priority_PRI_43_BIT NVIC_IPR10_PRI_43_BIT

#define NVIC_IPR11 (*(volatile unsigned *)0xE000E42C)
#define Irq_44_to_47_Priority NVIC_IPR11
#define NVIC_IPR11_OFFSET 0x32C
#define Irq_44_to_47_Priority_OFFSET NVIC_IPR11_OFFSET
#define NVIC_IPR11_PRI_44_MASK 0xFF
#define Irq_44_to_47_Priority_PRI_44_MASK NVIC_IPR11_PRI_44_MASK
#define NVIC_IPR11_PRI_44_BIT 0
#define Irq_44_to_47_Priority_PRI_44_BIT NVIC_IPR11_PRI_44_BIT
#define NVIC_IPR11_PRI_45_MASK 0xFF00
#define Irq_44_to_47_Priority_PRI_45_MASK NVIC_IPR11_PRI_45_MASK
#define NVIC_IPR11_PRI_45_BIT 8
#define Irq_44_to_47_Priority_PRI_45_BIT NVIC_IPR11_PRI_45_BIT
#define NVIC_IPR11_PRI_46_MASK 0xFF0000
#define Irq_44_to_47_Priority_PRI_46_MASK NVIC_IPR11_PRI_46_MASK
#define NVIC_IPR11_PRI_46_BIT 16
#define Irq_44_to_47_Priority_PRI_46_BIT NVIC_IPR11_PRI_46_BIT
#define NVIC_IPR11_PRI_47_MASK 0xFF000000
#define Irq_44_to_47_Priority_PRI_47_MASK NVIC_IPR11_PRI_47_MASK
#define NVIC_IPR11_PRI_47_BIT 24
#define Irq_44_to_47_Priority_PRI_47_BIT NVIC_IPR11_PRI_47_BIT

#define NVIC_IPR12 (*(volatile unsigned *)0xE000E430)
#define Irq_48_to_51_Priority NVIC_IPR12
#define NVIC_IPR12_OFFSET 0x330
#define Irq_48_to_51_Priority_OFFSET NVIC_IPR12_OFFSET
#define NVIC_IPR12_PRI_48_MASK 0xFF
#define Irq_48_to_51_Priority_PRI_48_MASK NVIC_IPR12_PRI_48_MASK
#define NVIC_IPR12_PRI_48_BIT 0
#define Irq_48_to_51_Priority_PRI_48_BIT NVIC_IPR12_PRI_48_BIT
#define NVIC_IPR12_PRI_49_MASK 0xFF00
#define Irq_48_to_51_Priority_PRI_49_MASK NVIC_IPR12_PRI_49_MASK
#define NVIC_IPR12_PRI_49_BIT 8
#define Irq_48_to_51_Priority_PRI_49_BIT NVIC_IPR12_PRI_49_BIT
#define NVIC_IPR12_PRI_50_MASK 0xFF0000
#define Irq_48_to_51_Priority_PRI_50_MASK NVIC_IPR12_PRI_50_MASK
#define NVIC_IPR12_PRI_50_BIT 16
#define Irq_48_to_51_Priority_PRI_50_BIT NVIC_IPR12_PRI_50_BIT
#define NVIC_IPR12_PRI_51_MASK 0xFF000000
#define Irq_48_to_51_Priority_PRI_51_MASK NVIC_IPR12_PRI_51_MASK
#define NVIC_IPR12_PRI_51_BIT 24
#define Irq_48_to_51_Priority_PRI_51_BIT NVIC_IPR12_PRI_51_BIT

#define NVIC_IPR13 (*(volatile unsigned *)0xE000E434)
#define Irq_52_to_55_Priority NVIC_IPR13
#define NVIC_IPR13_OFFSET 0x334
#define Irq_52_to_55_Priority_OFFSET NVIC_IPR13_OFFSET
#define NVIC_IPR13_PRI_52_MASK 0xFF
#define Irq_52_to_55_Priority_PRI_52_MASK NVIC_IPR13_PRI_52_MASK
#define NVIC_IPR13_PRI_52_BIT 0
#define Irq_52_to_55_Priority_PRI_52_BIT NVIC_IPR13_PRI_52_BIT
#define NVIC_IPR13_PRI_53_MASK 0xFF00
#define Irq_52_to_55_Priority_PRI_53_MASK NVIC_IPR13_PRI_53_MASK
#define NVIC_IPR13_PRI_53_BIT 8
#define Irq_52_to_55_Priority_PRI_53_BIT NVIC_IPR13_PRI_53_BIT
#define NVIC_IPR13_PRI_54_MASK 0xFF0000
#define Irq_52_to_55_Priority_PRI_54_MASK NVIC_IPR13_PRI_54_MASK
#define NVIC_IPR13_PRI_54_BIT 16
#define Irq_52_to_55_Priority_PRI_54_BIT NVIC_IPR13_PRI_54_BIT
#define NVIC_IPR13_PRI_55_MASK 0xFF000000
#define Irq_52_to_55_Priority_PRI_55_MASK NVIC_IPR13_PRI_55_MASK
#define NVIC_IPR13_PRI_55_BIT 24
#define Irq_52_to_55_Priority_PRI_55_BIT NVIC_IPR13_PRI_55_BIT

#define NVIC_IPR14 (*(volatile unsigned *)0xE000E438)
#define Irq_56_to_59_Priority NVIC_IPR14
#define NVIC_IPR14_OFFSET 0x338
#define Irq_56_to_59_Priority_OFFSET NVIC_IPR14_OFFSET
#define NVIC_IPR14_PRI_56_MASK 0xFF
#define Irq_56_to_59_Priority_PRI_56_MASK NVIC_IPR14_PRI_56_MASK
#define NVIC_IPR14_PRI_56_BIT 0
#define Irq_56_to_59_Priority_PRI_56_BIT NVIC_IPR14_PRI_56_BIT
#define NVIC_IPR14_PRI_57_MASK 0xFF00
#define Irq_56_to_59_Priority_PRI_57_MASK NVIC_IPR14_PRI_57_MASK
#define NVIC_IPR14_PRI_57_BIT 8
#define Irq_56_to_59_Priority_PRI_57_BIT NVIC_IPR14_PRI_57_BIT
#define NVIC_IPR14_PRI_58_MASK 0xFF0000
#define Irq_56_to_59_Priority_PRI_58_MASK NVIC_IPR14_PRI_58_MASK
#define NVIC_IPR14_PRI_58_BIT 16
#define Irq_56_to_59_Priority_PRI_58_BIT NVIC_IPR14_PRI_58_BIT
#define NVIC_IPR14_PRI_59_MASK 0xFF000000
#define Irq_56_to_59_Priority_PRI_59_MASK NVIC_IPR14_PRI_59_MASK
#define NVIC_IPR14_PRI_59_BIT 24
#define Irq_56_to_59_Priority_PRI_59_BIT NVIC_IPR14_PRI_59_BIT

#define NVIC_IPR15 (*(volatile unsigned *)0xE000E43C)
#define Irq_60_to_63_Priority NVIC_IPR15
#define NVIC_IPR15_OFFSET 0x33C
#define Irq_60_to_63_Priority_OFFSET NVIC_IPR15_OFFSET
#define NVIC_IPR15_PRI_60_MASK 0xFF
#define Irq_60_to_63_Priority_PRI_60_MASK NVIC_IPR15_PRI_60_MASK
#define NVIC_IPR15_PRI_60_BIT 0
#define Irq_60_to_63_Priority_PRI_60_BIT NVIC_IPR15_PRI_60_BIT
#define NVIC_IPR15_PRI_61_MASK 0xFF00
#define Irq_60_to_63_Priority_PRI_61_MASK NVIC_IPR15_PRI_61_MASK
#define NVIC_IPR15_PRI_61_BIT 8
#define Irq_60_to_63_Priority_PRI_61_BIT NVIC_IPR15_PRI_61_BIT
#define NVIC_IPR15_PRI_62_MASK 0xFF0000
#define Irq_60_to_63_Priority_PRI_62_MASK NVIC_IPR15_PRI_62_MASK
#define NVIC_IPR15_PRI_62_BIT 16
#define Irq_60_to_63_Priority_PRI_62_BIT NVIC_IPR15_PRI_62_BIT
#define NVIC_IPR15_PRI_63_MASK 0xFF000000
#define Irq_60_to_63_Priority_PRI_63_MASK NVIC_IPR15_PRI_63_MASK
#define NVIC_IPR15_PRI_63_BIT 24
#define Irq_60_to_63_Priority_PRI_63_BIT NVIC_IPR15_PRI_63_BIT

#define NVIC_IPR16 (*(volatile unsigned *)0xE000E440)
#define Irq_64_to_67_Priority NVIC_IPR16
#define NVIC_IPR16_OFFSET 0x340
#define Irq_64_to_67_Priority_OFFSET NVIC_IPR16_OFFSET
#define NVIC_IPR16_PRI_64_MASK 0xFF
#define Irq_64_to_67_Priority_PRI_64_MASK NVIC_IPR16_PRI_64_MASK
#define NVIC_IPR16_PRI_64_BIT 0
#define Irq_64_to_67_Priority_PRI_64_BIT NVIC_IPR16_PRI_64_BIT
#define NVIC_IPR16_PRI_65_MASK 0xFF00
#define Irq_64_to_67_Priority_PRI_65_MASK NVIC_IPR16_PRI_65_MASK
#define NVIC_IPR16_PRI_65_BIT 8
#define Irq_64_to_67_Priority_PRI_65_BIT NVIC_IPR16_PRI_65_BIT
#define NVIC_IPR16_PRI_66_MASK 0xFF0000
#define Irq_64_to_67_Priority_PRI_66_MASK NVIC_IPR16_PRI_66_MASK
#define NVIC_IPR16_PRI_66_BIT 16
#define Irq_64_to_67_Priority_PRI_66_BIT NVIC_IPR16_PRI_66_BIT
#define NVIC_IPR16_PRI_67_MASK 0xFF000000
#define Irq_64_to_67_Priority_PRI_67_MASK NVIC_IPR16_PRI_67_MASK
#define NVIC_IPR16_PRI_67_BIT 24
#define Irq_64_to_67_Priority_PRI_67_BIT NVIC_IPR16_PRI_67_BIT

#define NVIC_IPR17 (*(volatile unsigned *)0xE000E444)
#define Irq_68_to_71_Priority NVIC_IPR17
#define NVIC_IPR17_OFFSET 0x344
#define Irq_68_to_71_Priority_OFFSET NVIC_IPR17_OFFSET
#define NVIC_IPR17_PRI_68_MASK 0xFF
#define Irq_68_to_71_Priority_PRI_68_MASK NVIC_IPR17_PRI_68_MASK
#define NVIC_IPR17_PRI_68_BIT 0
#define Irq_68_to_71_Priority_PRI_68_BIT NVIC_IPR17_PRI_68_BIT
#define NVIC_IPR17_PRI_69_MASK 0xFF00
#define Irq_68_to_71_Priority_PRI_69_MASK NVIC_IPR17_PRI_69_MASK
#define NVIC_IPR17_PRI_69_BIT 8
#define Irq_68_to_71_Priority_PRI_69_BIT NVIC_IPR17_PRI_69_BIT
#define NVIC_IPR17_PRI_70_MASK 0xFF0000
#define Irq_68_to_71_Priority_PRI_70_MASK NVIC_IPR17_PRI_70_MASK
#define NVIC_IPR17_PRI_70_BIT 16
#define Irq_68_to_71_Priority_PRI_70_BIT NVIC_IPR17_PRI_70_BIT
#define NVIC_IPR17_PRI_71_MASK 0xFF000000
#define Irq_68_to_71_Priority_PRI_71_MASK NVIC_IPR17_PRI_71_MASK
#define NVIC_IPR17_PRI_71_BIT 24
#define Irq_68_to_71_Priority_PRI_71_BIT NVIC_IPR17_PRI_71_BIT

#define NVIC_IPR18 (*(volatile unsigned *)0xE000E448)
#define Irq_72_to_75_Priority NVIC_IPR18
#define NVIC_IPR18_OFFSET 0x348
#define Irq_72_to_75_Priority_OFFSET NVIC_IPR18_OFFSET
#define NVIC_IPR18_PRI_72_MASK 0xFF
#define Irq_72_to_75_Priority_PRI_72_MASK NVIC_IPR18_PRI_72_MASK
#define NVIC_IPR18_PRI_72_BIT 0
#define Irq_72_to_75_Priority_PRI_72_BIT NVIC_IPR18_PRI_72_BIT
#define NVIC_IPR18_PRI_73_MASK 0xFF00
#define Irq_72_to_75_Priority_PRI_73_MASK NVIC_IPR18_PRI_73_MASK
#define NVIC_IPR18_PRI_73_BIT 8
#define Irq_72_to_75_Priority_PRI_73_BIT NVIC_IPR18_PRI_73_BIT
#define NVIC_IPR18_PRI_74_MASK 0xFF0000
#define Irq_72_to_75_Priority_PRI_74_MASK NVIC_IPR18_PRI_74_MASK
#define NVIC_IPR18_PRI_74_BIT 16
#define Irq_72_to_75_Priority_PRI_74_BIT NVIC_IPR18_PRI_74_BIT
#define NVIC_IPR18_PRI_75_MASK 0xFF000000
#define Irq_72_to_75_Priority_PRI_75_MASK NVIC_IPR18_PRI_75_MASK
#define NVIC_IPR18_PRI_75_BIT 24
#define Irq_72_to_75_Priority_PRI_75_BIT NVIC_IPR18_PRI_75_BIT

#define NVIC_IPR19 (*(volatile unsigned *)0xE000E44C)
#define Irq_76_to_79_Priority NVIC_IPR19
#define NVIC_IPR19_OFFSET 0x34C
#define Irq_76_to_79_Priority_OFFSET NVIC_IPR19_OFFSET
#define NVIC_IPR19_PRI_76_MASK 0xFF
#define Irq_76_to_79_Priority_PRI_76_MASK NVIC_IPR19_PRI_76_MASK
#define NVIC_IPR19_PRI_76_BIT 0
#define Irq_76_to_79_Priority_PRI_76_BIT NVIC_IPR19_PRI_76_BIT
#define NVIC_IPR19_PRI_77_MASK 0xFF00
#define Irq_76_to_79_Priority_PRI_77_MASK NVIC_IPR19_PRI_77_MASK
#define NVIC_IPR19_PRI_77_BIT 8
#define Irq_76_to_79_Priority_PRI_77_BIT NVIC_IPR19_PRI_77_BIT
#define NVIC_IPR19_PRI_78_MASK 0xFF0000
#define Irq_76_to_79_Priority_PRI_78_MASK NVIC_IPR19_PRI_78_MASK
#define NVIC_IPR19_PRI_78_BIT 16
#define Irq_76_to_79_Priority_PRI_78_BIT NVIC_IPR19_PRI_78_BIT
#define NVIC_IPR19_PRI_79_MASK 0xFF000000
#define Irq_76_to_79_Priority_PRI_79_MASK NVIC_IPR19_PRI_79_MASK
#define NVIC_IPR19_PRI_79_BIT 24
#define Irq_76_to_79_Priority_PRI_79_BIT NVIC_IPR19_PRI_79_BIT

#define NVIC_IPR20 (*(volatile unsigned *)0xE000E450)
#define Irq_80_to_83_Priority NVIC_IPR20
#define NVIC_IPR20_OFFSET 0x350
#define Irq_80_to_83_Priority_OFFSET NVIC_IPR20_OFFSET
#define NVIC_IPR20_PRI_80_MASK 0xFF
#define Irq_80_to_83_Priority_PRI_80_MASK NVIC_IPR20_PRI_80_MASK
#define NVIC_IPR20_PRI_80_BIT 0
#define Irq_80_to_83_Priority_PRI_80_BIT NVIC_IPR20_PRI_80_BIT
#define NVIC_IPR20_PRI_81_MASK 0xFF00
#define Irq_80_to_83_Priority_PRI_81_MASK NVIC_IPR20_PRI_81_MASK
#define NVIC_IPR20_PRI_81_BIT 8
#define Irq_80_to_83_Priority_PRI_81_BIT NVIC_IPR20_PRI_81_BIT
#define NVIC_IPR20_PRI_82_MASK 0xFF0000
#define Irq_80_to_83_Priority_PRI_82_MASK NVIC_IPR20_PRI_82_MASK
#define NVIC_IPR20_PRI_82_BIT 16
#define Irq_80_to_83_Priority_PRI_82_BIT NVIC_IPR20_PRI_82_BIT
#define NVIC_IPR20_PRI_83_MASK 0xFF000000
#define Irq_80_to_83_Priority_PRI_83_MASK NVIC_IPR20_PRI_83_MASK
#define NVIC_IPR20_PRI_83_BIT 24
#define Irq_80_to_83_Priority_PRI_83_BIT NVIC_IPR20_PRI_83_BIT

#define NVIC_IPR21 (*(volatile unsigned *)0xE000E454)
#define Irq_84_to_87_Priority NVIC_IPR21
#define NVIC_IPR21_OFFSET 0x354
#define Irq_84_to_87_Priority_OFFSET NVIC_IPR21_OFFSET
#define NVIC_IPR21_PRI_84_MASK 0xFF
#define Irq_84_to_87_Priority_PRI_84_MASK NVIC_IPR21_PRI_84_MASK
#define NVIC_IPR21_PRI_84_BIT 0
#define Irq_84_to_87_Priority_PRI_84_BIT NVIC_IPR21_PRI_84_BIT
#define NVIC_IPR21_PRI_85_MASK 0xFF00
#define Irq_84_to_87_Priority_PRI_85_MASK NVIC_IPR21_PRI_85_MASK
#define NVIC_IPR21_PRI_85_BIT 8
#define Irq_84_to_87_Priority_PRI_85_BIT NVIC_IPR21_PRI_85_BIT
#define NVIC_IPR21_PRI_86_MASK 0xFF0000
#define Irq_84_to_87_Priority_PRI_86_MASK NVIC_IPR21_PRI_86_MASK
#define NVIC_IPR21_PRI_86_BIT 16
#define Irq_84_to_87_Priority_PRI_86_BIT NVIC_IPR21_PRI_86_BIT
#define NVIC_IPR21_PRI_87_MASK 0xFF000000
#define Irq_84_to_87_Priority_PRI_87_MASK NVIC_IPR21_PRI_87_MASK
#define NVIC_IPR21_PRI_87_BIT 24
#define Irq_84_to_87_Priority_PRI_87_BIT NVIC_IPR21_PRI_87_BIT

#define NVIC_IPR22 (*(volatile unsigned *)0xE000E458)
#define Irq_88_to_91_Priority NVIC_IPR22
#define NVIC_IPR22_OFFSET 0x358
#define Irq_88_to_91_Priority_OFFSET NVIC_IPR22_OFFSET
#define NVIC_IPR22_PRI_88_MASK 0xFF
#define Irq_88_to_91_Priority_PRI_88_MASK NVIC_IPR22_PRI_88_MASK
#define NVIC_IPR22_PRI_88_BIT 0
#define Irq_88_to_91_Priority_PRI_88_BIT NVIC_IPR22_PRI_88_BIT
#define NVIC_IPR22_PRI_89_MASK 0xFF00
#define Irq_88_to_91_Priority_PRI_89_MASK NVIC_IPR22_PRI_89_MASK
#define NVIC_IPR22_PRI_89_BIT 8
#define Irq_88_to_91_Priority_PRI_89_BIT NVIC_IPR22_PRI_89_BIT
#define NVIC_IPR22_PRI_90_MASK 0xFF0000
#define Irq_88_to_91_Priority_PRI_90_MASK NVIC_IPR22_PRI_90_MASK
#define NVIC_IPR22_PRI_90_BIT 16
#define Irq_88_to_91_Priority_PRI_90_BIT NVIC_IPR22_PRI_90_BIT
#define NVIC_IPR22_PRI_91_MASK 0xFF000000
#define Irq_88_to_91_Priority_PRI_91_MASK NVIC_IPR22_PRI_91_MASK
#define NVIC_IPR22_PRI_91_BIT 24
#define Irq_88_to_91_Priority_PRI_91_BIT NVIC_IPR22_PRI_91_BIT

#define NVIC_IPR23 (*(volatile unsigned *)0xE000E45C)
#define Irq_92_to_95_Priority NVIC_IPR23
#define NVIC_IPR23_OFFSET 0x35C
#define Irq_92_to_95_Priority_OFFSET NVIC_IPR23_OFFSET
#define NVIC_IPR23_PRI_92_MASK 0xFF
#define Irq_92_to_95_Priority_PRI_92_MASK NVIC_IPR23_PRI_92_MASK
#define NVIC_IPR23_PRI_92_BIT 0
#define Irq_92_to_95_Priority_PRI_92_BIT NVIC_IPR23_PRI_92_BIT
#define NVIC_IPR23_PRI_93_MASK 0xFF00
#define Irq_92_to_95_Priority_PRI_93_MASK NVIC_IPR23_PRI_93_MASK
#define NVIC_IPR23_PRI_93_BIT 8
#define Irq_92_to_95_Priority_PRI_93_BIT NVIC_IPR23_PRI_93_BIT
#define NVIC_IPR23_PRI_94_MASK 0xFF0000
#define Irq_92_to_95_Priority_PRI_94_MASK NVIC_IPR23_PRI_94_MASK
#define NVIC_IPR23_PRI_94_BIT 16
#define Irq_92_to_95_Priority_PRI_94_BIT NVIC_IPR23_PRI_94_BIT
#define NVIC_IPR23_PRI_95_MASK 0xFF000000
#define Irq_92_to_95_Priority_PRI_95_MASK NVIC_IPR23_PRI_95_MASK
#define NVIC_IPR23_PRI_95_BIT 24
#define Irq_92_to_95_Priority_PRI_95_BIT NVIC_IPR23_PRI_95_BIT

#define NVIC_IPR24 (*(volatile unsigned *)0xE000E460)
#define Irq_96_to_99_Priority NVIC_IPR24
#define NVIC_IPR24_OFFSET 0x360
#define Irq_96_to_99_Priority_OFFSET NVIC_IPR24_OFFSET
#define NVIC_IPR24_PRI_96_MASK 0xFF
#define Irq_96_to_99_Priority_PRI_96_MASK NVIC_IPR24_PRI_96_MASK
#define NVIC_IPR24_PRI_96_BIT 0
#define Irq_96_to_99_Priority_PRI_96_BIT NVIC_IPR24_PRI_96_BIT
#define NVIC_IPR24_PRI_97_MASK 0xFF00
#define Irq_96_to_99_Priority_PRI_97_MASK NVIC_IPR24_PRI_97_MASK
#define NVIC_IPR24_PRI_97_BIT 8
#define Irq_96_to_99_Priority_PRI_97_BIT NVIC_IPR24_PRI_97_BIT
#define NVIC_IPR24_PRI_98_MASK 0xFF0000
#define Irq_96_to_99_Priority_PRI_98_MASK NVIC_IPR24_PRI_98_MASK
#define NVIC_IPR24_PRI_98_BIT 16
#define Irq_96_to_99_Priority_PRI_98_BIT NVIC_IPR24_PRI_98_BIT
#define NVIC_IPR24_PRI_99_MASK 0xFF000000
#define Irq_96_to_99_Priority_PRI_99_MASK NVIC_IPR24_PRI_99_MASK
#define NVIC_IPR24_PRI_99_BIT 24
#define Irq_96_to_99_Priority_PRI_99_BIT NVIC_IPR24_PRI_99_BIT

#define NVIC_IPR25 (*(volatile unsigned *)0xE000E464)
#define Irq_100_to_103_Priority NVIC_IPR25
#define NVIC_IPR25_OFFSET 0x364
#define Irq_100_to_103_Priority_OFFSET NVIC_IPR25_OFFSET
#define NVIC_IPR25_PRI_100_MASK 0xFF
#define Irq_100_to_103_Priority_PRI_100_MASK NVIC_IPR25_PRI_100_MASK
#define NVIC_IPR25_PRI_100_BIT 0
#define Irq_100_to_103_Priority_PRI_100_BIT NVIC_IPR25_PRI_100_BIT
#define NVIC_IPR25_PRI_101_MASK 0xFF00
#define Irq_100_to_103_Priority_PRI_101_MASK NVIC_IPR25_PRI_101_MASK
#define NVIC_IPR25_PRI_101_BIT 8
#define Irq_100_to_103_Priority_PRI_101_BIT NVIC_IPR25_PRI_101_BIT
#define NVIC_IPR25_PRI_102_MASK 0xFF0000
#define Irq_100_to_103_Priority_PRI_102_MASK NVIC_IPR25_PRI_102_MASK
#define NVIC_IPR25_PRI_102_BIT 16
#define Irq_100_to_103_Priority_PRI_102_BIT NVIC_IPR25_PRI_102_BIT
#define NVIC_IPR25_PRI_103_MASK 0xFF000000
#define Irq_100_to_103_Priority_PRI_103_MASK NVIC_IPR25_PRI_103_MASK
#define NVIC_IPR25_PRI_103_BIT 24
#define Irq_100_to_103_Priority_PRI_103_BIT NVIC_IPR25_PRI_103_BIT

#define NVIC_IPR26 (*(volatile unsigned *)0xE000E468)
#define Irq_104_to_107_Priority NVIC_IPR26
#define NVIC_IPR26_OFFSET 0x368
#define Irq_104_to_107_Priority_OFFSET NVIC_IPR26_OFFSET
#define NVIC_IPR26_PRI_104_MASK 0xFF
#define Irq_104_to_107_Priority_PRI_104_MASK NVIC_IPR26_PRI_104_MASK
#define NVIC_IPR26_PRI_104_BIT 0
#define Irq_104_to_107_Priority_PRI_104_BIT NVIC_IPR26_PRI_104_BIT
#define NVIC_IPR26_PRI_105_MASK 0xFF00
#define Irq_104_to_107_Priority_PRI_105_MASK NVIC_IPR26_PRI_105_MASK
#define NVIC_IPR26_PRI_105_BIT 8
#define Irq_104_to_107_Priority_PRI_105_BIT NVIC_IPR26_PRI_105_BIT
#define NVIC_IPR26_PRI_106_MASK 0xFF0000
#define Irq_104_to_107_Priority_PRI_106_MASK NVIC_IPR26_PRI_106_MASK
#define NVIC_IPR26_PRI_106_BIT 16
#define Irq_104_to_107_Priority_PRI_106_BIT NVIC_IPR26_PRI_106_BIT
#define NVIC_IPR26_PRI_107_MASK 0xFF000000
#define Irq_104_to_107_Priority_PRI_107_MASK NVIC_IPR26_PRI_107_MASK
#define NVIC_IPR26_PRI_107_BIT 24
#define Irq_104_to_107_Priority_PRI_107_BIT NVIC_IPR26_PRI_107_BIT

#define NVIC_IPR27 (*(volatile unsigned *)0xE000E46C)
#define Irq_108_to_111_Priority NVIC_IPR27
#define NVIC_IPR27_OFFSET 0x36C
#define Irq_108_to_111_Priority_OFFSET NVIC_IPR27_OFFSET
#define NVIC_IPR27_PRI_108_MASK 0xFF
#define Irq_108_to_111_Priority_PRI_108_MASK NVIC_IPR27_PRI_108_MASK
#define NVIC_IPR27_PRI_108_BIT 0
#define Irq_108_to_111_Priority_PRI_108_BIT NVIC_IPR27_PRI_108_BIT
#define NVIC_IPR27_PRI_109_MASK 0xFF00
#define Irq_108_to_111_Priority_PRI_109_MASK NVIC_IPR27_PRI_109_MASK
#define NVIC_IPR27_PRI_109_BIT 8
#define Irq_108_to_111_Priority_PRI_109_BIT NVIC_IPR27_PRI_109_BIT
#define NVIC_IPR27_PRI_110_MASK 0xFF0000
#define Irq_108_to_111_Priority_PRI_110_MASK NVIC_IPR27_PRI_110_MASK
#define NVIC_IPR27_PRI_110_BIT 16
#define Irq_108_to_111_Priority_PRI_110_BIT NVIC_IPR27_PRI_110_BIT
#define NVIC_IPR27_PRI_111_MASK 0xFF000000
#define Irq_108_to_111_Priority_PRI_111_MASK NVIC_IPR27_PRI_111_MASK
#define NVIC_IPR27_PRI_111_BIT 24
#define Irq_108_to_111_Priority_PRI_111_BIT NVIC_IPR27_PRI_111_BIT

#define NVIC_IP0 (*(volatile unsigned char *)0xE000E400)
#define NVIC_IP0_OFFSET 0x300

#define NVIC_IP1 (*(volatile unsigned char *)0xE000E401)
#define NVIC_IP1_OFFSET 0x301

#define NVIC_IP2 (*(volatile unsigned char *)0xE000E402)
#define NVIC_IP2_OFFSET 0x302

#define NVIC_IP3 (*(volatile unsigned char *)0xE000E403)
#define NVIC_IP3_OFFSET 0x303

#define NVIC_IP4 (*(volatile unsigned char *)0xE000E404)
#define NVIC_IP4_OFFSET 0x304

#define NVIC_IP5 (*(volatile unsigned char *)0xE000E405)
#define NVIC_IP5_OFFSET 0x305

#define NVIC_IP6 (*(volatile unsigned char *)0xE000E406)
#define NVIC_IP6_OFFSET 0x306

#define NVIC_IP7 (*(volatile unsigned char *)0xE000E407)
#define NVIC_IP7_OFFSET 0x307

#define NVIC_IP8 (*(volatile unsigned char *)0xE000E408)
#define NVIC_IP8_OFFSET 0x308

#define NVIC_IP9 (*(volatile unsigned char *)0xE000E409)
#define NVIC_IP9_OFFSET 0x309

#define NVIC_IP10 (*(volatile unsigned char *)0xE000E40A)
#define NVIC_IP10_OFFSET 0x30A

#define NVIC_IP11 (*(volatile unsigned char *)0xE000E40B)
#define NVIC_IP11_OFFSET 0x30B

#define NVIC_IP12 (*(volatile unsigned char *)0xE000E40C)
#define NVIC_IP12_OFFSET 0x30C

#define NVIC_IP13 (*(volatile unsigned char *)0xE000E40D)
#define NVIC_IP13_OFFSET 0x30D

#define NVIC_IP14 (*(volatile unsigned char *)0xE000E40E)
#define NVIC_IP14_OFFSET 0x30E

#define NVIC_IP15 (*(volatile unsigned char *)0xE000E40F)
#define NVIC_IP15_OFFSET 0x30F

#define NVIC_IP16 (*(volatile unsigned char *)0xE000E410)
#define NVIC_IP16_OFFSET 0x310

#define NVIC_IP17 (*(volatile unsigned char *)0xE000E411)
#define NVIC_IP17_OFFSET 0x311

#define NVIC_IP18 (*(volatile unsigned char *)0xE000E412)
#define NVIC_IP18_OFFSET 0x312

#define NVIC_IP19 (*(volatile unsigned char *)0xE000E413)
#define NVIC_IP19_OFFSET 0x313

#define NVIC_IP20 (*(volatile unsigned char *)0xE000E414)
#define NVIC_IP20_OFFSET 0x314

#define NVIC_IP21 (*(volatile unsigned char *)0xE000E415)
#define NVIC_IP21_OFFSET 0x315

#define NVIC_IP22 (*(volatile unsigned char *)0xE000E416)
#define NVIC_IP22_OFFSET 0x316

#define NVIC_IP23 (*(volatile unsigned char *)0xE000E417)
#define NVIC_IP23_OFFSET 0x317

#define NVIC_IP24 (*(volatile unsigned char *)0xE000E418)
#define NVIC_IP24_OFFSET 0x318

#define NVIC_IP25 (*(volatile unsigned char *)0xE000E419)
#define NVIC_IP25_OFFSET 0x319

#define NVIC_IP26 (*(volatile unsigned char *)0xE000E41A)
#define NVIC_IP26_OFFSET 0x31A

#define NVIC_IP27 (*(volatile unsigned char *)0xE000E41B)
#define NVIC_IP27_OFFSET 0x31B

#define NVIC_IP28 (*(volatile unsigned char *)0xE000E41C)
#define NVIC_IP28_OFFSET 0x31C

#define NVIC_IP29 (*(volatile unsigned char *)0xE000E41D)
#define NVIC_IP29_OFFSET 0x31D

#define NVIC_IP30 (*(volatile unsigned char *)0xE000E41E)
#define NVIC_IP30_OFFSET 0x31E

#define NVIC_IP31 (*(volatile unsigned char *)0xE000E41F)
#define NVIC_IP31_OFFSET 0x31F

#define NVIC_IP32 (*(volatile unsigned char *)0xE000E420)
#define NVIC_IP32_OFFSET 0x320

#define NVIC_IP33 (*(volatile unsigned char *)0xE000E421)
#define NVIC_IP33_OFFSET 0x321

#define NVIC_IP34 (*(volatile unsigned char *)0xE000E422)
#define NVIC_IP34_OFFSET 0x322

#define NVIC_IP35 (*(volatile unsigned char *)0xE000E423)
#define NVIC_IP35_OFFSET 0x323

#define NVIC_IP36 (*(volatile unsigned char *)0xE000E424)
#define NVIC_IP36_OFFSET 0x324

#define NVIC_IP37 (*(volatile unsigned char *)0xE000E425)
#define NVIC_IP37_OFFSET 0x325

#define NVIC_IP38 (*(volatile unsigned char *)0xE000E426)
#define NVIC_IP38_OFFSET 0x326

#define NVIC_IP39 (*(volatile unsigned char *)0xE000E427)
#define NVIC_IP39_OFFSET 0x327

#define NVIC_IP40 (*(volatile unsigned char *)0xE000E428)
#define NVIC_IP40_OFFSET 0x328

#define NVIC_IP41 (*(volatile unsigned char *)0xE000E429)
#define NVIC_IP41_OFFSET 0x329

#define NVIC_IP42 (*(volatile unsigned char *)0xE000E42A)
#define NVIC_IP42_OFFSET 0x32A

#define NVIC_IP43 (*(volatile unsigned char *)0xE000E42B)
#define NVIC_IP43_OFFSET 0x32B

#define NVIC_IP44 (*(volatile unsigned char *)0xE000E42C)
#define NVIC_IP44_OFFSET 0x32C

#define NVIC_IP45 (*(volatile unsigned char *)0xE000E42D)
#define NVIC_IP45_OFFSET 0x32D

#define NVIC_IP46 (*(volatile unsigned char *)0xE000E42E)
#define NVIC_IP46_OFFSET 0x32E

#define NVIC_IP47 (*(volatile unsigned char *)0xE000E42F)
#define NVIC_IP47_OFFSET 0x32F

#define NVIC_IP48 (*(volatile unsigned char *)0xE000E430)
#define NVIC_IP48_OFFSET 0x330

#define NVIC_IP49 (*(volatile unsigned char *)0xE000E431)
#define NVIC_IP49_OFFSET 0x331

#define NVIC_IP50 (*(volatile unsigned char *)0xE000E432)
#define NVIC_IP50_OFFSET 0x332

#define NVIC_IP51 (*(volatile unsigned char *)0xE000E433)
#define NVIC_IP51_OFFSET 0x333

#define NVIC_IP52 (*(volatile unsigned char *)0xE000E434)
#define NVIC_IP52_OFFSET 0x334

#define NVIC_IP53 (*(volatile unsigned char *)0xE000E435)
#define NVIC_IP53_OFFSET 0x335

#define NVIC_IP54 (*(volatile unsigned char *)0xE000E436)
#define NVIC_IP54_OFFSET 0x336

#define NVIC_IP55 (*(volatile unsigned char *)0xE000E437)
#define NVIC_IP55_OFFSET 0x337

#define NVIC_IP56 (*(volatile unsigned char *)0xE000E438)
#define NVIC_IP56_OFFSET 0x338

#define NVIC_IP57 (*(volatile unsigned char *)0xE000E439)
#define NVIC_IP57_OFFSET 0x339

#define NVIC_IP58 (*(volatile unsigned char *)0xE000E43A)
#define NVIC_IP58_OFFSET 0x33A

#define NVIC_IP59 (*(volatile unsigned char *)0xE000E43B)
#define NVIC_IP59_OFFSET 0x33B

#define NVIC_IP60 (*(volatile unsigned char *)0xE000E43C)
#define NVIC_IP60_OFFSET 0x33C

#define NVIC_IP61 (*(volatile unsigned char *)0xE000E43D)
#define NVIC_IP61_OFFSET 0x33D

#define NVIC_IP62 (*(volatile unsigned char *)0xE000E43E)
#define NVIC_IP62_OFFSET 0x33E

#define NVIC_IP63 (*(volatile unsigned char *)0xE000E43F)
#define NVIC_IP63_OFFSET 0x33F

#define NVIC_IP64 (*(volatile unsigned char *)0xE000E440)
#define NVIC_IP64_OFFSET 0x340

#define NVIC_IP65 (*(volatile unsigned char *)0xE000E441)
#define NVIC_IP65_OFFSET 0x341

#define NVIC_IP66 (*(volatile unsigned char *)0xE000E442)
#define NVIC_IP66_OFFSET 0x342

#define NVIC_IP67 (*(volatile unsigned char *)0xE000E443)
#define NVIC_IP67_OFFSET 0x343

#define NVIC_IP68 (*(volatile unsigned char *)0xE000E444)
#define NVIC_IP68_OFFSET 0x344

#define NVIC_IP69 (*(volatile unsigned char *)0xE000E445)
#define NVIC_IP69_OFFSET 0x345

#define NVIC_IP70 (*(volatile unsigned char *)0xE000E446)
#define NVIC_IP70_OFFSET 0x346

#define NVIC_IP71 (*(volatile unsigned char *)0xE000E447)
#define NVIC_IP71_OFFSET 0x347

#define NVIC_IP72 (*(volatile unsigned char *)0xE000E448)
#define NVIC_IP72_OFFSET 0x348

#define NVIC_IP73 (*(volatile unsigned char *)0xE000E449)
#define NVIC_IP73_OFFSET 0x349

#define NVIC_IP74 (*(volatile unsigned char *)0xE000E44A)
#define NVIC_IP74_OFFSET 0x34A

#define NVIC_IP75 (*(volatile unsigned char *)0xE000E44B)
#define NVIC_IP75_OFFSET 0x34B

#define NVIC_IP76 (*(volatile unsigned char *)0xE000E44C)
#define NVIC_IP76_OFFSET 0x34C

#define NVIC_IP77 (*(volatile unsigned char *)0xE000E44D)
#define NVIC_IP77_OFFSET 0x34D

#define NVIC_IP78 (*(volatile unsigned char *)0xE000E44E)
#define NVIC_IP78_OFFSET 0x34E

#define NVIC_IP79 (*(volatile unsigned char *)0xE000E44F)
#define NVIC_IP79_OFFSET 0x34F

#define NVIC_IP80 (*(volatile unsigned char *)0xE000E450)
#define NVIC_IP80_OFFSET 0x350

#define NVIC_IP81 (*(volatile unsigned char *)0xE000E451)
#define NVIC_IP81_OFFSET 0x351

#define NVIC_IP82 (*(volatile unsigned char *)0xE000E452)
#define NVIC_IP82_OFFSET 0x352

#define NVIC_IP83 (*(volatile unsigned char *)0xE000E453)
#define NVIC_IP83_OFFSET 0x353

#define NVIC_IP84 (*(volatile unsigned char *)0xE000E454)
#define NVIC_IP84_OFFSET 0x354

#define NVIC_IP85 (*(volatile unsigned char *)0xE000E455)
#define NVIC_IP85_OFFSET 0x355

#define NVIC_IP86 (*(volatile unsigned char *)0xE000E456)
#define NVIC_IP86_OFFSET 0x356

#define NVIC_IP87 (*(volatile unsigned char *)0xE000E457)
#define NVIC_IP87_OFFSET 0x357

#define NVIC_IP88 (*(volatile unsigned char *)0xE000E458)
#define NVIC_IP88_OFFSET 0x358

#define NVIC_IP89 (*(volatile unsigned char *)0xE000E459)
#define NVIC_IP89_OFFSET 0x359

#define NVIC_IP90 (*(volatile unsigned char *)0xE000E45A)
#define NVIC_IP90_OFFSET 0x35A

#define NVIC_IP91 (*(volatile unsigned char *)0xE000E45B)
#define NVIC_IP91_OFFSET 0x35B

#define NVIC_IP92 (*(volatile unsigned char *)0xE000E45C)
#define NVIC_IP92_OFFSET 0x35C

#define NVIC_IP93 (*(volatile unsigned char *)0xE000E45D)
#define NVIC_IP93_OFFSET 0x35D

#define NVIC_IP94 (*(volatile unsigned char *)0xE000E45E)
#define NVIC_IP94_OFFSET 0x35E

#define NVIC_IP95 (*(volatile unsigned char *)0xE000E45F)
#define NVIC_IP95_OFFSET 0x35F

#define NVIC_IP96 (*(volatile unsigned char *)0xE000E460)
#define NVIC_IP96_OFFSET 0x360

#define NVIC_IP97 (*(volatile unsigned char *)0xE000E461)
#define NVIC_IP97_OFFSET 0x361

#define NVIC_IP98 (*(volatile unsigned char *)0xE000E462)
#define NVIC_IP98_OFFSET 0x362

#define NVIC_IP99 (*(volatile unsigned char *)0xE000E463)
#define NVIC_IP99_OFFSET 0x363

#define NVIC_IP100 (*(volatile unsigned char *)0xE000E464)
#define NVIC_IP100_OFFSET 0x364

#define NVIC_IP101 (*(volatile unsigned char *)0xE000E465)
#define NVIC_IP101_OFFSET 0x365

#define NVIC_IP102 (*(volatile unsigned char *)0xE000E466)
#define NVIC_IP102_OFFSET 0x366

#define NVIC_IP103 (*(volatile unsigned char *)0xE000E467)
#define NVIC_IP103_OFFSET 0x367

#define NVIC_IP104 (*(volatile unsigned char *)0xE000E468)
#define NVIC_IP104_OFFSET 0x368

#define NVIC_IP105 (*(volatile unsigned char *)0xE000E469)
#define NVIC_IP105_OFFSET 0x369

#define NVIC_IP106 (*(volatile unsigned char *)0xE000E46A)
#define NVIC_IP106_OFFSET 0x36A

#define NVIC_IP107 (*(volatile unsigned char *)0xE000E46B)
#define NVIC_IP107_OFFSET 0x36B

#define NVIC_IP108 (*(volatile unsigned char *)0xE000E46C)
#define NVIC_IP108_OFFSET 0x36C

#define NVIC_IP109 (*(volatile unsigned char *)0xE000E46D)
#define NVIC_IP109_OFFSET 0x36D

#define NVIC_IP110 (*(volatile unsigned char *)0xE000E46E)
#define NVIC_IP110_OFFSET 0x36E

#define NVIC_IP111 (*(volatile unsigned char *)0xE000E46F)
#define NVIC_IP111_OFFSET 0x36F

#define NVIC_STIR (*(volatile unsigned *)0xE000EF00)
#define SCB_SW_TRIG NVIC_STIR
#define NVIC_STIR_OFFSET 0xE00
#define SCB_SW_TRIG_OFFSET NVIC_STIR_OFFSET
#define NVIC_STIR_INTID_MASK 0x3FF
#define SCB_SW_TRIG_INTID_MASK NVIC_STIR_INTID_MASK
#define NVIC_STIR_INTID_BIT 0
#define SCB_SW_TRIG_INTID_BIT NVIC_STIR_INTID_BIT

#define SCB_BASE_ADDRESS 0xE000ED00

#define SCB_CPUID (*(volatile unsigned *)0xE000ED00)
#define CPUID_Base SCB_CPUID
#define SCB_CPUID_OFFSET 0x0
#define CPUID_Base_OFFSET SCB_CPUID_OFFSET
#define SCB_CPUID_IMPLEMENTER_MASK 0xFF000000
#define CPUID_Base_IMPLEMENTER_MASK SCB_CPUID_IMPLEMENTER_MASK
#define SCB_CPUID_IMPLEMENTER_BIT 24
#define CPUID_Base_IMPLEMENTER_BIT SCB_CPUID_IMPLEMENTER_BIT
#define SCB_CPUID_VARIANT_MASK 0xF00000
#define CPUID_Base_VARIANT_MASK SCB_CPUID_VARIANT_MASK
#define SCB_CPUID_VARIANT_BIT 20
#define CPUID_Base_VARIANT_BIT SCB_CPUID_VARIANT_BIT
#define SCB_CPUID_PARTNO_MASK 0xFFF0
#define CPUID_Base_PARTNO_MASK SCB_CPUID_PARTNO_MASK
#define SCB_CPUID_PARTNO_BIT 4
#define CPUID_Base_PARTNO_BIT SCB_CPUID_PARTNO_BIT
#define SCB_CPUID_REVISION_MASK 0xF
#define CPUID_Base_REVISION_MASK SCB_CPUID_REVISION_MASK
#define SCB_CPUID_REVISION_BIT 0
#define CPUID_Base_REVISION_BIT SCB_CPUID_REVISION_BIT

#define SCB_ICSR (*(volatile unsigned *)0xE000ED04)
#define Interrupt_Control_State SCB_ICSR
#define SCB_ICSR_OFFSET 0x4
#define Interrupt_Control_State_OFFSET SCB_ICSR_OFFSET
#define SCB_ICSR_NMIPENDSET_MASK 0x80000000
#define Interrupt_Control_State_NMIPENDSET_MASK SCB_ICSR_NMIPENDSET_MASK
#define SCB_ICSR_NMIPENDSET 0x80000000
#define Interrupt_Control_State_NMIPENDSET SCB_ICSR_NMIPENDSET
#define SCB_ICSR_NMIPENDSET_BIT 31
#define Interrupt_Control_State_NMIPENDSET_BIT SCB_ICSR_NMIPENDSET_BIT
#define SCB_ICSR_PENDSVSET_MASK 0x10000000
#define Interrupt_Control_State_PENDSVSET_MASK SCB_ICSR_PENDSVSET_MASK
#define SCB_ICSR_PENDSVSET 0x10000000
#define Interrupt_Control_State_PENDSVSET SCB_ICSR_PENDSVSET
#define SCB_ICSR_PENDSVSET_BIT 28
#define Interrupt_Control_State_PENDSVSET_BIT SCB_ICSR_PENDSVSET_BIT
#define SCB_ICSR_PENDSVCLR_MASK 0x8000000
#define Interrupt_Control_State_PENDSVCLR_MASK SCB_ICSR_PENDSVCLR_MASK
#define SCB_ICSR_PENDSVCLR 0x8000000
#define Interrupt_Control_State_PENDSVCLR SCB_ICSR_PENDSVCLR
#define SCB_ICSR_PENDSVCLR_BIT 27
#define Interrupt_Control_State_PENDSVCLR_BIT SCB_ICSR_PENDSVCLR_BIT
#define SCB_ICSR_PENDSTSET_MASK 0x4000000
#define Interrupt_Control_State_PENDSTSET_MASK SCB_ICSR_PENDSTSET_MASK
#define SCB_ICSR_PENDSTSET 0x4000000
#define Interrupt_Control_State_PENDSTSET SCB_ICSR_PENDSTSET
#define SCB_ICSR_PENDSTSET_BIT 26
#define Interrupt_Control_State_PENDSTSET_BIT SCB_ICSR_PENDSTSET_BIT
#define SCB_ICSR_PENDSTCLR_MASK 0x2000000
#define Interrupt_Control_State_PENDSTCLR_MASK SCB_ICSR_PENDSTCLR_MASK
#define SCB_ICSR_PENDSTCLR 0x2000000
#define Interrupt_Control_State_PENDSTCLR SCB_ICSR_PENDSTCLR
#define SCB_ICSR_PENDSTCLR_BIT 25
#define Interrupt_Control_State_PENDSTCLR_BIT SCB_ICSR_PENDSTCLR_BIT
#define SCB_ICSR_ISRPREEMPT_MASK 0x800000
#define Interrupt_Control_State_ISRPREEMPT_MASK SCB_ICSR_ISRPREEMPT_MASK
#define SCB_ICSR_ISRPREEMPT 0x800000
#define Interrupt_Control_State_ISRPREEMPT SCB_ICSR_ISRPREEMPT
#define SCB_ICSR_ISRPREEMPT_BIT 23
#define Interrupt_Control_State_ISRPREEMPT_BIT SCB_ICSR_ISRPREEMPT_BIT
#define SCB_ICSR_ISRPENDING_MASK 0x400000
#define Interrupt_Control_State_ISRPENDING_MASK SCB_ICSR_ISRPENDING_MASK
#define SCB_ICSR_ISRPENDING 0x400000
#define Interrupt_Control_State_ISRPENDING SCB_ICSR_ISRPENDING
#define SCB_ICSR_ISRPENDING_BIT 22
#define Interrupt_Control_State_ISRPENDING_BIT SCB_ICSR_ISRPENDING_BIT
#define SCB_ICSR_VECTPENDING_MASK 0x1FF000
#define Interrupt_Control_State_VECTPENDING_MASK SCB_ICSR_VECTPENDING_MASK
#define SCB_ICSR_VECTPENDING_BIT 12
#define Interrupt_Control_State_VECTPENDING_BIT SCB_ICSR_VECTPENDING_BIT
#define SCB_ICSR_RETTOBASE_MASK 0x800
#define Interrupt_Control_State_RETTOBASE_MASK SCB_ICSR_RETTOBASE_MASK
#define SCB_ICSR_RETTOBASE 0x800
#define Interrupt_Control_State_RETTOBASE SCB_ICSR_RETTOBASE
#define SCB_ICSR_RETTOBASE_BIT 11
#define Interrupt_Control_State_RETTOBASE_BIT SCB_ICSR_RETTOBASE_BIT
#define SCB_ICSR_VECTACTIVE_MASK 0x1FF
#define Interrupt_Control_State_VECTACTIVE_MASK SCB_ICSR_VECTACTIVE_MASK
#define SCB_ICSR_VECTACTIVE_BIT 0
#define Interrupt_Control_State_VECTACTIVE_BIT SCB_ICSR_VECTACTIVE_BIT

#define SCB_VTOR (*(volatile unsigned *)0xE000ED08)
#define Vector_Table_Offset SCB_VTOR
#define SCB_VTOR_OFFSET 0x8
#define Vector_Table_Offset_OFFSET SCB_VTOR_OFFSET
#define SCB_VTOR_TBLBASE_MASK 0x20000000
#define Vector_Table_Offset_TBLBASE_MASK SCB_VTOR_TBLBASE_MASK
#define SCB_VTOR_TBLBASE 0x20000000
#define Vector_Table_Offset_TBLBASE SCB_VTOR_TBLBASE
#define SCB_VTOR_TBLBASE_BIT 29
#define Vector_Table_Offset_TBLBASE_BIT SCB_VTOR_TBLBASE_BIT
#define SCB_VTOR_TBLOFF_MASK 0x1FFFFF80
#define Vector_Table_Offset_TBLOFF_MASK SCB_VTOR_TBLOFF_MASK
#define SCB_VTOR_TBLOFF_BIT 7
#define Vector_Table_Offset_TBLOFF_BIT SCB_VTOR_TBLOFF_BIT

#define SCB_AIRCR (*(volatile unsigned *)0xE000ED0C)
#define Application_Interrupt_and_Reset_Control SCB_AIRCR
#define SCB_AIRCR_OFFSET 0xC
#define Application_Interrupt_and_Reset_Control_OFFSET SCB_AIRCR_OFFSET
#define SCB_AIRCR_VECTKEY_MASK 0xFFFF0000
#define Application_Interrupt_and_Reset_Control_VECTKEY_MASK SCB_AIRCR_VECTKEY_MASK
#define SCB_AIRCR_VECTKEY_BIT 16
#define Application_Interrupt_and_Reset_Control_VECTKEY_BIT SCB_AIRCR_VECTKEY_BIT
#define SCB_AIRCR_ENDIANESS_MASK 0x8000
#define Application_Interrupt_and_Reset_Control_ENDIANESS_MASK SCB_AIRCR_ENDIANESS_MASK
#define SCB_AIRCR_ENDIANESS 0x8000
#define Application_Interrupt_and_Reset_Control_ENDIANESS SCB_AIRCR_ENDIANESS
#define SCB_AIRCR_ENDIANESS_BIT 15
#define Application_Interrupt_and_Reset_Control_ENDIANESS_BIT SCB_AIRCR_ENDIANESS_BIT
#define SCB_AIRCR_PRIGROUP_MASK 0x700
#define Application_Interrupt_and_Reset_Control_PRIGROUP_MASK SCB_AIRCR_PRIGROUP_MASK
#define SCB_AIRCR_PRIGROUP_BIT 8
#define Application_Interrupt_and_Reset_Control_PRIGROUP_BIT SCB_AIRCR_PRIGROUP_BIT
#define SCB_AIRCR_SYSRESETREQ_MASK 0x4
#define Application_Interrupt_and_Reset_Control_SYSRESETREQ_MASK SCB_AIRCR_SYSRESETREQ_MASK
#define SCB_AIRCR_SYSRESETREQ 0x4
#define Application_Interrupt_and_Reset_Control_SYSRESETREQ SCB_AIRCR_SYSRESETREQ
#define SCB_AIRCR_SYSRESETREQ_BIT 2
#define Application_Interrupt_and_Reset_Control_SYSRESETREQ_BIT SCB_AIRCR_SYSRESETREQ_BIT
#define SCB_AIRCR_VECTCLRACTIVE_MASK 0x2
#define Application_Interrupt_and_Reset_Control_VECTCLRACTIVE_MASK SCB_AIRCR_VECTCLRACTIVE_MASK
#define SCB_AIRCR_VECTCLRACTIVE 0x2
#define Application_Interrupt_and_Reset_Control_VECTCLRACTIVE SCB_AIRCR_VECTCLRACTIVE
#define SCB_AIRCR_VECTCLRACTIVE_BIT 1
#define Application_Interrupt_and_Reset_Control_VECTCLRACTIVE_BIT SCB_AIRCR_VECTCLRACTIVE_BIT
#define SCB_AIRCR_VECTRESET_MASK 0x1
#define Application_Interrupt_and_Reset_Control_VECTRESET_MASK SCB_AIRCR_VECTRESET_MASK
#define SCB_AIRCR_VECTRESET 0x1
#define Application_Interrupt_and_Reset_Control_VECTRESET SCB_AIRCR_VECTRESET
#define SCB_AIRCR_VECTRESET_BIT 0
#define Application_Interrupt_and_Reset_Control_VECTRESET_BIT SCB_AIRCR_VECTRESET_BIT

#define SCB_SCR (*(volatile unsigned *)0xE000ED10)
#define System_Control SCB_SCR
#define SCB_SCR_OFFSET 0x10
#define System_Control_OFFSET SCB_SCR_OFFSET
#define SCB_SCR_SEVONPEND_MASK 0x10
#define System_Control_SEVONPEND_MASK SCB_SCR_SEVONPEND_MASK
#define SCB_SCR_SEVONPEND 0x10
#define System_Control_SEVONPEND SCB_SCR_SEVONPEND
#define SCB_SCR_SEVONPEND_BIT 4
#define System_Control_SEVONPEND_BIT SCB_SCR_SEVONPEND_BIT
#define SCB_SCR_SLEEPDEEP_MASK 0x4
#define System_Control_SLEEPDEEP_MASK SCB_SCR_SLEEPDEEP_MASK
#define SCB_SCR_SLEEPDEEP 0x4
#define System_Control_SLEEPDEEP SCB_SCR_SLEEPDEEP
#define SCB_SCR_SLEEPDEEP_BIT 2
#define System_Control_SLEEPDEEP_BIT SCB_SCR_SLEEPDEEP_BIT
#define SCB_SCR_SLEEPONEXIT_MASK 0x2
#define System_Control_SLEEPONEXIT_MASK SCB_SCR_SLEEPONEXIT_MASK
#define SCB_SCR_SLEEPONEXIT 0x2
#define System_Control_SLEEPONEXIT SCB_SCR_SLEEPONEXIT
#define SCB_SCR_SLEEPONEXIT_BIT 1
#define System_Control_SLEEPONEXIT_BIT SCB_SCR_SLEEPONEXIT_BIT

#define SCB_CCR (*(volatile unsigned *)0xE000ED14)
#define Configuration_Control SCB_CCR
#define SCB_CCR_OFFSET 0x14
#define Configuration_Control_OFFSET SCB_CCR_OFFSET
#define SCB_CCR_STKALIGN_MASK 0x200
#define Configuration_Control_STKALIGN_MASK SCB_CCR_STKALIGN_MASK
#define SCB_CCR_STKALIGN 0x200
#define Configuration_Control_STKALIGN SCB_CCR_STKALIGN
#define SCB_CCR_STKALIGN_BIT 9
#define Configuration_Control_STKALIGN_BIT SCB_CCR_STKALIGN_BIT
#define SCB_CCR_BFHFNMIGN_MASK 0x100
#define Configuration_Control_BFHFNMIGN_MASK SCB_CCR_BFHFNMIGN_MASK
#define SCB_CCR_BFHFNMIGN 0x100
#define Configuration_Control_BFHFNMIGN SCB_CCR_BFHFNMIGN
#define SCB_CCR_BFHFNMIGN_BIT 8
#define Configuration_Control_BFHFNMIGN_BIT SCB_CCR_BFHFNMIGN_BIT
#define SCB_CCR_DIV_0_TRP_MASK 0x10
#define Configuration_Control_DIV_0_TRP_MASK SCB_CCR_DIV_0_TRP_MASK
#define SCB_CCR_DIV_0_TRP 0x10
#define Configuration_Control_DIV_0_TRP SCB_CCR_DIV_0_TRP
#define SCB_CCR_DIV_0_TRP_BIT 4
#define Configuration_Control_DIV_0_TRP_BIT SCB_CCR_DIV_0_TRP_BIT
#define SCB_CCR_UNALIGN_TRP_MASK 0x8
#define Configuration_Control_UNALIGN_TRP_MASK SCB_CCR_UNALIGN_TRP_MASK
#define SCB_CCR_UNALIGN_TRP 0x8
#define Configuration_Control_UNALIGN_TRP SCB_CCR_UNALIGN_TRP
#define SCB_CCR_UNALIGN_TRP_BIT 3
#define Configuration_Control_UNALIGN_TRP_BIT SCB_CCR_UNALIGN_TRP_BIT
#define SCB_CCR_USERSETMPEND_MASK 0x2
#define Configuration_Control_USERSETMPEND_MASK SCB_CCR_USERSETMPEND_MASK
#define SCB_CCR_USERSETMPEND 0x2
#define Configuration_Control_USERSETMPEND SCB_CCR_USERSETMPEND
#define SCB_CCR_USERSETMPEND_BIT 1
#define Configuration_Control_USERSETMPEND_BIT SCB_CCR_USERSETMPEND_BIT
#define SCB_CCR_NONBASETHRDENA_MASK 0x1
#define Configuration_Control_NONBASETHRDENA_MASK SCB_CCR_NONBASETHRDENA_MASK
#define SCB_CCR_NONBASETHRDENA 0x1
#define Configuration_Control_NONBASETHRDENA SCB_CCR_NONBASETHRDENA
#define SCB_CCR_NONBASETHRDENA_BIT 0
#define Configuration_Control_NONBASETHRDENA_BIT SCB_CCR_NONBASETHRDENA_BIT

#define SCB_SHPR1 (*(volatile unsigned *)0xE000ED18)
#define System_Handlers_4_7_Priority SCB_SHPR1
#define SCB_SHPR1_OFFSET 0x18
#define System_Handlers_4_7_Priority_OFFSET SCB_SHPR1_OFFSET
#define SCB_SHPR1_PRI_4_MASK 0xFF
#define System_Handlers_4_7_Priority_PRI_4_MASK SCB_SHPR1_PRI_4_MASK
#define SCB_SHPR1_PRI_4_BIT 0
#define System_Handlers_4_7_Priority_PRI_4_BIT SCB_SHPR1_PRI_4_BIT
#define SCB_SHPR1_PRI_5_MASK 0xFF00
#define System_Handlers_4_7_Priority_PRI_5_MASK SCB_SHPR1_PRI_5_MASK
#define SCB_SHPR1_PRI_5_BIT 8
#define System_Handlers_4_7_Priority_PRI_5_BIT SCB_SHPR1_PRI_5_BIT
#define SCB_SHPR1_PRI_6_MASK 0xFF0000
#define System_Handlers_4_7_Priority_PRI_6_MASK SCB_SHPR1_PRI_6_MASK
#define SCB_SHPR1_PRI_6_BIT 16
#define System_Handlers_4_7_Priority_PRI_6_BIT SCB_SHPR1_PRI_6_BIT
#define SCB_SHPR1_PRI_7_MASK 0xFF000000
#define System_Handlers_4_7_Priority_PRI_7_MASK SCB_SHPR1_PRI_7_MASK
#define SCB_SHPR1_PRI_7_BIT 24
#define System_Handlers_4_7_Priority_PRI_7_BIT SCB_SHPR1_PRI_7_BIT

#define SCB_SHPR2 (*(volatile unsigned *)0xE000ED1C)
#define System_Handlers_8_11_Priority SCB_SHPR2
#define SCB_SHPR2_OFFSET 0x1C
#define System_Handlers_8_11_Priority_OFFSET SCB_SHPR2_OFFSET
#define SCB_SHPR2_PRI_8_MASK 0xFF
#define System_Handlers_8_11_Priority_PRI_8_MASK SCB_SHPR2_PRI_8_MASK
#define SCB_SHPR2_PRI_8_BIT 0
#define System_Handlers_8_11_Priority_PRI_8_BIT SCB_SHPR2_PRI_8_BIT
#define SCB_SHPR2_PRI_9_MASK 0xFF00
#define System_Handlers_8_11_Priority_PRI_9_MASK SCB_SHPR2_PRI_9_MASK
#define SCB_SHPR2_PRI_9_BIT 8
#define System_Handlers_8_11_Priority_PRI_9_BIT SCB_SHPR2_PRI_9_BIT
#define SCB_SHPR2_PRI_10_MASK 0xFF0000
#define System_Handlers_8_11_Priority_PRI_10_MASK SCB_SHPR2_PRI_10_MASK
#define SCB_SHPR2_PRI_10_BIT 16
#define System_Handlers_8_11_Priority_PRI_10_BIT SCB_SHPR2_PRI_10_BIT
#define SCB_SHPR2_PRI_11_MASK 0xFF000000
#define System_Handlers_8_11_Priority_PRI_11_MASK SCB_SHPR2_PRI_11_MASK
#define SCB_SHPR2_PRI_11_BIT 24
#define System_Handlers_8_11_Priority_PRI_11_BIT SCB_SHPR2_PRI_11_BIT

#define SCB_SHPR3 (*(volatile unsigned *)0xE000ED20)
#define System_Handlers_12_15_Priority SCB_SHPR3
#define SCB_SHPR3_OFFSET 0x20
#define System_Handlers_12_15_Priority_OFFSET SCB_SHPR3_OFFSET
#define SCB_SHPR3_PRI_12_MASK 0xFF
#define System_Handlers_12_15_Priority_PRI_12_MASK SCB_SHPR3_PRI_12_MASK
#define SCB_SHPR3_PRI_12_BIT 0
#define System_Handlers_12_15_Priority_PRI_12_BIT SCB_SHPR3_PRI_12_BIT
#define SCB_SHPR3_PRI_13_MASK 0xFF00
#define System_Handlers_12_15_Priority_PRI_13_MASK SCB_SHPR3_PRI_13_MASK
#define SCB_SHPR3_PRI_13_BIT 8
#define System_Handlers_12_15_Priority_PRI_13_BIT SCB_SHPR3_PRI_13_BIT
#define SCB_SHPR3_PRI_14_MASK 0xFF0000
#define System_Handlers_12_15_Priority_PRI_14_MASK SCB_SHPR3_PRI_14_MASK
#define SCB_SHPR3_PRI_14_BIT 16
#define System_Handlers_12_15_Priority_PRI_14_BIT SCB_SHPR3_PRI_14_BIT
#define SCB_SHPR3_PRI_15_MASK 0xFF000000
#define System_Handlers_12_15_Priority_PRI_15_MASK SCB_SHPR3_PRI_15_MASK
#define SCB_SHPR3_PRI_15_BIT 24
#define System_Handlers_12_15_Priority_PRI_15_BIT SCB_SHPR3_PRI_15_BIT

#define SCB_SHP0 (*(volatile unsigned char *)0xE000ED18)
#define SCB_SHP0_OFFSET 0x18

#define SCB_SHP1 (*(volatile unsigned char *)0xE000ED19)
#define SCB_SHP1_OFFSET 0x19

#define SCB_SHP2 (*(volatile unsigned char *)0xE000ED1A)
#define SCB_SHP2_OFFSET 0x1A

#define SCB_SHP3 (*(volatile unsigned char *)0xE000ED1B)
#define SCB_SHP3_OFFSET 0x1B

#define SCB_SHP4 (*(volatile unsigned char *)0xE000ED1C)
#define SCB_SHP4_OFFSET 0x1C

#define SCB_SHP5 (*(volatile unsigned char *)0xE000ED1D)
#define SCB_SHP5_OFFSET 0x1D

#define SCB_SHP6 (*(volatile unsigned char *)0xE000ED1E)
#define SCB_SHP6_OFFSET 0x1E

#define SCB_SHP7 (*(volatile unsigned char *)0xE000ED1F)
#define SCB_SHP7_OFFSET 0x1F

#define SCB_SHP8 (*(volatile unsigned char *)0xE000ED20)
#define SCB_SHP8_OFFSET 0x20

#define SCB_SHP9 (*(volatile unsigned char *)0xE000ED21)
#define SCB_SHP9_OFFSET 0x21

#define SCB_SHP10 (*(volatile unsigned char *)0xE000ED22)
#define SCB_SHP10_OFFSET 0x22

#define SCB_SHP11 (*(volatile unsigned char *)0xE000ED23)
#define SCB_SHP11_OFFSET 0x23

#define SCB_SHCRS (*(volatile unsigned *)0xE000ED24)
#define System_Handler_Control_and_State SCB_SHCRS
#define SCB_SHCRS_OFFSET 0x24
#define System_Handler_Control_and_State_OFFSET SCB_SHCRS_OFFSET
#define SCB_SHCRS_USGFAULTENA_MASK 0x40000
#define System_Handler_Control_and_State_USGFAULTENA_MASK SCB_SHCRS_USGFAULTENA_MASK
#define SCB_SHCRS_USGFAULTENA 0x40000
#define System_Handler_Control_and_State_USGFAULTENA SCB_SHCRS_USGFAULTENA
#define SCB_SHCRS_USGFAULTENA_BIT 18
#define System_Handler_Control_and_State_USGFAULTENA_BIT SCB_SHCRS_USGFAULTENA_BIT
#define SCB_SHCRS_BUSFAULTENA_MASK 0x20000
#define System_Handler_Control_and_State_BUSFAULTENA_MASK SCB_SHCRS_BUSFAULTENA_MASK
#define SCB_SHCRS_BUSFAULTENA 0x20000
#define System_Handler_Control_and_State_BUSFAULTENA SCB_SHCRS_BUSFAULTENA
#define SCB_SHCRS_BUSFAULTENA_BIT 17
#define System_Handler_Control_and_State_BUSFAULTENA_BIT SCB_SHCRS_BUSFAULTENA_BIT
#define SCB_SHCRS_MEMFAULTENA_MASK 0x10000
#define System_Handler_Control_and_State_MEMFAULTENA_MASK SCB_SHCRS_MEMFAULTENA_MASK
#define SCB_SHCRS_MEMFAULTENA 0x10000
#define System_Handler_Control_and_State_MEMFAULTENA SCB_SHCRS_MEMFAULTENA
#define SCB_SHCRS_MEMFAULTENA_BIT 16
#define System_Handler_Control_and_State_MEMFAULTENA_BIT SCB_SHCRS_MEMFAULTENA_BIT
#define SCB_SHCRS_SVCALLPENDED_MASK 0x8000
#define System_Handler_Control_and_State_SVCALLPENDED_MASK SCB_SHCRS_SVCALLPENDED_MASK
#define SCB_SHCRS_SVCALLPENDED 0x8000
#define System_Handler_Control_and_State_SVCALLPENDED SCB_SHCRS_SVCALLPENDED
#define SCB_SHCRS_SVCALLPENDED_BIT 15
#define System_Handler_Control_and_State_SVCALLPENDED_BIT SCB_SHCRS_SVCALLPENDED_BIT
#define SCB_SHCRS_BUSFAULTPENDED_MASK 0x4000
#define System_Handler_Control_and_State_BUSFAULTPENDED_MASK SCB_SHCRS_BUSFAULTPENDED_MASK
#define SCB_SHCRS_BUSFAULTPENDED 0x4000
#define System_Handler_Control_and_State_BUSFAULTPENDED SCB_SHCRS_BUSFAULTPENDED
#define SCB_SHCRS_BUSFAULTPENDED_BIT 14
#define System_Handler_Control_and_State_BUSFAULTPENDED_BIT SCB_SHCRS_BUSFAULTPENDED_BIT
#define SCB_SHCRS_MEMFAULTPENDED_MASK 0x2000
#define System_Handler_Control_and_State_MEMFAULTPENDED_MASK SCB_SHCRS_MEMFAULTPENDED_MASK
#define SCB_SHCRS_MEMFAULTPENDED 0x2000
#define System_Handler_Control_and_State_MEMFAULTPENDED SCB_SHCRS_MEMFAULTPENDED
#define SCB_SHCRS_MEMFAULTPENDED_BIT 13
#define System_Handler_Control_and_State_MEMFAULTPENDED_BIT SCB_SHCRS_MEMFAULTPENDED_BIT
#define SCB_SHCRS_USGFAULTPENDED_MASK 0x1000
#define System_Handler_Control_and_State_USGFAULTPENDED_MASK SCB_SHCRS_USGFAULTPENDED_MASK
#define SCB_SHCRS_USGFAULTPENDED 0x1000
#define System_Handler_Control_and_State_USGFAULTPENDED SCB_SHCRS_USGFAULTPENDED
#define SCB_SHCRS_USGFAULTPENDED_BIT 12
#define System_Handler_Control_and_State_USGFAULTPENDED_BIT SCB_SHCRS_USGFAULTPENDED_BIT
#define SCB_SHCRS_SYSTICKACT_MASK 0x800
#define System_Handler_Control_and_State_SYSTICKACT_MASK SCB_SHCRS_SYSTICKACT_MASK
#define SCB_SHCRS_SYSTICKACT 0x800
#define System_Handler_Control_and_State_SYSTICKACT SCB_SHCRS_SYSTICKACT
#define SCB_SHCRS_SYSTICKACT_BIT 11
#define System_Handler_Control_and_State_SYSTICKACT_BIT SCB_SHCRS_SYSTICKACT_BIT
#define SCB_SHCRS_PENDSVACT_MASK 0x400
#define System_Handler_Control_and_State_PENDSVACT_MASK SCB_SHCRS_PENDSVACT_MASK
#define SCB_SHCRS_PENDSVACT 0x400
#define System_Handler_Control_and_State_PENDSVACT SCB_SHCRS_PENDSVACT
#define SCB_SHCRS_PENDSVACT_BIT 10
#define System_Handler_Control_and_State_PENDSVACT_BIT SCB_SHCRS_PENDSVACT_BIT
#define SCB_SHCRS_MONITORACT_MASK 0x100
#define System_Handler_Control_and_State_MONITORACT_MASK SCB_SHCRS_MONITORACT_MASK
#define SCB_SHCRS_MONITORACT 0x100
#define System_Handler_Control_and_State_MONITORACT SCB_SHCRS_MONITORACT
#define SCB_SHCRS_MONITORACT_BIT 8
#define System_Handler_Control_and_State_MONITORACT_BIT SCB_SHCRS_MONITORACT_BIT
#define SCB_SHCRS_SVCALLACT_MASK 0x80
#define System_Handler_Control_and_State_SVCALLACT_MASK SCB_SHCRS_SVCALLACT_MASK
#define SCB_SHCRS_SVCALLACT 0x80
#define System_Handler_Control_and_State_SVCALLACT SCB_SHCRS_SVCALLACT
#define SCB_SHCRS_SVCALLACT_BIT 7
#define System_Handler_Control_and_State_SVCALLACT_BIT SCB_SHCRS_SVCALLACT_BIT
#define SCB_SHCRS_USGFAULTACT_MASK 0x8
#define System_Handler_Control_and_State_USGFAULTACT_MASK SCB_SHCRS_USGFAULTACT_MASK
#define SCB_SHCRS_USGFAULTACT 0x8
#define System_Handler_Control_and_State_USGFAULTACT SCB_SHCRS_USGFAULTACT
#define SCB_SHCRS_USGFAULTACT_BIT 3
#define System_Handler_Control_and_State_USGFAULTACT_BIT SCB_SHCRS_USGFAULTACT_BIT
#define SCB_SHCRS_BUSFAULTACT_MASK 0x2
#define System_Handler_Control_and_State_BUSFAULTACT_MASK SCB_SHCRS_BUSFAULTACT_MASK
#define SCB_SHCRS_BUSFAULTACT 0x2
#define System_Handler_Control_and_State_BUSFAULTACT SCB_SHCRS_BUSFAULTACT
#define SCB_SHCRS_BUSFAULTACT_BIT 1
#define System_Handler_Control_and_State_BUSFAULTACT_BIT SCB_SHCRS_BUSFAULTACT_BIT
#define SCB_SHCRS_MEMFAULTACT_MASK 0x1
#define System_Handler_Control_and_State_MEMFAULTACT_MASK SCB_SHCRS_MEMFAULTACT_MASK
#define SCB_SHCRS_MEMFAULTACT 0x1
#define System_Handler_Control_and_State_MEMFAULTACT SCB_SHCRS_MEMFAULTACT
#define SCB_SHCRS_MEMFAULTACT_BIT 0
#define System_Handler_Control_and_State_MEMFAULTACT_BIT SCB_SHCRS_MEMFAULTACT_BIT

#define SCB_MMSR (*(volatile unsigned char *)0xE000ED28)
#define Memory_Manage_Fault_Status SCB_MMSR
#define SCB_MMSR_OFFSET 0x28
#define Memory_Manage_Fault_Status_OFFSET SCB_MMSR_OFFSET
#define SCB_MMSR_MMARVALID_MASK 0x80
#define Memory_Manage_Fault_Status_MMARVALID_MASK SCB_MMSR_MMARVALID_MASK
#define SCB_MMSR_MMARVALID 0x80
#define Memory_Manage_Fault_Status_MMARVALID SCB_MMSR_MMARVALID
#define SCB_MMSR_MMARVALID_BIT 7
#define Memory_Manage_Fault_Status_MMARVALID_BIT SCB_MMSR_MMARVALID_BIT
#define SCB_MMSR_MSTKERR_MASK 0x10
#define Memory_Manage_Fault_Status_MSTKERR_MASK SCB_MMSR_MSTKERR_MASK
#define SCB_MMSR_MSTKERR 0x10
#define Memory_Manage_Fault_Status_MSTKERR SCB_MMSR_MSTKERR
#define SCB_MMSR_MSTKERR_BIT 4
#define Memory_Manage_Fault_Status_MSTKERR_BIT SCB_MMSR_MSTKERR_BIT
#define SCB_MMSR_MUNSTKERR_MASK 0x8
#define Memory_Manage_Fault_Status_MUNSTKERR_MASK SCB_MMSR_MUNSTKERR_MASK
#define SCB_MMSR_MUNSTKERR 0x8
#define Memory_Manage_Fault_Status_MUNSTKERR SCB_MMSR_MUNSTKERR
#define SCB_MMSR_MUNSTKERR_BIT 3
#define Memory_Manage_Fault_Status_MUNSTKERR_BIT SCB_MMSR_MUNSTKERR_BIT
#define SCB_MMSR_DACCVIOL_MASK 0x2
#define Memory_Manage_Fault_Status_DACCVIOL_MASK SCB_MMSR_DACCVIOL_MASK
#define SCB_MMSR_DACCVIOL 0x2
#define Memory_Manage_Fault_Status_DACCVIOL SCB_MMSR_DACCVIOL
#define SCB_MMSR_DACCVIOL_BIT 1
#define Memory_Manage_Fault_Status_DACCVIOL_BIT SCB_MMSR_DACCVIOL_BIT
#define SCB_MMSR_IACCVIOL_MASK 0x1
#define Memory_Manage_Fault_Status_IACCVIOL_MASK SCB_MMSR_IACCVIOL_MASK
#define SCB_MMSR_IACCVIOL 0x1
#define Memory_Manage_Fault_Status_IACCVIOL SCB_MMSR_IACCVIOL
#define SCB_MMSR_IACCVIOL_BIT 0
#define Memory_Manage_Fault_Status_IACCVIOL_BIT SCB_MMSR_IACCVIOL_BIT

#define SCB_BFSR (*(volatile unsigned char *)0xE000ED29)
#define Bus_Fault_Status SCB_BFSR
#define SCB_BFSR_OFFSET 0x29
#define Bus_Fault_Status_OFFSET SCB_BFSR_OFFSET
#define SCB_BFSR_BFARVALID_MASK 0x80
#define Bus_Fault_Status_BFARVALID_MASK SCB_BFSR_BFARVALID_MASK
#define SCB_BFSR_BFARVALID 0x80
#define Bus_Fault_Status_BFARVALID SCB_BFSR_BFARVALID
#define SCB_BFSR_BFARVALID_BIT 7
#define Bus_Fault_Status_BFARVALID_BIT SCB_BFSR_BFARVALID_BIT
#define SCB_BFSR_STKERR_MASK 0x10
#define Bus_Fault_Status_STKERR_MASK SCB_BFSR_STKERR_MASK
#define SCB_BFSR_STKERR 0x10
#define Bus_Fault_Status_STKERR SCB_BFSR_STKERR
#define SCB_BFSR_STKERR_BIT 4
#define Bus_Fault_Status_STKERR_BIT SCB_BFSR_STKERR_BIT
#define SCB_BFSR_UNSTKERR_MASK 0x8
#define Bus_Fault_Status_UNSTKERR_MASK SCB_BFSR_UNSTKERR_MASK
#define SCB_BFSR_UNSTKERR 0x8
#define Bus_Fault_Status_UNSTKERR SCB_BFSR_UNSTKERR
#define SCB_BFSR_UNSTKERR_BIT 3
#define Bus_Fault_Status_UNSTKERR_BIT SCB_BFSR_UNSTKERR_BIT
#define SCB_BFSR_IMPRECISERR_MASK 0x4
#define Bus_Fault_Status_IMPRECISERR_MASK SCB_BFSR_IMPRECISERR_MASK
#define SCB_BFSR_IMPRECISERR 0x4
#define Bus_Fault_Status_IMPRECISERR SCB_BFSR_IMPRECISERR
#define SCB_BFSR_IMPRECISERR_BIT 2
#define Bus_Fault_Status_IMPRECISERR_BIT SCB_BFSR_IMPRECISERR_BIT
#define SCB_BFSR_PRECISERR_MASK 0x2
#define Bus_Fault_Status_PRECISERR_MASK SCB_BFSR_PRECISERR_MASK
#define SCB_BFSR_PRECISERR 0x2
#define Bus_Fault_Status_PRECISERR SCB_BFSR_PRECISERR
#define SCB_BFSR_PRECISERR_BIT 1
#define Bus_Fault_Status_PRECISERR_BIT SCB_BFSR_PRECISERR_BIT
#define SCB_BFSR_IBUSERR_MASK 0x1
#define Bus_Fault_Status_IBUSERR_MASK SCB_BFSR_IBUSERR_MASK
#define SCB_BFSR_IBUSERR 0x1
#define Bus_Fault_Status_IBUSERR SCB_BFSR_IBUSERR
#define SCB_BFSR_IBUSERR_BIT 0
#define Bus_Fault_Status_IBUSERR_BIT SCB_BFSR_IBUSERR_BIT

#define SCB_UFSR (*(volatile unsigned short *)0xE000ED2A)
#define Usage_Fault_Status SCB_UFSR
#define SCB_UFSR_OFFSET 0x2A
#define Usage_Fault_Status_OFFSET SCB_UFSR_OFFSET
#define SCB_UFSR_DIVBYZERO_MASK 0x200
#define Usage_Fault_Status_DIVBYZERO_MASK SCB_UFSR_DIVBYZERO_MASK
#define SCB_UFSR_DIVBYZERO 0x200
#define Usage_Fault_Status_DIVBYZERO SCB_UFSR_DIVBYZERO
#define SCB_UFSR_DIVBYZERO_BIT 9
#define Usage_Fault_Status_DIVBYZERO_BIT SCB_UFSR_DIVBYZERO_BIT
#define SCB_UFSR_UNALIGNED_MASK 0x100
#define Usage_Fault_Status_UNALIGNED_MASK SCB_UFSR_UNALIGNED_MASK
#define SCB_UFSR_UNALIGNED 0x100
#define Usage_Fault_Status_UNALIGNED SCB_UFSR_UNALIGNED
#define SCB_UFSR_UNALIGNED_BIT 8
#define Usage_Fault_Status_UNALIGNED_BIT SCB_UFSR_UNALIGNED_BIT
#define SCB_UFSR_NOCP_MASK 0x8
#define Usage_Fault_Status_NOCP_MASK SCB_UFSR_NOCP_MASK
#define SCB_UFSR_NOCP 0x8
#define Usage_Fault_Status_NOCP SCB_UFSR_NOCP
#define SCB_UFSR_NOCP_BIT 3
#define Usage_Fault_Status_NOCP_BIT SCB_UFSR_NOCP_BIT
#define SCB_UFSR_INVPC_MASK 0x4
#define Usage_Fault_Status_INVPC_MASK SCB_UFSR_INVPC_MASK
#define SCB_UFSR_INVPC 0x4
#define Usage_Fault_Status_INVPC SCB_UFSR_INVPC
#define SCB_UFSR_INVPC_BIT 2
#define Usage_Fault_Status_INVPC_BIT SCB_UFSR_INVPC_BIT
#define SCB_UFSR_INVSTATE_MASK 0x2
#define Usage_Fault_Status_INVSTATE_MASK SCB_UFSR_INVSTATE_MASK
#define SCB_UFSR_INVSTATE 0x2
#define Usage_Fault_Status_INVSTATE SCB_UFSR_INVSTATE
#define SCB_UFSR_INVSTATE_BIT 1
#define Usage_Fault_Status_INVSTATE_BIT SCB_UFSR_INVSTATE_BIT
#define SCB_UFSR_UNDEFINSTR_MASK 0x1
#define Usage_Fault_Status_UNDEFINSTR_MASK SCB_UFSR_UNDEFINSTR_MASK
#define SCB_UFSR_UNDEFINSTR 0x1
#define Usage_Fault_Status_UNDEFINSTR SCB_UFSR_UNDEFINSTR
#define SCB_UFSR_UNDEFINSTR_BIT 0
#define Usage_Fault_Status_UNDEFINSTR_BIT SCB_UFSR_UNDEFINSTR_BIT

#define SCB_HFSR (*(volatile unsigned *)0xE000ED2C)
#define Hard_Fault_Status SCB_HFSR
#define SCB_HFSR_OFFSET 0x2C
#define Hard_Fault_Status_OFFSET SCB_HFSR_OFFSET
#define SCB_HFSR_DEBUGEVT_MASK 0x80000000
#define Hard_Fault_Status_DEBUGEVT_MASK SCB_HFSR_DEBUGEVT_MASK
#define SCB_HFSR_DEBUGEVT 0x80000000
#define Hard_Fault_Status_DEBUGEVT SCB_HFSR_DEBUGEVT
#define SCB_HFSR_DEBUGEVT_BIT 31
#define Hard_Fault_Status_DEBUGEVT_BIT SCB_HFSR_DEBUGEVT_BIT
#define SCB_HFSR_FORCED_MASK 0x40000000
#define Hard_Fault_Status_FORCED_MASK SCB_HFSR_FORCED_MASK
#define SCB_HFSR_FORCED 0x40000000
#define Hard_Fault_Status_FORCED SCB_HFSR_FORCED
#define SCB_HFSR_FORCED_BIT 30
#define Hard_Fault_Status_FORCED_BIT SCB_HFSR_FORCED_BIT
#define SCB_HFSR_VECTTBL_MASK 0x2
#define Hard_Fault_Status_VECTTBL_MASK SCB_HFSR_VECTTBL_MASK
#define SCB_HFSR_VECTTBL 0x2
#define Hard_Fault_Status_VECTTBL SCB_HFSR_VECTTBL
#define SCB_HFSR_VECTTBL_BIT 1
#define Hard_Fault_Status_VECTTBL_BIT SCB_HFSR_VECTTBL_BIT

#define SCB_DFSR (*(volatile unsigned *)0xE000ED30)
#define Debug_Fault_Status SCB_DFSR
#define SCB_DFSR_OFFSET 0x30
#define Debug_Fault_Status_OFFSET SCB_DFSR_OFFSET
#define SCB_DFSR_EXTERNAL_MASK 0x8
#define Debug_Fault_Status_EXTERNAL_MASK SCB_DFSR_EXTERNAL_MASK
#define SCB_DFSR_EXTERNAL 0x8
#define Debug_Fault_Status_EXTERNAL SCB_DFSR_EXTERNAL
#define SCB_DFSR_EXTERNAL_BIT 3
#define Debug_Fault_Status_EXTERNAL_BIT SCB_DFSR_EXTERNAL_BIT
#define SCB_DFSR_VCATCH_MASK 0x8
#define Debug_Fault_Status_VCATCH_MASK SCB_DFSR_VCATCH_MASK
#define SCB_DFSR_VCATCH 0x8
#define Debug_Fault_Status_VCATCH SCB_DFSR_VCATCH
#define SCB_DFSR_VCATCH_BIT 3
#define Debug_Fault_Status_VCATCH_BIT SCB_DFSR_VCATCH_BIT
#define SCB_DFSR_DWTTRAP_MASK 0x4
#define Debug_Fault_Status_DWTTRAP_MASK SCB_DFSR_DWTTRAP_MASK
#define SCB_DFSR_DWTTRAP 0x4
#define Debug_Fault_Status_DWTTRAP SCB_DFSR_DWTTRAP
#define SCB_DFSR_DWTTRAP_BIT 2
#define Debug_Fault_Status_DWTTRAP_BIT SCB_DFSR_DWTTRAP_BIT
#define SCB_DFSR_BKPT_MASK 0x2
#define Debug_Fault_Status_BKPT_MASK SCB_DFSR_BKPT_MASK
#define SCB_DFSR_BKPT 0x2
#define Debug_Fault_Status_BKPT SCB_DFSR_BKPT
#define SCB_DFSR_BKPT_BIT 1
#define Debug_Fault_Status_BKPT_BIT SCB_DFSR_BKPT_BIT
#define SCB_DFSR_HALTED_MASK 0x1
#define Debug_Fault_Status_HALTED_MASK SCB_DFSR_HALTED_MASK
#define SCB_DFSR_HALTED 0x1
#define Debug_Fault_Status_HALTED SCB_DFSR_HALTED
#define SCB_DFSR_HALTED_BIT 0
#define Debug_Fault_Status_HALTED_BIT SCB_DFSR_HALTED_BIT

#define SCB_MMFAR (*(volatile unsigned *)0xE000ED34)
#define Memory_Manage_Fault_Address SCB_MMFAR
#define SCB_MMFAR_OFFSET 0x34
#define Memory_Manage_Fault_Address_OFFSET SCB_MMFAR_OFFSET

#define SCB_BFAR (*(volatile unsigned *)0xE000ED38)
#define Bus_Fault_Address SCB_BFAR
#define SCB_BFAR_OFFSET 0x38
#define Bus_Fault_Address_OFFSET SCB_BFAR_OFFSET

#define SCB_AFSR (*(volatile unsigned *)0xE000ED3C)
#define Auxillary_Fault_Address SCB_AFSR
#define SCB_AFSR_OFFSET 0x3C
#define Auxillary_Fault_Address_OFFSET SCB_AFSR_OFFSET

#define SCB_PFR0 (*(volatile unsigned *)0xE000ED40)
#define PFR0 SCB_PFR0
#define SCB_PFR0_OFFSET 0x40
#define PFR0_OFFSET SCB_PFR0_OFFSET

#define SCB_PFR1 (*(volatile unsigned *)0xE000ED44)
#define PFR1 SCB_PFR1
#define SCB_PFR1_OFFSET 0x44
#define PFR1_OFFSET SCB_PFR1_OFFSET

#define SCB_DFR0 (*(volatile unsigned *)0xE000ED48)
#define DFR0 SCB_DFR0
#define SCB_DFR0_OFFSET 0x48
#define DFR0_OFFSET SCB_DFR0_OFFSET

#define SCB_AFR0 (*(volatile unsigned *)0xE000ED4C)
#define AFR0 SCB_AFR0
#define SCB_AFR0_OFFSET 0x4C
#define AFR0_OFFSET SCB_AFR0_OFFSET

#define SCB_MMFR0 (*(volatile unsigned *)0xE000ED50)
#define MMFR0 SCB_MMFR0
#define SCB_MMFR0_OFFSET 0x50
#define MMFR0_OFFSET SCB_MMFR0_OFFSET

#define SCB_MMFR1 (*(volatile unsigned *)0xE000ED54)
#define MMFR1 SCB_MMFR1
#define SCB_MMFR1_OFFSET 0x54
#define MMFR1_OFFSET SCB_MMFR1_OFFSET

#define SCB_MMFR2 (*(volatile unsigned *)0xE000ED58)
#define MMFR2 SCB_MMFR2
#define SCB_MMFR2_OFFSET 0x58
#define MMFR2_OFFSET SCB_MMFR2_OFFSET

#define SCB_MMFR3 (*(volatile unsigned *)0xE000ED5C)
#define MMFR3 SCB_MMFR3
#define SCB_MMFR3_OFFSET 0x5C
#define MMFR3_OFFSET SCB_MMFR3_OFFSET

#define SCB_ISAR0 (*(volatile unsigned *)0xE000ED60)
#define ISAR0 SCB_ISAR0
#define SCB_ISAR0_OFFSET 0x60
#define ISAR0_OFFSET SCB_ISAR0_OFFSET

#define SCB_ISAR1 (*(volatile unsigned *)0xE000ED64)
#define ISAR1 SCB_ISAR1
#define SCB_ISAR1_OFFSET 0x64
#define ISAR1_OFFSET SCB_ISAR1_OFFSET

#define SCB_ISAR2 (*(volatile unsigned *)0xE000ED68)
#define ISAR2 SCB_ISAR2
#define SCB_ISAR2_OFFSET 0x68
#define ISAR2_OFFSET SCB_ISAR2_OFFSET

#define SCB_ISAR3 (*(volatile unsigned *)0xE000ED6C)
#define ISAR3 SCB_ISAR3
#define SCB_ISAR3_OFFSET 0x6C
#define ISAR3_OFFSET SCB_ISAR3_OFFSET

#define SCB_ISAR4 (*(volatile unsigned *)0xE000ED70)
#define ISAR4 SCB_ISAR4
#define SCB_ISAR4_OFFSET 0x70
#define ISAR4_OFFSET SCB_ISAR4_OFFSET

#define MPU_BASE_ADDRESS 0xE000ED90

#define MPU_TYPE (*(volatile unsigned *)0xE000ED90)
#define MPU_Type MPU_TYPE
#define MPU_TYPE_OFFSET 0x0
#define MPU_Type_OFFSET MPU_TYPE_OFFSET
#define MPU_TYPE_IREGION_MASK 0xFF0000
#define MPU_Type_IREGION_MASK MPU_TYPE_IREGION_MASK
#define MPU_TYPE_IREGION_BIT 16
#define MPU_Type_IREGION_BIT MPU_TYPE_IREGION_BIT
#define MPU_TYPE_DREGION_MASK 0xFF00
#define MPU_Type_DREGION_MASK MPU_TYPE_DREGION_MASK
#define MPU_TYPE_DREGION_BIT 8
#define MPU_Type_DREGION_BIT MPU_TYPE_DREGION_BIT
#define MPU_TYPE_SEPARATE_MASK 0xFF
#define MPU_Type_SEPARATE_MASK MPU_TYPE_SEPARATE_MASK
#define MPU_TYPE_SEPARATE_BIT 0
#define MPU_Type_SEPARATE_BIT MPU_TYPE_SEPARATE_BIT

#define MPU_CTRL (*(volatile unsigned *)0xE000ED94)
#define MPU_Control MPU_CTRL
#define MPU_CTRL_OFFSET 0x4
#define MPU_Control_OFFSET MPU_CTRL_OFFSET
#define MPU_CTRL_PRIVDEFENA_MASK 0x4
#define MPU_Control_PRIVDEFENA_MASK MPU_CTRL_PRIVDEFENA_MASK
#define MPU_CTRL_PRIVDEFENA 0x4
#define MPU_Control_PRIVDEFENA MPU_CTRL_PRIVDEFENA
#define MPU_CTRL_PRIVDEFENA_BIT 2
#define MPU_Control_PRIVDEFENA_BIT MPU_CTRL_PRIVDEFENA_BIT
#define MPU_CTRL_HFNMIENA_MASK 0x2
#define MPU_Control_HFNMIENA_MASK MPU_CTRL_HFNMIENA_MASK
#define MPU_CTRL_HFNMIENA 0x2
#define MPU_Control_HFNMIENA MPU_CTRL_HFNMIENA
#define MPU_CTRL_HFNMIENA_BIT 1
#define MPU_Control_HFNMIENA_BIT MPU_CTRL_HFNMIENA_BIT
#define MPU_CTRL_ENABLE_MASK 0x1
#define MPU_Control_ENABLE_MASK MPU_CTRL_ENABLE_MASK
#define MPU_CTRL_ENABLE 0x1
#define MPU_Control_ENABLE MPU_CTRL_ENABLE
#define MPU_CTRL_ENABLE_BIT 0
#define MPU_Control_ENABLE_BIT MPU_CTRL_ENABLE_BIT

#define MPU_RNR (*(volatile unsigned *)0xE000ED98)
#define MPU_Region_Number MPU_RNR
#define MPU_RNR_OFFSET 0x8
#define MPU_Region_Number_OFFSET MPU_RNR_OFFSET
#define MPU_RNR_REGION_MASK 0xFF
#define MPU_Region_Number_REGION_MASK MPU_RNR_REGION_MASK
#define MPU_RNR_REGION_BIT 0
#define MPU_Region_Number_REGION_BIT MPU_RNR_REGION_BIT

#define MPU_RBAR (*(volatile unsigned *)0xE000ED9C)
#define MPU_Region_Base_Address MPU_RBAR
#define MPU_RBAR_OFFSET 0xC
#define MPU_Region_Base_Address_OFFSET MPU_RBAR_OFFSET
#define MPU_RBAR_ADDR_MASK 0xFFFFFFE0
#define MPU_Region_Base_Address_ADDR_MASK MPU_RBAR_ADDR_MASK
#define MPU_RBAR_ADDR_BIT 5
#define MPU_Region_Base_Address_ADDR_BIT MPU_RBAR_ADDR_BIT
#define MPU_RBAR_VALID_MASK 0x10
#define MPU_Region_Base_Address_VALID_MASK MPU_RBAR_VALID_MASK
#define MPU_RBAR_VALID 0x10
#define MPU_Region_Base_Address_VALID MPU_RBAR_VALID
#define MPU_RBAR_VALID_BIT 4
#define MPU_Region_Base_Address_VALID_BIT MPU_RBAR_VALID_BIT
#define MPU_RBAR_REGION_MASK 0xF
#define MPU_Region_Base_Address_REGION_MASK MPU_RBAR_REGION_MASK
#define MPU_RBAR_REGION_BIT 0
#define MPU_Region_Base_Address_REGION_BIT MPU_RBAR_REGION_BIT

#define MPU_RASR (*(volatile unsigned *)0xE000EDA0)
#define MPU_Attribute_and_Size MPU_RASR
#define MPU_RASR_OFFSET 0x10
#define MPU_Attribute_and_Size_OFFSET MPU_RASR_OFFSET
#define MPU_RASR_XN_MASK 0x10000000
#define MPU_Attribute_and_Size_XN_MASK MPU_RASR_XN_MASK
#define MPU_RASR_XN 0x10000000
#define MPU_Attribute_and_Size_XN MPU_RASR_XN
#define MPU_RASR_XN_BIT 28
#define MPU_Attribute_and_Size_XN_BIT MPU_RASR_XN_BIT
#define MPU_RASR_AP_MASK 0x7000000
#define MPU_Attribute_and_Size_AP_MASK MPU_RASR_AP_MASK
#define MPU_RASR_AP_BIT 24
#define MPU_Attribute_and_Size_AP_BIT MPU_RASR_AP_BIT
#define MPU_RASR_TEX_MASK 0x380000
#define MPU_Attribute_and_Size_TEX_MASK MPU_RASR_TEX_MASK
#define MPU_RASR_TEX_BIT 19
#define MPU_Attribute_and_Size_TEX_BIT MPU_RASR_TEX_BIT
#define MPU_RASR_S_MASK 0x40000
#define MPU_Attribute_and_Size_S_MASK MPU_RASR_S_MASK
#define MPU_RASR_S 0x40000
#define MPU_Attribute_and_Size_S MPU_RASR_S
#define MPU_RASR_S_BIT 18
#define MPU_Attribute_and_Size_S_BIT MPU_RASR_S_BIT
#define MPU_RASR_C_MASK 0x20000
#define MPU_Attribute_and_Size_C_MASK MPU_RASR_C_MASK
#define MPU_RASR_C 0x20000
#define MPU_Attribute_and_Size_C MPU_RASR_C
#define MPU_RASR_C_BIT 17
#define MPU_Attribute_and_Size_C_BIT MPU_RASR_C_BIT
#define MPU_RASR_B_MASK 0x10000
#define MPU_Attribute_and_Size_B_MASK MPU_RASR_B_MASK
#define MPU_RASR_B 0x10000
#define MPU_Attribute_and_Size_B MPU_RASR_B
#define MPU_RASR_B_BIT 16
#define MPU_Attribute_and_Size_B_BIT MPU_RASR_B_BIT
#define MPU_RASR_SRD_MASK 0xFF00
#define MPU_Attribute_and_Size_SRD_MASK MPU_RASR_SRD_MASK
#define MPU_RASR_SRD_BIT 8
#define MPU_Attribute_and_Size_SRD_BIT MPU_RASR_SRD_BIT
#define MPU_RASR_SIZE_MASK 0x3E
#define MPU_Attribute_and_Size_SIZE_MASK MPU_RASR_SIZE_MASK
#define MPU_RASR_SIZE_BIT 1
#define MPU_Attribute_and_Size_SIZE_BIT MPU_RASR_SIZE_BIT
#define MPU_RASR_ENABLE_MASK 0x1
#define MPU_Attribute_and_Size_ENABLE_MASK MPU_RASR_ENABLE_MASK
#define MPU_RASR_ENABLE 0x1
#define MPU_Attribute_and_Size_ENABLE MPU_RASR_ENABLE
#define MPU_RASR_ENABLE_BIT 0
#define MPU_Attribute_and_Size_ENABLE_BIT MPU_RASR_ENABLE_BIT

#define CoreDebug_BASE_ADDRESS 0xE000EDF0

#define DHCSR (*(volatile unsigned *)0xE000EDF0)
#define DHCSR_OFFSET 0x0

#define DCRSR (*(volatile unsigned *)0xE000EDF4)
#define DCRSR_OFFSET 0x4

#define DCRDR (*(volatile unsigned *)0xE000EDF8)
#define DCRDR_OFFSET 0x8

#define DEMCR (*(volatile unsigned *)0xE000EDFC)
#define DEMCR_OFFSET 0xC

#define ID_space_BASE_ADDRESS 0xE000EFD0

#define PID4 (*(volatile unsigned char *)0xE000EFD0)
#define PID4_OFFSET 0x0

#define PID5 (*(volatile unsigned char *)0xE000EFD4)
#define PID5_OFFSET 0x4

#define PID6 (*(volatile unsigned char *)0xE000EFD8)
#define PID6_OFFSET 0x8

#define PID7 (*(volatile unsigned char *)0xE000EFDC)
#define PID7_OFFSET 0xC

#define PID0 (*(volatile unsigned char *)0xE000EFE0)
#define PID0_OFFSET 0x10

#define PID1 (*(volatile unsigned char *)0xE000EFE4)
#define PID1_OFFSET 0x14

#define PID2 (*(volatile unsigned char *)0xE000EFE8)
#define PID2_OFFSET 0x18

#define PID3 (*(volatile unsigned char *)0xE000EFEC)
#define PID3_OFFSET 0x1C

#define CID0 (*(volatile unsigned char *)0xE000EFF0)
#define CID0_OFFSET 0x20

#define CID1 (*(volatile unsigned char *)0xE000EFF4)
#define CID1_OFFSET 0x24

#define CID2 (*(volatile unsigned char *)0xE000EFF8)
#define CID2_OFFSET 0x28

#define CID3 (*(volatile unsigned char *)0xE000EFFC)
#define CID3_OFFSET 0x2C


#endif
