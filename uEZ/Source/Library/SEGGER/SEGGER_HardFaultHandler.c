/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2016 SEGGER Microcontroller GmbH & Co. KG         *
*                                                                    *
*     Internet: segger.com   Support: support_embos@segger.com       *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : SEGGER_HardFaultHandler.c
Purpose : Generic SEGGER HardFault handler for Cortex-M
--------  END-OF-HEADER  ---------------------------------------------
*/

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define SYSHND_CTRL  (*(volatile uint32_t*)  (0xE000ED24u))  // System Handler Control and State Register
#define NVIC_MFSR    (*(volatile unsigned char*) (0xE000ED28u))  // Memory Management Fault Status Register
#define NVIC_BFSR    (*(volatile unsigned char*) (0xE000ED29u))  // Bus Fault Status Register
#define NVIC_UFSR    (*(volatile unsigned short*)(0xE000ED2Au))  // Usage Fault Status Register
#define NVIC_HFSR    (*(volatile unsigned short*)(0xE000ED2Cu))  // Hard Fault Status Register
#define NVIC_DFSR    (*(volatile unsigned short*)(0xE000ED30u))  // Debug Fault Status Register
#define NVIC_BFAR    (*(volatile uint32_t*)  (0xE000ED38u))  // Bus Fault Manage Address Register
#define NVIC_AFSR    (*(volatile unsigned short*)(0xE000ED3Cu))  // Auxiliary Fault Status Register

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static volatile uint32_t _Continue;  // Set this variable to 1 to run further

static struct {
  struct {
    volatile uint32_t r0;            // Register R0
    volatile uint32_t r1;            // Register R1
    volatile uint32_t r2;            // Register R2
    volatile uint32_t r3;            // Register R3
    volatile uint32_t r12;           // Register R12
    volatile uint32_t lr;            // Link register
    volatile uint32_t pc;            // Program counter
    union {
      volatile uint32_t byte;
      struct {
        uint32_t IPSR : 8;           // Interrupt Program Status register (IPSR)
        uint32_t EPSR : 19;          // Execution Program Status register (EPSR)
        uint32_t APSR : 5;           // Application Program Status register (APSR)
      } bits;
    } psr;                               // Program status register.
  } SavedRegs;

  union {
    volatile uint32_t byte;
    struct {
      uint32_t MEMFAULTACT    : 1;   // Read as 1 if memory management fault is active
      uint32_t BUSFAULTACT    : 1;   // Read as 1 if bus fault exception is active
      uint32_t UnusedBits1    : 1;
      uint32_t USGFAULTACT    : 1;   // Read as 1 if usage fault exception is active
      uint32_t UnusedBits2    : 3;
      uint32_t SVCALLACT      : 1;   // Read as 1 if SVC exception is active
      uint32_t MONITORACT     : 1;   // Read as 1 if debug monitor exception is active
      uint32_t UnusedBits3    : 1;
      uint32_t PENDSVACT      : 1;   // Read as 1 if PendSV exception is active
      uint32_t SYSTICKACT     : 1;   // Read as 1 if SYSTICK exception is active
      uint32_t USGFAULTPENDED : 1;   // Usage fault pended; usage fault started but was replaced by a higher-priority exception
      uint32_t MEMFAULTPENDED : 1;   // Memory management fault pended; memory management fault started but was replaced by a higher-priority exception
      uint32_t BUSFAULTPENDED : 1;   // Bus fault pended; bus fault handler was started but was replaced by a higher-priority exception
      uint32_t SVCALLPENDED   : 1;   // SVC pended; SVC was started but was replaced by a higher-priority exception
      uint32_t MEMFAULTENA    : 1;   // Memory management fault handler enable
      uint32_t BUSFAULTENA    : 1;   // Bus fault handler enable
      uint32_t USGFAULTENA    : 1;   // Usage fault handler enable
    } bits;
  } syshndctrl;                          // System Handler Control and State Register (0xE000ED24)

  union {
    volatile unsigned char byte;
    struct {
      unsigned char IACCVIOL    : 1;     // Instruction access violation
      unsigned char DACCVIOL    : 1;     // Data access violation
      unsigned char UnusedBits  : 1;
      unsigned char MUNSTKERR   : 1;     // Unstacking error
      unsigned char MSTKERR     : 1;     // Stacking error
      unsigned char UnusedBits2 : 2;
      unsigned char MMARVALID   : 1;     // Indicates the MMAR is valid
    } bits;
  } mfsr;                                // Memory Management Fault Status Register (0xE000ED28)

  union {
    volatile uint32_t byte;
    struct {
      uint32_t IBUSERR    : 1;       // Instruction access violation
      uint32_t PRECISERR  : 1;       // Precise data access violation
      uint32_t IMPREISERR : 1;       // Imprecise data access violation
      uint32_t UNSTKERR   : 1;       // Unstacking error
      uint32_t STKERR     : 1;       // Stacking error
      uint32_t UnusedBits : 2;
      uint32_t BFARVALID  : 1;       // Indicates BFAR is valid
    } bits;
  } bfsr;                                // Bus Fault Status Register (0xE000ED29)
  volatile uint32_t bfar;            // Bus Fault Manage Address Register (0xE000ED38)

