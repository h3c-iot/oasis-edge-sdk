INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/device/led)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/app/func)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/hal/spi)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/json)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/tools)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/shadow)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external)

ADD_LIBRARY (edge_device OBJECT
        ${PROJECT_SOURCE_DIR}/src/device/led/oasis_led.c
        ${PROJECT_SOURCE_DIR}/src/hal/spi/oasis_spi.c
        ${PROJECT_SOURCE_DIR}/src/shadow/oasis_shadow.c
        )
