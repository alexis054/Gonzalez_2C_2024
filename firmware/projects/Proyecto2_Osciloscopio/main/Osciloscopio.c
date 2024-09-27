/**
 * @file main.c
 * @brief Main file for the application that converts analog signals to digital and sends them to a serial plotter on the PC.
 *
 * @author Gonzalez Alexis
 * @date 2024-09-27
 */

/**
 * @brief Includes necessary header files.
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
#define BUFFER_SIZE 231
#define CONFIG_MEASURE_PERIOD 2000

/**
 * @brief Defines global variables.
 */
TaskHandle_t main_task_handle = NULL;
TaskHandle_t ConvertirAD_task_handle = NULL;

const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
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
        UartSendString(UART_PC, "\r" );
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

    serial_config_t serial_pc ={
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = ConvertirAD,
		.param_p = NULL,
	};
	UartInit(&serial_pc);
    timer_config_t  timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_MEASURE_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);
    xTaskCreate(&ConvertirAD, "ConvertirAD", 2048, NULL, 5, &ConvertirAD_task_handle);
    TimerStart(timer_medicion.timer);
}