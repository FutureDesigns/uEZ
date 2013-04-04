/**************************************************
 *
 * Part two of the system initialization code, contains C-level
 * initialization.
 *
 * Copyright 2006 IAR Systems. All rights reserved.
 *
 * $Revision: 22760 $
 *
 **************************************************/


; --------------------------------------------------
; Module ?cmain, C-level initialization.
;	
	
        MODULE  ?cmain

; ----------------------------------------
; This is where cstartup (part one of the initialization code) jumps
; when done.
;

        SECTION .text:CODE:NOROOT(2)

        PUBLIC  ?main
        EXTERN  __iar_data_init2
        EXTERN  __low_level_init
        EXTERN  __call_ctors
        EXTERN  main
        EXTERN UEZBSPInit
        EXTERN  exit

        ARM
?main:
        REQUIRE ?jump_to_main

	      MOV	r0,#1
        BL      __low_level_init

        CMP     r0, #0
        BLNE    __iar_data_init2

        ;; Fall through.

; ----------------------------------------
; Call the constructors of all global C++ objects.
;
; This segment is only included if needed.
;

        SECTION .text:CODE:NOROOT(2)

        ARM
        ;; This label is REQUIREd by ILink if dynamic initialization
        ;; is needed.
        PUBLIC  __iar_cstart_call_ctors

__iar_cstart_call_ctors:

        ;; ILink guarantees that PREINIT_ARRAY and INIT_ARRAY are
        ;; grouped together.
        LDR     r0, pibase
        LDR     r1, ilimit
        BL      __call_ctors

        ;; Fall through

; ----------------------------------------
; Call the "main" function (i.e. the user application). If it returns
; call "exit". Should that too return call our low-level termination
; function "_exit". As a final resort, call the function responsible
; for halting the application "__exit" (repeatedly).
;

        SECTION .text:CODE:NOROOT(2)
        EXTERN  __exit

        ARM

__iar_init$$done:               ; Copy initialization is done

?jump_to_main:
        MOV     r0, #0                  ; argc, no parameters.
        BL      main
;;        BL      UEZBSPInit
        BL      exit

exit_loop
        BL      __exit
        B       exit_loop

; ----------------------------------------
; Explicitly encoded constant table.
;
; This should be placed in a section with the same name as the code above
; to ensure the code can reach the entries.
;
; If we would use the "=" notation the constant table would be placed
; after the constructor initalization section, making it impossible to
; fall through to the next section.
;

        SECTION .text:CODE:NOROOT(2)

        DATA

        EXTERN  SHT$$PREINIT_ARRAY$$Base
pibase  DC32    SHT$$PREINIT_ARRAY$$Base

        EXTERN  SHT$$INIT_ARRAY$$Limit
ilimit  DC32    SHT$$INIT_ARRAY$$Limit

        END
