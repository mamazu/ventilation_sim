#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "simulation.hpp"

TEST_CASE("filling a rectangle with size 1")
{
    World world(2, {
                       Cell::Air,
                       Cell::Air,
                       // below:
                       Cell::Air,
                       Cell::Air,
                   });

    SimulationSettings settings;
    settings.currentMaterial = Cell::Sand;
    settings.brushSize = 1;
    setRectangle(world, Point(0, 0), Point(2, 2), settings);

    REQUIRE(world.Cells[0] == Cell::Sand);
}

TEST_CASE("filling a rectangle with size 2")
{
    World world(2, {
                       Cell::Air,
                       Cell::Air,
                       // below:
                       Cell::Air,
                       Cell::Air,
                   });

    SimulationSettings settings;
    settings.currentMaterial = Cell::Sand;
    settings.brushSize = 2;
    setRectangle(world, Point(0, 0), Point(2, 2), settings);

    for (const Cell& cell : world.Cells) {
        REQUIRE(cell == Cell::Sand);
    }
}

TEST_CASE("filling a bigger rectangle than the world")
{
    World world(1, {
                       Cell::Air,
                   });

    SimulationSettings settings;
    settings.currentMaterial = Cell::Sand;
    settings.brushSize = 5;
    setRectangle(world, Point(0, 0), Point(1, 1), settings);

    for (const Cell& cell : world.Cells) {
        REQUIRE(cell == Cell::Sand);
    }
}

