/**
 * @file main.c
 * @brief Cree un nuevo proyecto en el que modifique la actividad del punto 1 de manera de utilizar interrupciones para el control de las teclas y el control de tiempos (Timers).
 * @author <NAME>
 * @date 2023-01-01
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
#include <uart_mcu.h>
/*==================[macros and definitions]=================================*/
#define PERIODO_MEDICION 1000000
#define DISTANCIA_MENOR 10
#define DISTANCIA_MEDIA 20
#define DISTANCIA_MAXIMA 30


/*==================[internal data definition]===============================*/
TaskHandle_t MedirDistancia_task_handle = NULL;
bool Activate=true;
bool Hold=false;
uint16_t distancia = 0;
uint16_t distancia_hold = 0;
/*==================[internal functions declaration]=========================*/
/**
 * @brief Función de interrupción para el timer A.
 * @details Notifica a la tarea de medición de distancia.
 */
void funcTimerA(void)
{
	vTaskNotifyGiveFromISR(MedirDistancia_task_handle,pdFALSE);
}

/**
 * @brief Tarea para medir la distancia y controlar los LEDs.
 * @param pvParameter Parámetro de entrada de la tarea.
 */
static void MedirDistancia(void *pvParameter)
{
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
			{
			LcdItsE0803Write(distancia);
			UartSendString(UART_PC, (char *)UartItoa(distancia, 10));
			UartSendString(UART_PC, " cm ");
			UartSendString(UART_PC, "\r\n" );
			distancia_hold = distancia;
			}
			else 
			{
				UartSendString(UART_PC, (char *)UartItoa(distancia_hold, 10));
				UartSendString(UART_PC, " cm ");
				UartSendString(UART_PC, "\r\n" );
			}

			
		}
	}
}

/**
 * @brief Función para leer las teclas y cambiar el estado de las variables de control.
 * @param pvParameter Parámetro de entrada de la función.
 */

 void LeerTeclas(void *pvParameter)
{	
	bool *flag= (bool*) pvParameter;
	*flag=!*flag;

}	
/**
 * @brief Función para leer el puerto serie y cambiar el estado de las variables de control.
 * @param pvParameter Parámetro de entrada de la función.
 */	
void LeerPuertoSerie (void *pvParameter){

	uint8_t lectura; 
	UartReadByte(UART_PC, &lectura);

	if (lectura == 'O'){
		Activate = !Activate;
		Hold = false;
	}

	else if (lectura == 'H'){
		Hold= !Hold;
	}

}
 

/*==================[external functions definition]==========================*/
/**
 * @brief Función principal del programa.
 * @details Inicializa los periféricos, crea las tareas y configura los timers.
 */
void app_main(void){
	
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();
	LedsOffAll();

	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = LeerPuertoSerie,
		.param_p = NULL,
	};
	UartInit(&serial_pc);

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