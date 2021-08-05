#pragma once

#include "simulation.hpp"
#include <string>
#include <vector>

struct SimulationSettings {
    int timeBetweenStepsInMilliseconds = 3;
    bool isPaused = false;
    int brushSize = 20;
    Cell currentMaterial = Cell::Snow;
    float brushStrength = 1.0;
};

void clearWorld(World& world);
void saveWorldToFile(const World& world, const std::string& fileName);
void loadWorldFromFile(World& world, const std::string& fileName);
