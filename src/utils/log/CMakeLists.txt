INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/tools)

ADD_LIBRARY (utils_log OBJECT
        ${PROJECT_SOURCE_DIR}/src/utils/log/oasis_log.c
)