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

World simulateStep(const Cell& front, const Point& worldSize)
{
    const ptrdiff_t worldWidth = worldSize.x;
    const ptrdiff_t worldHeight = worldSize.y;
    World result(worldSize, Cell::Air);

    // std::vector::operator[] is very expensive on Debug under MSVC, so we use this pointer instead
    Cell* const newWorld = result.Cells.data();

    size_t cellIndex = result.Cells.size();
    for (ptrdiff_t y = (worldHeight - 1); y >= 0; --y) {
        for (ptrdiff_t x = (worldWidth - 1); x >= 0; --x) {
            --cellIndex;
            const Cell& cell = (&front)[cellIndex];
            switch (cell) {
            case Cell::Air:
                continue;

            case Cell::Snow: {
                const size_t belowIndex = (cellIndex + worldWidth);
                if ((belowIndex < result.Cells.size()) && (newWorld[belowIndex] == Cell::Air)) {
                    newWorld[cellIndex] = Cell::Air;
                    newWorld[belowIndex] = cell;
                } else {
                    newWorld[cellIndex] = cell;
                }
                break;
            }

            case Cell::Sand: {
                const size_t belowIndex = (cellIndex + worldWidth);
                if (belowIndex < result.Cells.size()) {
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
