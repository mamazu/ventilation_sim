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
                const std::optional<size_t> belowIndex = getIndexFromCoordinates(Point(x, y + 1), worldSize);
                const std::optional<size_t> belowRightIndex = getIndexFromCoordinates(Point(x + 1, y + 1), worldSize);
                const std::optional<size_t> belowLeftIndex = getIndexFromCoordinates(Point(x - 1, y + 1), worldSize);
                if (belowIndex && isPermissive(newWorld[*belowIndex])) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[*belowIndex] = cell;
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
