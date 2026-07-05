idf_build_get_property(target IDF_TARGET)

# Always compiled source files
set(srcs "src/animation.c"
    )

# Always included headers
set(includes "inc")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES board_support drivers_hal
                       REQUIRES driver freertos esp_driver_i2c esp_driver_gpio esp_common)