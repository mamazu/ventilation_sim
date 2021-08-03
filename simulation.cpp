#include "simulation.hpp"

size_t getIndexFromCoordinates(const Point& coordinates, const size_t worldWidth)
{
    return (coordinates.y * worldWidth) + coordinates.x;
}

bool isPermissive(const Cell& cell)
{
    return cell == Cell::Air;
}

std::vector<Cell> simulateStep(const Cell& front, const sf::Vector2u& worldSize)
{
    const size_t worldWidth = worldSize.x;
    const size_t worldHeight = worldSize.y;
    std::vector<Cell> newWorld(worldWidth * worldHeight);
    for (size_t y = 0; y < worldHeight; ++y) {
        for (size_t x = 0; x < worldWidth; ++x) {
            const size_t cellIndex = getIndexFromCoordinates(Point(x, y), worldWidth);
            const Cell& cell = (&front)[cellIndex];
            switch (cell) {
            case Cell::Air:
                continue;

            case Cell::Snow: {
                if (y == (worldHeight - 1)) {
                    newWorld[cellIndex] = cell;
                    continue;
                }
                const size_t belowIndex = getIndexFromCoordinates(Point(x, y + 1), worldWidth);
                const Cell& below = (&front)[belowIndex];
                if (isPermissive(below)) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[belowIndex] = cell;
                } else {
                    newWorld[cellIndex] = cell;
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
