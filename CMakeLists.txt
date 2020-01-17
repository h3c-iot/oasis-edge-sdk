CMAKE_MINIMUM_REQUIRED (VERSION 3.4)

PROJECT (edge_app_demo C)

SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#SET (CMAKE_C_COMPILER "arm-none-linux-gnueabi-gcc")

SET (CMAKE_BUILE_TYPE DEBUG)
SET (CMAKE_C_FLAGS_DEBUG "-g -Wall")

ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/app)
ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/device)
#ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/hal)
#ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/ota)
#ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/plugin)
ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/shadow)
ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/utils/json)
ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/utils/log)
ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/utils/tools)
ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/protocal/mqtt)

ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/src/sample/mqtt)

ADD_LIBRARY (edge_app_sdk STATIC                
                $<TARGET_OBJECTS:edge_mqtt>
                $<TARGET_OBJECTS:edge_device>
                $<TARGET_OBJECTS:edge_shadow>
                $<TARGET_OBJECTS:edge_func>
                $<TARGET_OBJECTS:utils_log>
                $<TARGET_OBJECTS:utils_json>
                $<TARGET_OBJECTS:utils_tools>
                )

FIND_PACKAGE (Threads)

TARGET_LINK_LIBRARIES (edge_app_sdk ${CMAKE_THREAD_LIBS_INIT})
