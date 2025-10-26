#include "xparameters.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdlib.h>
#include "timers.h"


#define USE_MUTEX 1

#define NUM_OF_TASKS 3



SemaphoreHandle_t  xMutex = NULL;



void vPrinterTask(void *pvParameters){

	int TaskNumber = (int)(intptr_t)pvParameters;


	for(int i = 0; i < 50; i++){

#if USE_MUTEX

	if(xSemaphoreTake (xMutex, pdMS_TO_TICKS(10)) == pdTRUE){
		xil_printf("Task %d: messege %d\r\n",TaskNumber, i);
		xSemaphoreGive(xMutex);
	}
#else
	xil_printf("Task %d: messege %d\r\n",TaskNumber, i);
#endif
	vTaskDelay(pdMS_TO_TICKS(3*TaskNumber+1));
	}
	vTaskDelete(NULL);
}


int main(void)
{

	xMutex = xSemaphoreCreateMutex();
	BaseType_t rc;

	for (int i = 0; i < NUM_OF_TASKS; i++){
		rc = xTaskCreate(vPrinterTask, "PRN", configMINIMAL_STACK_SIZE + 128, (void *)(intptr_t)i, tskIDLE_PRIORITY + 1, NULL);

	if(rc != pdPASS){
		xil_printf("Failed to create task");
	}
	}

	vTaskStartScheduler();

    return 0;
}
