MENGINE_PROJECT(DazzlePlugin)

ADD_FILTER(
src
    DazzleInterface.h

    DazzlePlugin.cpp
    DazzlePlugin.h
    DazzlePlugin.def

    DazzleEffect.cpp
    DazzleEffect.h
    ResourceDazzleEffect.cpp
    ResourceDazzleEffect.h
    DataflowDZZ.cpp
    DataflowDZZ.h
    
    DazzleDataInterface.h
    
    DazzleData.cpp
    DazzleData.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/dazzle/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DAZZLE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}dazzle${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
