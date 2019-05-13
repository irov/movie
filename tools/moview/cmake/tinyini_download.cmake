include(ExternalProject)

ExternalProject_Add(tinyini_download PREFIX tinyini
        GIT_REPOSITORY https://github.com/irov/tinyini.git
		GIT_PROGRESS TRUE
            
        UPDATE_COMMAND ""
      
        CMAKE_ARGS 
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DTINYINI_EXTERNAL_BUILD=ON
			-DTINYINI_INSTALL=ON
            
    )

ExternalProject_Get_Property(tinyini_download INSTALL_DIR)
set(TINYINI_INCLUDE_DIR ${INSTALL_DIR}/include)
set(TINYINI_LIBRARY_DIR ${INSTALL_DIR}/lib)

add_library(tinyini STATIC IMPORTED)

set_target_properties(tinyini PROPERTIES IMPORTED_LOCATION ${TINYINI_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}tinyini${CMAKE_STATIC_LIBRARY_SUFFIX})