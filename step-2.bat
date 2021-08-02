.\vcpkg\vcpkg.exe install imgui-sfml:x64-windows catch2:x64-windows || exit /B 1
.\vcpkg\downloads\tools\cmake-3.20.2-windows\cmake-3.20.2-windows-i386\bin\cmake.exe -B ..\build_ventilation_sim -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake || exit /B 1
