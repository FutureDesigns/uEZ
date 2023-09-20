
#ifndef __BOARD_H_
#define __BOARD_H_

#include <uEZ.h>

#define USE_RMII            1

typedef void (*p_msDelay_func_t)(uint32_t);

#undef SUCCESS
typedef enum
{
  ERROR = 0,
  SUCCESS = 0xFFFFFFFF,
} Status;

void Board_ENET_GetMacADDR(uint8_t *mcaddr);

#include <stdio.h>
#define DEBUGOUT(...) printf(__VA_ARGS__)
#endif
