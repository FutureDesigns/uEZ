/*
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

#ifndef SNTP_EXAMPLE_H
#define SNTP_EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

// Platform specific function to set the RTC.
void sntp_set_system_time(uint32_t sec);
// future placeholder
void sntp_set_system_time_ntp(uint32_t sec, uint32_t frac);
// get timeval struct of current RTC time.
void get_system_time_tv(struct timeval *tv);

// when local dhcp time not available can manually register a server
void sntp_register_named_server(uint8_t idx, const char *server);
// can stop the service and maybe only re-init during run time once a week.
void stop_sntp_server(void);
// use to restart the service after it was stopped or servers need to be changed.
void sntp_restart(void);
// example showing how to init for dhcp mode, but we already init this way
void sntp_example_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SNTP_EXAMPLE_H */
