#include "imgui-SFML.h"
#include "imgui.h"
#include "simulation.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <array>
#include <fstream>
#include <iostream>

#define VENT_UNREACHABLE() __assume(false)

namespace {
sf::Color renderCell(const Cell& cell)
{
    switch (cell) {
    case Cell::Air:
        return sf::Color(0, 0, 0, 255);
    case Cell::Snow:
        return sf::Color(255, 255, 255, 255);
    case Cell::Wall:
        return sf::Color(128, 128, 128, 255);
    case Cell::Sand:
        return sf::Color(180, 110, 0, 255);
    }
    VENT_UNREACHABLE();
}

void renderWorld(sf::Image& into, const Cell& front, const Point& worldSize)
{
    assert(static_cast<ptrdiff_t>(into.getSize().x) == worldSize.x);
    assert(static_cast<ptrdiff_t>(into.getSize().y) == worldSize.y);

    // pixels are marked const for some reason
    sf::Uint8* const pixels = const_cast<sf::Uint8*>(into.getPixelsPtr());

    const size_t numberOfCells = (worldSize.x * worldSize.y);
    for (size_t i = 0; i < numberOfCells; ++i) {
        const sf::Color cellColor = renderCell((&front)[i]);
        sf::Uint8* const cellPixel = pixels + (i * 4);
        cellPixel[0] = cellColor.r;
        cellPixel[1] = cellColor.g;
        cellPixel[2] = cellColor.b;
        cellPixel[3] = cellColor.a;
    }
}

void clearWorld(std::vector<Cell>& world)
{
    std::fill(world.begin(), world.end(), Cell::Air);
}

void saveWorldToFile(const std::vector<Cell>& world, const std::string& fileName)
{
    std::ofstream file(fileName, std::ofstream::binary);
    file.write(reinterpret_cast<const char*>(world.data()), sizeof(Cell) * world.size());
}

void loadWorldFromFile(std::vector<Cell>& world, const std::string& fileName)
{
    std::ifstream file(fileName, std::ifstream::binary);
    file.read(reinterpret_cast<char*>(world.data()), sizeof(Cell) * world.size());
}

}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Ventilation Simulator 2021");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    const Point worldSize(window.getSize().x, window.getSize().y);

    std::vector<Cell> world(worldSize.x * worldSize.y);

    sf::Image worldImage;
    worldImage.create(static_cast<unsigned>(worldSize.x), static_cast<unsigned>(worldSize.y));

    bool isMouseLeftDown = false;
    int brushSize = 20;
    sf::Vector2u mousePosition;

    sf::Clock worldStepClock;
    sf::Time nextWorldStep = worldStepClock.getElapsedTime();

    int timeBetweenStepsInMilliseconds = 3;
    bool isPaused = false;

    Cell currentToolIndex = Cell::Snow;
    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused()) {
                continue;
            }

            if (!ImGui::GetIO().WantCaptureMouse) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        isMouseLeftDown = true;
                    }
                    mousePosition = sf::Vector2u(event.mouseButton.x, event.mouseButton.y);
                }

                if (event.type == sf::Event::MouseButtonReleased) {
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        isMouseLeftDown = false;
                    }
                }

                if (event.type == sf::Event::MouseMoved) {
                    mousePosition = sf::Vector2u(event.mouseMove.x, event.mouseMove.y);
                }
            }
        }

        if (isMouseLeftDown) {
            for (ptrdiff_t y = -brushSize; y < brushSize; ++y) {
                for (ptrdiff_t x = -brushSize; x < brushSize; ++x) {
                    const std::optional<size_t> index = getIndexFromCoordinates(Point(
                                                                                    static_cast<ptrdiff_t>(mousePosition.x) + x,
                                                                                    static_cast<ptrdiff_t>(mousePosition.y) + y),
                        worldSize);
                    if (index) {
                        world[*index] = currentToolIndex;
                    }
                }
            }
        }

        const sf::Time startedStepping = worldStepClock.getElapsedTime();
        const sf::Time stopStepping = (startedStepping + sf::milliseconds(15));
        for (;;) {
            const sf::Time elapsed = worldStepClock.getElapsedTime();
            if (elapsed >= stopStepping) {
                break;
            }
            if (elapsed < nextWorldStep) {
                break;
            }

            if (!isPaused) {
                std::vector<Cell> newWorld = simulateStep(world.front(), worldSize);
                world = std::move(newWorld);
            }
            nextWorldStep += sf::milliseconds(timeBetweenStepsInMilliseconds);
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        if (ImGui::BeginMainMenuBar()) {
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
                if (ImGui::MenuItem("Quit", "Ctrl+Q")) {
                    return 0;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Toolbox");
        ImGui::SliderInt("Brush Size", &brushSize, 1, 100);

        const char* const items[] = { "Air", "Snow", "Wall", "Sand" };
        const char* currentTool = items[static_cast<size_t>(currentToolIndex)];

        if (ImGui::BeginCombo("Select tool", currentTool)) {
            for (size_t i = 0; i < std::size(items); i++) {
                bool isSelected = (currentTool == items[i]);
                if (ImGui::Selectable(items[i], isSelected)) {
                    currentTool = items[i];
                    currentToolIndex = static_cast<Cell>(i);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::End();

        ImGui::Begin("Speed");
        ImGui::SliderInt("Time between steps (ms)", &timeBetweenStepsInMilliseconds, 0, 1000);
        ImGui::Checkbox("Pause", &isPaused);
        ImGui::End();

        window.clear();

        renderWorld(worldImage, world.front(), worldSize);

        sf::Texture worldTexture;
        if (!worldTexture.loadFromImage(worldImage)) {
            throw std::runtime_error("Could not load image into texture");
        }

        sf::Sprite worldSprite(worldTexture);
        window.draw(worldSprite);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
