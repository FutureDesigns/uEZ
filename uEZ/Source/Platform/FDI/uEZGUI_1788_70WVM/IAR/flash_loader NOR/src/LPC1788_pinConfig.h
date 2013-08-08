/*---------------------------------------------------------------------------*
 *   This data created with Pins1788ToH v1.00 - Copyright Future Designs, Inc.
 *---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*
 * Port 0 Configuration:
 *---------------------------------------------------------------------------*/
  /* ----- Pin P0[0 ]: UART3 ----- */
//LPC_IOCON->P0_0  |= 0; // GPIO
//LPC_IOCON->P0_0  |= 1; // CAN_RD1
  LPC_IOCON->P0_0  |= 2; // U3_TXD
//LPC_IOCON->P0_0  |= 3; // I2C1_SDA
//LPC_IOCON->P0_0  |= 4; // U0_TXD
//LPC_IOCON->P0_0  = (LPC_IOCON->P0_0  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_0                     0
  #define PINCLR_P0_0                     0
  #define PINDIR_P0_0                     0 // Input

  /* ----- Pin P0[1 ]: UART3 ----- */
//LPC_IOCON->P0_1  |= 0; // GPIO
//LPC_IOCON->P0_1  |= 1; // CAN1_TD
  LPC_IOCON->P0_1  |= 2; // U3_RXD
//LPC_IOCON->P0_1  |= 3; // I2C1_SCL
//LPC_IOCON->P0_1  |= 4; // U0_RXD
//LPC_IOCON->P0_1  = (LPC_IOCON->P0_1  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_1                     0
  #define PINCLR_P0_1                     0
  #define PINDIR_P0_1                     0 // Input

  /* ----- Pin P0[2 ]: ISP DEBUG ----- */ 
//LPC_IOCON->P0_2  |= 0; // GPIO
  LPC_IOCON->P0_2  |= 1; // U0_TXD
//LPC_IOCON->P0_2  |= 2; // U3_TXD
//LPC_IOCON->P0_2  = (LPC_IOCON->P0_2  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_2                     0
  #define PINCLR_P0_2                     0
  #define PINDIR_P0_2                     0 // Input

  /* ----- Pin P0[3 ]: ISP DEBUG ----- */ 
//LPC_IOCON->P0_3  |= 0; // GPIO
  LPC_IOCON->P0_3  |= 1; // U0_RXD
//LPC_IOCON->P0_3  |= 2; // U3_RXD
//LPC_IOCON->P0_3  = (LPC_IOCON->P0_3  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_3                     0
  #define PINCLR_P0_3                     0
  #define PINDIR_P0_3                     0 // Input

  /* ----- Pin P0[4 ]: GPIO ----- */
  LPC_IOCON->P0_4  |= 0; // GPIO
//LPC_IOCON->P0_4  |= 1; // I2S_RX_SCK
//LPC_IOCON->P0_4  |= 2; // CAN_RD2
//LPC_IOCON->P0_4  |= 3; // T2_CAP0
//LPC_IOCON->P0_4  |= 7; // LCD_VD0
//LPC_IOCON->P0_4  = (LPC_IOCON->P0_4  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_4                     0
  #define PINCLR_P0_4                     0
  #define PINDIR_P0_4                     0 // Input

  /* ----- Pin P0[5 ]: GPIO ----- */
  LPC_IOCON->P0_5  |= 0; // GPIO
//LPC_IOCON->P0_5  |= 1; // I2S_RX_WS
//LPC_IOCON->P0_5  |= 2; // CAN_TD2
//LPC_IOCON->P0_5  |= 3; // T2_CAP1
//LPC_IOCON->P0_5  |= 7; // LCD_VD1
//LPC_IOCON->P0_5  = (LPC_IOCON->P0_5  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_5                     0
  #define PINCLR_P0_5                     0
  #define PINDIR_P0_5                     0 // Input

  /* ----- Pin P0[6 ]: GPIO ----- */
  LPC_IOCON->P0_6  |= 0; // GPIO
//LPC_IOCON->P0_6  |= 1; // I2S_RX_SDA
//LPC_IOCON->P0_6  |= 2; // SSP1_SSEL1
//LPC_IOCON->P0_6  |= 3; // T2_MAT0
//LPC_IOCON->P0_6  |= 4; // U1_RTS
//LPC_IOCON->P0_6  |= 7; // LCD_VD8
//LPC_IOCON->P0_6  = (LPC_IOCON->P0_6  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_6                     0
  #define PINCLR_P0_6                     0
  #define PINDIR_P0_6                     0 // Input

  /* ----- Pin P0[7 ]: GPIO ----- */
//LPC_IOCON->P0_7  |= 0; // GPIO
  LPC_IOCON->P0_7  |= 1; // I2S_TX_SCK
//LPC_IOCON->P0_7  |= 2; // SSP1_SCK
//LPC_IOCON->P0_7  |= 3; // T2_MAT1
//LPC_IOCON->P0_7  |= 4; // RTC_EV0
//LPC_IOCON->P0_7  = (LPC_IOCON->P0_7  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_7                     0
  #define PINCLR_P0_7                     0
  #define PINDIR_P0_7                     0 // Input

  /* ----- Pin P0[8 ]: GPIO ----- */
  LPC_IOCON->P0_8  |= 0; // GPIO
//LPC_IOCON->P0_8  |= 1; // I2S_TX_WS
//LPC_IOCON->P0_8  |= 2; // SSP1_MISO
//LPC_IOCON->P0_8  |= 3; // T2_MAT2
//LPC_IOCON->P0_8  |= 4; // RTC_EV1
//LPC_IOCON->P0_8  |= 7; // LCD_VD16
//LPC_IOCON->P0_8  = (LPC_IOCON->P0_8  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_8                     0
  #define PINCLR_P0_8                     0
  #define PINDIR_P0_8                     0 // Input

  /* ----- Pin P0[9 ]: GPIO ----- */
  LPC_IOCON->P0_9  |= 0; // GPIO
//LPC_IOCON->P0_9  |= 1; // I2S_TX_SDA
//LPC_IOCON->P0_9  |= 2; // SSP1_MOSI
//LPC_IOCON->P0_9  |= 3; // T2_MAT3
//LPC_IOCON->P0_9  |= 4; // RTC_EV2
//LPC_IOCON->P0_9  |= 7; // LCD_VD17
//LPC_IOCON->P0_9  = (LPC_IOCON->P0_9  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_9                     0
  #define PINCLR_P0_9                     0
  #define PINDIR_P0_9                     0 // Input

  /* ----- Pin P0[10]: GPIO ----- */
  LPC_IOCON->P0_10 |= 0; // GPIO
//LPC_IOCON->P0_10 |= 1; // U2_TXD
//LPC_IOCON->P0_10 |= 2; // I2C2_SDA
//LPC_IOCON->P0_10 |= 3; // T3_MAT0
//LPC_IOCON->P0_10 = (LPC_IOCON->P0_10 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_10                    0
  #define PINCLR_P0_10                    0
  #define PINDIR_P0_10                    1 // Output

  /* ----- Pin P0[11]: EXP GPIO ----- */
  LPC_IOCON->P0_11 |= 0; // GPIO
//LPC_IOCON->P0_11 |= 1; // U2_RXD
//LPC_IOCON->P0_11 |= 2; // I2C2_SCL
//LPC_IOCON->P0_11 |= 3; // T3_MAT1
//LPC_IOCON->P0_11 = (LPC_IOCON->P0_11 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_11                    0
  #define PINCLR_P0_11                    0
  #define PINDIR_P0_11                    0 // Input

  /* ----- Pin P0[12]: ----- */ 
  LPC_IOCON->P0_12 |= 0; // GPIO
