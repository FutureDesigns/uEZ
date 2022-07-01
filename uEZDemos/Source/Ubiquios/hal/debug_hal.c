/*
 * ubiquiOS porting layer for STM32 Nucleo: Debug Logging
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#include "ubiquios.h"
#include <uEZ.h>
#include <uEZStream.h>
#include <Source\Library\StreamIO\StdInOut\StdInOut.h>


static T_uezDevice G_StreamDevice = 0;
static TUInt32 initialised = EFalse;

static void
debug_hal_init(void)
{
    G_StreamDevice = StdinGet();
}


/**
 * Put character method for our debug serial port
 */
static void
debug_putc(ub_opaque_arg_t opaque_arg, char ch)
{
    TUInt32 bytesSent;

    if(!initialised){
        initialised = ETrue;
        debug_hal_init();
    }

    if(G_StreamDevice){
        UEZStreamWrite(G_StreamDevice, (void*)&ch, 1, &bytesSent, 100);
        if(ch =='\n'){
            ch = '\r';
            UEZStreamWrite(G_StreamDevice, (void*)&ch, 1, &bytesSent, 100);
        }
    }
}


/**
 * Flush method for our debug serial port. Busy loop until all
 * characters have been sent from our ring buffer.
 */
static void
debug_fflush(ub_opaque_arg_t opaque_arg)
{
    if(!initialised){
        initialised = ETrue;
        debug_hal_init();
    }

    if(G_StreamDevice){
        UEZStreamFlush(G_StreamDevice);
    }
}


/**
 * Declare a ustdio file descriptor for our serial port so that we can
 * override \c ustdout and \c ustderr below.
 */
//static ub_ustdio_file_t debug_serial_file =
//{
//    debug_putc, debug_fflush,
//    { .p = NULL }
//};


/**
 * ubiquiOS has weak definitions of \c ustdout and \c ustderr which
 * redirect output to a black hole. We override these here to cause
 * that same output to come out a serial port using the debug HAL
 * we've just implemented.
 */
//ub_ustdio_file_t *ustdout = &debug_serial_file;
//ub_ustdio_file_t *ustderr = &debug_serial_file;
