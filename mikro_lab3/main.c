#include "xparameters.h"
#include "xil_printf.h"
#include "xtmrctr.h"
#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <limits.h>

#define M_VALUE 1000000
#define Q_VALUE (3.14159265358979323846 / 2.0)

static TaskHandle_t xOneTaskHandle = NULL;



XTmrCtr globTimerInstance;

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



double CalculateRange(int start_count) {
    double sum = 0.0;
    for (int i = start_count; i <= M_VALUE; i++) {
        sum += (sin(i * Q_VALUE)/i);
    }
    return sum;
}

void FreeRTOS_Calc(void *pvParameters) {
portTASK_USES_FLOATING_POINT();

int param = *(int *)pvParameters;

xil_printf("param = %d\r\n", param);


for (int i=1; i<=param ; i++){
	xTaskCreate(test,
	    			"MainCalc",
					configMINIMAL_STACK_SIZE * 4,
					(void *)&param,
					tskIDLE_PRIORITY + 1,
					&xOneTaskHandle);
	
}




for(;;);
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

    double wynik_0 = CalculateRange(1);
    u32 t1 = XTmrCtr_GetValue(&globTimerInstance, 0);

    double seconds_0 = (double)t1 / (double)XPAR_TMRCTR_0_CLOCK_FREQ_HZ;

    xil_printf("Wynik obliczen bez taskow:");
    printDouble8(wynik_0);
    xil_printf("  Czas:");
    printDouble8(seconds_0);
    xil_printf("\r\n");

    //ile taskow
    int param = 1;

    xil_printf("Starting FreeRTOS scheduler\r\n");
    xTaskCreate(FreeRTOS_Calc,
    			"MainCalc",
				configMINIMAL_STACK_SIZE * 4,
				(void *)&param,
				tskIDLE_PRIORITY + 2,
				&xOneTaskHandle);
    vTaskStartScheduler();


    for (;;);
    return 0;
}



