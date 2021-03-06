MENGINE_PROJECT(SDLApplication)

ADD_FILTER(
src
    main.cpp
    SDLApplication.cpp
    SDLApplication.h
    SDLMessageBoxLogger.cpp
    SDLMessageBoxLogger.h
    SDLStdioLogger.cpp
    SDLStdioLogger.h
)

if(MENGINE_TARGET_ANDROID)
    ADD_FILTER(
    android
        AndroidLogger.cpp
        AndroidLogger.h
    )
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

if(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${SRC_FILES})
endif()

if(MENGINE_TARGET_UNIX)
    ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES})
endif()

if(MENGINE_TARGET_ANDROID)
    ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_FILES})
endif()

if(MENGINE_TARGET_IOS AND NOT MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{HOME}/Desktop/deploy-ios-plist")

    set(myApp_MEDIA "$ENV{HOME}/Desktop/deploy-ios-media.xcassets")
    set_source_files_properties(${myApp_MEDIA} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set(DEPLOY_DATA "$ENV{HOME}/Desktop/deploy-ios-data")
    file(GLOB_RECURSE myApp_DATAS "${DEPLOY_DATA}/*")
    list(FILTER myApp_DATAS EXCLUDE REGEX ".*.DS_Store$")
    foreach(myApp_DATA ${myApp_DATAS})
        file(RELATIVE_PATH RES ${DEPLOY_DATA} ${myApp_DATA})
        get_filename_component(RES_LOC ${RES} PATH)

        set_source_files_properties(${myApp_DATA} PROPERTIES MACOSX_PACKAGE_LOCATION "deploy-ios-data/${RES_LOC}" HEADER_FILE_ONLY 1)
        string(REPLACE "/" "\\" ide_group_name "Data/${RES_LOC}")
        source_group(${ide_group_name} FILES ${myApp_DATA})
    endforeach()

    string(TIMESTAMP MACOSX_FRAMEWORK_BUNDLE_VERSION "%Y.%m.%d.%H%M%S")
    string(TIMESTAMP MACOSX_FRAMEWORK_SHORT_VERSION_STRING "%Y.%m.%d")

    ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${myApp_MEDIA} ${myApp_DATAS} ${SRC_FILES})

    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE YES)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST info.plist.in)
    
    SET_XCODE_PROPERTY(${PROJECT_NAME} INSTALL_PATH "$(LOCAL_APPS_DIR)")
    SET_XCODE_PROPERTY(${PROJECT_NAME} TARGETED_DEVICE_FAMILY "1,2")
    SET_XCODE_PROPERTY(${PROJECT_NAME} COMBINE_HIDPI_IMAGES "NO")

    SET_XCODE_PROPERTY(${PROJECT_NAME} ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
    SET_XCODE_PROPERTY(${PROJECT_NAME} ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME LaunchImage)
endif()

if(MENGINE_TARGET_IOS AND MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES})
endif()

if(MENGINE_TARGET_MACOS)
    set(myApp_ICON "$ENV{HOME}/Desktop/deploy-mac-icons/icon.icns")
    set_source_files_properties(${myApp_ICON} PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources")

    if(MENGINE_PLUGIN_ASTRALAX)
    	set(myApp_ASTRALAX "$ENV{HOME}/Desktop/magic3d.dylib")
    	set_source_files_properties(${myApp_ASTRALAX} PROPERTIES
			MACOSX_PACKAGE_LOCATION "MacOS")
	else()
        set(myApp_ASTRALAX)
    endif()

    if(MENGINE_PLUGIN_STEAM)
        set(myApp_STEAM "$ENV{HOME}/Desktop/libsteam_api.dylib")
        set_source_files_properties(${myApp_STEAM} PROPERTIES
        	MACOSX_PACKAGE_LOCATION "MacOS")
    else()
        set(myApp_STEAM)
    endif()
    
    set(DEPLOY_DATA "$ENV{HOME}/Desktop/deploy-mac-data")
    file(GLOB_RECURSE myApp_DATAS "${DEPLOY_DATA}/*")
    list(FILTER myApp_DATAS EXCLUDE REGEX ".*.DS_Store$")
    foreach(myApp_DATA ${myApp_DATAS})
        file(RELATIVE_PATH RES ${DEPLOY_DATA} ${myApp_DATA})
        get_filename_component(RES_LOC ${RES} PATH)

        set_source_files_properties(${myApp_DATA} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/${RES_LOC}" HEADER_FILE_ONLY 1)
        string(REPLACE "/" "\\" ide_group_name "Resources/${RES_LOC}")
        source_group(${ide_group_name} FILES ${myApp_DATA})
    endforeach()

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{HOME}/Desktop/deploy-mac-plist")
    
    string(TIMESTAMP MACOSX_FRAMEWORK_BUNDLE_VERSION "%Y.%m.%d.%H%M%S")
    string(TIMESTAMP MACOSX_FRAMEWORK_SHORT_VERSION_STRING "%Y.%m.%d")
	
    ADD_EXECUTABLE( ${PROJECT_NAME} MACOSX_BUNDLE ${myApp_DATAS} ${myApp_ICON} ${myApp_ASTRALAX} ${myApp_STEAM} ${SRC_FILES} )

	SET_XCODE_PROPERTY( ${PROJECT_NAME} OTHER_CODE_SIGN_FLAGS "--deep" )
    
    SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE icon.icns )
    SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST info.plist.in)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Bootstrapper)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${APPLICATION_LINK_LIBRARIES})

