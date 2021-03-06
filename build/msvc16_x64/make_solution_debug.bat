@echo off

set "CONFIGURATION=Debug"
set "SOLUTION_DIR=..\solutions\libmovie_msvc16_x64_%CONFIGURATION%"

@pushd ..
@mkdir %SOLUTION_DIR%
@pushd %SOLUTION_DIR%

CMake -G "Visual Studio 16 2019" -A x64 "%CD%\..\.." -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% -DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION% -DLIBMOVIE_EXAMPLES_BUILD:BOOL=TRUE

@popd
@popd

@echo on
@pause
