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

World simulateStep(const Cell& front, const Point& worldSize)
{
    const ptrdiff_t worldWidth = worldSize.x;
    const ptrdiff_t worldHeight = worldSize.y;
    std::vector<Cell> result(worldWidth * worldHeight);

    // std::vector::operator[] is very expensive on Debug under MSVC, so we use this pointer instead
    Cell* const newWorld = result.data();

    size_t cellIndex = result.size();
    for (ptrdiff_t y = (worldHeight - 1); y >= 0; --y) {
        for (ptrdiff_t x = (worldWidth - 1); x >= 0; --x) {
            --cellIndex;
            const Cell& cell = (&front)[cellIndex];
            switch (cell) {
            case Cell::Air:
                continue;

            case Cell::Snow: {
                const std::optional<size_t> belowIndex = getIndexFromCoordinates(Point(x, y + 1), worldSize);
                if (belowIndex && (newWorld[*belowIndex] == Cell::Air)) {
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
                const bool hasBelow = (belowIndex < result.size());
                if (hasBelow) {
                    if (newWorld[belowIndex] == Cell::Air) {
                        newWorld[cellIndex] = Cell::Air;
                        newWorld[belowIndex] = cell;
                        break;
                    }

                    if (x < (worldWidth - 1)) {
                        const size_t belowRightIndex = (belowIndex + 1);
                        if (newWorld[belowRightIndex] == Cell::Air) {
                            newWorld[cellIndex] = Cell::Air;
                            newWorld[belowRightIndex] = cell;
                            break;
                        }
                    }

                    if (x > 0) {
                        const size_t belowLeftIndex = (belowIndex - 1);
                        if (newWorld[belowLeftIndex] == Cell::Air) {
                            newWorld[cellIndex] = Cell::Air;
                            newWorld[belowLeftIndex] = cell;
                            break;
                        }
                    }
                }

                newWorld[cellIndex] = cell;
                break;
            }

            case Cell::Wall:
                newWorld[cellIndex] = cell;
                break;
            }
        }
    }
    return result;
}
