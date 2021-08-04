#include "simulation.hpp"
#include <array>
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

size_t World::getEmptyCells() const
{
    return std::count(Cells.begin(), Cells.end(), Cell::Air);
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

std::pair<CellsChanged, World> simulateStep(const World& world)
{
    const ptrdiff_t worldWidth = world.Width;
    size_t cellsChanged = 0;
    if (worldWidth == 0) {
        return { 0, world };
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
                    cellsChanged++;
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
                        cellsChanged++;
                        break;
                    }

                    const std::array<bool, 2> isWithinBounds = {
                        (x < (worldWidth - 1)),
                        (x > 0)
                    };

                    static constexpr std::array<ptrdiff_t, 2> horizontalOffsets = {
                        1,
                        -1
                    };

                    bool fellToTheSide = false;
                    for (size_t i = 0; i < horizontalOffsets.size(); ++i) {
                        if (!isWithinBounds[i]) {
                            continue;
                        }
                        const size_t belowLeftRightIndex = (belowIndex + horizontalOffsets[i]);
                        if (canFallInto(newWorld[belowLeftRightIndex])) {
                            newWorld[cellIndex] = Cell::Air;
                            newWorld[belowLeftRightIndex] = fall(cell, newWorld[belowLeftRightIndex]);
                            fellToTheSide = true;
                            cellsChanged++;
                            break;
                        }
                    }
                    if (fellToTheSide) {
                        break;
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
    return { cellsChanged, result };
}

void setRectangle(World& world, const Point& center, const Point& worldSize, const SimulationSettings& settings)
{
    float nextDot = 0;
    for (ptrdiff_t y = -settings.brushSize; y < settings.brushSize; ++y) {
        for (ptrdiff_t x = -settings.brushSize; x < settings.brushSize; ++x) {
            if (nextDot >= 1.0) {
                nextDot -= 1.0;
                const std::optional<size_t> index = getIndexFromCoordinates(Point(
                                                                                static_cast<ptrdiff_t>(center.x) + x,
                                                                                static_cast<ptrdiff_t>(center.y) + y),
                    worldSize);
                if (index) {
                    world.Cells[*index] = settings.currentMaterial;
                }
            }

            nextDot += settings.brushStrength;
        }
    }
}
