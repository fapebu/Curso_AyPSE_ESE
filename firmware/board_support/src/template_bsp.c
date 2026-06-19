/**
 * @file template_bsp.c
 * @author Nombre Apellido
 * @brief BSP driver template for an external component.
 * @version 0.1
 * @date AAAA-MM-DD
 *
 * @copyright Copyright (c) AAAA
 *
 */

/*==================[inclusions]=============================================*/
#include "template_bsp.h"
/* TODO: Include the HAL headers needed to access hardware resources.
 * BSP drivers must only use HAL functions, never ESP-IDF APIs directly.
 * Examples:
 *   #include "gpio_hal.h"
 *   #include "uart_hal.h"
 *   #include "spi_hal.h"
 */
/*==================[macros and definitions]=================================*/
/* TODO: Define the GPIO pins or peripheral instances used by this component.
 * Map the component's physical pins to HAL identifiers here.
 * Example:
 *   #define SENSOR_GPIO_DATA    GPIO_3
 *   #define SENSOR_GPIO_ENABLE  GPIO_4
 */

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

uint8_t TemplateBSPInit(void){
	/* TODO: Implement component initialization using HAL function calls.
	 * This function should:
	 *   1. Configure the required GPIO or peripheral via HAL
	 *   2. Set the component to a known initial state
	 *   3. Return true on success, false on failure
	 *
	 * Example:
	 *   GPIOInit(SENSOR_GPIO_DATA, GPIO_INPUT);
	 *   GPIOInit(SENSOR_GPIO_ENABLE, GPIO_OUTPUT);
	 *   GPIOOff(SENSOR_GPIO_ENABLE);
	 */
	return true;
}

/*==================[end of file]============================================*/
