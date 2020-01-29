include(ExternalProject)

ExternalProject_Add(glfw_download PREFIX glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG "3.3"
        GIT_PROGRESS TRUE
            
        UPDATE_COMMAND ""
      
        CMAKE_ARGS 
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DBUILD_SHARED_LIBS=OFF
            -DGLFW_BUILD_EXAMPLES=OFF
            -DGLFW_BUILD_TESTS=OFF
            -DGLFW_BUILD_DOCS=OFF
            -DGLFW_INSTALL=ON
            -DGLFW_VULKAN_STATIC=OFF
            
    )

ExternalProject_Get_Property(glfw_download INSTALL_DIR)
set(GLFW_INCLUDE_DIR ${INSTALL_DIR}/include)
set(GLFW_LIBRARY_DIR ${INSTALL_DIR}/lib)

add_library(glfw STATIC IMPORTED)

set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${GLFW_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX})