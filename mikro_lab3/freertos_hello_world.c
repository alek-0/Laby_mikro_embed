#include "xparameters.h"
#include "xil_printf.h"
#include "xtmrctr.h"
#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <limits.h>

#define NO_OF_TASKS 3             // 0 = no RTOS, >0 = FreeRTOS mode
#define TIMER_DEVICE_ID XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_COUNTER_0 0
#define M_VALUE 1000000
#define Q_VALUE (3.14159265358979323846 / 2.0)

XTmrCtr globTimerInstance;
TaskHandle_t mainTaskHandle = NULL;

typedef struct {
    int startIdx;
    int endIdx;
    double result;
} CalcData;

// ------------------------------------------------------------------
// Print double with 8 digits precision
void printDouble8(double fval) {
    double fval2 = fval;
    int integerPart, fractionalPart;
    if (fval2 > INT32_MAX || fval2 <= INT32_MIN) {
        xil_printf("XXX.XXXXXXXX");
        return;
    }
    int sign = 0;
    if (fval2 < 0) sign = 1;
    fval2 = fabs(fval2);
    integerPart = (int)fval2;
    fractionalPart = (int)((fval2 - integerPart) * 100000000);
    if (sign) xil_printf("-");
    xil_printf("%d.%08d", integerPart, fractionalPart);
}

// ------------------------------------------------------------------
// Actual calculation
void CalculateRange(CalcData *data) {
    double sum = 0.0;
    for (int i = data->startIdx; i < data->endIdx; i++) {
        double x = (double)i / (double)M_VALUE;
        sum += sin(Q_VALUE * x);
    }
    data->result = sum;
}

// ------------------------------------------------------------------
// Worker task for FreeRTOS
void WorkerTask(void *pvParameters) {
    portTASK_USES_FLOATING_POINT(); // enable FPU in task

    CalcData *calc = (CalcData *)pvParameters;
    CalculateRange(calc);

    // Notify main task
    xTaskNotifyGive(mainTaskHandle);
    vTaskDelete(NULL);
}

// ------------------------------------------------------------------
// Main RTOS calculation task
void FreeRTOS_Calc(void *pvParameters) {
    portTASK_USES_FLOATING_POINT(); // enable FPU in main RTOS task

    xil_printf("Starting FreeRTOS calculation with %d tasks...\r\n", NO_OF_TASKS);

    XTmrCtr_Reset(&globTimerInstance, TIMER_COUNTER_0);
    XTmrCtr_Start(&globTimerInstance, TIMER_COUNTER_0);

    CalcData calcData[NO_OF_TASKS];
    int step = M_VALUE / NO_OF_TASKS;

    for (int i = 0; i < NO_OF_TASKS; i++) {
        calcData[i].startIdx = i * step;
        calcData[i].endIdx = (i == NO_OF_TASKS - 1) ? M_VALUE : (i + 1) * step;
        calcData[i].result = 0.0;

        xTaskCreate(WorkerTask, "Worker", configMINIMAL_STACK_SIZE * 2,
                    &calcData[i], tskIDLE_PRIORITY + 1, NULL);
    }

    // Wait for all tasks to complete
    for (int i = 0; i < NO_OF_TASKS; i++) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    XTmrCtr_Stop(&globTimerInstance, TIMER_COUNTER_0);
    u32 t_end = XTmrCtr_GetValue(&globTimerInstance, TIMER_COUNTER_0);
    double seconds = (double)t_end / (double)XPAR_TMRCTR_0_CLOCK_FREQ_HZ;

    double total = 0.0;
    for (int i = 0; i < NO_OF_TASKS; i++) total += calcData[i].result;

    xil_printf("Result = ");
    printDouble8(total);
    xil_printf(", time = ");
    printDouble8(seconds);
    xil_printf(" s\r\n");

    vTaskDelete(NULL);
}

// ------------------------------------------------------------------
int main(void) {
    xil_printf("=== Lab: Comparison of RTOS vs non-RTOS calculation ===\r\n");

    int status = XTmrCtr_Initialize(&globTimerInstance, TIMER_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("Timer initialization failed!\r\n");
        return -1;
    }
    XTmrCtr_SetOptions(&globTimerInstance, TIMER_COUNTER_0, XTC_AUTO_RELOAD_OPTION);

#if NO_OF_TASKS == 0
    xil_printf("Starting calculation without FreeRTOS...\r\n");

    CalcData single;
    single.startIdx = 0;
    single.endIdx = M_VALUE;
    single.result = 0.0;

    XTmrCtr_Reset(&globTimerInstance, TIMER_COUNTER_0);
    XTmrCtr_Start(&globTimerInstance, TIMER_COUNTER_0);

    CalculateRange(&single);

    XTmrCtr_Stop(&globTimerInstance, TIMER_COUNTER_0);
    u32 t_end = XTmrCtr_GetValue(&globTimerInstance, TIMER_COUNTER_0);
    double seconds = (double)t_end / (double)XPAR_TMRCTR_0_CLOCK_FREQ_HZ;

    xil_printf("Result = ");
    printDouble8(single.result);
    xil_printf(", time = ");
    printDouble8(seconds);
    xil_printf(" s\r\n");

#else
    xil_printf("Starting FreeRTOS scheduler...\r\n");
    xTaskCreate(FreeRTOS_Calc, "MainCalc", configMINIMAL_STACK_SIZE * 4, NULL,
                tskIDLE_PRIORITY + 2, &mainTaskHandle);
    vTaskStartScheduler();
#endif

    for (;;);
    return 0;
}
