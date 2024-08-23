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
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

// Definición de la estructura gpioConf_t
typedef struct {
    gpio_t pin;  /*!< GPIO pin number */
    io_t dir;    /*!< GPIO direction '0' IN;  '1' OUT */
} gpioConf_t;


// Función que cambia el estado de los GPIO según el dígito BCD
void setGpioFromBcd(uint8_t bcd, gpioConf_t * ptrGPIO) 
{
    // Iterar sobre los 4 bits del BCD
    for (uint8_t i = 0; i < 4; i++) 
	{
    GPIOINIT(ptrGPIO->pin, ptrGPIO->dir);

    }
}
/*==================[external functions definition]==========================*/
void app_main(void){

    gpioConf_t bits[4] = {{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};
	setGpioFromBcd();
}
/*==================[end of file]============================================*/