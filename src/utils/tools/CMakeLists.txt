INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/tools)

ADD_LIBRARY (utils_tools OBJECT
        ${PROJECT_SOURCE_DIR}/src/utils/tools/oasis_tools.c
)