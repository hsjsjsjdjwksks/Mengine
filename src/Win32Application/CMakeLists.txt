set( MY_LIB_NAME Win32Application )

ADD_FILTER(
MessageBoxLogger
    MessageBoxLogger.cpp
    MessageBoxLogger.h
)

ADD_FILTER(
CriticalErrorsMonitor
	CriticalErrorsMonitor.h
	CriticalErrorsMonitor.cpp
)

if(MSVC)
ADD_FILTER(
main	
    win32_main.cpp
)
endif()

if(MINGW)
ADD_FILTER(
main	
    mingw32_main.cpp
)
endif()

    
ADD_FILTER(
Win32Application
	Win32Application.cpp
	Win32Application.h
)

add_definitions(-DUNICODE)

if(MINGW)
    # resource compilation for mingw
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
                                           -i${CMAKE_CURRENT_SOURCE_DIR}/Win32Application.rc
                                           -o ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o)
    SET(SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o)
  else(MINGW)
    SET(SRC_FILES ${SRC_FILES} Win32Application.rc)
  endif(MINGW)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/tinyini/include
    ${THIRDPARTY_DIR}/libmath/include
    )
    
    
#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${APPLICATION_LIBS} ) 

if(NOT MSVC)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} libole32.a )
endif()

if(MSVC)
	set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )
endif()

if(MINGW)
	set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS -Wl,-Map,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Win32Application.map )
endif()