/*
Cree un nuevo proyecto en el que modifique la actividad del
 punto 1 de manera de utilizar interrupciones para el control de las teclas y el control de tiempos (Timers). 


 */
/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <hc_sr04.h>
#include <lcditse0803.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <led.h>
#include <switch.h>
#include <timer_mcu.h>
#include <gpio_mcu.h>
#include <stdbool.h>
/*==================[macros and definitions]=================================*/
#define PERIODO_MEDICION 1000000
#define DISTANCIA_MENOR 10
#define DISTANCIA_MEDIA 20
#define DISTANCIA_MAXIMA 30

bool Activate=true;
bool Hold=false;

/*==================[internal data definition]===============================*/
TaskHandle_t MedirDistancia_task_handle = NULL;
TaskHandle_t LeerTeclas_task_handle = NULL;


/*==================[internal functions declaration]=========================*/
void funcTimerA(void)
{
	vTaskNotifyGiveFromISR(MedirDistancia_task_handle,pdFALSE);

}
static void MedirDistancia(void *pvParameter)
{

	uint16_t distancia=0;


	while(1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (Activate==true)
		{
				distancia = HcSr04ReadDistanceInCentimeters();

			if (distancia < DISTANCIA_MENOR)
			{
				LedsOffAll();
			}
			if (distancia > DISTANCIA_MENOR && distancia < DISTANCIA_MEDIA)
			{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}

			if (distancia > DISTANCIA_MEDIA && distancia < DISTANCIA_MAXIMA)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}
			if (distancia > DISTANCIA_MAXIMA)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
			}
			
			if (Hold == false)
			LcdItsE0803Write(distancia);
			
		}
	}
}

 void LeerTeclas(void *pvParameter)
{	
	bool *flag= (bool*) pvParameter;
	*flag=!*flag;

}		

/*==================[external functions definition]==========================*/
void app_main(void){
	
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();
	LedsOffAll();

	SwitchActivInt(SWITCH_1, LeerTeclas, &Activate);
	SwitchActivInt(SWITCH_2, LeerTeclas, &Hold);

	timer_config_t timerA = {
        .timer = TIMER_A,
        .period =PERIODO_MEDICION ,
        .func_p = funcTimerA,
        .param_p = NULL
    };	

	 TimerInit(&timerA);

	xTaskCreate(&MedirDistancia, "MedirDistancia", 2048, NULL, 5, &MedirDistancia_task_handle);
	//xTaskCreate(&LeerTeclas, "LeerTeclas", 2048, NULL, 5, &LeerTeclas_task_handle);

	TimerStart(timerA.timer);
}
/*==================[end of file]============================================*/