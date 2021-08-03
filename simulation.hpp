#pragma once
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <vector>

enum class Cell : char {
    Air,
    Snow,
    Wall,
};

using Point = sf::Vector2<ptrdiff_t>;

std::optional<size_t> getIndexFromCoordinates(const Point& coordinates, const Point worldSize);
bool isPermissive(const Cell& cell);
std::vector<Cell> simulateStep(const Cell& front, const Point& worldSize);
