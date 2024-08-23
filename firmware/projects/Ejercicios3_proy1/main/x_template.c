/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
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
            
	 	vTaskDelay(led->periodo / portTICK_PERIOD_MS);
        
		}
	}
}

/**
 * @brief Converts a decimal number to a BCD array.
 *
 * This function takes a decimal number, the number of digits, and a pointer to an array.
 * It converts the decimal number to a BCD array and stores it in the provided array.
 *
 * @param data The decimal number to be converted.
 * @param digits The number of digits in the decimal number.
 * @param bcd_number A pointer to the array where the BCD array will be stored.
 *
 * @return Returns 0 if the conversion is successful.
 */
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t * bcd_number) {
 
    for (int8_t i = digits - 1; i >= 0; i--) {
        bcd_number[i] = (data % 10); 
        data /= 10;
    }
 while(n>2)
 {
    
 }

    return 0;
}

void app_main(void){
/*	LedsInit();
	LedsOffAll();
	 my_leds.mode=TOGGLE;
	 my_leds.n_led=3;
	 my_leds.n_ciclos=1000;
	 my_leds.periodo=100;     
		 control_led(&my_leds);*/



		 uint8_t bcd_number[4];
		uint32_t data=12345;
        convertToBcdArray(data, 4, &bcd_number);
          printf("Muestro por consola: ");
	printf("%d",bcd_number[0]);
	printf("%d",bcd_number[1]);
	printf("%d",bcd_number[2]);
	printf("\n");        

   

}



/*==================[end of file]============================================*/