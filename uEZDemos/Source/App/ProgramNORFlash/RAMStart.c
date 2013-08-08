#include  <uEZ.h>

void __stackless StartFromRAM(void)  
{
  asm (  "    MOV r0, #0x10000000" );
  asm (  "    LDR sp, [r0]" );
  asm (  "    ADD r0, r0, #4" );
  asm (  "    LDR r0, [r0] ");
  asm (  "    BX r0  " );
}
