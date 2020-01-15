INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/json)

ADD_LIBRARY (utils_json OBJECT
        ${PROJECT_SOURCE_DIR}/src/utils/json/cJSON.c
)