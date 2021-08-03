#include "own_imgui.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include <array>

constexpr std::array<char*, 4> itemLabels{ "Air", "Snow", "Wall", "Sand" };

void menuBar(World& world)
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
    ImGui::EndMainMenuBar();
}

void addBrushTreeNode(SimulationSettings& settings)
{
    if (!ImGui::TreeNode("Brush Settings")) {
        return;
    }
    ImGui::SliderInt("Size", &settings.brushSize, 1, 100);

    const char* currentLabel = itemLabels[static_cast<size_t>(settings.currentTool)];

    if (ImGui::BeginCombo("Material", currentLabel)) {
        for (size_t i = 0; i < itemLabels.size(); i++) {
            bool isSelected = itemLabels[i] == currentLabel;
            if (ImGui::Selectable(itemLabels[i], isSelected)) {
                currentLabel = itemLabels[i];
                settings.currentTool = static_cast<Cell>(i);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::TreePop();
}

void addSimulationSettingsNode(SimulationSettings& settings)
{
    if (!ImGui::TreeNode("Speed")) {
        return;
    }
    ImGui::SliderInt("Time between steps (ms)", &settings.timeBetweenStepsInMilliseconds, 0, 1000);
    ImGui::Checkbox("Pause", &settings.isPaused);
    ImGui::TreePop();
}

void renderUI(World& world, SimulationSettings& settings)
{
    menuBar(world);

    ImGui::Begin("Toolbox");
    addBrushTreeNode(settings);
    addSimulationSettingsNode(settings);
    ImGui::End();
}
