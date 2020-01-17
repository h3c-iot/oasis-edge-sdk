INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/device/led)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/app/func)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/tools)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/shadow/)

ADD_EXECUTABLE (edge_app_demo
                ${PROJECT_SOURCE_DIR}/src/sample/mqtt/edgeClient.c
                )
                
TARGET_LINK_LIBRARIES (edge_app_demo edge_app_sdk)