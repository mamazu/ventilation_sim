#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>

enum class Cell : char {
    Air,
    Snow,
    Wall,
};

using Point = sf::Vector2<ptrdiff_t>;

size_t getIndexFromCoordinates(const Point& coordinates, const size_t worldWidth);
bool isPermissive(const Cell& cell);
std::vector<Cell> simulateStep(const Cell& front, const sf::Vector2u& worldSize);