//LPC_IOCON->P0_12 |= 1; // USB_PPWR2n
//LPC_IOCON->P0_12 |= 2; // SSP1_MISO
//LPC_IOCON->P0_12 |= 3; // AD0_6
  LPC_IOCON->P0_12 = (LPC_IOCON->P0_12 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_12                    0
  #define PINCLR_P0_12                    0
  #define PINDIR_P0_12                    0 // Input

  /* ----- Pin P0[13]: ----- */ 
//LPC_IOCON->P0_13 |= 0; // GPIO
  LPC_IOCON->P0_13 |= 1; // USB_UP_LED2
//LPC_IOCON->P0_13 |= 2; // SSP1_MOSI
//LPC_IOCON->P0_13 |= 3; // AD0_7
//LPC_IOCON->P0_13 = (LPC_IOCON->P0_13 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_13                    0
  #define PINCLR_P0_13                    0
  #define PINDIR_P0_13                    0 // Input

  /* ----- Pin P0[14]: USB CONNECT2 ----- */
//LPC_IOCON->P0_14 |= 0; // GPIO
//LPC_IOCON->P0_14 |= 1; // USB_HSTEN2n
//LPC_IOCON->P0_14 |= 2; // SSP1_SSEL
  LPC_IOCON->P0_14 |= 3; // USB_CONNECT2
//LPC_IOCON->P0_14 = (LPC_IOCON->P0_14 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_14                    0
  #define PINCLR_P0_14                    0
  #define PINDIR_P0_14                    0 // Input

  /* ----- Pin P0[15]: UART1_TX ----- */
//LPC_IOCON->P0_15 |= 0; // GPIO
  LPC_IOCON->P0_15 |= 1; // U1_TXD
//LPC_IOCON->P0_15 |= 2; // SSP0_SCK
#if HAVE_LOC_REGISTERS
  LPC_IOCON->LOC_SSP0_SCK = 2;
#endif
  //LPC_IOCON->P0_15 = (LPC_IOCON->P0_15 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_15                    0
  #define PINCLR_P0_15                    0
  #define PINDIR_P0_15                    0 // Input

  /* ----- Pin P0[16]: UART1_RX ----- */
//LPC_IOCON->P0_16 |= 0; // GPIO
  LPC_IOCON->P0_16 |= 1; // U1_RXD
//LPC_IOCON->P0_16 |= 2; // SSP0_SSEL
  LPC_IOCON->P0_16 = (LPC_IOCON->P0_16 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_16                    1
  #define PINCLR_P0_16                    0
  #define PINDIR_P0_16                    1 // Output

  /* ----- Pin P0[17]: UART1_CTS ----- */
//LPC_IOCON->P0_17 |= 0; // GPIO
  LPC_IOCON->P0_17 |= 1; // U1_CTS
//LPC_IOCON->P0_17 |= 2; // SSP0_MISO
#if HAVE_LOC_REGISTERS
      LPC_IOCON->LOC_SSP0_MISO = 2;
#endif
//LPC_IOCON->P0_17 = (LPC_IOCON->P0_17 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_17                    0
  #define PINCLR_P0_17                    0
  #define PINDIR_P0_17                    0 // Input

  /* ----- Pin P0[18]: Serial Flash CS ----- */
  LPC_IOCON->P0_18 |= 0; // GPIO
//LPC_IOCON->P0_18 |= 1; // U1_DCD
//LPC_IOCON->P0_18 |= 2; // SSP0_MOSI
#if HAVE_LOC_REGISTERS
    LPC_IOCON->LOC_SSP0_MOSI = 2;
#endif
  //LPC_IOCON->P0_18 = (LPC_IOCON->P0_18 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_18                    0
  #define PINCLR_P0_18                    0
  #define PINDIR_P0_18                    0 // Input

  /* ----- Pin P0[19]: EXP ----- */
  LPC_IOCON->P0_19 |= 0; // GPIO
//LPC_IOCON->P0_19 |= 1; // U1_DSR
//LPC_IOCON->P0_19 |= 2; // SD_CLK
//LPC_IOCON->P0_19 |= 3; // I2C1_SDA
//      LPC_IOCON->P0_19 = (LPC_IOCON->P0_19 & ~(3<<10)) | (1<<10);
#if HAVE_LOC_REGISTERS
      LPC_IOCON->LOC_I2C1_SDA = 2;
#endif
  //LPC_IOCON->P0_19 = (LPC_IOCON->P0_19 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_19                    0
  #define PINCLR_P0_19                    0
  #define PINDIR_P0_19                    0 // Input

  /* ----- Pin P0[20]: EXP ----- */
  LPC_IOCON->P0_20 |= 0; // GPIO
//LPC_IOCON->P0_20 |= 1; // U1_DTR
//LPC_IOCON->P0_20 |= 2; // SD_CMD
//LPC_IOCON->P0_20 |= 3; // I2C1_SCL
//    LPC_IOCON->P0_20 = (LPC_IOCON->P0_20 & ~(3<<10)) | (1<<10);
#if HAVE_LOC_REGISTERS
      LPC_IOCON->LOC_I2C1_SCL = 2;
#endif
  //LPC_IOCON->P0_20 = (LPC_IOCON->P0_20 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_20                    0
  #define PINCLR_P0_20                    0
  #define PINDIR_P0_20                    0 // Input

  /* ----- Pin P0[21]: EXP ----- */
  LPC_IOCON->P0_21 |= 0; // GPIO
//LPC_IOCON->P0_21 |= 1; // U1_RI
//LPC_IOCON->P0_21 |= 2; // SD_PWR
//LPC_IOCON->P0_21 |= 3; // U4_OE
//LPC_IOCON->P0_21 |= 4; // CAN_RD1
//LPC_IOCON->P0_21 = (LPC_IOCON->P0_21 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_21                    0
  #define PINCLR_P0_21                    0
  #define PINDIR_P0_21                    0 // Input

  /* ----- Pin P0[22]: EXP ----- */
  LPC_IOCON->P0_22 |= 0; // GPIO
//LPC_IOCON->P0_22 |= 1; // U1_RTS
//LPC_IOCON->P0_22 |= 2; // SD_DAT_0
//LPC_IOCON->P0_22 |= 3; // U4_TXD
//LPC_IOCON->P0_22 = (LPC_IOCON->P0_22 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_22                    0
  #define PINCLR_P0_22                    0
  #define PINDIR_P0_22                    0 // Input

  /* ----- Pin P0[23]: TOUCH; X+ (RIGHT) ----- */ 
//LPC_IOCON->P0_23 |= 0; // GPIO
  LPC_IOCON->P0_23 |= 1; // AD0_0
//LPC_IOCON->P0_23 |= 2; // I2S_RX_SCK
//LPC_IOCON->P0_23 |= 3; // T3_CAP0
LPC_IOCON->P0_23 = (LPC_IOCON->P0_23 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_23                    0
  #define PINCLR_P0_23                    0
  #define PINDIR_P0_23                    0 // Input

  /* ----- Pin P0[24]: TOUCH; Y+ (UP) ----- */ 
//LPC_IOCON->P0_24 |= 0; // GPIO
  LPC_IOCON->P0_24 |= 1; // AD0_1
//LPC_IOCON->P0_24 |= 2; // I2S_RX_WS
//LPC_IOCON->P0_24 |= 3; // T3_CAP1
LPC_IOCON->P0_24 = (LPC_IOCON->P0_24 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_24                    0
  #define PINCLR_P0_24                    0
  #define PINDIR_P0_24                    0 // Input

  /* ----- Pin P0[25]: NO CONNECT ----- */
  LPC_IOCON->P0_25 |= 0; // GPIO
//LPC_IOCON->P0_25 |= 1; // AD0_2
//LPC_IOCON->P0_25 |= 2; // I2S_RX_SDA
//LPC_IOCON->P0_25 |= 3; // U3_TXD
//LPC_IOCON->P0_25 = (LPC_IOCON->P0_25 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_25                    0
  #define PINCLR_P0_25                    0
  #define PINDIR_P0_25                    0 // Input

  /* ----- Pin P0[26]: EXP ----- */
//LPC_IOCON->P0_26 |= 0; // GPIO
//LPC_IOCON->P0_26 |= 1; // AD0_3
//TESTING:
LPC_IOCON->P0_26 = 2|  // DAC_OUT
        (0<<3)| // No pull-up or pull-down
        (0<<5)| // Disable hysteresis (not needed, output pin)
        (0<<6)| // Do not invert input
        (0<<7)| // Analog mode
        (0<<8)| // No glitch filter
        (0<<10)| // Normal push pull
        (1<<16); // Enable DAC
//LPC_IOCON->P0_26 |= 3; // U3_RXD
//LPC_IOCON->P0_26 = (LPC_IOCON->P0_26 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P0_26                    0
  #define PINCLR_P0_26                    0
  #define PINDIR_P0_26                    0 // Input

  /* ----- Pin P0[27]: NC ----- */
  LPC_IOCON->P0_27 |= 0; // GPIO
//LPC_IOCON->P0_27 |= 1; // I2C0_SDA
//    LPC_IOCON->P0_27 = (LPC_IOCON->P0_27 & ~(3<<10)) | (1<<10);
#if HAVE_LOC_REGISTERS
//      LPC_IOCON->LOC_I2C0_SDA = 1;
#endif
  //LPC_IOCON->P0_27 |= 2; // USB_SDA
//LPC_IOCON->P0_27 = (LPC_IOCON->P0_27 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_27                    0
  #define PINCLR_P0_27                    0
  #define PINDIR_P0_27                    0 // Input

  /* ----- Pin P0[28]: NC ----- */
  LPC_IOCON->P0_28 |= 0; // GPIO
//LPC_IOCON->P0_28 |= 1; // I2C0_SCL
//      LPC_IOCON->P0_28 = (LPC_IOCON->P0_28 & ~(3<<10)) | (1<<10);
#if HAVE_LOC_REGISTERS
      LPC_IOCON->LOC_I2C0_SCL = 1;
#endif
  //LPC_IOCON->P0_28 |= 2; // USB_SCL
//LPC_IOCON->P0_28 = (LPC_IOCON->P0_28 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_28                    0
  #define PINCLR_P0_28                    0
  #define PINDIR_P0_28                    0 // Input

  /* ----- Pin P0[29]: USB HOST DATA+ ----- */ 
//LPC_IOCON->P0_29 |= 0; // GPIO
  LPC_IOCON->P0_29 |= 1; // USB_DPLUS1
//LPC_IOCON->P0_29 |= 2; // EINT_0
//LPC_IOCON->P0_29 = (LPC_IOCON->P0_29 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_29                    0
  #define PINCLR_P0_29                    0
  #define PINDIR_P0_29                    0 // Input

  /* ----- Pin P0[30]: USB HOST DATA- ----- */ 
//LPC_IOCON->P0_30 |= 0; // GPIO
  LPC_IOCON->P0_30 |= 1; // USB_DMINUS1
//LPC_IOCON->P0_30 |= 2; // EINT_1
//LPC_IOCON->P0_30 = (LPC_IOCON->P0_30 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_30                    0
  #define PINCLR_P0_30                    0
  #define PINDIR_P0_30                    0 // Input

  /* ----- Pin P0[31]: USB DEVICE DATA+ ----- */ 
//LPC_IOCON->P0_31 |= 0; // GPIO
  LPC_IOCON->P0_31 |= 1; // USB_D+2
//LPC_IOCON->P0_31 = (LPC_IOCON->P0_31 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P0_31                    0
  #define PINCLR_P0_31                    0
  #define PINDIR_P0_31                    0 // Input



/*---------------------------------------------------------------------------*
 * Port 1 Configuration:
 *---------------------------------------------------------------------------*/
  /* ----- Pin P1[0 ]: ENET_TXD0 ----- */ 
//LPC_IOCON->P1_0  |= 0; // GPIO
  LPC_IOCON->P1_0  |= 1; // ENET_TXD0
//LPC_IOCON->P1_0  |= 3; // T3_CAP1
//LPC_IOCON->P1_0  |= 4; // SSP2_SCK
//LPC_IOCON->P1_0  = (LPC_IOCON->P1_0  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_0                     0
  #define PINCLR_P1_0                     0
  #define PINDIR_P1_0                     0 // Input

  /* ----- Pin P1[1 ]: ENET_TXD1 ----- */ 
//LPC_IOCON->P1_1  |= 0; // GPIO
  LPC_IOCON->P1_1  |= 1; // ENET_TXD1
//LPC_IOCON->P1_1  |= 3; // T3_MAT3
//LPC_IOCON->P1_1  |= 4; // SSP2_MOSI
//LPC_IOCON->P1_1  = (LPC_IOCON->P1_1  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_1                     0
  #define PINCLR_P1_1                     0
  #define PINDIR_P1_1                     0 // Input

  /* ----- Pin P1[2 ]: Backlight PWM ----- */
//LPC_IOCON->P1_2  |= 0; // GPIO
//LPC_IOCON->P1_2  |= 1; // ENET_TXD2
//LPC_IOCON->P1_2  |= 2; // SD_CLK
LPC_IOCON->P1_2  |= 3; // PWM0_1
//LPC_IOCON->P1_2  = (LPC_IOCON->P1_2  & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_2                     0
  #define PINCLR_P1_2                     0
  #define PINDIR_P1_2                     0 // Input

  /* ----- Pin P1[3 ]: NC ----- */ 
  LPC_IOCON->P1_3  |= 0; // GPIO
//LPC_IOCON->P1_3  |= 1; // ENET_TXD3
//LPC_IOCON->P1_3  |= 2; // SD_CMD
//LPC_IOCON->P1_3  |= 3; // PWM0_2
  LPC_IOCON->P1_3  = (LPC_IOCON->P1_3  & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_3                     0
  #define PINCLR_P1_3                     0
  #define PINDIR_P1_3                     0 // Input

  /* ----- Pin P1[4 ]: ENET_TX_EN ----- */ 
//LPC_IOCON->P1_4  |= 0; // GPIO
  LPC_IOCON->P1_4  |= 1; // ENET_TX_ENn
//LPC_IOCON->P1_4  |= 3; // T3_MAT2
//LPC_IOCON->P1_4  |= 4; // SSP2_MISO
//LPC_IOCON->P1_4  = (LPC_IOCON->P1_4  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_4                     0
  #define PINCLR_P1_4                     0
  #define PINDIR_P1_4                     0 // Input

  /* ----- Pin P1[5 ]: Hearbeat ----- */
  LPC_IOCON->P1_5  |= 0; // GPIO
//LPC_IOCON->P1_5  |= 1; // ENET_TX_ER
//LPC_IOCON->P1_5  |= 2; // SD_PWR
//LPC_IOCON->P1_5  |= 3; // PWM0_3
//LPC_IOCON->P1_5  = (LPC_IOCON->P1_5  & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_5                     0
  #define PINCLR_P1_5                     0
  #define PINDIR_P1_5                     1 // Output

  /* ----- Pin P1[6 ]: Expansion Board connected output pin ----- */
  LPC_IOCON->P1_6  |= 0; // GPIO
//LPC_IOCON->P1_6  |= 1; // ENET_TX_CLK
//LPC_IOCON->P1_6  |= 2; // SD_DAT_0
//LPC_IOCON->P1_6  |= 3; // PWM0_4
//LPC_IOCON->P1_6  = (LPC_IOCON->P1_6  & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_6                     0
  #define PINCLR_P1_6                     0
  #define PINDIR_P1_6                     1 // Output

  /* ----- Pin P1[7 ]: NC ----- */ 
  LPC_IOCON->P1_7  |= 0; // GPIO
//LPC_IOCON->P1_7  |= 1; // ENET_COL
//LPC_IOCON->P1_7  |= 2; // SD_DAT_1
//LPC_IOCON->P1_7  |= 3; // PWM0_5
//LPC_IOCON->P1_7  = (LPC_IOCON->P1_7  & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_7                     0
  #define PINCLR_P1_7                     0
  #define PINDIR_P1_7                     0 // Input

  /* ----- Pin P1[8 ]: EXP - ENET_CRS_DV/ ENET_CRS  ----- */
//LPC_IOCON->P1_8  |= 0; // GPIO
  LPC_IOCON->P1_8  |= 1; // ENET_CRS
//LPC_IOCON->P1_8  |= 3; // T3_MAT1
//LPC_IOCON->P1_8  |= 4; // SSP2_SSEL
//LPC_IOCON->P1_8  = (LPC_IOCON->P1_8  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_8                     0
  #define PINCLR_P1_8                     0
  #define PINDIR_P1_8                     0 // Input

  /* ----- Pin P1[9 ]: ENET_RXD0  ----- */ 
//LPC_IOCON->P1_9  |= 0; // GPIO
  LPC_IOCON->P1_9  |= 1; // ENET_RXD0
//LPC_IOCON->P1_9  |= 2; // T3_MAT0
//LPC_IOCON->P1_9  = (LPC_IOCON->P1_9  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_9                     0
  #define PINCLR_P1_9                     0
  #define PINDIR_P1_9                     0 // Input

  /* ----- Pin P1[10]: ENET_RXD1  ----- */ 
//LPC_IOCON->P1_10 |= 0; // GPIO
  LPC_IOCON->P1_10 |= 1; // ENET_RXD1
//LPC_IOCON->P1_10 |= 2; // T3_CAP0
//LPC_IOCON->P1_10 = (LPC_IOCON->P1_10 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_10                    0
  #define PINCLR_P1_10                    0
  #define PINDIR_P1_10                    0 // Input

  /* ----- Pin P1[11]: NC ----- */ 
  LPC_IOCON->P1_11 |= 0; // GPIO
//LPC_IOCON->P1_11 |= 1; // ENET_RXD2
//LPC_IOCON->P1_11 |= 2; // SD_DAT_2
//LPC_IOCON->P1_11 |= 3; // PWM0_6
  LPC_IOCON->P1_11 = (LPC_IOCON->P1_11 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_11                    0
  #define PINCLR_P1_11                    0
  #define PINDIR_P1_11                    0 // Input

  /* ----- Pin P1[12]: NC ----- */ 
  LPC_IOCON->P1_12 |= 0; // GPIO
//LPC_IOCON->P1_12 |= 1; // ENET_RXD3
//LPC_IOCON->P1_12 |= 2; // SD_DAT_3
//LPC_IOCON->P1_12 |= 3; // PWM_CAP0
  LPC_IOCON->P1_12 = (LPC_IOCON->P1_12 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_12                    0
  #define PINCLR_P1_12                    0
  #define PINDIR_P1_12                    0 // Input

  /* ----- Pin P1[13]: NC ----- */ 
  LPC_IOCON->P1_13 |= 0; // GPIO
//LPC_IOCON->P1_13 |= 1; // ENET_RX_DV
//LPC_IOCON->P1_13 = (LPC_IOCON->P1_13 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_13                    0
  #define PINCLR_P1_13                    0
  #define PINDIR_P1_13                    1 // Output

  /* ----- Pin P1[14]: ENET_RX_ER  ----- */ 
//LPC_IOCON->P1_14 |= 0; // GPIO
  LPC_IOCON->P1_14 |= 1; // ENET_RX_ER
//LPC_IOCON->P1_14 = (LPC_IOCON->P1_14 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_14                    0
  #define PINCLR_P1_14                    0
  #define PINDIR_P1_14                    0 // Input

  /* ----- Pin P1[15]: ENET_REF_CLK /ENET_RX_CLK  ----- */ 
//LPC_IOCON->P1_15 |= 0; // GPIO
  LPC_IOCON->P1_15 |= 1; // ENET_RX_CLK
//LPC_IOCON->P1_15 |= 3; // I2C2_SDA
//LPC_IOCON->P1_15 = (LPC_IOCON->P1_15 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_15                    0
  #define PINCLR_P1_15                    0
  #define PINDIR_P1_15                    0 // Input

  /* ----- Pin P1[16]: ENET_MDC  ----- */ 
//LPC_IOCON->P1_16 |= 0; // GPIO
  LPC_IOCON->P1_16 |= 1; // ENET_MDC
//LPC_IOCON->P1_16 = (LPC_IOCON->P1_16 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_16                    0
  #define PINCLR_P1_16                    0
  #define PINDIR_P1_16                    0 // Input

  /* ----- Pin P1[17]: ENET_MDIO  ----- */ 
//LPC_IOCON->P1_17 |= 0; // GPIO
  LPC_IOCON->P1_17 |= 1; // ENET_MDIO
//LPC_IOCON->P1_17 = (LPC_IOCON->P1_17 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_17                    0
  #define PINCLR_P1_17                    0
  #define PINDIR_P1_17                    0 // Input
#if HAVE_LOC_REGISTERS
  LPC_IOCON->LOC_ENET_MDIO = 0x01;  // Change to MDIO
#endif

  /* ----- Pin P1[18]: NC ----- */
  LPC_IOCON->P1_18 |= 0; // GPIO
//LPC_IOCON->P1_18 |= 1; // USB_UP_LED1
//LPC_IOCON->P1_18 |= 2; // PWM1_1
//LPC_IOCON->P1_18 |= 3; // T1_CAP0
//LPC_IOCON->P1_18 |= 5; // SSP1_MISO
LPC_IOCON->P1_18 = (LPC_IOCON->P1_18 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_18                    0
  #define PINCLR_P1_18                    0
  #define PINDIR_P1_18                    0 // Input

  /* ----- Pin P1[19]: USB HOST POWER ENABLE OUTPUT, USB HW ----- */ 
//LPC_IOCON->P1_19 |= 0; // GPIO
//LPC_IOCON->P1_19 |= 1; // USB_TX_E1n
  LPC_IOCON->P1_19 |= 2; // USB_PPWR1n
//LPC_IOCON->P1_19 |= 3; // T1_CAP1
//LPC_IOCON->P1_19 |= 4; // MC_0A
//LPC_IOCON->P1_19 |= 5; // SSP1_SCK
//LPC_IOCON->P1_19 |= 6; // U2_OE
//LPC_IOCON->P1_19 = (LPC_IOCON->P1_19 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_19                    0
  #define PINCLR_P1_19                    0
  #define PINDIR_P1_19                    0 // Input

  /* ----- Pin P1[20]: LCDVD[10] ----- */ 
//LPC_IOCON->P1_20 |= 0; // GPIO
//LPC_IOCON->P1_20 |= 1; // USB_TX_DP1
//LPC_IOCON->P1_20 |= 2; // PWM1_2
//LPC_IOCON->P1_20 |= 3; // QEI_PHA
//LPC_IOCON->P1_20 |= 4; // MC_FB0
//LPC_IOCON->P1_20 |= 5; // SSP0_SCK0
//LPC_IOCON->P1_20 |= 6; // LCD_VD6
  LPC_IOCON->P1_20 |= 7; // LCD_VD10
//LPC_IOCON->P1_20 = (LPC_IOCON->P1_20 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_20                    0
  #define PINCLR_P1_20                    0
  #define PINDIR_P1_20                    0 // Input

  /* ----- Pin P1[21]: LCDVD[11] ----- */ 
//LPC_IOCON->P1_21 |= 0; // GPIO
//LPC_IOCON->P1_21 |= 1; // USB_TX_DM1
//LPC_IOCON->P1_21 |= 2; // PWM1_3
//LPC_IOCON->P1_21 |= 3; // SSP0_SSEL
//LPC_IOCON->P1_21 |= 4; // MCABORTn
//LPC_IOCON->P1_21 |= 6; // LCD_VD7
  LPC_IOCON->P1_21 |= 7; // LCD_VD11
//LPC_IOCON->P1_21 = (LPC_IOCON->P1_21 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_21                    0
  #define PINCLR_P1_21                    0
  #define PINDIR_P1_21                    0 // Input

  /* ----- Pin P1[22]: LCDVD[12] ----- */ 
//LPC_IOCON->P1_22 |= 0; // GPIO
//LPC_IOCON->P1_22 |= 1; // USB_RCV1
//LPC_IOCON->P1_22 |= 2; // USB_PWRD1
//LPC_IOCON->P1_22 |= 3; // T1_MAT0
//LPC_IOCON->P1_22 |= 4; // MC_0B
//LPC_IOCON->P1_22 |= 5; // SSP1_MOSI
//LPC_IOCON->P1_22 |= 6; // LCD_VD8
  LPC_IOCON->P1_22 |= 7; // LCD_VD12
//LPC_IOCON->P1_22 = (LPC_IOCON->P1_22 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_22                    0
  #define PINCLR_P1_22                    0
  #define PINDIR_P1_22                    0 // Input

  /* ----- Pin P1[23]: LCDVD[13] ----- */ 
//LPC_IOCON->P1_23 |= 0; // GPIO
//LPC_IOCON->P1_23 |= 1; // USB_RX_DP1
//LPC_IOCON->P1_23 |= 2; // PWM1_4
//LPC_IOCON->P1_23 |= 3; // QEI_PHB
//LPC_IOCON->P1_23 |= 4; // MC_FB1
//LPC_IOCON->P1_23 |= 5; // SSP0_MISO
//LPC_IOCON->P1_23 |= 6; // LCD_VD9
  LPC_IOCON->P1_23 |= 7; // LCD_VD13
//LPC_IOCON->P1_23 = (LPC_IOCON->P1_23 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_23                    0
  #define PINCLR_P1_23                    0
  #define PINDIR_P1_23                    0 // Input

  /* ----- Pin P1[24]: LCDVD[14]  ----- */ 
//LPC_IOCON->P1_24 |= 0; // GPIO
//LPC_IOCON->P1_24 |= 1; // USB_RX_DM1
//LPC_IOCON->P1_24 |= 2; // PWM1_5
//LPC_IOCON->P1_24 |= 3; // QEI_IDX
//LPC_IOCON->P1_24 |= 4; // MC_FB2
//LPC_IOCON->P1_24 |= 5; // SSP0_MOSI
//LPC_IOCON->P1_24 |= 6; // LCD_VD10
  LPC_IOCON->P1_24 |= 7; // LCD_VD14
//LPC_IOCON->P1_24 = (LPC_IOCON->P1_24 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_24                    0
  #define PINCLR_P1_24                    0
  #define PINDIR_P1_24                    0 // Input

  /* ----- Pin P1[25]: LCDVD[15]  ----- */ 
//LPC_IOCON->P1_25 |= 0; // GPIO
//LPC_IOCON->P1_25 |= 1; // USB_LS1n
//LPC_IOCON->P1_25 |= 2; // USB_HSTEN1n
//LPC_IOCON->P1_25 |= 3; // T1_MAT1
//LPC_IOCON->P1_25 |= 4; // MC_1A
//LPC_IOCON->P1_25 |= 5; // CLKOUT
//LPC_IOCON->P1_25 |= 6; // LCD_VD11
  LPC_IOCON->P1_25 |= 7; // LCD_VD15
//LPC_IOCON->P1_25 = (LPC_IOCON->P1_25 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_25                    0
  #define PINCLR_P1_25                    0
  #define PINDIR_P1_25                    0 // Input

  /* ----- Pin P1[26]: LCDVD[20]  ----- */ 
//LPC_IOCON->P1_26 |= 0; // GPIO
//LPC_IOCON->P1_26 |= 1; // USB_SSPND1n
//LPC_IOCON->P1_26 |= 2; // PWM1_6
//LPC_IOCON->P1_26 |= 3; // T0_CAP0
//LPC_IOCON->P1_26 |= 4; // MC_1B
//LPC_IOCON->P1_26 |= 5; // SSP1_SSEL
//LPC_IOCON->P1_26 |= 6; // LCD_VD12
  LPC_IOCON->P1_26 |= 7; // LCD_VD20
//LPC_IOCON->P1_26 = (LPC_IOCON->P1_26 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_26                    0
  #define PINCLR_P1_26                    0
  #define PINDIR_P1_26                    0 // Input

  /* ----- Pin P1[27]: LCDVD[21]  ----- */ 
//LPC_IOCON->P1_27 |= 0; // GPIO
//LPC_IOCON->P1_27 |= 1; // USB_INT1n
//LPC_IOCON->P1_27 |= 2; // USB_OVRCR1n
//LPC_IOCON->P1_27 |= 3; // T0_CAP1
//LPC_IOCON->P1_27 |= 4; // CLKOUT
//LPC_IOCON->P1_27 |= 6; // LCD_VD13
  LPC_IOCON->P1_27 |= 7; // LCD_VD21
//LPC_IOCON->P1_27 = (LPC_IOCON->P1_27 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_27                    0
  #define PINCLR_P1_27                    0
  #define PINDIR_P1_27                    0 // Input

  /* ----- Pin P1[28]: LCDVD[22]  ----- */ 
//LPC_IOCON->P1_28 |= 0; // GPIO
//LPC_IOCON->P1_28 |= 1; // USB_SCL1
//LPC_IOCON->P1_28 |= 2; // PWM1_CAP0
//LPC_IOCON->P1_28 |= 3; // T0_MAT0
//LPC_IOCON->P1_28 |= 4; // MC_2A
//LPC_IOCON->P1_28 |= 5; // SSP0_SSEL
//LPC_IOCON->P1_28 |= 6; // LCD_VD14
  LPC_IOCON->P1_28 |= 7; // LCD_VD22
//LPC_IOCON->P1_28 = (LPC_IOCON->P1_28 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_28                    0
  #define PINCLR_P1_28                    0
  #define PINDIR_P1_28                    0 // Input

  /* ----- Pin P1[29]: LCDVD[23]  ----- */ 
//LPC_IOCON->P1_29 |= 0; // GPIO
//LPC_IOCON->P1_29 |= 1; // USB_SDA1
//LPC_IOCON->P1_29 |= 2; // PWM1_CAP1
//LPC_IOCON->P1_29 |= 3; // T0_MAT1
//LPC_IOCON->P1_29 |= 4; // MC_2B
//LPC_IOCON->P1_29 |= 5; // U4_TXD
//LPC_IOCON->P1_29 |= 6; // LCD_VD15
  LPC_IOCON->P1_29 |= 7; // LCD_VD23
//LPC_IOCON->P1_29 = (LPC_IOCON->P1_29 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P1_29                    0
  #define PINCLR_P1_29                    0
  #define PINDIR_P1_29                    0 // Input

  /* ----- Pin P1[30]: USB DEVICE POWER DETECT - VBUS ----- */ 
  LPC_IOCON->P1_30 |= 0; // GPIO
//LPC_IOCON->P1_30 |= 1; // USB_PWRD2
//??LPC_IOCON->P1_30 |= 2; // USB_VBUS
//LPC_IOCON->P1_30 |= 3; // AD0_4
//LPC_IOCON->P1_30 |= 4; // I2C0_SDA
//LPC_IOCON->P1_30 |= 5; // U3_OE
  LPC_IOCON->P1_30 = (LPC_IOCON->P1_30 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P1_30                    0
  #define PINCLR_P1_30                    0
  #define PINDIR_P1_30                    0 // Input

  /* ----- Pin P1[31]: EXP ----- */
  LPC_IOCON->P1_31 |= 0; // GPIO
//LPC_IOCON->P1_31 |= 1; // USB_OVRCR2n
//LPC_IOCON->P1_31 |= 2; // SSP1_SCK
//LPC_IOCON->P1_31 |= 3; // AD0_5
//LPC_IOCON->P1_31 |= 4; // I2C0_SCL
  LPC_IOCON->P1_31 = (LPC_IOCON->P1_31 & ~(3<<3)) | (1<<3); // Pull-down 
  #define PINSET_P1_31                    0
  #define PINCLR_P1_31                    0
  #define PINDIR_P1_31                    0 // Input



/*---------------------------------------------------------------------------*
 * Port 2 Configuration:
 *---------------------------------------------------------------------------*/
  /* ----- Pin P2[0 ]: LCDPWR ----- */ 
//LPC_IOCON->P2_0  |= 0; // GPIO
//LPC_IOCON->P2_0  |= 1; // PWM1_1
//LPC_IOCON->P2_0  |= 2; // U1_TXD
  LPC_IOCON->P2_0  |= 7; // LCD_PWR
//LPC_IOCON->P2_0  = (LPC_IOCON->P2_0  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_0                     1
  #define PINCLR_P2_0                     0
  #define PINDIR_P2_0                     1 // Output

  /* ----- Pin P2[1 ]: New SPKR ----- */
//LPC_IOCON->P2_1  |= 0; // GPIO
  LPC_IOCON->P2_1  |= 1; // PWM1_2
//LPC_IOCON->P2_1  |= 2; // U1_RXD
//LPC_IOCON->P2_1  |= 7; // LCD_LE
//LPC_IOCON->P2_1  = (LPC_IOCON->P2_1  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_1                     0
  #define PINCLR_P2_1                     0
  #define PINDIR_P2_1                     0 // Input

  /* ----- Pin P2[2 ]: LCDDCLK  ----- */ 
//LPC_IOCON->P2_2  |= 0; // GPIO
//LPC_IOCON->P2_2  |= 1; // PWM1_3
//LPC_IOCON->P2_2  |= 2; // U1_CTS
//LPC_IOCON->P2_2  |= 3; // T2_MAT3
//LPC_IOCON->P2_2  |= 5; // TRACEDATA_3
  LPC_IOCON->P2_2  |= 7; // LCD_DCLK
//LPC_IOCON->P2_2  = (LPC_IOCON->P2_2  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_2                     0
  #define PINCLR_P2_2                     0
  #define PINDIR_P2_2                     0 // Input

  /* ----- Pin P2[3 ]: Volume Control  ----- */
  LPC_IOCON->P2_3  |= 0; // GPIO
//LPC_IOCON->P2_3  |= 1; // PWM1_4
//LPC_IOCON->P2_3  |= 2; // U1_DCD
//LPC_IOCON->P2_3  |= 3; // T2_MAT2
//LPC_IOCON->P2_3  |= 5; // TRACEDATA_2
//LPC_IOCON->P2_3  |= 7; // LCD_FP
//LPC_IOCON->P2_3  = (LPC_IOCON->P2_3  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_3                     0
  #define PINCLR_P2_3                     0
  #define PINDIR_P2_3                     0 // Input

  /* ----- Pin P2[4 ]: LCDENAB ----- */ 
//LPC_IOCON->P2_4  |= 0; // GPIO
//LPC_IOCON->P2_4  |= 1; // PWM1_5
//LPC_IOCON->P2_4  |= 2; // U1_DSR
//LPC_IOCON->P2_4  |= 3; // T2_MAT1
//LPC_IOCON->P2_4  |= 5; // TRACEDATA_1
  LPC_IOCON->P2_4  |= 7; // LCD_ENAB_M
//LPC_IOCON->P2_4  = (LPC_IOCON->P2_4  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_4                     0
  #define PINCLR_P2_4                     0
  #define PINDIR_P2_4                     0 // Input

  /* ----- Pin P2[5 ]: Audio Amp Mode ----- */
  LPC_IOCON->P2_5  |= 0; // GPIO
//LPC_IOCON->P2_5  |= 1; // PWM1_6
//LPC_IOCON->P2_5  |= 2; // U1_DTR
//LPC_IOCON->P2_5  |= 3; // T2_MAT0
//LPC_IOCON->P2_5  |= 5; // TRACEDATA_0
//LPC_IOCON->P2_5  |= 7; // LCD_LP
//LPC_IOCON->P2_5  = (LPC_IOCON->P2_5  & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P2_5                     1
  #define PINCLR_P2_5                     0
  #define PINDIR_P2_5                     1 // Output

  /* ----- Pin P2[6 ]: LCDVD[4]  ----- */ 
//LPC_IOCON->P2_6  |= 0; // GPIO
//LPC_IOCON->P2_6  |= 1; // PWM1_CAP0
//LPC_IOCON->P2_6  |= 2; // U1_RI
//LPC_IOCON->P2_6  |= 3; // T2_CAP0
//LPC_IOCON->P2_6  |= 4; // U2_OE
//LPC_IOCON->P2_6  |= 5; // TRACECLK
//LPC_IOCON->P2_6  |= 6; // LCD_VD0
  LPC_IOCON->P2_6  |= 7; // LCD_VD4
//LPC_IOCON->P2_6  = (LPC_IOCON->P2_6  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_6                     0
  #define PINCLR_P2_6                     0
  #define PINDIR_P2_6                     0 // Input

  /* ----- Pin P2[7 ]: LCDVD[5]  ----- */ 
//LPC_IOCON->P2_7  |= 0; // GPIO
//LPC_IOCON->P2_7  |= 1; // CAN_RD2
//LPC_IOCON->P2_7  |= 2; // U1_RTS
//LPC_IOCON->P2_7  |= 6; // LCD_VD1
  LPC_IOCON->P2_7  |= 7; // LCD_VD5
//LPC_IOCON->P2_7  = (LPC_IOCON->P2_7  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_7                     0
  #define PINCLR_P2_7                     0
  #define PINDIR_P2_7                     0 // Input

  /* ----- Pin P2[8 ]: LCDVD[6]  ----- */ 
//LPC_IOCON->P2_8  |= 0; // GPIO
//LPC_IOCON->P2_8  |= 1; // CAN_TD2
//LPC_IOCON->P2_8  |= 2; // U2_TXD
//LPC_IOCON->P2_8  |= 3; // U1_CTS
//LPC_IOCON->P2_8  |= 4; // ENET_MDC
//LPC_IOCON->P2_8  |= 6; // LCD_VD2
  LPC_IOCON->P2_8  |= 7; // LCD_VD6
//LPC_IOCON->P2_8  = (LPC_IOCON->P2_8  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_8                     0
  #define PINCLR_P2_8                     0
  #define PINDIR_P2_8                     0 // Input

  /* ----- Pin P2[9 ]: LCDVD[7]  ----- */ 
//LPC_IOCON->P2_9  |= 0; // GPIO
//LPC_IOCON->P2_9  |= 1; // USB_CONNECT1
//LPC_IOCON->P2_9  |= 2; // U2_RXD
//LPC_IOCON->P2_9  |= 3; // U4_RXD
//LPC_IOCON->P2_9  |= 4; // ENET_MDIO
//LPC_IOCON->P2_9  |= 6; // LCD_VD3
  LPC_IOCON->P2_9  |= 7; // LCD_VD7
//LPC_IOCON->P2_9  = (LPC_IOCON->P2_9  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_9                     0
  #define PINCLR_P2_9                     0
  #define PINDIR_P2_9                     0 // Input

  /* ----- Pin P2[10]: ETHERNET PHY IRQ INPUT /  ISP_ENTRY ----- */ 
  LPC_IOCON->P2_10 |= 0; // GPIO
//LPC_IOCON->P2_10 |= 1; // EINT0
//LPC_IOCON->P2_10 |= 2; // NMI
  LPC_IOCON->P2_10 = (LPC_IOCON->P2_10 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_10                    0
  #define PINCLR_P2_10                    0
  #define PINDIR_P2_10                    0 // Input

  /* ----- Pin P2[11]: EINT1n / RTC_IRQ ----- */
  LPC_IOCON->P2_11 |= 0; // GPIO
//LPC_IOCON->P2_11 |= 1; // EINT1
//LPC_IOCON->P2_11 |= 2; // SD_DAT_1
//LPC_IOCON->P2_11 |= 3; // I2S_TX_SCK
//LPC_IOCON->P2_11 |= 7; // LCD_CLKIN
  LPC_IOCON->P2_11 = (LPC_IOCON->P2_11 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_11                    0
  #define PINCLR_P2_11                    0
  #define PINDIR_P2_11                    0 // Input

  /* ----- Pin P2[12]: LCDVD[18]  ----- */ 
//LPC_IOCON->P2_12 |= 0; // GPIO
//LPC_IOCON->P2_12 |= 1; // EINT2
//LPC_IOCON->P2_12 |= 2; // SD_DAT_2
//LPC_IOCON->P2_12 |= 3; // I2S_TX_WS
//LPC_IOCON->P2_12 |= 4; // LCD_VD4
//LPC_IOCON->P2_12 |= 5; // LCD_VD3
//LPC_IOCON->P2_12 |= 6; // LCD_VD8
  LPC_IOCON->P2_12 |= 7; // LCD_VD18
//LPC_IOCON->P2_12 = (LPC_IOCON->P2_12 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_12                    0
  #define PINCLR_P2_12                    0
  #define PINDIR_P2_12                    0 // Input

  /* ----- Pin P2[13]: LCDVD[19]  ----- */ 
//LPC_IOCON->P2_13 |= 0; // GPIO
//LPC_IOCON->P2_13 |= 1; // EINT3
//LPC_IOCON->P2_13 |= 2; // SD_DAT_3
//LPC_IOCON->P2_13 |= 3; // I2S_TX_SDA
//LPC_IOCON->P2_13 |= 5; // LCD_VD5
//LPC_IOCON->P2_13 |= 6; // LCD_VD9
  LPC_IOCON->P2_13 |= 7; // LCD_VD19
//LPC_IOCON->P2_13 = (LPC_IOCON->P2_13 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_13                    0
  #define PINCLR_P2_13                    0
  #define PINDIR_P2_13                    0 // Input

  /* ----- Pin P2[14]: Local I2C SDA1 ----- */
//LPC_IOCON->P2_14 |= 0; // GPIO
//LPC_IOCON->P2_14 |= 1; // EMC_CS2n
  LPC_IOCON->P2_14 |= 2; // I2C1_SDA
//LPC_IOCON->P2_14 |= 3; // T2_CAP0
//  LPC_IOCON->P2_14 = (LPC_IOCON->P2_14 & ~(3<<3)) | (2<<3); // Pull-up
  LPC_IOCON->P2_14 = (LPC_IOCON->P2_14 & ~(3<<10)) | (1<<10);
  #define PINSET_P2_14                    0
  #define PINCLR_P2_14                    0
  #define PINDIR_P2_14                    0 // Input

  /* ----- Pin P2[15]: NC ----- */ 
//LPC_IOCON->P2_15 |= 0; // GPIO
//LPC_IOCON->P2_15 |= 1; // EMC_CS3n
  LPC_IOCON->P2_15 |= 2; // I2C1_SCL
//LPC_IOCON->P2_15 |= 3; // T2_CAP1
      LPC_IOCON->P2_15 = (LPC_IOCON->P2_15 & ~(3<<10)) | (1<<10);
//  LPC_IOCON->P2_15 = (LPC_IOCON->P2_15 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P2_15                    0
  #define PINCLR_P2_15                    0
  #define PINDIR_P2_15                    0 // Input

  /* ----- Pin P2[16]: SDRAM CASn ----- */
//LPC_IOCON->P2_16 |= 0; // GPIO
  LPC_IOCON->P2_16 |= 1; // EMC_CASn
//LPC_IOCON->P2_16 = (LPC_IOCON->P2_16 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_16                    0
  #define PINCLR_P2_16                    0
  #define PINDIR_P2_16                    0 // Input

  /* ----- Pin P2[17]: SDRAM RASn ----- */
//LPC_IOCON->P2_17 |= 0; // GPIO
  LPC_IOCON->P2_17 |= 1; // EMC_RASn
//LPC_IOCON->P2_17 = (LPC_IOCON->P2_17 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_17                    0
  #define PINCLR_P2_17                    0
  #define PINDIR_P2_17                    0 // Input

  /* ----- Pin P2[18]: SDRAM CLK OUT ----- */
//LPC_IOCON->P2_18 |= 0; // GPIO
  LPC_IOCON->P2_18 |= 1; // EMC_CLK_0
//LPC_IOCON->P2_18 = (LPC_IOCON->P2_18 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_18                    0
  #define PINCLR_P2_18                    0
  #define PINDIR_P2_18                    0 // Input

  /* ----- Pin P2[19]: NC ----- */
  LPC_IOCON->P2_19 |= 0; // GPIO
//LPC_IOCON->P2_19 |= 1; // EMC_CLK_1
LPC_IOCON->P2_19 = (LPC_IOCON->P2_19 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P2_19                    0
  #define PINCLR_P2_19                    0
  #define PINDIR_P2_19                    0 // Input

  /* ----- Pin P2[20]: SDRAM CSn ----- */
//LPC_IOCON->P2_20 |= 0; // GPIO
  LPC_IOCON->P2_20 |= 1; // EMC_DYCS0n
//LPC_IOCON->P2_20 = (LPC_IOCON->P2_20 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_20                    0
  #define PINCLR_P2_20                    0
  #define PINDIR_P2_20                    0 // Input

  /* ----- Pin P2[21]: Micro SD CS ----- */
  LPC_IOCON->P2_21 |= 0; // GPIO
//LPC_IOCON->P2_21 |= 1; // EMC_DYCS1n
  LPC_IOCON->P2_21 = (LPC_IOCON->P2_21 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_21                    1
  #define PINCLR_P2_21                    0
  #define PINDIR_P2_21                    1 // Output

  /* ----- Pin P2[22]: SCK0 ----- */
//LPC_IOCON->P2_22 |= 0; // GPIO
//LPC_IOCON->P2_22 |= 1; // EMC_DYCS2n
  LPC_IOCON->P2_22 |= 2; // SSP0_SCK
//LPC_IOCON->P2_22 |= 3; // T3_CAP0
//LPC_IOCON->P2_22 = (LPC_IOCON->P2_22 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P2_22                    0
  #define PINCLR_P2_22                    0
  #define PINDIR_P2_22                    0 // Input

  /* ----- Pin P2[23]: WIFI CS ----- */
  LPC_IOCON->P2_23 |= 0; // GPIO
//LPC_IOCON->P2_23 |= 1; // EMC_DYCS3n
//LPC_IOCON->P2_23 |= 2; // SSP0_SSEL
//LPC_IOCON->P2_23 |= 3; // T3_CAP1
//LPC_IOCON->P2_23 = (LPC_IOCON->P2_23 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P2_23                    1
  #define PINCLR_P2_23                    0
  #define PINDIR_P2_23                    1 // Output

  /* ----- Pin P2[24]: SDRAM Clock Enable ----- */
//LPC_IOCON->P2_24 |= 0; // GPIO
  LPC_IOCON->P2_24 |= 1; // EMC_CKE0
//LPC_IOCON->P2_24 = (LPC_IOCON->P2_24 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_24                    0
  #define PINCLR_P2_24                    0
  #define PINDIR_P2_24                    0 // Input

  /* ----- Pin P2[25]: 3V_ERR ----- */
  LPC_IOCON->P2_25 |= 0; // GPIO
//LPC_IOCON->P2_25 |= 1; // EMC_CKE1
  LPC_IOCON->P2_25 = (LPC_IOCON->P2_25 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_25                    0
  #define PINCLR_P2_25                    0
  #define PINDIR_P2_25                    0 // Input

  /* ----- Pin P2[26]: MISO0 ----- */
//LPC_IOCON->P2_26 |= 0; // GPIO
//LPC_IOCON->P2_26 |= 1; // EMC_CKE2
  LPC_IOCON->P2_26 |= 2; // SSP0_MISO
//LPC_IOCON->P2_26 |= 3; // T3_MAT0
//LPC_IOCON->P2_26 = (LPC_IOCON->P2_26 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_26                    0
  #define PINCLR_P2_26                    0
  #define PINDIR_P2_26                    0 // Input

  /* ----- Pin P2[27]: MOSI0 ----- */
//LPC_IOCON->P2_27 |= 0; // GPIO
//LPC_IOCON->P2_27 |= 1; // EMC_CKE3
  LPC_IOCON->P2_27 |= 2; // SSP0_MOSI
//LPC_IOCON->P2_27 |= 3; // T3_MAT1
  LPC_IOCON->P2_27 = (LPC_IOCON->P2_27 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_27                    0
  #define PINCLR_P2_27                    0
  #define PINDIR_P2_27                    0 // Input

  /* ----- Pin P2[28]: SDRAM DATA Q Mask 0 ----- */
//LPC_IOCON->P2_28 |= 0; // GPIO
  LPC_IOCON->P2_28 |= 1; // EMC_DQM0
//LPC_IOCON->P2_28 = (LPC_IOCON->P2_28 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_28                    0
  #define PINCLR_P2_28                    0
  #define PINDIR_P2_28                    0 // Input

  /* ----- Pin P2[29]: SDRAM Data Q Mask 1 ----- */
//LPC_IOCON->P2_29 |= 0; // GPIO
  LPC_IOCON->P2_29 |= 1; // EMC_DQM1
//LPC_IOCON->P2_29 = (LPC_IOCON->P2_29 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_29                    0
  #define PINCLR_P2_29                    0
  #define PINDIR_P2_29                    0 // Input

  /* ----- Pin P2[30]: SDRAM Data Q Mask 2 ----- */
//LPC_IOCON->P2_30 |= 0; // GPIO
  LPC_IOCON->P2_30 |= 1; // EMC_DQM2
//LPC_IOCON->P2_30 |= 2; // I2C2_SDA
//LPC_IOCON->P2_30 |= 3; // T3_MAT2
//LPC_IOCON->P2_30 = (LPC_IOCON->P2_30 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_30                    0
  #define PINCLR_P2_30                    0
  #define PINDIR_P2_30                    0 // Input

  /* ----- Pin P2[31]: SDRAM Data Q Mask 3 ----- */
//LPC_IOCON->P2_31 |= 0; // GPIO
  LPC_IOCON->P2_31 |= 1; // EMC_DQM3
//LPC_IOCON->P2_31 |= 2; // I2C2_SCL
//LPC_IOCON->P2_31 |= 3; // T3_MAT3
//LPC_IOCON->P2_31 = (LPC_IOCON->P2_31 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P2_31                    0
  #define PINCLR_P2_31                    0
  #define PINDIR_P2_31                    0 // Input



/*---------------------------------------------------------------------------*
 * Port 3 Configuration:
 *---------------------------------------------------------------------------*/
  /* ----- Pin P3[0 ]: ----- */ 
//LPC_IOCON->P3_0  |= 0; // GPIO
  LPC_IOCON->P3_0  |= 1; // EMC_D0
//LPC_IOCON->P3_0  = (LPC_IOCON->P3_0  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_0                     0
  #define PINCLR_P3_0                     0
  #define PINDIR_P3_0                     0 // Input

  /* ----- Pin P3[1 ]: ----- */ 
//LPC_IOCON->P3_1  |= 0; // GPIO
  LPC_IOCON->P3_1  |= 1; // EMC_D1
//LPC_IOCON->P3_1  = (LPC_IOCON->P3_1  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_1                     0
  #define PINCLR_P3_1                     0
  #define PINDIR_P3_1                     0 // Input

  /* ----- Pin P3[2 ]: ----- */ 
//LPC_IOCON->P3_2  |= 0; // GPIO
  LPC_IOCON->P3_2  |= 1; // EMC_D2
//LPC_IOCON->P3_2  = (LPC_IOCON->P3_2  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_2                     0
  #define PINCLR_P3_2                     0
  #define PINDIR_P3_2                     0 // Input

  /* ----- Pin P3[3 ]: ----- */ 
//LPC_IOCON->P3_3  |= 0; // GPIO
  LPC_IOCON->P3_3  |= 1; // EMC_D3
//LPC_IOCON->P3_3  = (LPC_IOCON->P3_3  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_3                     0
  #define PINCLR_P3_3                     0
  #define PINDIR_P3_3                     0 // Input

  /* ----- Pin P3[4 ]: ----- */ 
//LPC_IOCON->P3_4  |= 0; // GPIO
  LPC_IOCON->P3_4  |= 1; // EMC_D4
//LPC_IOCON->P3_4  = (LPC_IOCON->P3_4  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_4                     0
  #define PINCLR_P3_4                     0
  #define PINDIR_P3_4                     0 // Input

  /* ----- Pin P3[5 ]: ----- */ 
//LPC_IOCON->P3_5  |= 0; // GPIO
  LPC_IOCON->P3_5  |= 1; // EMC_D5
//LPC_IOCON->P3_5  = (LPC_IOCON->P3_5  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_5                     0
  #define PINCLR_P3_5                     0
  #define PINDIR_P3_5                     0 // Input

  /* ----- Pin P3[6 ]: ----- */ 
//LPC_IOCON->P3_6  |= 0; // GPIO
  LPC_IOCON->P3_6  |= 1; // EMC_D6
//LPC_IOCON->P3_6  = (LPC_IOCON->P3_6  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_6                     0
  #define PINCLR_P3_6                     0
  #define PINDIR_P3_6                     0 // Input

  /* ----- Pin P3[7 ]: ----- */ 
//LPC_IOCON->P3_7  |= 0; // GPIO
  LPC_IOCON->P3_7  |= 1; // EMC_D7
//LPC_IOCON->P3_7  = (LPC_IOCON->P3_7  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_7                     0
  #define PINCLR_P3_7                     0
  #define PINDIR_P3_7                     0 // Input

  /* ----- Pin P3[8 ]: ----- */ 
//LPC_IOCON->P3_8  |= 0; // GPIO
  LPC_IOCON->P3_8  |= 1; // EMC_D8
//LPC_IOCON->P3_8  = (LPC_IOCON->P3_8  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_8                     0
  #define PINCLR_P3_8                     0
  #define PINDIR_P3_8                     0 // Input

  /* ----- Pin P3[9 ]: ----- */ 
//LPC_IOCON->P3_9  |= 0; // GPIO
  LPC_IOCON->P3_9  |= 1; // EMC_D9
//LPC_IOCON->P3_9  = (LPC_IOCON->P3_9  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_9                     0
  #define PINCLR_P3_9                     0
  #define PINDIR_P3_9                     0 // Input

  /* ----- Pin P3[10]: ----- */ 
//LPC_IOCON->P3_10 |= 0; // GPIO
  LPC_IOCON->P3_10 |= 1; // EMC_D10
//LPC_IOCON->P3_10 = (LPC_IOCON->P3_10 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_10                    0
  #define PINCLR_P3_10                    0
  #define PINDIR_P3_10                    0 // Input

  /* ----- Pin P3[11]: ----- */ 
//LPC_IOCON->P3_11 |= 0; // GPIO
  LPC_IOCON->P3_11 |= 1; // EMC_D11
//LPC_IOCON->P3_11 = (LPC_IOCON->P3_11 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_11                    0
  #define PINCLR_P3_11                    0
  #define PINDIR_P3_11                    0 // Input

  /* ----- Pin P3[12]: ----- */ 
//LPC_IOCON->P3_12 |= 0; // GPIO
  LPC_IOCON->P3_12 |= 1; // EMC_D12
//LPC_IOCON->P3_12 = (LPC_IOCON->P3_12 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_12                    0
  #define PINCLR_P3_12                    0
  #define PINDIR_P3_12                    0 // Input

  /* ----- Pin P3[13]: ----- */ 
//LPC_IOCON->P3_13 |= 0; // GPIO
  LPC_IOCON->P3_13 |= 1; // EMC_D13
//LPC_IOCON->P3_13 = (LPC_IOCON->P3_13 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_13                    0
  #define PINCLR_P3_13                    0
  #define PINDIR_P3_13                    0 // Input

  /* ----- Pin P3[14]: ----- */ 
//LPC_IOCON->P3_14 |= 0; // GPIO
  LPC_IOCON->P3_14 |= 1; // EMC_D14
//LPC_IOCON->P3_14 = (LPC_IOCON->P3_14 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_14                    0
  #define PINCLR_P3_14                    0
  #define PINDIR_P3_14                    0 // Input

  /* ----- Pin P3[15]: ----- */ 
//LPC_IOCON->P3_15 |= 0; // GPIO
  LPC_IOCON->P3_15 |= 1; // EMC_D15
//LPC_IOCON->P3_15 = (LPC_IOCON->P3_15 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_15                    0
  #define PINCLR_P3_15                    0
  #define PINDIR_P3_15                    0 // Input

  /* ----- Pin P3[16]: ----- */ 
//LPC_IOCON->P3_16 |= 0; // GPIO
  LPC_IOCON->P3_16 |= 1; // EMC_D16
//LPC_IOCON->P3_16 |= 2; // PWM0_1
//LPC_IOCON->P3_16 |= 3; // U1_TXD
//LPC_IOCON->P3_16 = (LPC_IOCON->P3_16 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_16                    0
  #define PINCLR_P3_16                    0
  #define PINDIR_P3_16                    0 // Input

  /* ----- Pin P3[17]: ----- */ 
//LPC_IOCON->P3_17 |= 0; // GPIO
  LPC_IOCON->P3_17 |= 1; // EMC_D17
//LPC_IOCON->P3_17 |= 2; // PWM0_2
//LPC_IOCON->P3_17 |= 3; // U1_RXD
//LPC_IOCON->P3_17 = (LPC_IOCON->P3_17 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_17                    0
  #define PINCLR_P3_17                    0
  #define PINDIR_P3_17                    0 // Input

  /* ----- Pin P3[18]: ----- */ 
//LPC_IOCON->P3_18 |= 0; // GPIO
  LPC_IOCON->P3_18 |= 1; // EMC_D18
//LPC_IOCON->P3_18 |= 2; // PWM0_3
//LPC_IOCON->P3_18 |= 3; // U1_CTS
//LPC_IOCON->P3_18 = (LPC_IOCON->P3_18 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_18                    0
  #define PINCLR_P3_18                    0
  #define PINDIR_P3_18                    0 // Input

  /* ----- Pin P3[19]: ----- */ 
//LPC_IOCON->P3_19 |= 0; // GPIO
  LPC_IOCON->P3_19 |= 1; // EMC_D19
//LPC_IOCON->P3_19 |= 2; // PWM0_4
//LPC_IOCON->P3_19 |= 3; // U1_DCD
//LPC_IOCON->P3_19 = (LPC_IOCON->P3_19 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_19                    0
  #define PINCLR_P3_19                    0
  #define PINDIR_P3_19                    0 // Input

  /* ----- Pin P3[20]: ----- */ 
//LPC_IOCON->P3_20 |= 0; // GPIO
  LPC_IOCON->P3_20 |= 1; // EMC_D20
//LPC_IOCON->P3_20 |= 2; // PWM0_5
//LPC_IOCON->P3_20 |= 3; // U1_DSR
//LPC_IOCON->P3_20 = (LPC_IOCON->P3_20 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_20                    0
  #define PINCLR_P3_20                    0
  #define PINDIR_P3_20                    0 // Input

  /* ----- Pin P3[21]: ----- */ 
//LPC_IOCON->P3_21 |= 0; // GPIO
  LPC_IOCON->P3_21 |= 1; // EMC_D21
//LPC_IOCON->P3_21 |= 2; // PWM0_6
//LPC_IOCON->P3_21 |= 3; // U1_DTR
//LPC_IOCON->P3_21 = (LPC_IOCON->P3_21 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_21                    0
  #define PINCLR_P3_21                    0
  #define PINDIR_P3_21                    0 // Input

  /* ----- Pin P3[22]: ----- */ 
//LPC_IOCON->P3_22 |= 0; // GPIO
  LPC_IOCON->P3_22 |= 1; // EMC_D22
//LPC_IOCON->P3_22 |= 2; // PWM0_CAP0
//LPC_IOCON->P3_22 |= 3; // U1_RI
//LPC_IOCON->P3_22 = (LPC_IOCON->P3_22 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_22                    0
  #define PINCLR_P3_22                    0
  #define PINDIR_P3_22                    0 // Input

  /* ----- Pin P3[23]: ----- */ 
//LPC_IOCON->P3_23 |= 0; // GPIO
  LPC_IOCON->P3_23 |= 1; // EMC_D23
//LPC_IOCON->P3_23 |= 2; // PWM1_CAP0
//LPC_IOCON->P3_23 |= 3; // T0_CAP0
//LPC_IOCON->P3_23 = (LPC_IOCON->P3_23 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_23                    0
  #define PINCLR_P3_23                    0
  #define PINDIR_P3_23                    0 // Input

  /* ----- Pin P3[24]: ----- */ 
//LPC_IOCON->P3_24 |= 0; // GPIO
  LPC_IOCON->P3_24 |= 1; // EMC_D24
//LPC_IOCON->P3_24 |= 2; // PWM1_1
//LPC_IOCON->P3_24 |= 3; // T0_CAP1
//LPC_IOCON->P3_24 = (LPC_IOCON->P3_24 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_24                    0
  #define PINCLR_P3_24                    0
  #define PINDIR_P3_24                    0 // Input

  /* ----- Pin P3[25]: ----- */ 
//LPC_IOCON->P3_25 |= 0; // GPIO
  LPC_IOCON->P3_25 |= 1; // EMC_D25
//LPC_IOCON->P3_25 |= 2; // PWM1_2
//LPC_IOCON->P3_25 |= 3; // T0_MAT0
//LPC_IOCON->P3_25 = (LPC_IOCON->P3_25 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_25                    0
  #define PINCLR_P3_25                    0
  #define PINDIR_P3_25                    0 // Input

  /* ----- Pin P3[26]: ----- */ 
//LPC_IOCON->P3_26 |= 0; // GPIO
  LPC_IOCON->P3_26 |= 1; // EMC_D26
//LPC_IOCON->P3_26 |= 2; // PWM1_3
//LPC_IOCON->P3_26 |= 3; // T0_MAT1
//LPC_IOCON->P3_26 = (LPC_IOCON->P3_26 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_26                    0
  #define PINCLR_P3_26                    0
  #define PINDIR_P3_26                    0 // Input

  /* ----- Pin P3[27]: ----- */ 
//LPC_IOCON->P3_27 |= 0; // GPIO
  LPC_IOCON->P3_27 |= 1; // EMC_D27
//LPC_IOCON->P3_27 |= 2; // PWM1_4
//LPC_IOCON->P3_27 |= 3; // T1_CAP0
//LPC_IOCON->P3_27 = (LPC_IOCON->P3_27 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_27                    0
  #define PINCLR_P3_27                    0
  #define PINDIR_P3_27                    0 // Input

  /* ----- Pin P3[28]: ----- */ 
//LPC_IOCON->P3_28 |= 0; // GPIO
  LPC_IOCON->P3_28 |= 1; // EMC_D28
//LPC_IOCON->P3_28 |= 2; // PWM1_5
//LPC_IOCON->P3_28 |= 3; // T1_CAP1
//LPC_IOCON->P3_28 = (LPC_IOCON->P3_28 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_28                    0
  #define PINCLR_P3_28                    0
  #define PINDIR_P3_28                    0 // Input

  /* ----- Pin P3[29]: ----- */ 
//LPC_IOCON->P3_29 |= 0; // GPIO
  LPC_IOCON->P3_29 |= 1; // EMC_D29
//LPC_IOCON->P3_29 |= 2; // PWM1_6
//LPC_IOCON->P3_29 |= 3; // T1_MAT0
//LPC_IOCON->P3_29 = (LPC_IOCON->P3_29 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_29                    0
  #define PINCLR_P3_29                    0
  #define PINDIR_P3_29                    0 // Input

  /* ----- Pin P3[30]: ----- */ 
//LPC_IOCON->P3_30 |= 0; // GPIO
  LPC_IOCON->P3_30 |= 1; // EMC_D30
//LPC_IOCON->P3_30 |= 2; // U1_RTS
//LPC_IOCON->P3_30 |= 3; // T1_MAT1
//LPC_IOCON->P3_30 = (LPC_IOCON->P3_30 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_30                    0
  #define PINCLR_P3_30                    0
  #define PINDIR_P3_30                    0 // Input

  /* ----- Pin P3[31]: ----- */ 
//LPC_IOCON->P3_31 |= 0; // GPIO
  LPC_IOCON->P3_31 |= 1; // EMC_D31
//LPC_IOCON->P3_31 |= 3; // T1_MAT2
//LPC_IOCON->P3_31 = (LPC_IOCON->P3_31 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P3_31                    0
  #define PINCLR_P3_31                    0
  #define PINDIR_P3_31                    0 // Input



/*---------------------------------------------------------------------------*
 * Port 4 Configuration:
 *---------------------------------------------------------------------------*/
  /* ----- Pin P4[0 ]: ----- */ 
//LPC_IOCON->P4_0  |= 0; // GPIO
  LPC_IOCON->P4_0  |= 1; // EMC_A0
//LPC_IOCON->P4_0  = (LPC_IOCON->P4_0  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_0                     0
  #define PINCLR_P4_0                     0
  #define PINDIR_P4_0                     0 // Input

  /* ----- Pin P4[1 ]: ----- */ 
//LPC_IOCON->P4_1  |= 0; // GPIO
  LPC_IOCON->P4_1  |= 1; // EMC_A1
//LPC_IOCON->P4_1  = (LPC_IOCON->P4_1  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_1                     0
  #define PINCLR_P4_1                     0
  #define PINDIR_P4_1                     0 // Input

  /* ----- Pin P4[2 ]: ----- */ 
//LPC_IOCON->P4_2  |= 0; // GPIO
  LPC_IOCON->P4_2  |= 1; // EMC_A2
//LPC_IOCON->P4_2  = (LPC_IOCON->P4_2  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_2                     0
  #define PINCLR_P4_2                     0
  #define PINDIR_P4_2                     0 // Input

  /* ----- Pin P4[3 ]: ----- */ 
//LPC_IOCON->P4_3  |= 0; // GPIO
  LPC_IOCON->P4_3  |= 1; // EMC_A3
//LPC_IOCON->P4_3  = (LPC_IOCON->P4_3  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_3                     0
  #define PINCLR_P4_3                     0
  #define PINDIR_P4_3                     0 // Input

  /* ----- Pin P4[4 ]: ----- */ 
//LPC_IOCON->P4_4  |= 0; // GPIO
  LPC_IOCON->P4_4  |= 1; // EMC_A4
//LPC_IOCON->P4_4  = (LPC_IOCON->P4_4  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_4                     0
  #define PINCLR_P4_4                     0
  #define PINDIR_P4_4                     0 // Input

  /* ----- Pin P4[5 ]: ----- */ 
//LPC_IOCON->P4_5  |= 0; // GPIO
  LPC_IOCON->P4_5  |= 1; // EMC_A5
//LPC_IOCON->P4_5  = (LPC_IOCON->P4_5  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_5                     0
  #define PINCLR_P4_5                     0
  #define PINDIR_P4_5                     0 // Input

  /* ----- Pin P4[6 ]: ----- */ 
//LPC_IOCON->P4_6  |= 0; // GPIO
  LPC_IOCON->P4_6  |= 1; // EMC_A6
//LPC_IOCON->P4_6  = (LPC_IOCON->P4_6  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_6                     0
  #define PINCLR_P4_6                     0
  #define PINDIR_P4_6                     0 // Input

  /* ----- Pin P4[7 ]: ----- */ 
//LPC_IOCON->P4_7  |= 0; // GPIO
  LPC_IOCON->P4_7  |= 1; // EMC_A7
//LPC_IOCON->P4_7  = (LPC_IOCON->P4_7  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_7                     0
  #define PINCLR_P4_7                     0
  #define PINDIR_P4_7                     0 // Input

  /* ----- Pin P4[8 ]: ----- */ 
//LPC_IOCON->P4_8  |= 0; // GPIO
  LPC_IOCON->P4_8  |= 1; // EMC_A8
//LPC_IOCON->P4_8  = (LPC_IOCON->P4_8  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_8                     0
  #define PINCLR_P4_8                     0
  #define PINDIR_P4_8                     0 // Input

  /* ----- Pin P4[9 ]: ----- */ 
//LPC_IOCON->P4_9  |= 0; // GPIO
  LPC_IOCON->P4_9  |= 1; // EMC_A9
//LPC_IOCON->P4_9  = (LPC_IOCON->P4_9  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_9                     0
  #define PINCLR_P4_9                     0
  #define PINDIR_P4_9                     0 // Input

  /* ----- Pin P4[10]: ----- */ 
//LPC_IOCON->P4_10 |= 0; // GPIO
  LPC_IOCON->P4_10 |= 1; // EMC_A10
//LPC_IOCON->P4_10 = (LPC_IOCON->P4_10 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_10                    0
  #define PINCLR_P4_10                    0
  #define PINDIR_P4_10                    0 // Input

  /* ----- Pin P4[11]: ----- */ 
//LPC_IOCON->P4_11 |= 0; // GPIO
  LPC_IOCON->P4_11 |= 1; // EMC_A11
//LPC_IOCON->P4_11 = (LPC_IOCON->P4_11 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_11                    0
  #define PINCLR_P4_11                    0
  #define PINDIR_P4_11                    0 // Input

  /* ----- Pin P4[12]: ----- */ 
//LPC_IOCON->P4_12 |= 0; // GPIO
  LPC_IOCON->P4_12 |= 1; // EMC_A12
//LPC_IOCON->P4_12 = (LPC_IOCON->P4_12 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_12                    0
  #define PINCLR_P4_12                    0
  #define PINDIR_P4_12                    0 // Input

  /* ----- Pin P4[13]: ----- */ 
//LPC_IOCON->P4_13 |= 0; // GPIO
  LPC_IOCON->P4_13 |= 1; // EMC_A13
//LPC_IOCON->P4_13 = (LPC_IOCON->P4_13 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_13                    0
  #define PINCLR_P4_13                    0
  #define PINDIR_P4_13                    0 // Input

  /* ----- Pin P4[14]: ----- */ 
//LPC_IOCON->P4_14 |= 0; // GPIO
  LPC_IOCON->P4_14 |= 1; // EMC_A14
//LPC_IOCON->P4_14 = (LPC_IOCON->P4_14 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_14                    0
  #define PINCLR_P4_14                    0
  #define PINDIR_P4_14                    0 // Input

  /* ----- Pin P4[15]: ----- */ 
//LPC_IOCON->P4_15 |= 0; // GPIO
  LPC_IOCON->P4_15 |= 1; // EMC_A15
//LPC_IOCON->P4_15 = (LPC_IOCON->P4_15 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_15                    0
  #define PINCLR_P4_15                    0
  #define PINDIR_P4_15                    0 // Input

  /* ----- Pin P4[16]: ----- */ 
//LPC_IOCON->P4_16 |= 0; // GPIO
  LPC_IOCON->P4_16 |= 1; // EMC_A16
//LPC_IOCON->P4_16 = (LPC_IOCON->P4_16 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_16                    0
  #define PINCLR_P4_16                    0
  #define PINDIR_P4_16                    0 // Input

  /* ----- Pin P4[17]: ----- */ 
//LPC_IOCON->P4_17 |= 0; // GPIO
  LPC_IOCON->P4_17 |= 1; // EMC_A17
//LPC_IOCON->P4_17 = (LPC_IOCON->P4_17 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_17                    0
  #define PINCLR_P4_17                    0
  #define PINDIR_P4_17                    0 // Input

  /* ----- Pin P4[18]: ----- */ 
//LPC_IOCON->P4_18 |= 0; // GPIO
  LPC_IOCON->P4_18 |= 1; // EMC_A18
//LPC_IOCON->P4_18 = (LPC_IOCON->P4_18 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_18                    0
  #define PINCLR_P4_18                    0
  #define PINDIR_P4_18                    0 // Input

  /* ----- Pin P4[19]: ----- */ 
//LPC_IOCON->P4_19 |= 0; // GPIO
  LPC_IOCON->P4_19 |= 1; // EMC_A19
//LPC_IOCON->P4_19 = (LPC_IOCON->P4_19 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_19                    0
  #define PINCLR_P4_19                    0
  #define PINDIR_P4_19                    0 // Input

  /* ----- Pin P4[20]:A20  ----- */
//LPC_IOCON->P4_20 |= 0; // GPIO
  LPC_IOCON->P4_20 |= 1; // EMC_A20
//LPC_IOCON->P4_20 |= 2; // I2C2_SDA
//    LPC_IOCON->P4_20 = (LPC_IOCON->P4_20 & ~(3<<10)) | (1<<10);
#if HAVE_LOC_REGISTERS
//    LPC_IOCON->LOC_I2C2_SDA = 2;
#endif
  //LPC_IOCON->P4_20 |= 3; // SSP1_SCK
//LPC_IOCON->P4_20 = (LPC_IOCON->P4_20 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_20                    0
  #define PINCLR_P4_20                    0
  #define PINDIR_P4_20                    0 // Input

  /* ----- Pin P4[21]: A21 ----- */
//LPC_IOCON->P4_21 |= 0; // GPIO
  LPC_IOCON->P4_21 |= 1; // EMC_A21
//LPC_IOCON->P4_21 |= 2; // I2C2_SCL
//    LPC_IOCON->P4_21 = (LPC_IOCON->P4_21 & ~(3<<10)) | (1<<10);
#if HAVE_LOC_REGISTERS
//    LPC_IOCON->LOC_I2C2_SCL = 2;
#endif
//LPC_IOCON->P4_21 |= 3; // SSP1_SSEL
//LPC_IOCON->P4_21 = (LPC_IOCON->P4_21 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_21                    0
  #define PINCLR_P4_21                    0
  #define PINDIR_P4_21                    0 // Input

  /* ----- Pin P4[22]: A22 ----- */
//LPC_IOCON->P4_22 |= 0; // GPIO
  LPC_IOCON->P4_22 |= 1; // EMC_A22
//LPC_IOCON->P4_22 |= 2; // U2_TXD
//LPC_IOCON->P4_22 |= 3; // SSP1_MISO
//LPC_IOCON->P4_22 = (LPC_IOCON->P4_22 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P4_22                    0
  #define PINCLR_P4_22                    0
  #define PINDIR_P4_22                    0 // Input

  /* ----- Pin P4[23]: A23 ----- */
//LPC_IOCON->P4_23 |= 0; // GPIO
  LPC_IOCON->P4_23 |= 1; // EMC_A23
//LPC_IOCON->P4_23 |= 2; // U2_RXD
//LPC_IOCON->P4_23 |= 3; // SSP1_MOSI
//LPC_IOCON->P4_23 = (LPC_IOCON->P4_23 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P4_23                    0
  #define PINCLR_P4_23                    0
  #define PINDIR_P4_23                    0 // Input

  /* ----- Pin P4[24]: SDRAM OEn ----- */
//LPC_IOCON->P4_24 |= 0; // GPIO
  LPC_IOCON->P4_24 |= 1; // EMC_OEn
  LPC_IOCON->P4_24 = (LPC_IOCON->P4_24 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_24                    0
  #define PINCLR_P4_24                    0
  #define PINDIR_P4_24                    0 // Input

  /* ----- Pin P4[25]: SDRAM WEn ----- */
//LPC_IOCON->P4_25 |= 0; // GPIO
  LPC_IOCON->P4_25 |= 1; // EMC_WEn
//LPC_IOCON->P4_25 = (LPC_IOCON->P4_25 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_25                    0
  #define PINCLR_P4_25                    0
  #define PINDIR_P4_25                    0 // Input

  /* ----- Pin P4[26]: USB1H_PWRD ----- */
  LPC_IOCON->P4_26 |= 0; // GPIO
//LPC_IOCON->P4_26 |= 1; // EMC_BLS0n
  LPC_IOCON->P4_26 = (LPC_IOCON->P4_26 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_26                    0
  #define PINCLR_P4_26                    0
  #define PINDIR_P4_26                    0 // Input

  /* ----- Pin P4[27]: USB1H_OVC ----- */
  LPC_IOCON->P4_27 |= 0; // GPIO
//LPC_IOCON->P4_27 |= 1; // EMC_BLS1n
  LPC_IOCON->P4_27 = (LPC_IOCON->P4_27 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_27                    0
  #define PINCLR_P4_27                    0
  #define PINDIR_P4_27                    0 // Input

  /* ----- Pin P4[28]: LCDCD[2] ----- */
//LPC_IOCON->P4_28 |= 0; // GPIO
//LPC_IOCON->P4_28 |= 1; // EMC_BLS2n
//LPC_IOCON->P4_28 |= 2; // TXD3
//LPC_IOCON->P4_28 |= 3; // MAT2_0
//LPC_IOCON->P4_28 |= 5; // LCD_VD6
//LPC_IOCON->P4_28 |= 6; // LCD_VD10
  LPC_IOCON->P4_28 |= 7; // LCD_VD2
//LPC_IOCON->P4_28 = (LPC_IOCON->P4_28 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_28                    0
  #define PINCLR_P4_28                    0
  #define PINDIR_P4_28                    0 // Input

  /* ----- Pin P4[29]: LCDVD[3] ----- */
//LPC_IOCON->P4_29 |= 0; // GPIO
//LPC_IOCON->P4_29 |= 1; // EMC_BLS3n
//LPC_IOCON->P4_29 |= 2; // U3_RXD
//LPC_IOCON->P4_29 |= 3; // T2_MAT1
//LPC_IOCON->P4_29 |= 4; // I2C2_SCL
//LPC_IOCON->P4_29 |= 5; // LCD_VD7
//LPC_IOCON->P4_29 |= 6; // LCD_VD11
  LPC_IOCON->P4_29 |= 7; // LCD_VD3
//LPC_IOCON->P4_29 = (LPC_IOCON->P4_29 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_29                    0
  #define PINCLR_P4_29                    0
  #define PINDIR_P4_29                    0 // Input

  /* ----- Pin P4[30]: Flash CS0n ----- */
//LPC_IOCON->P4_30 |= 0; // GPIO
  LPC_IOCON->P4_30 |= 1; // EMC_CS0n
  LPC_IOCON->P4_30 = (LPC_IOCON->P4_30 & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P4_30                    0
  #define PINCLR_P4_30                    0
  #define PINDIR_P4_30                    0 // Input

  /* ----- Pin P4[31]: LCD_RSTn ----- */
  LPC_IOCON->P4_31 |= 0; // GPIO
//LPC_IOCON->P4_31 |= 1; // EMC_CS1n
//  LPC_IOCON->P4_31 = (LPC_IOCON->P4_31 & ~(3<<3)) | (2<<3); // Pull-up
  #define PINSET_P4_31                    1
  #define PINCLR_P4_31                    0
  #define PINDIR_P4_31                    1 // Output



/*---------------------------------------------------------------------------*
 * Port 5 Configuration:
 *---------------------------------------------------------------------------*/
  /* ----- Pin P5[0 ]: ----- */ 
  LPC_IOCON->P5_0  |= 0; // GPIO
//LPC_IOCON->P5_0  |= 1; // EMC_A24
//LPC_IOCON->P5_0  |= 3; // T2_MAT2
  LPC_IOCON->P5_0  = (LPC_IOCON->P5_0  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P5_0                     0
  #define PINCLR_P5_0                     0
  #define PINDIR_P5_0                     0 // Input

  /* ----- Pin P5[1 ]: ----- */ 
  LPC_IOCON->P5_1  |= 0; // GPIO
//LPC_IOCON->P5_1  |= 1; // EMC_A25
//LPC_IOCON->P5_1  |= 3; // T2_MAT3
  LPC_IOCON->P5_1  = (LPC_IOCON->P5_1  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P5_1                     0
  #define PINCLR_P5_1                     0
  #define PINDIR_P5_1                     0 // Input

  /* ----- Pin P5[2 ]: ----- */ 
//LPC_IOCON->P5_2  |= 0; // GPIO
//  LPC_IOCON->P5_2  = (LPC_IOCON->P5_2  & ~(3<<3)) | (2<<3); // Pull-up
//LPC_IOCON->P5_2  |= 3; // T3_MAT2
  LPC_IOCON->P5_2  |= 5; // I2C0_SDA
    LPC_IOCON->P5_2  =
            (5<<0)| // FUNC: I2C0_SDA pin mux
            (0<<6)| // INVERT: Not inverted
            (0<<8)| // HS: I2C 50ns glitch filter and slew rate control enabled
            (1<<9); // HIDRIVE: Output drive sink is 20 mA for Fast Mode Plus

  #define PINSET_P5_2                     0
  #define PINCLR_P5_2                     0
  #define PINDIR_P5_2                     0 // Input

  /* ----- Pin P5[3 ]: ----- */ 
//LPC_IOCON->P5_3  |= 0; // GPIO
//  LPC_IOCON->P5_3  = (LPC_IOCON->P5_3  & ~(3<<3)) | (2<<3); // Pull-up
//LPC_IOCON->P5_3  |= 4; // U4_RXD
  LPC_IOCON->P5_3  |= 5; // I2C0_SCL
      LPC_IOCON->P5_3  =
              (5<<0)| // FUNC: I2C0_SCL pin mux
              (0<<6)| // INVERT: Not inverted
              (0<<8)| // HS: I2C 50ns glitch filter and slew rate control enabled
              (1<<9); // HIDRIVE: Output drive sink is 20 mA for Fast Mode Plus
  #define PINSET_P5_3                     0
  #define PINCLR_P5_3                     0
  #define PINDIR_P5_3                     0 // Input

  /* ----- Pin P5[4 ]: ----- */ 
  LPC_IOCON->P5_4  |= 0; // GPIO
//LPC_IOCON->P5_4  |= 1; // U0_OE
//LPC_IOCON->P5_4  |= 3; // T3_MAT3
//LPC_IOCON->P5_4  |= 4; // U4_TXD
  LPC_IOCON->P5_4  = (LPC_IOCON->P5_4  & ~(3<<3)) | (2<<3); // Pull-up 
  #define PINSET_P5_4                     0
  #define PINCLR_P5_4                     0
  #define PINDIR_P5_4                     0 // Input

  /* ----- Pin P5[5 ]: Not used ----- */ 
  #define PINSET_P5_5                     0
  #define PINCLR_P5_5                     0
  #define PINDIR_P5_5                     0 // Input

  /* ----- Pin P5[6 ]: Not used ----- */ 
  #define PINSET_P5_6                     0
  #define PINCLR_P5_6                     0
  #define PINDIR_P5_6                     0 // Input

  /* ----- Pin P5[7 ]: Not used ----- */ 
  #define PINSET_P5_7                     0
  #define PINCLR_P5_7                     0
  #define PINDIR_P5_7                     0 // Input

  /* ----- Pin P5[8 ]: Not used ----- */ 
  #define PINSET_P5_8                     0
  #define PINCLR_P5_8                     0
  #define PINDIR_P5_8                     0 // Input

  /* ----- Pin P5[9 ]: Not used ----- */ 
  #define PINSET_P5_9                     0
  #define PINCLR_P5_9                     0
  #define PINDIR_P5_9                     0 // Input

  /* ----- Pin P5[10]: Not used ----- */ 
  #define PINSET_P5_10                    0
  #define PINCLR_P5_10                    0
  #define PINDIR_P5_10                    0 // Input

  /* ----- Pin P5[11]: Not used ----- */ 
  #define PINSET_P5_11                    0
  #define PINCLR_P5_11                    0
  #define PINDIR_P5_11                    0 // Input

  /* ----- Pin P5[12]: Not used ----- */ 
  #define PINSET_P5_12                    0
  #define PINCLR_P5_12                    0
  #define PINDIR_P5_12                    0 // Input

  /* ----- Pin P5[13]: Not used ----- */ 
  #define PINSET_P5_13                    0
  #define PINCLR_P5_13                    0
  #define PINDIR_P5_13                    0 // Input

  /* ----- Pin P5[14]: Not used ----- */ 
  #define PINSET_P5_14                    0
  #define PINCLR_P5_14                    0
  #define PINDIR_P5_14                    0 // Input

  /* ----- Pin P5[15]: Not used ----- */ 
  #define PINSET_P5_15                    0
  #define PINCLR_P5_15                    0
  #define PINDIR_P5_15                    0 // Input

  /* ----- Pin P5[16]: Not used ----- */ 
  #define PINSET_P5_16                    0
  #define PINCLR_P5_16                    0
  #define PINDIR_P5_16                    0 // Input

  /* ----- Pin P5[17]: Not used ----- */ 
  #define PINSET_P5_17                    0
  #define PINCLR_P5_17                    0
  #define PINDIR_P5_17                    0 // Input

  /* ----- Pin P5[18]: Not used ----- */ 
  #define PINSET_P5_18                    0
  #define PINCLR_P5_18                    0
  #define PINDIR_P5_18                    0 // Input

  /* ----- Pin P5[19]: Not used ----- */ 
  #define PINSET_P5_19                    0
  #define PINCLR_P5_19                    0
  #define PINDIR_P5_19                    0 // Input

  /* ----- Pin P5[20]: Not used ----- */ 
  #define PINSET_P5_20                    0
  #define PINCLR_P5_20                    0
  #define PINDIR_P5_20                    0 // Input

  /* ----- Pin P5[21]: Not used ----- */ 
  #define PINSET_P5_21                    0
  #define PINCLR_P5_21                    0
  #define PINDIR_P5_21                    0 // Input

  /* ----- Pin P5[22]: Not used ----- */ 
  #define PINSET_P5_22                    0
  #define PINCLR_P5_22                    0
  #define PINDIR_P5_22                    0 // Input

  /* ----- Pin P5[23]: Not used ----- */ 
  #define PINSET_P5_23                    0
  #define PINCLR_P5_23                    0
  #define PINDIR_P5_23                    0 // Input

  /* ----- Pin P5[24]: Not used ----- */ 
  #define PINSET_P5_24                    0
  #define PINCLR_P5_24                    0
  #define PINDIR_P5_24                    0 // Input

  /* ----- Pin P5[25]: Not used ----- */ 
  #define PINSET_P5_25                    0
  #define PINCLR_P5_25                    0
  #define PINDIR_P5_25                    0 // Input

  /* ----- Pin P5[26]: Not used ----- */ 
  #define PINSET_P5_26                    0
  #define PINCLR_P5_26                    0
  #define PINDIR_P5_26                    0 // Input

  /* ----- Pin P5[27]: Not used ----- */ 
  #define PINSET_P5_27                    0
  #define PINCLR_P5_27                    0
  #define PINDIR_P5_27                    0 // Input

  /* ----- Pin P5[28]: Not used ----- */ 
  #define PINSET_P5_28                    0
  #define PINCLR_P5_28                    0
  #define PINDIR_P5_28                    0 // Input

  /* ----- Pin P5[29]: Not used ----- */ 
  #define PINSET_P5_29                    0
  #define PINCLR_P5_29                    0
  #define PINDIR_P5_29                    0 // Input

  /* ----- Pin P5[30]: Not used ----- */ 
  #define PINSET_P5_30                    0
  #define PINCLR_P5_30                    0
  #define PINDIR_P5_30                    0 // Input

  /* ----- Pin P5[31]: Not used ----- */ 
  #define PINSET_P5_31                    0
  #define PINCLR_P5_31                    0
  #define PINDIR_P5_31                    0 // Input

