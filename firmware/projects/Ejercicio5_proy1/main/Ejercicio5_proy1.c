/*
Escribir una función que reciba como parámetro un dígito BCD y un vector de estructuras del tipo gpioConf_t. 
Incluya el archivo de cabecera gpio_mcu.h


#include “gpio_mcu.h”

typedef struct
{
	gpio_t pin;			!< GPIO pin number 
	io_t dir;			!< GPIO direction '0' IN;  '1' OUT
} gpioConf_t;
Defina un vector que mapee los bits de la siguiente manera:

b0 -> GPIO_20
b1 ->  GPIO_21
b2 -> GPIO_22
b3 -> GPIO_23

La función deberá cambiar el estado de cada GPIO, a ‘0’ o a ‘1’, 
según el estado del bit correspondiente en el BCD ingresado. Ejemplo: b0 se encuentra en ‘1’, el estado de GPIO_20 debe setearse. 
*/  



/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"


// Definición de la estructura gpioConf_t
typedef struct {
    gpio_t pin;  /*!< GPIO pin number */
    io_t dir;    /*!< GPIO direction '0' IN;  '1' OUT */
} gpioConf_t;


// Función que cambia el estado de los GPIO según el dígito BCD
void setGpioFromBcd(uint8_t bcd, gpioConf_t * ptrGPIO) 
{

    for (uint8_t i = 0; i < 4; i++)
     {
            GPIOInit(ptrGPIO[i].pin, ptrGPIO[i].dir);
       if(bcd & 1)
       {
           GPIOOn(ptrGPIO[i].pin);
        } else {
           GPIOOff(ptrGPIO[i].pin);
        }
        bcd= bcd >> 1;
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){

    gpioConf_t bits[4] = {{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};
    uint8_t bcd = 9;
	setGpioFromBcd(bcd,bits);

}
/*==================[end of file]============================================*/