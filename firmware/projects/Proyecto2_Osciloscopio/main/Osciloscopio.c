/**
 * @file Osciloscopio.c
 * @brief Main file for the application that converts analog signals to digital and sends them to a serial plotter on the PC.
 *
 * @author Gonzalez Alexis
 * @date 10-04-2022
 */
#include <stdio.h>
#include <stdint.h>
#include <analog_io_mcu.h>
#include <stdbool.h>
#include <uart_mcu.h>
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Defines constants and macros.
 */
#define BUFFER_SIZE 256
#define CONFIG_MEASURE_PERIOD 2000

/**
 * @brief Defines global variables.
 */
TaskHandle_t main_task_handle = NULL;
TaskHandle_t ConvertirAD_task_handle = NULL;
TaskHandle_t ConvertirDA_task_handle = NULL;
unsigned char ecg[BUFFER_SIZE] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17
} ;
int ECG_FREQUENCY = 4000;

int indice = 0;
timer_config_t timer_ecg = {
    .timer = TIMER_B,
    .period = 0,
    .func_p = NULL,
    .param_p = NULL
};

/**
 * @brief Function to be executed when the Timer A interrupt occurs.
 *        This function notifies the ConvertirAD task to perform its task.
 *
 * @param[in] param Unused parameter.
 */
void FuncTimerA(void* param)
{
    // Notify the ConvertirAD task to perform its task.
    vTaskNotifyGiveFromISR(ConvertirAD_task_handle, pdFALSE);
}
void FuncTimerB(void* param)
{
    // Notify the ConvertirAD task to perform its task.
    vTaskNotifyGiveFromISR(ConvertirDA_task_handle, pdFALSE);
}

/**
 * @brief Function to convert analog signals to digital and send them to the PC via UART.
 *
 * @param[in] param Unused parameter.
 */
static void ConvertirAD(void *param)
{
    while (1) {
        // Wait for the notification from the FuncTimerA ISR to perform the task.
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        uint16_t lectura;
        // Read the analog signal from channel CH1 and store the digital value in the 'lectura' variable.
        AnalogInputReadSingle(CH1, &lectura);

        // Convert the digital value to a string and send it to the PC via UART.
        UartSendString(UART_PC, (char *)UartItoa(lectura, 10));
        UartSendString(UART_PC, "\r");
    }
}
/**
 * @brief Function to convert digital signals to analog and write them to the analog output.
 *
 * This function is responsible for converting digital signals to analog and writing them to the analog output.
 * It continuously loops, waiting for a notification to start the conversion process.
 *
 * @param[in] paramr.
 */
static void ConvertirAAnalogico(void *param)
{
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (indice == BUFFER_SIZE) {
            indice = 0;
        }

        uint8_t dato;
        dato = ecg[indice];

        AnalogOutputWrite(dato);

        indice++;
    }
}

/**
 * @brief Main function that initializes the necessary components and starts the tasks.
 */
void app_main(void)
{
    analog_input_config_t analogic_input_config =
    {
        .input = CH1,
        .mode = ADC_SINGLE,
    };

    AnalogInputInit(&analogic_input_config);
    AnalogOutputInit();

    serial_config_t serial_pc = {
        .port = UART_PC,
        .baud_rate = 115200,
        .func_p = ConvertirAD,
        .param_p = NULL,
    };
    UartInit(&serial_pc);
    timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

    timer_ecg.period = ECG_FREQUENCY;
    timer_ecg.func_p = FuncTimerB;

    TimerInit(&timer_medicion);
    TimerInit(&timer_ecg);
    xTaskCreate(&ConvertirAD, "ConvertirAD", 2048, NULL, 5, &ConvertirAD_task_handle);
    xTaskCreate(&ConvertirAAnalogico, "ConvertirAAnalogico", 2048, NULL, 5, &ConvertirDA_task_handle);
    TimerStart(timer_medicion.timer);
    TimerStart(timer_ecg.timer);
}