INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/json)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/tools)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocal/mqtt/external)

ADD_LIBRARY (edge_func OBJECT
        ${PROJECT_SOURCE_DIR}/src/app/func/oasis_func.c
)