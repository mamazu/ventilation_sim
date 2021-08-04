#include "own_imgui.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include <array>

constexpr std::array<char*, 4> itemLabels { "Air", "Snow", "Wall", "Sand" };

void menuBar(World& world, bool& isDemoVisible)
{
    if (!ImGui::BeginMainMenuBar()) {
        return;
    }

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            clearWorld(world);
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            saveWorldToFile(world, "world.dat");
        }
        if (ImGui::MenuItem("Load", "Ctrl+O")) {
            loadWorldFromFile(world, "world.dat");
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("ImGui demo")) {
            isDemoVisible = !isDemoVisible;
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void addBrushTreeNode(SimulationSettings& settings)
{
    for (size_t i = 0; i < itemLabels.size(); i++) {
        if (ImGui::RadioButton(itemLabels[i], (settings.currentTool == static_cast<Cell>(i)))) {
            settings.currentTool = static_cast<Cell>(i);
        }
        if ((i + 1) < itemLabels.size()) {
            ImGui::SameLine();
        }
    }

    ImGui::SliderInt("Size", &settings.brushSize, 1, 100);
}

void addSimulationSettingsNode(SimulationSettings& settings)
{
    ImGui::SliderInt("Time between steps (ms)", &settings.timeBetweenStepsInMilliseconds, 0, 1000);
    ImGui::Checkbox("Pause", &settings.isPaused);
}

void renderUI(World& world, SimulationSettings& settings, bool& isDemoVisible)
{
    menuBar(world, isDemoVisible);

    ImGui::Begin("Toolbox");
    addBrushTreeNode(settings);
    addSimulationSettingsNode(settings);
    ImGui::End();

    if (isDemoVisible) {
        ImGui::ShowDemoWindow(&isDemoVisible);
    }
}
