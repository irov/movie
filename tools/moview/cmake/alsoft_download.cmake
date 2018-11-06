include(ExternalProject)

ExternalProject_Add(alsoft_download PREFIX alsoft
        GIT_REPOSITORY https://github.com/kcat/openal-soft.git
        GIT_TAG openal-soft-1.19.1
        GIT_PROGRESS TRUE

        UPDATE_COMMAND ""

        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DBUILD_SHARED_LIBS=OFF
            -DLIBTYPE=STATIC
            -DALSOFT_BACKEND_DSOUND=OFF
            -DALSOFT_BACKEND_WASAPI=OFF
            -DALSOFT_BUILD_ROUTER=OFF
            -DALSOFT_BUILD_ROUTER=OFF
            -DALSOFT_EXAMPLES=OFF
            -DALSOFT_NO_CONFIG_UTIL=ON
            -DALSOFT_TESTS=OFF
            -DALSOFT_UTILS=OFF
)

ExternalProject_Get_Property(alsoft_download INSTALL_DIR)
set(ALSOFT_INCLUDE_DIR ${INSTALL_DIR}/include)
set(ALSOFT_LIBRARY_DIR ${INSTALL_DIR}/lib)

if(WIN32)
    set(ALSOFT_LIB_FILE_NAME OpenAL32)
else()
    set(ALSOFT_LIB_FILE_NAME openal)
endif()

add_library(alsoft STATIC IMPORTED)

set_target_properties(alsoft PROPERTIES IMPORTED_LOCATION ${ALSOFT_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${ALSOFT_LIB_FILE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX})
