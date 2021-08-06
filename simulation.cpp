#include "simulation.hpp"

std::optional<size_t> getIndexFromCoordinates(const Point& coordinates, const Point worldSize)
{
    if ((coordinates.x < 0) || (coordinates.x >= worldSize.x)) {
        return std::nullopt;
    }
    if ((coordinates.y < 0) || (coordinates.y >= worldSize.y)) {
        return std::nullopt;
    }
    return (coordinates.y * worldSize.x) + coordinates.x;
}

ptrdiff_t getIndexFromCoordinates2(const Point& coordinates, const Point worldSize)
{
    if ((coordinates.x < 0) || (coordinates.x >= worldSize.x)) {
        return -1;
    }
    if ((coordinates.y < 0) || (coordinates.y >= worldSize.y)) {
        return -1;
    }
    return (coordinates.y * worldSize.x) + coordinates.x;
}

bool isPermissive(const Cell& cell)
{
    return cell == Cell::Air;
}

World simulateStep(const Cell& front, const Point& worldSize)
{
    const ptrdiff_t worldWidth = worldSize.x;
    const ptrdiff_t worldHeight = worldSize.y;
    std::vector<Cell> newWorld(worldWidth * worldHeight);
    size_t cellIndex = newWorld.size();
    for (ptrdiff_t y = (worldHeight - 1); y >= 0; --y) {
        for (ptrdiff_t x = (worldWidth - 1); x >= 0; --x) {
            --cellIndex;
            const Cell& cell = (&front)[cellIndex];
            switch (cell) {
            case Cell::Air:
                continue;

            case Cell::Snow: {
                const std::optional<size_t> belowIndex = getIndexFromCoordinates(Point(x, y + 1), worldSize);
                if (belowIndex && isPermissive(newWorld[*belowIndex])) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[*belowIndex] = cell;
                } else {
                    newWorld[cellIndex] = cell;
                }
                break;
            }

            case Cell::Sand: {
                // we don't use std::optional here due to very bad Debug performance under MSVC
                const size_t belowIndex = (cellIndex + worldWidth);
                const bool hasBelow = (belowIndex < newWorld.size());

                const std::optional<size_t> belowRightIndex = (hasBelow && (x < (worldWidth - 1))) ? (belowIndex + 1) : std::optional<size_t>();
                const std::optional<size_t> belowLeftIndex = (hasBelow && (x > 0)) ? (belowIndex - 1) : std::optional<size_t>();
                if (hasBelow && isPermissive(newWorld[belowIndex])) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[belowIndex] = cell;
                } else if (belowRightIndex && isPermissive(newWorld[*belowRightIndex])) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[*belowRightIndex] = cell;
                } else if (belowLeftIndex && isPermissive(newWorld[*belowLeftIndex])) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[*belowLeftIndex] = cell;
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
