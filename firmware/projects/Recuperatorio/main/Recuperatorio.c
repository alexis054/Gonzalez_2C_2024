/*! @mainpage Sistema de pesaje para camiones
 * @section genDesc General Description
 *
 * Este codigo es el encargado de manejar un sistema de pesaje, para esto mide la velocidad y distancia del vehiculo con la ayuda del sensor
 * de distancia HC_SR04, una vez que el vehiculo se encuentra detenido, realiza la medicion del peso como un promedio de 100 valores
 * (50 para cada galga) y envia por UART el peso promedio y la velocidad maxima, a su vez cuenta con la funcionalidad de abrir una barrera
 * con la tecla 'o', o cerrarla con la tecla 'c'
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	HC_SR04	 	| GPIO_3, GPIO2 |
 * | 	Galga1	 	| CH1		    |
 * | 	Galga2	 	| CH2		    |
 * | 	Barrera	 	| GPIO_20	    |
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/11/2024 | Document creation		                         |
 *
 * @author Gonzalez Alexis 
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <hc_sr04.h>
#include <led.h>
#include <uart_mcu.h>	
#include <gpio_mcu.h>
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <analog_io_mcu.h>

/*==================[macros and definitions]=================================*/

/**
 * @def PERIODO_MEDICION_DISTANCIA
 * @brief Periodo en microsegundos en el que se mide la distancia
 */
#define PERIODO_MEDICION_DISTANCIA 10000 //10 muestras por segundo
/**
 * @def DISTANCIA_MINIMA
 * @brief distancia en metros a la que se comenzara a medir la velocidad
 */
#define DISTANCIA_MINIMA 10 
 /**
 * @def PERIODO_MEDICION_PESO
 * @brief periodo en microsegundos en el que se toman las mediciones de peso
 */
#define PERIODO_MEDICION_PESO 500 //200 Muestras por segundo 
 /**
 * @def VELOCIDAD_ADVERTENCIA
 * @brief Velocidad en m/s a la que se prende el led de advertencia
 */
#define VELOCIDAD_ADVERTENCIA 8 
 /**
 * @def VEHICULO_DETENIDO
 * @brief Velocidad en m/s a la que el vehiculo se encuentra detenido
 */
#define VEHICULO_DETENIDO 0  
/**
 * @def VOLTAJE_MAXIMO
 * @brief Voltaje maximo del circuito de acondicionamiento
 */
#define VOLTAJE_MAXIMO 3.3 
/**
 * @def PESO_MAXIMO
 * @brief Peso maximo en kilogramos del circuito de acondicionamiento
 */
#define PESO_MAXIMO 20000
/**
 * @def CANTIDAD_MEDICIONES
 * @brief Cantidad de mediciones que se realizan para promediar el peso
 */
#define CANTIDAD_MEDICIONES 50 
/**
 * @def CANTIDAD_GALGAS
 * @brief Cantidad de galgas que toman medidas de peso
 */
#define CANTIDAD_GALGAS 2 


/*==================[internal data definition]===============================*/

TaskHandle_t MedirDistancia_task_handle = NULL;
TaskHandle_t MedirPeso_task_handle = NULL;
bool vehiculo_detenido = false;
int velocidad_max = 0;


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
 * @brief Timer que se encarga de controlar la tarea de medir el peso.
 * 
 */
void funcTimerB(void)
{
	vTaskNotifyGiveFromISR(MedirPeso_task_handle,pdFALSE);

}
/**
 * @brief Tarea encargada de medir la distancia, calcular la velocidad y de encender los leds 
 * 
 */
static void MedirDistancia(void *pvParameter)
{
	unsigned int distancia_inicial = 0;
	unsigned int distancia_final = 0;
	int velocidad=0;
	
	while(1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);	
		distancia_inicial=distancia_final;
		distancia_final = HcSr04ReadDistanceInCentimeters();

		if(distancia_final<DISTANCIA_MINIMA)
		{
		velocidad=(distancia_inicial - distancia_final)/PERIODO_MEDICION_DISTANCIA;
		}
		if(velocidad>velocidad_max)
		{
		velocidad_max=velocidad;
		}
		if(velocidad>VELOCIDAD_ADVERTENCIA)
		{
			LedOff(LED_1);
			LedOff(LED_2);
			LedOn(LED_3);

			vehiculo_detenido = false;
		}

		if(velocidad>0 && velocidad<VELOCIDAD_ADVERTENCIA)
		{
			LedOff(LED_1);
			LedOn(LED_2);
			LedOff(LED_3);

			vehiculo_detenido = false;
		}

		if(velocidad==VEHICULO_DETENIDO)
		{
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);

			vehiculo_detenido = true;

		}	
		
	}			
}

