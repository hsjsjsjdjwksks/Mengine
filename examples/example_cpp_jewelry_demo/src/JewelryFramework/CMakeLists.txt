MENGINE_PROJECT(JewelryFramework)

ADD_FILTER(
JewelryFramework
    Jewelry.cpp
    Jewelry.h
    JewelryMatrix.cpp
    JewelryMatrix.h
    JewelryChain.cpp
    JewelryChain.h
    JewelryFramework.cpp
    JewelryFramework.h
    JewelryFrameworkPlugin.cpp
    JewelryFrameworkPlugin.h
    JewelryEventReceiver.cpp
    JewelryEventReceiver.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)
INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_EXAMPLEFRAMEWORK Frameworks)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER Frameworks
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} GOAPCook)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}jpp${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}jansson${MENGINE_LIB_SUFFIX})