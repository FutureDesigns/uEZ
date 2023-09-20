
#if (ENABLE_FREERTOS_DUBUG_FUNCS == 1) // if We can ever compile back in debug freertos functions successfully we can use this somewhere to print out task information.
#define ENABLE_FREERTOS_DUBUG_FUNCS		1
#define ENABLE_FREERTOS_RUN_STATS		0

#include "FreeRTOS.h"
#include "task.h"
int TaskInfo_Cmd(void) {
#ifdef FREERTOS_PLUS_TRACE
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x;
	uint32_t ulTotalRunTime;

	// Take a snapshot of the number of tasks in case it changes while this function is executing.
	uxArraySize = uxTaskGetNumberOfTasks();

	// Allocate a TaskStatus_t structure for each task. An array could be allocated statically at compile time.
	pxTaskStatusArray = (TaskStatus_t *) pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

	if( pxTaskStatusArray != NULL ) {
		// Generate raw status information about each task.
		uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

#if (ENABLE_FREERTOS_RUN_STATS == 1)
		uint32_t  ulStatsAsPercentage; // Not reporting runtime currently
		ulTotalRunTime /= 100UL; // For percentage calculations.
#endif

		for( x = 0; x < uxArraySize; x++ ) { // For each populated position in the pxTaskStatusArray array
			printf("%10s: ", pxTaskStatusArray[x].pcTaskName);

			switch(pxTaskStatusArray[x].eCurrentState){
			case 0:
				printf("RUN");
				break;
			case 1:
				printf("RDY");
				break;
			case 2:
				printf("BLK");
				break;
			case 3:
				printf("SUS");
				break;
			case 4:
				printf("DEL");
				break;
			case 5:
			default:
				printf("INV");
				break;
			}
			printf(", Prio: %02u, Base: %02u, ", pxTaskStatusArray[x].uxCurrentPriority, pxTaskStatusArray[x].uxBasePriority);
			printf("Free Stack Remaining: %04u\n", pxTaskStatusArray[x].usStackHighWaterMark);

#if (ENABLE_FREERTOS_RUN_STATS == 1)
			if( ulTotalRunTime > 0 ) {
				// What percentage of the total run time has the task used? This will always be rounded down to the nearest integer.
				// ulTotalRunTimeDiv100 has already been divided by 100.
				ulStatsAsPercentage = pxTaskStatusArray[x].ulRunTimeCounter / ulTotalRunTime;

				if( ulStatsAsPercentage > 0UL )	{
					printf("%s\t\t%lu\t\t%lu%%\r\n", pxTaskStatusArray[x].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter, ulStatsAsPercentage );
				} else {
					// If the percentage is zero here then the task has consumed less than 1% of the total run time.
					printf("%s\t\t%lu\t\t<1%%\r\n", pxTaskStatusArray[x].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter );
				}
			}
#endif
		}
		vPortFree( pxTaskStatusArray ); // The array is no longer needed, free the memory it consumes.
		printf("PASS: OK");
	} else {
		printf("FAIL: Mem Alloc\n");
	}
#endif // #ifdef FREERTOS_PLUS_TRACE
    return 0;
}
#endif
