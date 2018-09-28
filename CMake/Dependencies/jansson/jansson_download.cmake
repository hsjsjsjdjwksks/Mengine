set(EXTERNAL_PROJECT_NAME jansson)

set(EXTERNAL_LIBRARY_FILE_NAME jansson)

ExternalProject_Add(${EXTERNAL_PROJECT_NAME}_download PREFIX ${EXTERNAL_PROJECT_NAME}
        GIT_REPOSITORY https://github.com/akheron/jansson.git
        GIT_TAG "v2.11"
        GIT_PROGRESS TRUE

        UPDATE_COMMAND ""

        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_DIR}/${EXTERNAL_PROJECT_NAME}
            -DBUILD_SHARED_LIBS=OFF
            -DLIBTYPE=STATIC
            -DUSE_URANDOM=OFF
            -DUSE_WINDOWS_CRYPTOAPI=OFF
            -DJANSSON_STATIC_CRT=ON
            -DJANSSON_EXAMPLES=OFF
            -DJANSSON_BUILD_DOCS=OFF
            -DJANSSON_WITHOUT_TESTS=ON
            -DJANSSON_INSTALL=ON
)

ExternalProject_Get_Property(${EXTERNAL_PROJECT_NAME}_download INSTALL_DIR)
set(EXTERNAL_PROJECT_INCLUDE_DIR ${INSTALL_DIR}/include)
set(EXTERNAL_PROJECT_LIBRARY_DIR ${INSTALL_DIR}/lib)

add_library(${EXTERNAL_PROJECT_NAME} STATIC IMPORTED)

set_target_properties(${EXTERNAL_PROJECT_NAME} PROPERTIES IMPORTED_LOCATION ${EXTERNAL_PROJECT_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${EXTERNAL_LIBRARY_FILE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX})

