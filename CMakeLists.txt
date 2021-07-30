cmake_minimum_required(VERSION 3.20)
project(ventilation_sim)

add_definitions(/std:c++17)

add_executable(ventilation_sim main.cpp)

find_package(SFML COMPONENTS graphics REQUIRED)
find_package(imgui REQUIRED)
find_package(ImGui-SFML REQUIRED)
target_link_libraries(ventilation_sim PRIVATE ImGui-SFML::ImGui-SFML)
