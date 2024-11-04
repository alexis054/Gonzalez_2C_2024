/*! @mainpage Examen - Sensor Para Bicicletas
 *
 * @section genDesc General Description
 *
 * 	se diseña un sistema de deteccion de peligros para bicicletas
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  		|  		 ESP32   		|
 * |:----------------------:|:----------------------|
 * | 	Sensor de Distancia	| 		GPIO_3, GPIO_2	|
 * | 	Acelerometro		|		CH0, CH1, CH2	|
 * |	Buzzer				|		GPIO_20			|
 * |	Modulo Bluetooth	|		UART    		|
 * 
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 4/11/2024 | Document creation		                         |
 *
 * @author Alexis Gonzalez()
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <hc_sr04.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <led.h>
#include <timer_mcu.h>
#include <gpio_mcu.h>
#include <stdbool.h>
#include <uart_mcu.h>
#include <analog_io_mcu.h>
#include <math.h>
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/
/**
 * @def PERIODO_MEDICION
 * @brief Periodo en microsegundos en el que se mide la distancia 
 */
#define PERIODO_MEDICION 500000 
/**
 * @def DISTANCIA_MINIMA
 * @brief distancia minima en metros 
 */
#define DISTANCIA_MINIMA 3
/**
 * @def DISTANCIA_MAXIMA
 * @brief distancia maxima en metros 
 */
#define DISTANCIA_MAXIMA 5

/**
 * @def TASA_MUESTREO
 * @brief periodo en microsegundos para la medicion del acelerometro
 */
#define TASA_MUESTREO 10000

/**
 * @def TASA_MUESTREO
 * @brief Umbral de  aceleracion en volts para determinar la caida
 */
#define MAXIMA_ACELERACION 2.4

/*==================[internal data definition]===============================*/

TaskHandle_t MedirDistancia_task_handle = NULL;
TaskHandle_t ConvertirAD_task_handle = NULL;
TaskHandle_t ControlBuzzer_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
/**
 * @brief Timer que se encarga de controlar la tarea de medir la distancia.
 * 
 */
void funcTimerA(void)
{
	vTaskNotifyGiveFromISR(MedirDistancia_task_handle,pdFALSE);
}
/**
 * @brief Timer que se encarga de controlar la tarea de convertir de analogico a digital.
 * 
 */
void FuncTimerB(void* param)
{
    
    vTaskNotifyGiveFromISR(ConvertirAD_task_handle, pdFALSE);
}

/**
 * @brief Tarea encargada de medir la distancia, de encender los leds y enviar por puerto serie un mensaje
 * 
 */
static void MedirDistancia(void *pvParameter)
{

	uint16_t distancia=0;


	while(1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);	
	
			distancia = HcSr04ReadDistanceInCentimeters();

			if (distancia < DISTANCIA_MINIMA) //Se prenden los 3 leds
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);

			UartSendString(UART_CONNECTOR, (char *)"Peligro, vehículo cerca");			
			UartSendString(UART_CONNECTOR, "\r\n" );
			}
			if (distancia > DISTANCIA_MAXIMA) //se prende el led verde el resto se apagan
			{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);

			
			}

			if (distancia < DISTANCIA_MAXIMA && distancia > DISTANCIA_MINIMA) //se prende led verde y rojo
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);

			UartSendString(UART_CONNECTOR, (char *)"Precaución, vehículo cerca");			
			UartSendString(UART_CONNECTOR, "\r\n" );
			}
			
	}
	
}

/**
 * @brief Tarea encargada de obtener la informacion brindada por el acelerometro conviertiendo la senial de analogico a digital, ademas envia un mensaje
 * si se detecta una caida.
 * 
 */
static void ConvertirAD(void *param)
{
		uint16_t aceleracion_total;
        uint16_t X;
		uint16_t Y;
		uint16_t Z;
    while (1) 
	{

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		
		
        // Read the analog signal from channel CH1 and store the digital value in the 'lectura' variable.
        AnalogInputReadSingle(CH0, &X);
		AnalogInputReadSingle(CH1, &Y);
		AnalogInputReadSingle(CH2, &Z);

	aceleracion_total = sqrt(X * X +Y * Y + Z*Z);
	if(aceleracion_total>MAXIMA_ACELERACION)
	{
		UartSendString(UART_CONNECTOR, (char *)"Caída detectada");			
		UartSendString(UART_CONNECTOR, "\r\n" );
	} 
        
    }
}
/**
 * @brief Funcion encargada de controlar el sonido del buzzer.
 * 
 */
static void ControlBuzzer()
{
	GPIOInit(GPIO_20, 1);
	uint16_t distancia=0;
	
	while (1)
	{
    distancia = HcSr04ReadDistanceInCentimeters();
	if(distancia <DISTANCIA_MINIMA)
	{
	GPIOToggle(GPIO_20);	
	vTaskDelay(500/portTICK_PERIOD_MS);
	}
	else
	{
		if (distancia < DISTANCIA_MAXIMA && distancia > DISTANCIA_MINIMA)
		{
			GPIOToggle(GPIO_20);	
			vTaskDelay(250/portTICK_PERIOD_MS);
			
		}
		else 
	{
		GPIOOff(GPIO_20);
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
	
	}
	
	}

}


/*==================[external functions definition]==========================*/
void app_main(void)
{


LedsInit();
HcSr04Init(GPIO_3, GPIO_2);
LedsOffAll(); 



timer_config_t timerA = {
        .timer = TIMER_A,
        .period =PERIODO_MEDICION ,
        .func_p = funcTimerA,
        .param_p = NULL
    };

timer_config_t timerB = {
        .timer = TIMER_B,
        .period =TASA_MUESTREO,
        .func_p = FuncTimerB,
        .param_p = NULL
    };

TimerInit(&timerA);
TimerInit(&timerB);

xTaskCreate(&MedirDistancia, "MedirDistancia", 2048, NULL, 5, &MedirDistancia_task_handle);
xTaskCreate(&ConvertirAD, "ConvertirAD", 2048, NULL, 5, &ConvertirAD_task_handle);
xTaskCreate(&ControlBuzzer, "ControlBuzzer", 2048, NULL, 5, &ControlBuzzer_task_handle);
TimerStart(timerA.timer);
TimerStart(timerA.timer);
ControlBuzzer(); 

}
/*==================[end of file]============================================*/