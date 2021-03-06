MENGINE_PROJECT(AstralaxPlugin)

ADD_FILTER(
src
    AstralaxInterface.h

    AstralaxModule.cpp
    AstralaxModule.h
    AstralaxEmitter.cpp
    AstralaxEmitter.h
    AstralaxEmitter2.cpp
    AstralaxEmitter2.h
    AstralaxEmitterContainer.cpp
    AstralaxEmitterContainer.h
    ResourceAstralax.cpp
    ResourceAstralax.h
    LoaderResourceAstralax.cpp
    LoaderResourceAstralax.h
    AstralaxService.h
    AstralaxService.cpp
    
    ResourceAstralaxValidator.cpp
    ResourceAstralaxValidator.h

    AstralaxPlugin.cpp
    AstralaxPlugin.h
    AstralaxPlugin.def
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AstralaxScriptEmbedding.cpp
        AstralaxScriptEmbedding.h    
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

add_definitions(-DMAGIC_3D)
add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

if(MENGINE_PLATFORM_IOS)
  INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax-ios)
else()
  INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ASTRALAX)

if(MENGINE_TOOLCHAIN_MINGW)
    if(MENGINE_ARCHITECTURE_X86)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax/libmagic3d.a)
    endif()
    
    if(MENGINE_ARCHITECTURE_X64)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MENGINE_CMAKE_DEPENDENCIES_DIR}/astralax/libmagic3d.a)
    endif()
endif()

if(MENGINE_TOOLCHAIN_MSVC)
    if(MENGINE_ARCHITECTURE_X86)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax/magic3d.lib)
    endif()
    
    if(MENGINE_ARCHITECTURE_X64)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax/magic3d_x64.lib)
    endif()
endif()

if(MENGINE_TOOLCHAIN_XCODE AND MENGINE_TARGET_MACOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax/magic3d.dylib)
endif()

if(MENGINE_TOOLCHAIN_XCODE AND MENGINE_TARGET_IOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax-ios/bitcode/libimagic3d.a)
endif()

if(MENGINE_TOOLCHAIN_GRADLE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax-ios/bitcode/libimagic3d.a)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
