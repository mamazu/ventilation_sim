#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "simulation.hpp"

TEST_CASE("empty world")
{
    Cell dummyWorld;
    const std::vector<Cell> result = simulateStep(dummyWorld, Point(0, 0));
    const std::vector<Cell> expected;
    REQUIRE(expected == result);
}

TEST_CASE("simplest non-empty world")
{
    std::vector<Cell> world = { Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), Point(1, 1));
    const std::vector<Cell> expected = { Cell::Air };
    REQUIRE(expected == result);
}

TEST_CASE("wall does not fall")
{
    std::vector<Cell> world = { Cell::Wall, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), Point(1, 2));
    const std::vector<Cell> expected = { Cell::Wall, Cell::Air };
    REQUIRE(expected == result);
}

TEST_CASE("falling materials")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand);
    std::vector<Cell> world = { material, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), Point(1, 2));
    const std::vector<Cell> expected = { Cell::Air, material };
    REQUIRE(expected == result);
}

TEST_CASE("falling materials leave no gaps")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand);
    std::vector<Cell> world = { material, material, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), Point(1, 3));
    const std::vector<Cell> expected = { Cell::Air, material, material };
    REQUIRE(expected == result);
}

TEST_CASE("materials collect at the bottom")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand);
    std::vector<Cell> world = { material };
    const std::vector<Cell> result = simulateStep(world.front(), Point(1, 1));
    const std::vector<Cell> expected = { material };
    REQUIRE(expected == result);
}

TEST_CASE("collisions")
{
    const Cell top = GENERATE(Cell::Wall, Cell::Snow, Cell::Sand);
    const Cell bottom = GENERATE(Cell::Wall, Cell::Snow, Cell::Sand);
    const std::vector<Cell> world = { top, bottom };
    const std::vector<Cell> result = simulateStep(world.front(), Point(1, 2));
    REQUIRE(world == result);
}

TEST_CASE("snow stops snow")
{
    std::vector<Cell> world = {
        Cell::Snow,
        Cell::Snow
    };
    const std::vector<Cell> result = simulateStep(world.front(), Point(1, 2));
    const std::vector<Cell> expected = { Cell::Snow, Cell::Snow };
    REQUIRE(expected == result);
}

TEST_CASE("snow stacks")
{
    const std::vector<Cell> world = {
        Cell::Air, Cell::Snow, Cell::Air,
        // below:
        Cell::Air, Cell::Snow, Cell::Air
    };
    const std::vector<Cell> result = simulateStep(world.front(), Point(3, 2));
    REQUIRE(world == result);
}

TEST_CASE("sand flows right")
{
    std::vector<Cell> world = {
        Cell::Sand, Cell::Air,
        // below:
        Cell::Sand, Cell::Air
    };
    const std::vector<Cell> result = simulateStep(world.front(), Point(2, 2));
    const std::vector<Cell> expected = {
        Cell::Air, Cell::Air,
        // below:
        Cell::Sand, Cell::Sand
    };
    REQUIRE(expected == result);
}

TEST_CASE("sand flows left")
{
    std::vector<Cell> world = {
        Cell::Air, Cell::Sand,
        // below:
        Cell::Air, Cell::Sand
    };
    const std::vector<Cell> result = simulateStep(world.front(), Point(2, 2));
    const std::vector<Cell> expected = {
        Cell::Air, Cell::Air,
        // below:
        Cell::Sand, Cell::Sand
    };
    REQUIRE(expected == result);
}

TEST_CASE("sand doesn't disappear")
{
    const std::vector<Cell> world = {
        Cell::Sand, Cell::Sand, Cell::Sand,
        // below:
        Cell::Sand, Cell::Air, Cell::Air
    };
    const std::vector<Cell> result = simulateStep(world.front(), Point(3, 2));
    const std::vector<Cell> expected = {
        Cell::Sand, Cell::Air, Cell::Air,
        // below:
        Cell::Sand, Cell::Sand, Cell::Sand
    };
    REQUIRE(expected == result);
}

TEST_CASE("getIndexFromCoordinates out of bounds")
{
    REQUIRE(0 == getIndexFromCoordinates(Point(0, 0), Point(1, 1)));

    REQUIRE(!getIndexFromCoordinates(Point(-1, 0), Point(1, 1)));
    REQUIRE(!getIndexFromCoordinates(Point(1, 0), Point(1, 1)));

    REQUIRE(!getIndexFromCoordinates(Point(0, -1), Point(1, 1)));
    REQUIRE(!getIndexFromCoordinates(Point(0, 1), Point(1, 1)));
}
