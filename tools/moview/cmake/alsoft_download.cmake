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
            -DFORCE_STATIC_VCRT=STATIC
            -DALSOFT_STATIC_LIBGCC=ON
            -DALSOFT_DLOPEN=OFF
            -DALSOFT_WERROR=OFF
            -DALSOFT_UTILS=OFF
            -DALSOFT_NO_CONFIG_UTIL=ON
            -DALSOFT_EXAMPLES=OFF
            -DALSOFT_TESTS=OFF
            -DALSOFT_CONFIG=OFF
            -DALSOFT_HRTF_DEFS=OFF
            -DALSOFT_AMBDEC_PRESETS=OFF
            -DALSOFT_NO_UID_DEFS=OFF
            -DALSOFT_EMBED_HRTF_DATA=OFF
            -DALSOFT_BUILD_IMPORT_LIB=OFF
            -DALSOFT_BACKEND_WAVE=OFF
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
