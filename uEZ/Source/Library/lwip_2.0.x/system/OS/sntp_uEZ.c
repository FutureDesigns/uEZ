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
#if defined ( __ICCARM__ )
  #pragma diag_suppress=Pe223
#endif

#include <time.h>
#include <sys/time.h> // uez has own timeval definition

#include "lwip/opt.h"
#include "lwip/apps/sntp.h"
#include "arch/sntp_uEZ.h"
#include "lwip/netif.h"
#include <uEZTimeDate.h>

void sntp_set_system_time(uint32_t sec)
{
#ifdef LWIP_DEBUG
  char buf[32];
#endif
  struct tm current_time_val;
  time_t current_time = (time_t)sec;
  T_uezTimeDate td;

  // convert seconds to tm struct. Typical MCU RTCs don't support microsecond time setting in RTC.
#if defined(_WIN32) || defined(WIN32) || defined ( __ICCARM__ )
  localtime_s(&current_time_val, &current_time);
#else
  localtime_r(&current_time, &current_time_val);
#endif

#ifdef LWIP_DEBUG
  strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &current_time_val);
  LWIP_PLATFORM_DIAG(("SNTP time: %s\n", buf));  
#endif
  
  td.iDate.iDay = current_time_val.tm_mday;
  td.iDate.iMonth = current_time_val.tm_mon+1; // start from 1
  td.iDate.iYear = current_time_val.tm_year; // since 1900

  td.iTime.iHour = current_time_val.tm_hour;
  td.iTime.iMinute = current_time_val.tm_min;
  td.iTime.iSecond = current_time_val.tm_sec;
  UEZTimeDateSet(&td);
}

void sntp_set_system_time_ntp(uint32_t sec, uint32_t frac)
{
  //sntp_set_system_time(sec);
  // set fraction in rtc
  (void) sec;
  (void) frac;
}

void get_system_time_tv(struct timeval *tv)
{  
  struct tm current_time_val;
  T_uezTimeDate td;

  UEZTimeDateGet(&td);

  current_time_val.tm_mday = td.iDate.iDay;
  current_time_val.tm_mon = td.iDate.iMonth-1; // start from 0
  current_time_val.tm_year= td.iDate.iYear; // since 1900

  current_time_val.tm_hour = td.iTime.iHour;
  current_time_val.tm_min = td.iTime.iMinute;
  current_time_val.tm_sec = td.iTime.iSecond;

  time_t current_time = mktime(&current_time_val);
  
  tv->tv_sec = current_time;
  tv->tv_usec = 0;
}

void sntp_example_init(void)
{
  LOCK_TCPIP_CORE();
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
#if LWIP_DHCP
  sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
#else /* LWIP_DHCP */
#if LWIP_IPV4
  sntp_setserver(0, netif_ip_gw4(netif_default));
#endif /* LWIP_IPV4 */
#endif /* LWIP_DHCP */
  sntp_init();
  UNLOCK_TCPIP_CORE();
}

void sntp_restart(void)
{
  LOCK_TCPIP_CORE();
  sntp_stop();
  sntp_init();
  UNLOCK_TCPIP_CORE();
}

void stop_sntp_server(void)
{
  LOCK_TCPIP_CORE();
  sntp_stop();
  UNLOCK_TCPIP_CORE();
}

void sntp_register_named_server(uint8_t idx, const char *server)
{
  LOCK_TCPIP_CORE();
  sntp_setservername(idx, server);
  UNLOCK_TCPIP_CORE();
}

#if defined ( __ICCARM__ )
#pragma diag_default=Pe223
#endif
