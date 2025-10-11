/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* Xilinx includes. */
#include "xil_printf.h"

/* Delays in milliseconds */
#define DELAY_100_MS    100UL
#define DELAY_150_MS    150UL

/* Task handles */
static TaskHandle_t xPrintOneHandle = NULL;
static TaskHandle_t xPrintTwoHandle = NULL;

/* Task prototypes */
static void print_one(void *pvParameters);
static void print_two(void *pvParameters);

int main(void)
{
    xil_printf("Hello from FreeRTOS priority demo\r\n");

    /* Create two tasks with different priorities to demonstrate preemption. */
    xTaskCreate(print_one,               /* Task function */
                "PrintOne",              /* Name (for debugging) */
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,        /* Lower priority */
                &xPrintOneHandle);

    xTaskCreate(print_two,
                "PrintTwo",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 1,    /* Higher priority -> will preempt print_one */
                &xPrintTwoHandle);

    /* Start scheduler */
    vTaskStartScheduler();

    /* Should never reach here */
    for (;;);
    return 0;
}

/*-----------------------------------------------------------*/
static void print_one(void *pvParameters)
{
    const TickType_t x100ms = pdMS_TO_TICKS(DELAY_100_MS);

    for (int j = 0; j < 5; j++) {
        /* Simulate periodic work */
        vTaskDelay(x100ms);

        /* Some CPU work: print numbers (takes time) */
        for (int i = 0; i < 100; i++) {
            xil_printf("task1 (low prio): %d\r\n", i);
        }
    }

    /* Finished -> delete self */
    xil_printf("task1 finished and deleting itself\r\n");
    vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/
static void print_two(void *pvParameters)
{
    const TickType_t x150ms = pdMS_TO_TICKS(DELAY_150_MS);

    for (int j = 0; j < 5; j++) {
        /* Simulate periodic work */
        vTaskDelay(x150ms);

        /* Some CPU work: print numbers (takes time) */
        for (int i = 0; i < 100; i++) {
            xil_printf("task2 (high prio): %d\r\n", i);
        }
    }

    /* Finished -> delete self */
    xil_printf("task2 finished and deleting itself\r\n");
    vTaskDelete(NULL);
}
