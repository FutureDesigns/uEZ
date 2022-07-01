//#include "board.h"
#include <string.h>
#include <UEZ.h>

extern void UEZBSPDelayMS(uint32_t aMilliseconds);
extern TUInt32 UEZTickCounterGet(void);
/* Returns the MAC address assigned to this board */
void Board_ENET_GetMacADDR(uint8_t *mcaddr)
{
	uint8_t boardmac[] = {0x00, 0x60, 0x37, 0x12, 0x34, 0x56};

	memcpy(mcaddr, boardmac, 6);
}

void msDelay(uint32_t ms)
{
    UEZBSPDelayMS(ms);
}

#if 0
uint32_t sys_now(void)
{
    return UEZTickCounterGet();
}
#endif
