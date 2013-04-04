#include	<machine.h>
#include	<_h_c_lib.h>
#include	"typedefine.h"
#include	"stacksct.h"

#pragma inline_asm Change_PSW_PM_to_UserMode

void PowerON_Reset_PC(void);

#define PSW_init  0x00010000
#define FPSW_init 0x00000100

#pragma section ResetPRG
#pragma entry PowerON_Reset_PC

void PowerON_Reset_PC(void)
{ 
	set_intb((void *)__sectop("C$VECT"));
	set_fpsw(FPSW_init);

	_INITSCT();;

	_INIT_IOLIB();			// Use SIM I/O
		
	SystemInit();			// Use Hardware Setup
    nop();

	set_psw(PSW_init);		// Set Ubit & Ibit for PSW

	main();

	brk();
}
