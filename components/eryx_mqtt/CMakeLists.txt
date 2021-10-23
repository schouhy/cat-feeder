idf_component_register(SRCS "eryx_mqtt.c"
                    INCLUDE_DIRS "include"
                    REQUIRES mbedtls mqtt esp-tls)