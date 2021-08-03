#pragma once

#include <string>
#include <vector>
#include "simulation.hpp"

struct SimulationSettings {
    int timeBetweenStepsInMilliseconds = 3;
    bool isPaused = false;
    int brushSize = 20;
    Cell currentTool = Cell::Snow;
};

void clearWorld(World& world);
void saveWorldToFile(const World& world, const std::string& fileName);
void loadWorldFromFile(World& world, const std::string& fileName);
