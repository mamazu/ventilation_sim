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

bool isPermissive(const Cell& cell)
{
    return cell == Cell::Air;
}

World simulateStep(const Cell& front, const Point& worldSize)
{
    const size_t worldWidth = worldSize.x;
    const size_t worldHeight = worldSize.y;
    std::vector<Cell> newWorld(worldWidth * worldHeight);
    for (size_t y = 0; y < worldHeight; ++y) {
        for (size_t x = 0; x < worldWidth; ++x) {
            // index has to exist because x and y are already constrained
            const size_t cellIndex = *getIndexFromCoordinates(Point(x, y), worldSize);

            const Cell& cell = (&front)[cellIndex];
            switch (cell) {
            case Cell::Air:
                continue;

            case Cell::Snow: {
                const std::optional<size_t> belowIndex = getIndexFromCoordinates(Point(x, y + 1), worldSize);
                if (belowIndex && isPermissive((&front)[*belowIndex]) && isPermissive(newWorld[*belowIndex])) {
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
                if (belowIndex && isPermissive((&front)[*belowIndex]) && isPermissive(newWorld[*belowIndex])) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[*belowIndex] = cell;
                } else if (belowRightIndex && isPermissive((&front)[*belowRightIndex]) && isPermissive(newWorld[*belowRightIndex])) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[*belowRightIndex] = cell;
                } else if (belowLeftIndex && isPermissive((&front)[*belowLeftIndex]) && isPermissive(newWorld[*belowLeftIndex])) {
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
