REM Making the cmake path configurable so that this script can also be used in the CI pipeline
IF "%1" == "" (
    SET cmakeExecutable=".\vcpkg\downloads\tools\cmake-3.20.2-windows\cmake-3.20.2-windows-i386\bin\cmake.exe"
) ELSE (
    SET cmakeExecutable="%1"
)

%cmakeExecutable% -B ..\build_ventilation_sim -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake || exit /B 1
