#pragma once

#include "simulation.hpp"
#include <chrono>
#include <string>
#include <vector>

struct ProfilingInfo {
    size_t cellsChanged;
    size_t nonEmptyCells;
    std::chrono::milliseconds simulationTime;
    std::chrono::milliseconds renderTime;
};

void clearWorld(World& world);
void saveWorldToFile(const World& world, const std::string& fileName);
void loadWorldFromFile(World& world, const std::string& fileName);
