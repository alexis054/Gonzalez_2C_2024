/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number) {
    // Verificar si el número de dígitos es suficiente para el número dado
    if (digits < 1) {
        return -1; // Error: número de dígitos no válido
    }
    
    // Inicializar el arreglo con ceros
    for (int i = 0; i < digits; i++) {
        bcd_number[i] = 0;
    }

    // Convertir el número a BCD
    for (int i = 0; i < digits; i++) {
        if (data == 0) {
            break;
        }
        // Extraer el dígito menos significativo y almacenarlo en el arreglo
        bcd_number[digits - 1 - i] = data % 10;
        // Eliminar el dígito menos significativo del número
        data /= 10;
    }

    return 0; // Éxito
}

// Función para imprimir un número en formato binario de 4 bits
void printBcdArray(const uint8_t *bcd_number, uint8_t digits) {
    printf("BCD array: ");
    for (int i = 0; i < digits; i++) {
        // Imprimir cada dígito en formato binario de 4 bits
        for (int j = 3; j >= 0; j--) {
            printf("%d", (bcd_number[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}
/*==================[external functions definition]==========================*/
void app_main(void){
	

	uint32_t number = 20;  // Número decimal a convertir
    uint8_t digits = 2;          // Cantidad de dígitos en el número
    uint8_t bcd_array[8];        // Arreglo para almacenar los dígitos en BCD

    // Convertir el número a formato BCD
    if (convertToBcdArray(number, digits, bcd_array) == 0) {
        // Imprimir el arreglo en formato BCD
        printBcdArray(bcd_array, digits);
    } 
}
/*==================[end of file]============================================*/