#include <stdio.h>
#include <stdint.h>

void decimalToBcd(uint32_t number, uint8_t *bcd_array, uint8_t digits) {
    // Inicializar el arreglo con ceros
    for (int i = 0; i < digits; i++) {
        bcd_array[i] = 0;
    }

    // Convertir el número a BCD
    for (int i = 0; i < digits; i++) {
        if (number == 0) {
            break;
        }
        // Extraer el dígito menos significativo y almacenarlo en el arreglo
        bcd_array[digits - 1 - i] = number % 10;
        // Eliminar el dígito menos significativo del número
        number /= 10;
    }
}

void printBcdArray(const uint8_t *bcd_array, uint8_t digits) {
    printf("BCD array: ");
    for (int i = 0; i < digits; i++) {
        // Imprimir cada dígito en formato BCD de 4 bits
        printf("%04b ", bcd_array[i]); // Imprime el dígito en formato binario de 4 bits
    }
    printf("\n");
}

int main() {
    uint32_t number = 21;  // Número decimal a convertir
    uint8_t digits = 2;          // Cantidad de dígitos en el número
    uint8_t bcd_array[8];        // Arreglo para almacenar los dígitos en BCD

    decimalToBcd(number, bcd_array, digits);

    // Imprimir el arreglo en formato BCD
    printBcdArray(bcd_array, digits);

    return 0;
}