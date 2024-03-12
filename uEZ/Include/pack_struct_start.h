/*
 * FreeRTOS+TCP <DEVELOPMENT BRANCH>
 * Copyright (C) 2022 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*****************************************************************************
*
* See the following URL for an explanation of this file:
* http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Compiler_Porting.html
*
*****************************************************************************/
#ifdef __GNUC__
/* Nothing to do here. */
#endif
  
#ifdef __IAR_SYSTEMS_ICC__
/* Following line is to suppress the incorrect warning [Warning[Pe1644]: definition at
 * end of file not followed by a semicolon or a declarator] emitted by the IAR Compiler */
#pragma diag_suppress=Pe1644

/* Following line is to suppress the warning about the usage of forward declarations. */
#pragma diag_suppress=Pe301

__packed
#endif

#if (COMPILER_TYPE==RENESASRX)

#ifdef _SH
    #ifdef __RENESAS__
        #pragma pack 1
    #endif
#endif
#ifdef __RX
    #ifdef __CCRX__
        #pragma pack
    #endif
#endif

#endif
