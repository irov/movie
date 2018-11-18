cmake_minimum_required(VERSION 3.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

macro(ADD_MOVIE_TEST testname)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    
    ADD_EXECUTABLE(test_${testname} test_${testname}.c)
    TARGET_LINK_LIBRARIES(test_${testname} movie)

    set_target_properties (test_${testname} PROPERTIES
        FOLDER tests
    )

    ADD_TEST(NAME ${testname} COMMAND ./test_${testname} ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

ADD_MOVIE_TEST(create_movie_instance)
ADD_MOVIE_TEST(create_movie_data)
ADD_MOVIE_TEST(create_movie_stream)
ADD_MOVIE_TEST(load_movie_data)
ADD_MOVIE_TEST(create_movie_composition)
ADD_MOVIE_TEST(update_movie_composition)
ADD_MOVIE_TEST(compute_movie_mesh)
ADD_MOVIE_TEST(memory_leak)