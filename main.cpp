#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#define VENT_UNREACHABLE() __assume(false)

namespace {
enum class Cell {
    Air,
    Snow,
    Wall
};

size_t getIndexFromCoordinates(const sf::Vector2i& coordinates, const size_t worldWidth)
{
    return (coordinates.y * worldWidth) + coordinates.x;
}

sf::Color renderCell(const Cell& cell)
{
    switch (cell) {
    case Cell::Air:
        return sf::Color(0, 0, 0, 255);
    case Cell::Snow:
        return sf::Color(255, 255, 255, 255);
    case Cell::Wall:
        return sf::Color(128, 128, 128, 255);
    }
    VENT_UNREACHABLE();
}

void renderWorld(sf::Image& into, const Cell& front, const sf::Vector2u& worldSize)
{
    assert(into.getSize() == worldSize);

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

std::vector<Cell> simulateStep(const Cell& front, const sf::Vector2u& worldSize)
{
    const size_t worldWidth = worldSize.x;
    const size_t worldHeight = worldSize.y;
    std::vector<Cell> newWorld(worldWidth * worldHeight);
    for (size_t y = 0; y < worldHeight; ++y) {
        for (size_t x = 0; x < worldWidth; ++x) {
            const size_t cellIndex = getIndexFromCoordinates(sf::Vector2i(x, y), worldWidth);
            const Cell& cell = (&front)[cellIndex];
            switch (cell) {
            case Cell::Air:
                continue;

            case Cell::Snow: {
                if (y == (worldHeight - 1)) {
                    newWorld[cellIndex] = cell;
                    continue;
                }
                const size_t belowIndex = getIndexFromCoordinates(sf::Vector2i(x, y + 1), worldWidth);
                const Cell& below = (&front)[belowIndex];
                switch (below) {
                case Cell::Air:
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[belowIndex] = cell;
                    break;

                case Cell::Snow:
                case Cell::Wall:
                    newWorld[cellIndex] = cell;
                    break;
                }
                break;
            }

            case Cell::Wall:
                newWorld[cellIndex] = cell;
                break;
            }
        }
    }
    return newWorld;
}

void clearWorld(std::vector<Cell>& world)
{
    std::fill(world.begin(), world.end(), Cell::Air);
}
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Ventilation Simulator 2021");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    const sf::Vector2u worldSize(window.getSize().x, window.getSize().y);

    std::vector<Cell> world(worldSize.x * worldSize.y);

    sf::Image worldImage;
    worldImage.create(worldSize.x, worldSize.y);

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
                    switch (event.mouseButton.button) {
                    case sf::Mouse::Button::Left:
                        isMouseLeftDown = true;
                        break;
                    }
                    mousePosition = sf::Vector2u(event.mouseButton.x, event.mouseButton.y);
                }

                if (event.type == sf::Event::MouseButtonReleased) {
                    switch (event.mouseButton.button) {
                    case sf::Mouse::Button::Left:
                        isMouseLeftDown = false;
                        break;
                    }
                }

                if (event.type == sf::Event::MouseMoved) {
                    mousePosition = sf::Vector2u(event.mouseMove.x, event.mouseMove.y);
                }
            }
        }

        if (isPaused) {
            nextWorldStep = worldStepClock.getElapsedTime();
        } else {
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

                if (isMouseLeftDown) {
                    for (ptrdiff_t y = -brushSize; y < brushSize; ++y) {
                        for (ptrdiff_t x = -brushSize; x < brushSize; ++x) {
                            const size_t index = getIndexFromCoordinates(sf::Vector2i(
                                                                             static_cast<ptrdiff_t>(mousePosition.x) + x,
                                                                             static_cast<ptrdiff_t>(mousePosition.y) + y),
                                worldSize.x);
                            if (index < world.size()) {
                                world[index] = currentToolIndex;
                            }
                        }
                    }
                }

                std::vector<Cell> newWorld = simulateStep(world.front(), worldSize);
                world = std::move(newWorld);
                nextWorldStep += sf::milliseconds(timeBetweenStepsInMilliseconds);
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New", "Ctrl+N")) {
                    clearWorld(world);
                }
                ImGui::MenuItem("Save", "Ctrl+S");
                if (ImGui::MenuItem("Quit", "Ctrl+Q")) {
                    return 0;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Toolbox");
        ImGui::SliderInt("Brush Size", &brushSize, 1, 100);

        const char* items[] = { "Air", "Snow", "Wall" };
        const char* currentTool = items[static_cast<size_t>(currentToolIndex)];

        if (ImGui::BeginCombo("Select tool", currentTool)) {
            for (size_t i = 0; i < 3; i++) {
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
