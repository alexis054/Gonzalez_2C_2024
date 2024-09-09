/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <hc_sr04.h>
#include <lcditse0803.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <led.h>
#include <switch.h>

/*==================[macros and definitions]=================================*/
#define DISTANCIA_MENOR 10
#define DISTANCIA_MEDIA 20
#define DISTANCIA_MAXIMA 30
bool Activate=false;
bool Hold=false;
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
static void MedirDistancia(void *pvParameter)
{

	uint16_t distancia=0;


	while(1)
	{
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
	vTaskDelay(1000 / portTICK_PERIOD_MS); 
	}
}

static void LeerTeclas(void *pvParameter)
{
	uint8_t teclas;
	
	while(1)    
	{
    	teclas  = SwitchesRead();
    	switch(teclas)
		{
    		case SWITCH_1:

    			Activate=!Activate;
				
    		break;
    		case SWITCH_2:
    			
				Hold=!Hold;
				
    		break;
			
    	}
	vTaskDelay(300 / portTICK_PERIOD_MS);
	
	}
}		

/*==================[external functions definition]==========================*/
void app_main(void){
	
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();
	LedsOffAll();

	xTaskCreate(&MedirDistancia, "MedirDistancia", 2048, NULL, 5, NULL);
	xTaskCreate(&LeerTeclas, "LeerTeclas", 2048, NULL, 5, NULL);
}
/*==================[end of file]============================================*/