cmake_minimum_required(VERSION 3.20)
project(ventilation_sim)

add_definitions(/std:c++17)

# enable some additional warnings
add_definitions(/W4)

# warn about missing cases in switch
add_definitions(/w44062)

# make warnings errors
add_definitions(/WX)

add_library(ventilation STATIC simulation.hpp simulation.cpp)
find_package(SFML COMPONENTS graphics REQUIRED)
target_link_libraries(ventilation PUBLIC sfml-graphics)

find_package(Catch2 REQUIRED)
add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE ventilation Catch2::Catch2)

add_executable(ventilation_sim main.cpp)
find_package(imgui REQUIRED)
find_package(ImGui-SFML REQUIRED)
target_link_libraries(ventilation_sim PRIVATE ventilation ImGui-SFML::ImGui-SFML)
