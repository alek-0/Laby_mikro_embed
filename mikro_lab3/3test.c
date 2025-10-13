#include "xparameters.h"
#include "xil_printf.h"
#include "xtmrctr.h"
#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <limits.h>

#define N_OF_TASKS 3
#define M_VALUE 1000000
#define Q_VALUE (3.14159265358979323846 / 2.0)

static TaskHandle_t xOneTaskHandle = NULL;
static TaskHandle_t xCountingTask = NULL;

XTmrCtr globTimerInstance;

typedef struct {
    double result;
    int startIndex;
    int endIndex;
} ParamStr;

ParamStr taskData[N_OF_TASKS];


// Print double with 8 digits precision
void printDouble8(double fval);



double CalculateRange(int start_count, int end_count) {
    double sum = 0.0;
    for (int i = start_count; i <= end_count; i++) {
        sum += (sin(i * Q_VALUE)/i);
    }
    return sum;
}

void CountingTask(void *pvParameters){


	ParamStr *params = (ParamStr *)pvParameters;
	xil_printf("task startIndex: %d\r\n", params->startIndex);
	xil_printf("task endIndex: %d\r\n", params->endIndex);
	int a = params->startIndex;
	int b = params->endIndex;
	params->result = 0.0;
	params->result = CalculateRange(a,b);
	xil_printf("task result: ");
	printDouble8(params->result);
	xil_printf("\r\n");
	vTaskDelete(NULL);
}


void FreeRTOS_Calc(void *pvParameters) {
portTASK_USES_FLOATING_POINT();

int n = *(int *)pvParameters;

xil_printf("odczytany n: %d\r\n", n);

for(int i=0; i<n; i++) {
	taskData[i].result = 0.0;
	taskData[i].startIndex = i * M_VALUE/n + 1;
	xil_printf("startIndex: %d\r\n", taskData[i].startIndex);
	taskData[i].endIndex = (i+1) * M_VALUE/n;
	xil_printf("endtIndex: %d\r\n", taskData[i].endIndex);
}


for (int i=1; i<=n ; i++){
	xTaskCreate(CountingTask,
	    			"counting Task",
					configMINIMAL_STACK_SIZE * 4,
					(void *)&taskData[i-1],
					tskIDLE_PRIORITY + 1,
					&xCountingTask);
	xil_printf("task utworzony?\r\n");
}

vTaskDelete(NULL);
}




int main(void)
{
    xil_printf("RTOS task comparison\r\n");

    //init timera
    int status = XTmrCtr_Initialize(&globTimerInstance, 0);
    if (status != XST_SUCCESS) {
            xil_printf("Timer initialization failed!\r\n");
            return -1;
        }
    XTmrCtr_SetOptions(&globTimerInstance, 0, XTC_AUTO_RELOAD_OPTION);

    XTmrCtr_Reset(&globTimerInstance, 0);
    XTmrCtr_Start(&globTimerInstance, 0);

    double wynik_0 = CalculateRange(1, M_VALUE);
    u32 t1 = XTmrCtr_GetValue(&globTimerInstance, 0);

    double seconds_0 = (double)t1 / (double)XPAR_TMRCTR_0_CLOCK_FREQ_HZ;

    xil_printf("Wynik obliczen bez taskow:");
    printDouble8(wynik_0);
    xil_printf("  Czas:");
    printDouble8(seconds_0);
    xil_printf("\r\n");


    // test
    double wynik_1 = CalculateRange(1, 333333);
    printDouble8(wynik_1);
    xil_printf("\r\n");
    double wynik_2 = CalculateRange(333334, 666666);
    printDouble8(wynik_2);
    xil_printf("\r\n");
    double wynik_3 = CalculateRange(666667, 1000000);
    printDouble8(wynik_3);
    xil_printf("\r\n");
    double wynik_4 = wynik_1+wynik_2+wynik_3;
    printDouble8(wynik_4);
    xil_printf("\r\n");

    xil_printf("Starting FreeRTOS scheduler\r\n");

    int number_of_tasks = 3;

    xTaskCreate(FreeRTOS_Calc,
    			"MainCalc",
				configMINIMAL_STACK_SIZE * 4,
				(void *)&number_of_tasks,
				tskIDLE_PRIORITY + 4,
				&xOneTaskHandle);
    vTaskStartScheduler();


    for (;;);
    return 0;
}


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


