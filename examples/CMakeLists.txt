set(
sandbox_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sandbox/sandbox.c
)

INCLUDE_DIRECTORIES(sandbox ${INCLUDE_DIR})

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

ADD_EXECUTABLE(sandbox ${sandbox_SOURCES})

set_target_properties (sandbox PROPERTIES
        FOLDER examples
    )

TARGET_LINK_LIBRARIES(sandbox movie)

set(
common_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/common.h
)    

ADD_LIBRARY(example_common STATIC ${common_SOURCES})

set_target_properties (example_common PROPERTIES
        FOLDER examples
    )

set(
example1_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/example1.c
)

INCLUDE_DIRECTORIES(example1 ${INCLUDE_DIR})

ADD_EXECUTABLE(example1 ${example1_SOURCES})

set_target_properties (example1 PROPERTIES
        FOLDER examples
    )

TARGET_LINK_LIBRARIES(example1 example_common movie)

set(
example2_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/example2.c
)

INCLUDE_DIRECTORIES(example2 ${INCLUDE_DIR})

ADD_EXECUTABLE(example2 ${example2_SOURCES})

set_target_properties (example2 PROPERTIES
        FOLDER examples
    )

TARGET_LINK_LIBRARIES(example2 example_common movie)

set(
example3_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/example3.c
)

INCLUDE_DIRECTORIES(example3 ${INCLUDE_DIR})

ADD_EXECUTABLE(example3 ${example3_SOURCES})

set_target_properties (example3 PROPERTIES
        FOLDER examples
    )

TARGET_LINK_LIBRARIES(example3 example_common movie)