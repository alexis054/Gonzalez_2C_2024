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
#include <analog_io_mcu.h>
#include <stdbool.h>
#include <uart_mcu.h>
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
/*==================[macros and definitions]=================================*/
TaskHandle_t ConvertirAD_task_handle = NULL;
#define CONFIG_MEASURE_PERIOD 1000
#define TRESHOLD_VALUE 250
#define R_VALUE 500  // 1k ohm
#define C_VALUE 0.00001  // 100uF
#define ALPHA_VALUE (CONFIG_MEASURE_PERIOD / (R_VALUE * C_VALUE + CONFIG_MEASURE_PERIOD))
float previous_filtered_value = 0;
/*==================[internal data definition]===============================*/
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

float LowPassFilter(uint16_t current_value) {
    float filtered_value = ALPHA_VALUE * previous_filtered_value + (1 - ALPHA_VALUE) * current_value;
    previous_filtered_value = filtered_value;
    return filtered_value;
}

static void ConvertirAD(void *param)
{ while (1) {
        // Wait for the notification from the FuncTimerA ISR to perform the task.
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        uint16_t lectura;
        // Read the analog signal from channel CH1 and store the digital value in the 'lectura' variable.
        AnalogInputReadSingle(CH1, &lectura);

        // Apply the low-pass filter
        //float filtered_value = LowPassFilter(lectura);

        // Check if the filtered value is below the threshold
        if (lectura < TRESHOLD_VALUE) {
            // Turn on the LED
            LedOn(LED_1);
        } else {
            // Turn off the LED
            LedOff(LED_1);
        }

        // Convert the digital value to a string and send it to the PC via UART.
        UartSendString(UART_PC, (char *)UartItoa((uint16_t)lectura, 10));
        UartSendString(UART_PC, "\r");
    }
}

void app_main(void)
{
    analog_input_config_t analogic_input_config =
    {
        .input = CH1,
        .mode = ADC_SINGLE,
    };

    AnalogInputInit(&analogic_input_config);
    AnalogOutputInit();
	LedsInit();

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


    TimerInit(&timer_medicion);

    xTaskCreate(&ConvertirAD, "ConvertirAD", 2048, NULL, 5, &ConvertirAD_task_handle);
    
    TimerStart(timer_medicion.timer);

}	

/*==================[end of file]============================================*/