#pragma once
#include "main.hpp"
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <ostream>
#include <vector>

#define VENT_UNREACHABLE() __assume(false)

enum class Cell : char {
    Air,
    Snow,
    Wall,
    Sand,
    Eraser
};

using Point = sf::Vector2<ptrdiff_t>;

struct World {
    std::vector<Cell> Cells;
    size_t Width;

    World(const Point& size, Cell defaultMaterial);
    World(size_t width, std::initializer_list<Cell> cells);
};

bool operator==(const World& left, const World& right) noexcept;
std::ostream& operator<<(std::ostream& out, const World& value);

std::optional<size_t> getIndexFromCoordinates(const Point& coordinates, const Point worldSize);
World simulateStep(const World& world);
void setRectangle(World& world, const Point& center, const Point& worldSize, const SimulationSettings& settings);
