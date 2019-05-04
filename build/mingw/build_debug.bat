@echo off

set "CONFIGURATION=Debug"
set "SOLUTION_DIR=..\solutions\libmovie_mingw_%CONFIGURATION%"

@pushd ..
@mkdir %SOLUTION_DIR%
@pushd %SOLUTION_DIR%

CMake -G "MinGW Makefiles" "%CD%\..\.." -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% -DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION% -DLIBMOVIE_EXAMPLES_BUILD:BOOL=TRUE

mingw32-make

@popd
@popd

@echo on
@pause
