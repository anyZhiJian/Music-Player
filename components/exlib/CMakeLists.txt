file(GLOB_RECURSE SRC ./*.c)
idf_component_register(SRCS ${SRC}
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    REQUIRES esp_adc
                    REQUIRES soc
                    REQUIRES lwip
                    REQUIRES esp_wifi
                    REQUIRES nvs_flash
                    REQUIRES fatfs
                    REQUIRES esp_timer
                    )