TEST_CASE("empty world")
{
    const World dummyWorld(Point(0, 0), Cell::Air);
    const World expected(Point(0, 0), Cell::Air);

    const std::pair<CellsChanged, World> result = simulateStep(dummyWorld);
    REQUIRE(0 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("simplest non-empty world")
{
    const World world(1, { Cell::Air });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(1, { Cell::Air });
    REQUIRE(0 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("some materials do not fall")
{
    const Cell notFalling = GENERATE(Cell::Wall, Cell::Eraser);
    const World world(1, { notFalling, Cell::Air });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    REQUIRE(0 == result.first);
    REQUIRE(world == result.second);
}

TEST_CASE("falling materials")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand);
    const World world(1, { material, Cell::Air });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(1, { Cell::Air, material });
    REQUIRE(1 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("falling materials leave no gaps")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand);
    const World world(1, { material, material, Cell::Air });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(1, { Cell::Air, material, material });
    REQUIRE(2 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("materials collect at the bottom")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand, Cell::Eraser, Cell::Wall, Cell::Air);
    const World world(1, { material });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(1, { material });
    REQUIRE(0 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("collisions")
{
    const Cell top = GENERATE(Cell::Wall, Cell::Snow, Cell::Sand, Cell::Eraser);
    const Cell bottom = GENERATE(Cell::Wall, Cell::Snow, Cell::Sand);
    const World world(1, { top, bottom });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    REQUIRE(0 == result.first);
    REQUIRE(world == result.second);
}

TEST_CASE("snow stops snow")
{
    const World world(1, { Cell::Snow, Cell::Snow });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(1, { Cell::Snow, Cell::Snow });
    REQUIRE(0 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("snow stacks")
{
    const World world(3, { Cell::Air, Cell::Snow, Cell::Air,
                             // below:
                             Cell::Air, Cell::Snow, Cell::Air });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    REQUIRE(0 == result.first);
    REQUIRE(world == result.second);
}

TEST_CASE("sand flows right")
{
    const World world(2, { Cell::Sand, Cell::Air,
                             // below:
                             Cell::Sand, Cell::Air });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(2, { Cell::Air, Cell::Air,
                                // below:
                                Cell::Sand, Cell::Sand });
    REQUIRE(1 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("sand flows left")
{
    const World world(2, { Cell::Air, Cell::Sand,
                             // below:
                             Cell::Air, Cell::Sand });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(2,
        { Cell::Air, Cell::Air,
            // below:
            Cell::Sand, Cell::Sand });
    REQUIRE(1 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("sand doesn't disappear")
{
    const World world(3, { Cell::Sand, Cell::Sand, Cell::Sand,
                             // below:
                             Cell::Sand, Cell::Air, Cell::Air });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(3, { Cell::Sand, Cell::Air, Cell::Air,
                                // below:
                                Cell::Sand, Cell::Sand, Cell::Sand });
    REQUIRE(2 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("sand pile falls right")
{
    const World world(4, { Cell::Sand, Cell::Sand, Cell::Air, Cell::Air,
                             // below:
                             Cell::Sand, Cell::Sand, Cell::Air, Cell::Air,
                             // below:
                             Cell::Sand, Cell::Sand, Cell::Air, Cell::Air,
                             // below:
                             Cell::Sand, Cell::Sand, Cell::Air, Cell::Air,
                             // below:
                             Cell::Sand, Cell::Sand, Cell::Air, Cell::Air });
    const World result = simulateStep(world);
    const World expected(4,
        { Cell::Sand, Cell::Air, Cell::Air, Cell::Air,
            // below:
            Cell::Sand, Cell::Sand, Cell::Air, Cell::Air,
            // below:
            Cell::Sand, Cell::Sand, Cell::Air, Cell::Air,
            // below:
            Cell::Sand, Cell::Sand, Cell::Air, Cell::Air,
            // below:+
            Cell::Sand, Cell::Sand, Cell::Sand, Cell::Air });
    REQUIRE(expected == result);
}

TEST_CASE("sand pile falls left")
{
    const World world(4, { Cell::Air, Cell::Air, Cell::Sand, Cell::Sand,
                             // below:
                             Cell::Air, Cell::Air, Cell::Sand, Cell::Sand,
                             // below:
                             Cell::Air, Cell::Air, Cell::Sand, Cell::Sand,
                             // below:
                             Cell::Air, Cell::Air, Cell::Sand, Cell::Sand,
                             // below:
                             Cell::Air, Cell::Air, Cell::Sand, Cell::Sand });
    const World result = simulateStep(world);
    // sand falls faster to the left than to the right currently
    const World expected(4,
        { Cell::Air, Cell::Air, Cell::Air, Cell::Air,
            // below:
            Cell::Air, Cell::Air, Cell::Sand, Cell::Sand,
            // below:
            Cell::Air, Cell::Air, Cell::Sand, Cell::Sand,
            // below:
            Cell::Air, Cell::Sand, Cell::Sand, Cell::Sand,
            // below:
            Cell::Air, Cell::Sand, Cell::Sand, Cell::Sand });
    REQUIRE(expected == result);
}

TEST_CASE("eraser eats falling materials")
{
    const Cell falling = GENERATE(Cell::Snow, Cell::Sand);
    const World world(1, { falling, Cell::Eraser });
    const std::pair<CellsChanged, World> result = simulateStep(world);
    const World expected(1, { Cell::Air, Cell::Eraser });
    REQUIRE(1 == result.first);
    REQUIRE(expected == result.second);
}

TEST_CASE("Printing a world")
{
    std::ostringstream out;
    const World world(3, { Cell::Sand, Cell::Sand, Cell::Sand,
                             // below:
                             Cell::Sand, Cell::Air, Cell::Air });
    out << world;

    REQUIRE(out.str() == "SSS\nS__");
}

TEST_CASE("Printing an empty world")
{
    std::ostringstream out;
    const World world(3, {});
    out << world;

    REQUIRE(out.str() == "");
}

TEST_CASE("Printing a world (width = 1)")
{
    std::ostringstream out;
    const World world(1, { Cell::Air, Cell::Air });
    out << world;

    REQUIRE(out.str() == "_\n_");
}

TEST_CASE("getIndexFromCoordinates out of bounds")
{
    REQUIRE(0 == getIndexFromCoordinates(Point(0, 0), Point(1, 1)));

    REQUIRE(!getIndexFromCoordinates(Point(-1, 0), Point(1, 1)));
    REQUIRE(!getIndexFromCoordinates(Point(1, 0), Point(1, 1)));

    REQUIRE(!getIndexFromCoordinates(Point(0, -1), Point(1, 1)));
    REQUIRE(!getIndexFromCoordinates(Point(0, 1), Point(1, 1)));
}
