#include "simulation.hpp"
#include <cassert>

namespace {
char CellToChar(const Cell value)
{
    switch (value) {
    case Cell::Air:
        return '_';
    case Cell::Snow:
        return '*';
    case Cell::Wall:
        return 'W';
    case Cell::Sand:
        return 'S';
    case Cell::Eraser:
        return 'E';
    }
    VENT_UNREACHABLE();
}
}

World::World(const Point& size, Cell defaultMaterial)
    : Cells(size.x * size.y, defaultMaterial)
    , Width(size.x)
{
}

World::World(size_t width, std::initializer_list<Cell> cells)
    : Cells(cells)
    , Width(width)
{
    assert((Cells.size() % width) == 0);
}

bool operator==(const World& left, const World& right) noexcept
{
    return (left.Cells == right.Cells) && (left.Width == right.Width);
}

std::ostream& operator<<(std::ostream& out, const World& value)
{
    for (size_t i = 0; i < value.Cells.size(); i++) {
        out << CellToChar(value.Cells[i]);
        if ((i != value.Cells.size() - 1) && (i % value.Width == value.Width - 1)) {
            out << '\n';
        }
    }
    return out;
}

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

namespace {
bool canFallInto(const Cell into)
{
    switch (into) {
    case Cell::Air:
    case Cell::Eraser:
        return true;
    case Cell::Snow:
    case Cell::Sand:
    case Cell::Wall:
        return false;
    }
    VENT_UNREACHABLE();
}

[[nodiscard]] Cell fall(const Cell top, const Cell bottom)
{
    assert(canFallInto(bottom));
    switch (bottom) {
    case Cell::Air:
        return top;

    case Cell::Eraser:
        return bottom;

    case Cell::Snow:
    case Cell::Sand:
    case Cell::Wall:
        VENT_UNREACHABLE();
    }
    VENT_UNREACHABLE();
}
}

World simulateStep(const World& world)
{
    const ptrdiff_t worldWidth = world.Width;
    if (worldWidth == 0) {
        return world;
    }
    const ptrdiff_t worldHeight = world.Cells.size() / world.Width;
    World result(Point { worldWidth, worldHeight }, Cell::Air);

    // std::vector::operator[] is very expensive on Debug under MSVC, so we use this pointer instead
    Cell* const newWorld = result.Cells.data();

    size_t cellIndex = result.Cells.size();
    for (ptrdiff_t y = (worldHeight - 1); y >= 0; --y) {
        for (ptrdiff_t x = (worldWidth - 1); x >= 0; --x) {
            --cellIndex;
            // std::vector::operator[] is very expensive on Debug under MSVC, so we use this pointer instead
            const Cell& cell = world.Cells.data()[cellIndex];
            switch (cell) {
            case Cell::Air:
                continue;

            case Cell::Snow: {
                const size_t belowIndex = (cellIndex + worldWidth);
                if ((belowIndex < result.Cells.size()) && canFallInto(newWorld[belowIndex])) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[belowIndex] = fall(cell, newWorld[belowIndex]);
                } else {
                    newWorld[cellIndex] = cell;
                }
                break;
            }

            case Cell::Sand: {
                const size_t belowIndex = (cellIndex + worldWidth);
                if (belowIndex < result.Cells.size()) {
                    if (canFallInto(newWorld[belowIndex])) {
                        newWorld[cellIndex] = Cell::Air;
                        newWorld[belowIndex] = fall(cell, newWorld[belowIndex]);
                        break;
                    }

                    if (x < (worldWidth - 1)) {
                        const size_t belowRightIndex = (belowIndex + 1);
                        if (canFallInto(newWorld[belowRightIndex])) {
                            newWorld[cellIndex] = Cell::Air;
                            newWorld[belowRightIndex] = fall(cell, newWorld[belowRightIndex]);
                            break;
                        }
                    }

                    if (x > 0) {
                        const size_t belowLeftIndex = (belowIndex - 1);
                        if (canFallInto(newWorld[belowLeftIndex])) {
                            newWorld[cellIndex] = Cell::Air;
                            newWorld[belowLeftIndex] = fall(cell, newWorld[belowLeftIndex]);
                            break;
                        }
                    }
                }

                newWorld[cellIndex] = cell;
                break;
            }

            case Cell::Wall:
            case Cell::Eraser:
                newWorld[cellIndex] = cell;
                break;
            }
        }
    }
    return result;
}
