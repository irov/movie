cmake_minimum_required(VERSION 3.0)

PROJECT(movie C)

OPTION(LIBMOVIE_EXTERNAL_BUILD  "LIBMOVIE_EXTERNAL_BUILD" OFF)
OPTION(LIBMOVIE_EXAMPLES_BUILD  "LIBMOVIE_EXAMPLES_BUILD" ON)
OPTION(LIBMOVIE_COVERAGE  "LIBMOVIE_COVERAGE" OFF)
OPTION(LIBMOVIE_INSTALL  "LIBMOVIE_INSTALL" OFF)
OPTION(LIBMOVIE_TEST  "LIBMOVIE_TEST" OFF)
OPTION(LIBMOVIE_MEMORY_DEBUG "LIBMOVIE_MEMORY_DEBUG" OFF)

IF( NOT LIBMOVIE_EXTERNAL_BUILD )
    if(${CMAKE_C_COMPILER_ID} STREQUAL Clang)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
    elseif(${CMAKE_C_COMPILER_ID} STREQUAL AppleClang)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
    elseif(${CMAKE_C_COMPILER_ID} STREQUAL GNU)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
    elseif(${CMAKE_C_COMPILER_ID} STREQUAL Intel)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
    elseif(${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall /WX")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd5045")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4668")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4710")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4738")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4820")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4711")
    endif()
ENDIF()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

IF(LIBMOVIE_COVERAGE AND CMAKE_COMPILER_IS_GNUCXX)
    include(CodeCoverage)
    setup_target_for_coverage(${PROJECT_NAME}_coverage tests coverage)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
ENDIF()

MACRO( ADD_FILTER group_name )
	SOURCE_GROUP( ${group_name} FILES ${ARGN} )
	SET( SRC_FILES ${SRC_FILES} ${ARGN} )
ENDMACRO()

SET( INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include )
SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )

ADD_FILTER(
public
	${INCLUDE_DIR}/movie/movie.h
    ${INCLUDE_DIR}/movie/movie.hpp
    ${INCLUDE_DIR}/movie/movie_config.h
    ${INCLUDE_DIR}/movie/movie_data.h
    ${INCLUDE_DIR}/movie/movie_instance.h
    ${INCLUDE_DIR}/movie/movie_composition.h
    ${INCLUDE_DIR}/movie/movie_render.h
    ${INCLUDE_DIR}/movie/movie_providers.h
    ${INCLUDE_DIR}/movie/movie_resource.h
    ${INCLUDE_DIR}/movie/movie_skeleton.h
    ${INCLUDE_DIR}/movie/movie_type.h
    ${INCLUDE_DIR}/movie/movie_typedef.h
    ${INCLUDE_DIR}/movie/movie_version.h
)

ADD_FILTER(
source
    ${SOURCE_DIR}/movie_detail.h
    ${SOURCE_DIR}/movie_bezier.c
    ${SOURCE_DIR}/movie_bezier.h
	${SOURCE_DIR}/movie_data.c
    ${SOURCE_DIR}/movie_debug.h
    ${SOURCE_DIR}/movie_instance.c
    ${SOURCE_DIR}/movie_math.c
    ${SOURCE_DIR}/movie_math.h
    ${SOURCE_DIR}/movie_utils.h
    ${SOURCE_DIR}/movie_memory.h
    ${SOURCE_DIR}/movie_composition.c
    ${SOURCE_DIR}/movie_providers.c
    ${SOURCE_DIR}/movie_skeleton.c
    ${SOURCE_DIR}/movie_stream.c
    ${SOURCE_DIR}/movie_stream.h
    ${SOURCE_DIR}/movie_struct.h
    ${SOURCE_DIR}/movie_transformation.c
    ${SOURCE_DIR}/movie_transformation.h
)

INCLUDE_DIRECTORIES( ${PROJECT_NAME} ${INCLUDE_DIR} )

if(LIBMOVIE_MEMORY_DEBUG)
    ADD_DEFINITIONS(-DAE_MOVIE_MEMORY_DEBUG)
endif()

ADD_LIBRARY( ${PROJECT_NAME} STATIC ${SRC_FILES} )

if(LIBMOVIE_INSTALL)
    install(DIRECTORY include
            DESTINATION .
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

    install(TARGETS ${PROJECT_NAME}
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
endif()

if(LIBMOVIE_EXAMPLES_BUILD)
    add_subdirectory(examples)
endif()

if(LIBMOVIE_TEST)
    include(CTest)
    enable_testing()

    add_subdirectory(tests)
endif(LIBMOVIE_TEST)