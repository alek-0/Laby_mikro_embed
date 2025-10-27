#include "xparameters.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdlib.h>
#include "timers.h"

SemaphoreHandle_t xSemaphore[4];

void Thread (void *param) {

	int id = (int)(intptr_i) param;

	for(;;){
		xSemaphoreTake(sem[id], portMAX_DELAY);
		
		if (tutajwarunekswitch(id)){
			xil_printf("SW%d ON\r\n",id);
			if(id < 3){
				xSemaphoreGive(sem[id+1]);
			}
		} else {
		xil_printf("SW%d OFF\r\n");
			if(id<3){
				xSemaphoreTake(sem[id+1], 0);
			}
		}
	vTaskDelay(pdMS_TO_TICKS(200));
	xSemaphoreGive(sem[id]);
	}
}

int main(void)
{

	for(i=0; i<4; i++){
		sem[i] = xSemaphoreCreateBinary();
	}

	xSemaphoreGive(sem[0]);

	for(i = 0; i<4; i++){
		char name[10];
		sprintf(name, "T%d", i);
		xTaskCreate(Thread, name, configMINIMAL_STACK_SIZE + 128, (void *)(intptr_i)i, tskIDLE_PRIORITY + 1, NULL);
	}

	vTaskStartScheduler();

    return 0;
}
