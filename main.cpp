#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

namespace {
struct Cell {
    bool IsFilled = false;
};

size_t getIndexFromCoordinates(const sf::Vector2u& coordinates, const size_t worldWidth)
{
    return (coordinates.y * worldWidth) + coordinates.x;
}

sf::Color renderCell(const Cell& cell)
{
    return (cell.IsFilled ? sf::Color(255, 255, 255, 255) : sf::Color(0, 0, 0, 255));
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
            const size_t cellIndex = getIndexFromCoordinates(sf::Vector2u(x, y), worldWidth);
            const Cell& cell = (&front)[cellIndex];
            if (!cell.IsFilled) {
                continue;
            }
            if (y == (worldHeight - 1)) {
                newWorld[cellIndex].IsFilled = true;
                continue;
            }
            const size_t belowIndex = getIndexFromCoordinates(sf::Vector2u(x, y + 1), worldWidth);
            const Cell& below = (&front)[belowIndex];
            if (below.IsFilled) {
                newWorld[cellIndex].IsFilled = true;
            } else {
                newWorld[cellIndex].IsFilled = false;
                newWorld[belowIndex].IsFilled = true;
            }
        }
    }
    return newWorld;
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

    bool isMouseDown = false;
    sf::Vector2u mousePosition;

    sf::Clock worldStepClock;
    sf::Time nextWorldStep = worldStepClock.getElapsedTime();

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (ImGui::IsAnyItemActive()) {
                continue;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                isMouseDown = true;
                mousePosition = sf::Vector2u(event.mouseButton.x, event.mouseButton.y);
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                isMouseDown = false;
            }

            if (event.type == sf::Event::MouseMoved) {
                mousePosition = sf::Vector2u(event.mouseMove.x, event.mouseMove.y);
            }
        }

        if (isMouseDown) {
            const size_t index = getIndexFromCoordinates(mousePosition, worldSize.x);
            if (index < world.size()) {
                world[index].IsFilled = true;
            }
        }

        {
            const sf::Time startedStepping = worldStepClock.getElapsedTime();
            const sf::Time stopStepping = (startedStepping + sf::milliseconds(33));
            for (;;) {
                const sf::Time elapsed = worldStepClock.getElapsedTime();
                if (elapsed >= stopStepping) {
                    break;
                }
                if (elapsed < nextWorldStep) {
                    break;
                }
                std::vector<Cell> newWorld = simulateStep(world.front(), worldSize);
                world = std::move(newWorld);
                nextWorldStep += sf::milliseconds(20);
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("New", "Ctrl+N");
                ImGui::MenuItem("Save", "Ctrl+S");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

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
