MENGINE_PROJECT(GOAPPlugin)

ADD_FILTER(
src
    GOAPPlugin.h
    GOAPPlugin.cpp
    GOAPPlugin.def
    
    GOAPInterface.h
    
    GOAPService.h
    GOAPService.cpp
)

INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_GOAP)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}GOAP${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

ADD_SUBDIRECTORY(Tasks)