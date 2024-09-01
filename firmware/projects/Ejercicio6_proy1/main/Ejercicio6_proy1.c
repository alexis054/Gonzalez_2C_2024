/*
Escriba una función que reciba un dato de 32 bits,  la cantidad de dígitos de salida y dos vectores 
de estructuras del tipo  gpioConf_t. Uno  de estos vectores 
es igual al definido en el punto anterior y el otro vector mapea los puertos con el dígito del LCD a 
donde mostrar un dato:

Dígito 1 -> GPIO_19
Dígito 2 -> GPIO_18
Dígito 3 -> GPIO_9

La función deberá mostrar por display el valor que recibe. Reutilice las funciones creadas en el punto 4 y 5.
 Realice la documentación de este ejercicio usando Doxygen.

*/
/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;	

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

/**
 * @brief Convierte un número de 32 bits a un array BCD.
 * 
 * @param data El número de 32 bits a convertir.
 * @param digits La cantidad de dígitos de salida.
 * @param bcd_number El array donde se almacenarán los dígitos BCD.
 */
void convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	uint8_t digito;

	for (int i=(digits-1); i>=0; i--){
		digito = data % 10;
		bcd_number[i] = digito;
		data = data / 10;
	}
}

/**
 * @brief Inicializa los pines GPIO.
 * 
 * @param ptrGPIO Puntero al array de configuración GPIO.
 * @param iteraciones Número de pines a inicializar.
 */
void Inicializar(gpioConf_t *ptrGPIO, unsigned int iteraciones)
{
	for(unsigned int i=0; i<iteraciones;i++)
	{
		GPIOInit(ptrGPIO[i].pin, ptrGPIO[i].dir);
		GPIOOff(ptrGPIO[i].pin);
	}
}

/**
 * @brief Muestra un número en un display utilizando pines GPIO.
 * 
 * @param data El número de 32 bits a mostrar.
 * @param digits La cantidad de dígitos de salida.
 * @param ptr_bits Puntero al array de configuración de bits GPIO.
 * @param ptr_lcd Puntero al array de configuración de LCD GPIO.
 */
void MostrarDisplay (uint32_t data, uint8_t digits, gpioConf_t *ptr_bits, gpioConf_t *ptr_lcd ){
    if (digits > 3) {
        // Manejar error: demasiados dígitos
        return;
    }

    uint8_t bcd[digits];
    convertToBcdArray(data, digits, bcd);

    for(int i = 0; i < digits; i++)
	 {
        
        for (uint8_t j = 0; j < 4; j++) {
            if (bcd[i] & 1) {
                GPIOOn(ptr_bits[j].pin);
            } else {
                GPIOOff(ptr_bits[j].pin);
            }
		
			bcd[i] = bcd[i] >> 1;
        }
			GPIOOn(ptr_lcd[i].pin);
            GPIOOff(ptr_lcd[i].pin);
    }
}

void app_main(void){
	gpioConf_t bits[4] = {{GPIO_20,GPIO_OUTPUT},{GPIO_21,GPIO_OUTPUT},{GPIO_22,GPIO_OUTPUT},{GPIO_23,GPIO_OUTPUT}};
	gpioConf_t LCD[3] = {{GPIO_19,GPIO_OUTPUT},{GPIO_18,GPIO_OUTPUT},{GPIO_9,GPIO_OUTPUT}};
 	int cantidad_bits = 4;
	uint32_t numero = 123;
	uint8_t digitos = 3;

	 Inicializar(LCD, digitos);
    Inicializar(bits, cantidad_bits);
	MostrarDisplay(numero, digitos, bits, LCD);
}
/*==================[end of file]============================================*/