  union {
    volatile unsigned short byte;
    struct {
      unsigned short UNDEFINSTR : 1;     // Attempts to execute an undefined instruction
      unsigned short INVSTATE   : 1;     // Attempts to switch to an invalid state (e.g., ARM)
      unsigned short INVPC      : 1;     // Attempts to do an exception with a bad value in the EXC_RETURN number
      unsigned short NOCP       : 1;     // Attempts to execute a coprocessor instruction
      unsigned short UnusedBits : 4;
      unsigned short UNALIGNED  : 1;     // Indicates that an unaligned access fault has taken place
      unsigned short DIVBYZERO  : 1;     // Indicates a divide by zero has taken place (can be set only if DIV_0_TRP is set)
    } bits;
  } ufsr;                                // Usage Fault Status Register (0xE000ED2A)

  union {
    volatile uint32_t byte;
    struct {
      uint32_t UnusedBits  : 1;
      uint32_t VECTBL      : 1;      // Indicates hard fault is caused by failed vector fetch
      uint32_t UnusedBits2 : 27;
      uint32_t FORCED      : 1;      // Indicates hard fault is taken because of bus fault/memory management fault/usage fault
      uint32_t DEBUGEVT    : 1;      // Indicates hard fault is triggered by debug event
    } bits;
  } hfsr;                                // Hard Fault Status Register (0xE000ED2C)

  union {
    volatile uint32_t byte;
    struct {
      uint32_t HALTED   : 1;         // Halt requested in NVIC
      uint32_t BKPT     : 1;         // BKPT instruction executed
      uint32_t DWTTRAP  : 1;         // DWT match occurred
      uint32_t VCATCH   : 1;         // Vector fetch occurred
      uint32_t EXTERNAL : 1;         // EDBGRQ signal asserted
    } bits;
  } dfsr;                                // Debug Fault Status Register (0xE000ED30)

  volatile uint32_t afsr;            // Auxiliary Fault Status Register (0xE000ED3C), Vendor controlled (optional)
} HardFaultRegs;

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       HardFaultHandler()
*
*  Function description
*    C part of the hard fault handler which is called by the assembler
*    function HardFault_Handler
*/
void HardFaultHandler(uint32_t* pStack);
void HardFaultHandler(uint32_t* pStack) {
  //
  // In case we received a hard fault because of a breakpoint
  // instruction, we return. This may happen with the IAR compiler
  // when using semihosting for printf outputs.
  //
  if (NVIC_HFSR & (1u << 31)) {
    NVIC_HFSR |=  (1u << 31);     // Reset Hard Fault status
    *(pStack + 6u) += 2u;         // PC is located on stack at SP + 24 bytes. Increment PC by 2 to skip break instruction.
    return;                       // Return to interrupted application
  }
  //
  // Read NVIC registers
  //
  HardFaultRegs.syshndctrl.byte = SYSHND_CTRL;  // System Handler Control and State Register
  HardFaultRegs.mfsr.byte       = NVIC_MFSR;    // Memory Fault Status Register
  HardFaultRegs.bfsr.byte       = NVIC_BFSR;    // Bus Fault Status Register
  HardFaultRegs.bfar            = NVIC_BFAR;    // Bus Fault Manage Address Register
  HardFaultRegs.ufsr.byte       = NVIC_UFSR;    // Usage Fault Status Register
  HardFaultRegs.hfsr.byte       = NVIC_HFSR;    // Hard Fault Status Register
  HardFaultRegs.dfsr.byte       = NVIC_DFSR;    // Debug Fault Status Register
  HardFaultRegs.afsr            = NVIC_AFSR;    // Auxiliary Fault Status Register
  //
  // Halt execution
  // If NVIC registers indicate readable memory, change the variable value to != 0 to continue execution.
  //
  _Continue = 0u;
  while (_Continue == 0u) {
  }
  //
  // Read saved registers from the stack.
  //
  HardFaultRegs.SavedRegs.r0       = pStack[0];  // Register R0
  HardFaultRegs.SavedRegs.r1       = pStack[1];  // Register R1
  HardFaultRegs.SavedRegs.r2       = pStack[2];  // Register R2
  HardFaultRegs.SavedRegs.r3       = pStack[3];  // Register R3
  HardFaultRegs.SavedRegs.r12      = pStack[4];  // Register R12
  HardFaultRegs.SavedRegs.lr       = pStack[5];  // Link register LR
  HardFaultRegs.SavedRegs.pc       = pStack[6];  // Program counter PC
  HardFaultRegs.SavedRegs.psr.byte = pStack[7];  // Program status word PSR
  //
  // Halt execution
  // To step out of the HardFaultHandler, change the variable value to != 0.
  //
  _Continue = 0u;
  while (_Continue == 0u) {
  }
}

/*************************** End of file ****************************/
