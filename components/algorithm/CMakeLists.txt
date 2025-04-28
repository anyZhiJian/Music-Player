file(GLOB_RECURSE SRC ./*.c)
idf_component_register(SRCS ${SRC}
                    INCLUDE_DIRS "."
                    )
