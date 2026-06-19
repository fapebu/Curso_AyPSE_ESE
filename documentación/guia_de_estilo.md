# Guía de Estilo de Código

Esta guía establece las convenciones de escritura de código C para el curso de Arquitectura y Programación de Sistemas Embebidos. Su objetivo es garantizar que el código de todos los alumnos sea legible, coherente y fácil de mantener, independientemente de la capa de la arquitectura en la que se trabaje.

---

## 1. Organización de Archivos

Cada módulo de software se compone de **dos archivos**:

- Un **archivo de cabecera** (`.h`), ubicado en el directorio `inc/`.
- Un **archivo de implementación** (`.c`), ubicado en el directorio `src/`.

### Nombres de Archivos

Los nombres de archivos deben estar en **minúsculas con guiones bajos** (`snake_case`), ser descriptivos del módulo que representan, e incluir un sufijo que indique la capa a la que pertenecen:

| Capa | Sufijo | Ejemplo |
| :--- | :--- | :--- |
| HAL (Hardware Abstraction) | `_hal` | `gpio_hal.h`, `uart_hal.h` |
| Board Support | *(sin sufijo)* | `led.h`, `display.h` |
| Middleware | *(sin sufijo)* | `animation.h`, `protocol.h` |
| Aplicación | *(nombre del proyecto)* | `0_blinking.c` |

---

## 2. Estructura Interna de los Archivos

### Archivo de Cabecera (`.h`)

Todo archivo `.h` debe seguir esta estructura en orden:

```c
#ifndef NOMBRE_MODULO_H    /* Include guard: evita inclusiones múltiples */
#define NOMBRE_MODULO_H

/** \brief Descripción breve del módulo.
 *
 * Descripción extendida (opcional).
 *
 * @author Nombre Apellido
 *
 **/

/*==================[inclusions]=============================================*/

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

#endif /* #ifndef NOMBRE_MODULO_H */

/*==================[end of file]============================================*/
```

### Archivo de Implementación (`.c`)

Todo archivo `.c` debe seguir esta estructura en orden:

```c

/*==================[inclusions]=============================================*/

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/
```

> [!NOTE]
> Se recomienda mantener siempre los separadores de sección (`/*==================[...]...*/`), incluso si la sección está vacía, ya que son una buena referencia visual para ubicarse rápidamente dentro de un archivo.

---

## 3. Nomenclatura

### 3.1 Funciones

Las funciones **públicas** (declaradas en el `.h`) utilizan **PascalCase con prefijo del módulo**:

```c
void GPIOInit(gpio_t pin, io_t io);
uint8_t LedOn(led_t led);
uint8_t AnimationInit(void);
void AnimationRunStep(animation_t anim);
```

Las funciones **privadas** (solo en el `.c`, declaradas como `static`) utilizan **snake_case**:

```c
static void reset_step_counter(void);
static bool is_valid_pin(gpio_t pin);
```

### 3.2 Variables

Las variables (locales, globales y parámetros) utilizan **snake_case**:

```c
uint8_t current_step = 0;
animation_t last_anim = ANIMATION_COUNT;
bool isr_service_installed = false;
```

Las variables **privadas del módulo** (globales al `.c`) deben declararse siempre como `static`:

```c
static uint8_t current_step = 0;
```

### 3.3 Constantes y Macros

Las constantes definidas con `#define` y los valores de enumeraciones (`enum`) utilizan **SCREAMING_SNAKE_CASE** (todo en mayúsculas con guiones bajos):

```c
#define GPIO_QTY        24
#define CONFIG_BLINK_PERIOD  500

typedef enum {
    ANIMATION_BLINK_ALL = 0,
    ANIMATION_CHASE,
    ANIMATION_PING_PONG,
    ANIMATION_COUNT
} animation_t;
```

### 3.4 Tipos de Datos (`typedef`)

Los tipos de datos definidos con `typedef` utilizan **snake_case con sufijo `_t`**:

```c
typedef enum   { ... } animation_t;
typedef enum   { ... } gpio_t;
typedef enum   { ... } io_t;
typedef struct { ... } digital_io_t;
```

### 3.5 Resumen de Nomenclatura

| Elemento | Estilo | Ejemplo |
| :--- | :--- | :--- |
| Función pública | `PascalCase` con prefijo | `GPIOInit()`, `LedOn()` |
| Función privada (`static`) | `snake_case` | `reset_counter()` |
| Variable local / parámetro | `snake_case` | `current_step`, `pin` |
| Variable de módulo | `static` + `snake_case` | `static bool isr_installed` |
| Macro / `#define` | `SCREAMING_SNAKE_CASE` | `GPIO_QTY`, `LED_1` |
| Valor de `enum` | `SCREAMING_SNAKE_CASE` | `GPIO_INPUT`, `ANIMATION_CHASE` |
| Tipo (`typedef`) | `snake_case` + sufijo `_t` | `gpio_t`, `animation_t` |
| Archivo | `snake_case` | `gpio_hal.c`, `led.h` |

---

## 4. Documentación con Doxygen

### 4.1 Cabecera del módulo (en el `.h`)

Cada archivo `.h` debe incluir un bloque de documentación de módulo al inicio:

```c
/** \brief Descripción breve del módulo en una sola línea.
 *
 * Descripción extendida del módulo (opcional). Puede incluir
 * notas sobre el hardware que requiere, limitaciones conocidas, etc.
 *
 * @note Nota importante sobre el uso del módulo.
 *
 * @author Nombre Apellido (email@uner.edu.ar)
 *
 * @section changelog
 *
 * |   Date     | Description                                            |
 * |:----------:|:-------------------------------------------------------|
 * | DD/MM/AAAA | Document creation                                      |
 * | DD/MM/AAAA | Descripción de la modificación                         |
 *
 **/
```

### 4.2 Documentación de Funciones (en el `.h`)

Cada función pública debe estar documentada en el archivo `.h` con el siguiente formato:

```c
/**
 * @brief Descripción breve de lo que hace la función.
 *
 * Descripción extendida (opcional). Explica el comportamiento
 * detallado, efectos secundarios, o condiciones de uso.
 *
 * @param nombre_param Descripción del parámetro.
 * @param otro_param   Descripción del otro parámetro.
 * @return Descripción del valor de retorno.
 * @return true  Descripción del caso verdadero (si retorna bool).
 * @return false Descripción del caso falso (si retorna bool).
 */
uint8_t NombreModuloFuncion(tipo_t param);
```

Los campos de estructuras y valores de `enum` se documentan con comentarios `/**<` en la línea del elemento:

```c
typedef enum {
    GPIO_INPUT = 0,  /**< Input with pull-up resistor */
    GPIO_OUTPUT      /**< Output */
} io_t;

typedef struct {
    uint64_t pin;     /*!< GPIO pin number */
    bool     state;   /*!< Current output state */
} digital_io_t;
```

---

## 5. Reglas Generales de Estilo

- **Indentación:** Se usa **1 tabulador** para indentación.
- **Tipos de datos:** Se utilizan los tipos de ancho fijo de `<stdint.h>` (`uint8_t`, `uint16_t`, `int32_t`, etc.) en lugar de los tipos básicos de C (`int`, `long`, `char`).
- **Booleanos:** Se utiliza `<stdbool.h>` y los valores `true`/`false`. No usar `1` y `0` como reemplazo de booleanos.
- **`void` explícito:** Las funciones que no reciben parámetros deben declararse con `void` explícito: `void Funcion(void)`, no `void Funcion()`.
