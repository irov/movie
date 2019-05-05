cmake_minimum_required(VERSION 3.0)

if(WIN32)
    if(MINGW)
        set(CMAKE_STATIC_LIBRARY_PREFIX lib)
        set(CMAKE_STATIC_LIBRARY_SUFFIX .a)

        set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    else()
        set(CMAKE_STATIC_LIBRARY_PREFIX)
        set(CMAKE_STATIC_LIBRARY_SUFFIX .lib)
    endif()
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_STATIC_LIBRARY_PREFIX lib)
    set(CMAKE_STATIC_LIBRARY_SUFFIX .a)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")
endif()

if(APPLE)
    set(CMAKE_STATIC_LIBRARY_PREFIX lib)
    set(CMAKE_STATIC_LIBRARY_SUFFIX .a)
endif()

set(LIBMOVIE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

include(cmake/macro.cmake)
include(cmake/glad_download.cmake)
include(cmake/glfw_download.cmake)
include(cmake/imgui_download.cmake)
include(cmake/nativefiledialog_download.cmake)
include(cmake/stb_libs_download.cmake)
include(cmake/tinyini_download.cmake)

if(NOT APPLE)
    include(cmake/alsoft_download.cmake)
endif()

set(LIBMOVIE_EXTERNAL_BUILD ON CACHE BOOL "LIBMOVIE_EXTERNAL_BUILD" FORCE)
set(LIBMOVIE_EXAMPLES_BUILD OFF CACHE BOOL "LIBMOVIE_EXAMPLES_BUILD" FORCE)
set(LIBMOVIE_INSTALL ON CACHE BOOL "LIBMOVIE_INSTALL" FORCE)

add_subdirectory(${LIBMOVIE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/movie)

project( moview )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_CXX_STANDARD 14)

set(MOVIEW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MOVIEW_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB files_headers ${MOVIEW_SOURCE_DIR}/*.h)
file(GLOB files_sources ${MOVIEW_SOURCE_DIR}/*.cpp)

include_directories(
    ${MOVIEW_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${NATIVEFILEDIALOG_INCLUDE_DIR}
    ${LIBMOVIE_DIR}/include
    ${DEPENDENCIES_DIR}/stb
    ${ALSOFT_INCLUDE_DIR}
    ${TINYINI_INCLUDE_DIR}
)

if(UNIX AND NOT APPLE)
	FIND_PACKAGE(PkgConfig REQUIRED)
	PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

	LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})
endif()

set(resources_sources )

if(WIN32)
    if(MINGW)
        # resource compilation for mingw
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moview_rc.o
                           COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}/src
                                               -i${CMAKE_CURRENT_SOURCE_DIR}/src/moview.rc
                                               -o ${CMAKE_CURRENT_BINARY_DIR}/moview_rc.o)
        set(resources_sources ${resources_sources} ${CMAKE_CURRENT_BINARY_DIR}/moview_rc.o)
    else()
        set(resources_sources ${resources_sources} ${CMAKE_CURRENT_SOURCE_DIR}/src/moview.rc)
    endif()
endif()

add_executable(${PROJECT_NAME}
    ${files_headers}
    ${files_sources}
    ${resources_sources}
)

if(UNIX AND NOT APPLE)
    MESSAGE("Add Unix library")
    find_library(RT_LIBRARY rt)
    target_link_libraries(${PROJECT_NAME} ${RT_LIBRARY})

    find_library(MATH_LIBRARY m)
    target_link_libraries(${PROJECT_NAME} ${MATH_LIBRARY})

    target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${X11_X11_LIB} ${X11_Xrandr_LIB} ${X11_Xinerama_LIB} ${X11_Xcursor_LIB})

    if(X11_xf86vmode_FOUND)
        target_link_libraries(${PROJECT_NAME} ${X11_Xxf86vm_LIB})
    endif()

    find_package( Threads )
    target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

    target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})
endif()

if(APPLE)
    add_apple_framework(${PROJECT_NAME} Cocoa)
    add_apple_framework(${PROJECT_NAME} IOKit)
    add_apple_framework(${PROJECT_NAME} CoreVideo)
    add_apple_framework(${PROJECT_NAME} OpenAL)
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        shlwapi
        winmm
    )
endif()

if(NOT APPLE)
    target_link_libraries(${PROJECT_NAME}
        alsoft
    )
endif()

target_link_libraries(${PROJECT_NAME}
    movie
    glad
    glfw
    imgui
    nativefiledialog
    tinyini
)

if(NOT APPLE)
    add_dependencies(${PROJECT_NAME}
        alsoft_download
    )
endif()

add_dependencies(${PROJECT_NAME}
    glad_download
    glfw_download
    imgui_download
    nativefiledialog_download
    tinyini_download
)
