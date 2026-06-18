# Capa de Abstracción de Hardware (Hardware Abstraction Layer / HAL)

Esta capa aísla los detalles específicos del microcontrolador (en este caso, el **ESP32-C6**) para que las capas de software superiores no tengan que acceder directamente a sus registros o a APIs propietarias.

---

## Módulos Disponibles

### Abstracción de GPIO (`gpio_hal`)
Este controlador encapsula el uso de los pines de entrada/salida de propósito general (GPIO), simplificando la configuración de direcciones, niveles lógicos, filtros de ruido e interrupciones.

* **Cabecera:** [gpio_hal.h](./inc/gpio_hal.h)

---

## Interfaz de Programación (API)

* **`void GPIOInit(gpio_t pin, io_t io)`**: Configura un pin específico como entrada (`GPIO_INPUT`) o salida (`GPIO_OUTPUT`).
* **`void GPIOOn(gpio_t pin)`**: Pone el pin en estado alto (3.3V).
* **`void GPIOOff(gpio_t pin)`**: Pone el pin en estado bajo (0V).
* **`void GPIOState(gpio_t pin, bool state)`**: Establece el nivel lógico del pin (`true` para alto, `false` para bajo).
* **`void GPIOToggle(gpio_t pin)`**: Invierte el estado lógico actual del pin.
* **`bool GPIORead(gpio_t pin)`**: Retorna el nivel lógico actual del pin configurado como entrada.
* **`void GPIOActivInt(gpio_t pin, void *ptr_int_func, bool edge, void *args)`**: Habilita interrupciones en el pin para flancos de subida (`edge = true`) o bajada (`edge = false`), vinculando una función callback.
