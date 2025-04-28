file(GLOB_RECURSE SRC ./*.c)
idf_component_register(SRCS ${SRC}
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    REQUIRES exlib
                    REQUIRES simpleGL
                    REQUIRES softbus
                    REQUIRES lvgl
                    REQUIRES lvgl_port
                    REQUIRES algorithm
                    )
