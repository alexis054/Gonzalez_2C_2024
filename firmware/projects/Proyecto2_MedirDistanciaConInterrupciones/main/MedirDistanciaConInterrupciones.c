/**
 * @file MedirDistanciaConInterrupciones.c
 * @brief This file contains a program that measures distance using interruptions.
 *
 * This program initializes peripherals, such as sensors, LEDs, switches, and a timer, to measure distance using an ultrasonic sensor (HC-SR04) and display it on an LCD screen.
 * It also controls LEDs based on the measured distance and allows for activation and holding of the distance measurement process through switches.
 *
 * @author Gonzalez Alexis
 * @date 10-04-2022
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
/*==================[internal functions declaration]=========================*/
/**
 * @brief Timer callback function to give a task notification.
 */
void funcTimerA(void)
{
	vTaskNotifyGiveFromISR(MedirDistancia_task_handle,pdFALSE);

}
/**
 * @brief Task to measure distance using the HC-SR04 sensor and control LEDs based on the distance.
 *
 * @param pvParameter Pointer to task parameters
 */
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
/**
 * @brief Task to toggle a boolean flag passed as a parameter.
 *
 * @param pvParameter Pointer to a boolean flag to toggle.
 */

 void LeerTeclas(void *pvParameter)
{	
	bool *flag= (bool*) pvParameter;
	*flag=!*flag;

}		

/*==================[external functions definition]==========================*/
/**
 * @brief Main function that initializes peripherals and creates tasks for distance measurement.
 */
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
	

	TimerStart(timerA.timer);
}
/*==================[end of file]============================================*/