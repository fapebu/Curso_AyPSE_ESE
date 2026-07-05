idf_component_register(SRCS "src/gpio_hal.c"
                            "src/hal_bno086.c"
                            "src/template_hal.c"
                       INCLUDE_DIRS "inc"
                       REQUIRES esp_driver_i2c esp_driver_gpio)