if(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2main${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
endif()

if(MENGINE_TARGET_MACOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2main${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})    
endif()

if(MENGINE_TARGET_IOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2main${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} iconv)
endif()

if(MENGINE_TARGET_UNIX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2main${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} dl)
endif()

IF(MENGINE_TARGET_IOS)
    #ADD_APPLE_FRAMEWORK(Cocoa)
    #ADD_APPLE_FRAMEWORK(Carbon)
    #ADD_APPLE_FRAMEWORK(AppKit)
    ADD_APPLE_FRAMEWORK(CoreData)
    ADD_APPLE_FRAMEWORK(CoreAudio)
    ADD_APPLE_FRAMEWORK(CoreVideo)
    ADD_APPLE_FRAMEWORK(CoreFoundation)
    ADD_APPLE_FRAMEWORK(CoreMotion)
    ADD_APPLE_FRAMEWORK(CoreBluetooth)
    ADD_APPLE_FRAMEWORK(AudioToolbox)
    ADD_APPLE_FRAMEWORK(AVFoundation)
    ADD_APPLE_FRAMEWORK(IOKit)
    ADD_APPLE_FRAMEWORK(UIKit)
    ADD_APPLE_FRAMEWORK(GameController)
    ADD_APPLE_FRAMEWORK(MediaPlayer)
    ADD_APPLE_FRAMEWORK(QuartzCore)
    ADD_APPLE_FRAMEWORK(Foundation)
    ADD_APPLE_FRAMEWORK(OpenGLES)
    ADD_APPLE_FRAMEWORK(CoreGraphics)
    ADD_APPLE_FRAMEWORK(OpenAL)
    ADD_APPLE_FRAMEWORK(Security)
    ADD_APPLE_FRAMEWORK(Metal)
ENDIF()

IF(MENGINE_TARGET_ANDROID)
    add_library( SDL2
            SHARED
            IMPORTED )
             
    set_target_properties( # Specifies the target library.
            SDL2
            # Specifies the parameter you want to define.
            PROPERTIES IMPORTED_LOCATION

            # Provides the path to the library you want to import.
            ${MENGINE_REPOSITORY}/gradle/libraries/SDL2/build/intermediates/ndkBuild/${CMAKE_BUILD_TYPE}/obj/local/${ANDROID_ABI}/libSDL2.so
            )

    TARGET_LINK_LIBRARIES( ${PROJECT_NAME} SDL2)
    
    add_library( hidapi
            SHARED
            IMPORTED )
             
    set_target_properties( # Specifies the target library.
            hidapi
            # Specifies the parameter you want to define.
            PROPERTIES IMPORTED_LOCATION

            # Provides the path to the library you want to import.
            ${MENGINE_REPOSITORY}/gradle/libraries/SDL2/build/intermediates/ndkBuild/${CMAKE_BUILD_TYPE}/obj/local/${ANDROID_ABI}/libhidapi.so
            )

    TARGET_LINK_LIBRARIES( ${PROJECT_NAME} hidapi)

    target_link_libraries(  ${PROJECT_NAME}
                            stdc++
                            c
                            m
                            dl
                            log
                            android
                            OpenSLES
                            GLESv1_CM
                            GLESv2
    )
endif()

IF(MENGINE_TARGET_MACOS)
    ADD_APPLE_FRAMEWORK(Cocoa)
    ADD_APPLE_FRAMEWORK(Carbon)
    ADD_APPLE_FRAMEWORK(AppKit)
    ADD_APPLE_FRAMEWORK(CoreData)
    ADD_APPLE_FRAMEWORK(CoreAudio)
    ADD_APPLE_FRAMEWORK(CoreVideo)
    ADD_APPLE_FRAMEWORK(CoreFoundation)
    ADD_APPLE_FRAMEWORK(AudioUnit)
    ADD_APPLE_FRAMEWORK(AudioToolBox)
    ADD_APPLE_FRAMEWORK(IOKit)
    ADD_APPLE_FRAMEWORK(ForceFeedback)
    ADD_APPLE_FRAMEWORK(Foundation)
    ADD_APPLE_FRAMEWORK(OpenGL)
    ADD_APPLE_FRAMEWORK(OpenAL)
    ADD_APPLE_FRAMEWORK(Security)
    ADD_APPLE_FRAMEWORK(Metal)
    
    TARGET_LINK_LIBRARIES( ${PROJECT_NAME} iconv)
ENDIF()

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Winmm Version Imm32 Setupapi)
ENDIF()

IF(MENGINE_TOOLCHAIN_MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
ENDIF()

IF(MENGINE_TARGET_MACOS)
    SET_XCODE_PROPERTY(${PROJECT_NAME} CODE_SIGN_IDENTITY "Mac Developer")
    SET_XCODE_PROPERTY(${PROJECT_NAME} DEVELOPMENT_TEAM "$ENV{XCODE_ATTRIBUTE_DEVELOPMENT_TEAM}")
ENDIF()

IF(MENGINE_TARGET_IOS AND NOT MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    SET_XCODE_PROPERTY(${PROJECT_NAME} CODE_SIGN_IDENTITY "iPhone Developer")
    SET_XCODE_PROPERTY(${PROJECT_NAME} DEVELOPMENT_TEAM "$ENV{XCODE_ATTRIBUTE_DEVELOPMENT_TEAM}")
ENDIF()

IF(MENGINE_TARGET_IOS AND MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    SET_XCODE_PROPERTY(${PROJECT_NAME} CODE_SIGN_IDENTITY "")
    SET_XCODE_PROPERTY(${PROJECT_NAME} CODE_SIGNING_REQUIRED "NO")
    SET_XCODE_PROPERTY(${PROJECT_NAME} CODE_SIGNING_ALLOWED "NO")
ENDIF()

if(APPLICATION_DEPENDENCIES)
    ADD_DEPENDENCIES( ${PROJECT_NAME} ${APPLICATION_DEPENDENCIES})
endif()
