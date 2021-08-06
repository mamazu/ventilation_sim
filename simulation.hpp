#pragma once
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <vector>

enum class Cell : char {
    Air,
    Snow,
    Wall,
    Sand
};

using World = std::vector<Cell>;
using Point = sf::Vector2<ptrdiff_t>;

std::optional<size_t> getIndexFromCoordinates(const Point& coordinates, const Point worldSize);
World simulateStep(const Cell& front, const Point& worldSize);