/**
 * @brief Tarea encargada de medir el peso y enviar por puerto serie el peso promedio y la velocidad maxima 
 * 
 */
static void MedirPeso(void *pvParameter)
{
		uint16_t lectura1=0;
		uint16_t lectura2=0;
		int mediciones[CANTIDAD_MEDICIONES][CANTIDAD_GALGAS];
		unsigned int contador = 0;
		unsigned int lecturakg1=0;
		unsigned int lecturakg2=0;
		unsigned int acum=0;
		unsigned int peso_promedio=0;
 while (1) {
        // Wait for the notification from the FuncTimerA ISR to perform the task.
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        
        // Read the analog signal from channel CH1 and store the digital value in the 'lectura' variable.
		if(vehiculo_detenido == true) 
		{
        AnalogInputReadSingle(CH1, &lectura1);
		AnalogInputReadSingle(CH2, &lectura2);
		//paso de voltaje a peso el valor de lectura
		lecturakg1=lectura1*(PESO_MAXIMO/VOLTAJE_MAXIMO) ;
		lecturakg2=lectura2*(PESO_MAXIMO/VOLTAJE_MAXIMO) ;

		//almaceno este valor en un vector
		mediciones[contador][0]=lecturakg1;
		mediciones[contador][1]=lecturakg2;

		if(contador == 50) 
		{	
			for(unsigned int i=0; i<CANTIDAD_MEDICIONES; ++i)
			{
				for(unsigned int j=0; i<CANTIDAD_GALGAS; ++j)
				acum=mediciones[i][j];
			}
			peso_promedio=acum/(CANTIDAD_MEDICIONES*CANTIDAD_GALGAS);
        // Convert the digital value to a string and send it to the PC via UART.
		UartSendString(UART_PC, "Peso:");
        UartSendString(UART_PC, (char *)UartItoa(peso_promedio, 10));
        UartSendString(UART_PC, "\r");
		UartSendString(UART_PC, "“Velocidad máxima:");
        UartSendString(UART_PC, (char *)UartItoa(velocidad_max, 10));
        UartSendString(UART_PC, "\r");
		acum=0;
		contador=0;
		}

		else contador++;

 		}
 	}
}

/**
 * @brief Función para leer el puerto serie y cambiar el estado de las variables de control.
 * @param pvParameter Parámetro de entrada de la función.
 */	
void LeerPuertoSerie (void *pvParameter)
{
	uint8_t lectura; 
	UartReadByte(UART_PC, &lectura);

	if (lectura == 'o')
	{
		GPIOOn(GPIO_20);
	}

	else if (lectura == 'c')
	{
		GPIOOff(GPIO_20);
	}

}
/*==================[external functions definition]==========================*/
void app_main(void)
{
	//inicializo 
	LedsInit();
	HcSr04Init(GPIO_3, GPIO_2);
	GPIOInit(GPIO_20, 1);

	serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = LeerPuertoSerie,
		.param_p = NULL,
	};

	UartInit(&serial_pc);

	timer_config_t timerA = {
        .timer = TIMER_A,
        .period =PERIODO_MEDICION_DISTANCIA ,
        .func_p = funcTimerA,
        .param_p = NULL
    };

	timer_config_t timerB = {
        .timer = TIMER_A,
        .period =PERIODO_MEDICION_PESO ,
        .func_p = funcTimerB,
        .param_p = NULL
    };		

	 TimerInit(&timerA);
	 TimerInit(&timerB);
	xTaskCreate(&MedirDistancia, "MedirDistancia", 2048, NULL, 5, &MedirDistancia_task_handle);	
	xTaskCreate(&MedirPeso, "MedirPeso", 2048, NULL, 5, &MedirPeso_task_handle);	
	TimerStart(timerA.timer);
	TimerStart(timerB.timer);
}


/*==================[end of file]============================================*/