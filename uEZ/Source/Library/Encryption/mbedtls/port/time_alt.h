/**
 * \file time_alt.h
 *
 * \brief system time functions
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#ifndef MBEDTLS_TIME_ALT_H
#define MBEDTLS_TIME_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

// Include both of these before the below define checks and include that before time.h
#include <stdint.h>
#include <stdlib.h>

#ifndef _CLOCK_T_ // Rowley Runtime
  //#undef __CLOCK_T_DEFINED // not needed or used in Rowley yet
#else // Standard GCC runtime
  #ifndef __machine_clock_t_defined
    #define __machine_clock_t_defined
    typedef       _CLOCK_T_       clock_t;
    #define __clock_t_defined
    #define _CLOCK_T_DECLARED
  #endif
#endif

#include <time.h> // TODO can we include time.h yet on IAR and does it work?

time_t get_system_rtc_time_t( time_t *arg );

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_TIME_ALT_H */
