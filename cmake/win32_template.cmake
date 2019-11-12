set(CMAKE_STATIC_LIBRARY_PREFIX)
set(CMAKE_STATIC_LIBRARY_SUFFIX .lib)

SET(MENGINE_LIB_PREFIX "")
SET(MENGINE_LIB_SUFFIX ".lib")

set(CMAKE_DEBUG_POSTFIX "")

set(CMAKE_CXX_STANDARD 17)

IF(MENGINE_USE_PRECOMPILED_HEADER)
    set(MENGINE_C_DEBUG_INFORMATION_FORMAT "/Z7")
    set(MENGINE_CXX_DEBUG_INFORMATION_FORMAT "/Z7")
ELSE()
    set(MENGINE_C_DEBUG_INFORMATION_FORMAT "/ZI")
    set(MENGINE_CXX_DEBUG_INFORMATION_FORMAT "/ZI")
ENDIF()

set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W4 /wd4121 /wd4250 /Zm1000 /MP /EHsc /GR /UMBCS /D_UNICODE /DUNICODE")
set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG ${MENGINE_CXX_DEBUG_INFORMATION_FORMAT} /Ob0 /Od /RTC1")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/DNDEBUG ${MENGINE_CXX_DEBUG_INFORMATION_FORMAT} /O2 /Oi /Ot")
set(CMAKE_CXX_FLAGS_RELEASE "/DNDEBUG /O2 /Oi /Ot /GL")

set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W4 /Zm1000")
set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG ${MENGINE_C_DEBUG_INFORMATION_FORMAT} /Ob0 /Od /RTC1")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "/DNDEBUG ${MENGINE_C_DEBUG_INFORMATION_FORMAT} /O2 /Oi /Ot")
set(CMAKE_C_FLAGS_RELEASE "/DNDEBUG /O2 /Oi /Ot /GL")

set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /DEBUG /SAFESEH:NO /LTCG:OFF")
set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} /DEBUG /SAFESEH:NO /LTCG:OFF")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /LTCG")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG /SAFESEH:NO /LTCG:OFF")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /DEBUG /SAFESEH:NO /LTCG:OFF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /SAFESEH:NO /LTCG:OFF")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")

set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /LTCG:OFF")
set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /LTCG")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_ITERATOR_DEBUG_LEVEL=2")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -D_ITERATOR_DEBUG_LEVEL=0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_ITERATOR_DEBUG_LEVEL=0")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_ITERATOR_DEBUG_LEVEL=2")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -D_ITERATOR_DEBUG_LEVEL=0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_ITERATOR_DEBUG_LEVEL=0")

add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)





