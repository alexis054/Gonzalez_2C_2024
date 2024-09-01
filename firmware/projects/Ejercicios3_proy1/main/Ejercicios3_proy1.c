/*
Realice un función que reciba un puntero a una estructura LED como la que se muestra a continuación: 
struct leds
{
      uint8_t mode;       ON, OFF, TOGGLE
	uint8_t n_led;        indica el número de led a controlar
	uint8_t n_ciclos;   indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;    indica el tiempo de cada ciclo
} my_leds; 

Use como guía para la implementación el diagrama de flujo que se	 observa en la Figura 1

Configure la función en modo Toggle, 10 ciclos, 500 milisegundos y verifique el funcionamiento utilizando el Osciloscopio. (Esquemático y PCB)
Guarde una captura de la pantalla del osciloscopio en la carpeta del ejercicio 3 de su Repositorio.

 */
/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
enum modes {ON, OFF, TOGGLE };
/*==================[macros and definitions]=================================*/
struct leds
{
    enum modes mode;       //ON, OFF, TOGGLE
	uint8_t  n_led;       // indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;   // indica el tiempo de cada ciclo
} my_leds; 

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
 void control_led(struct leds* led) 
 {
	int temp_periodo=100; //necesario para calcular el periodo
	if (led->mode == ON) 
	{
			switch (led->n_led) {
			case 1:
				LedOn(LED_1);
				break;
			case 2:
				LedOn(LED_2);
				break;
			case 3:
				LedOn(LED_3);
				break;  
  		
		  }
	} 
	
	else if (led->mode == OFF)
	{
			switch (led->n_led) 
			{
			case 1:
			LedOff(LED_1);
			break;
			case 2:
			LedOff(LED_2);
			break;
			case 3:
			LedOff(LED_3);
			break;
   		    }
	}
	
	else if (led->mode == TOGGLE) 
	{
        for (uint8_t i = 0; i < led->n_ciclos; ++i) 
		{
          switch (led->n_led) 
		  {
			case 1:
			LedToggle(LED_1);
			break;
			case 2:
			LedToggle(LED_2);
			break;
			case 3:
			LedToggle(LED_3);
			break;
		  }
        for(unsigned int i = 0; i < led->periodo/temp_periodo; i++)    
	 	vTaskDelay(temp_periodo/portTICK_PERIOD_MS);
		}
	}
}


void app_main(void){
	LedsInit();
	LedsOffAll();
	 my_leds.mode=TOGGLE;
	 my_leds.n_led=3;
	 my_leds.n_ciclos=10;
	 my_leds.periodo=500;    
	 control_led(&my_leds);
  

}



/*==================[end of file]============================================*/