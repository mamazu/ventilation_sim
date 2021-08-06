#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "simulation.hpp"

TEST_CASE("empty world")
{
    World dummyWorld(Point(0, 0), Cell::Air);
    const World result = simulateStep(dummyWorld);
    const World expected(Point(0, 0), Cell::Air);
    REQUIRE(expected == result);
}

TEST_CASE("simplest non-empty world")
{
    World world(1, { Cell::Air });
    const World result = simulateStep(world);
    const World expected(1, { Cell::Air });
    REQUIRE(expected == result);
}

TEST_CASE("wall does not fall")
{
    World world(1, { Cell::Wall, Cell::Air });
    const World result = simulateStep(world);
    const World expected(1, { Cell::Wall, Cell::Air });
    REQUIRE(expected == result);
}

TEST_CASE("falling materials")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand);
    World world(1, { material, Cell::Air });
    const World result = simulateStep(world);
    const World expected(1, { Cell::Air, material });
    REQUIRE(expected == result);
}

TEST_CASE("falling materials leave no gaps")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand);
    World world(1, { material, material, Cell::Air });
    const World result = simulateStep(world);
    const World expected(1, { Cell::Air, material, material });
    REQUIRE(expected == result);
}

TEST_CASE("materials collect at the bottom")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Sand);
    World world(1, { material });
    const World result = simulateStep(world);
    const World expected(1, { material });
    REQUIRE(expected == result);
}

TEST_CASE("collisions")
{
    const Cell top = GENERATE(Cell::Wall, Cell::Snow, Cell::Sand);
    const Cell bottom = GENERATE(Cell::Wall, Cell::Snow, Cell::Sand);
    const World world(1, { top, bottom });
    const World result = simulateStep(world);
    REQUIRE(world == result);
}

TEST_CASE("snow stops snow")
{
    World world(1, { Cell::Snow, Cell::Snow });
    const World result = simulateStep(world);
    const World expected(1, { Cell::Snow, Cell::Snow });
    REQUIRE(expected == result);
}

TEST_CASE("snow stacks")
{
    const World world(3, { Cell::Air, Cell::Snow, Cell::Air,
                             // below:
                             Cell::Air, Cell::Snow, Cell::Air });
    const World result = simulateStep(world);
    REQUIRE(world == result);
}

TEST_CASE("sand flows right")
{
    World world(2, { Cell::Sand, Cell::Air,
                       // below:
                       Cell::Sand, Cell::Air });
    const World result = simulateStep(world);
    const World expected(2, { Cell::Air, Cell::Air,
                                // below:
                                Cell::Sand, Cell::Sand });
    REQUIRE(expected == result);
}

TEST_CASE("sand flows left")
{
    World world(2, { Cell::Air, Cell::Sand,
                       // below:
                       Cell::Air, Cell::Sand });
    const World result = simulateStep(world);
    const World expected(2,
        { Cell::Air, Cell::Air,
            // below:
            Cell::Sand, Cell::Sand });
    REQUIRE(expected == result);
}

TEST_CASE("sand doesn't disappear")
{
    World world(3, { Cell::Sand, Cell::Sand, Cell::Sand,
                       // below:
                       Cell::Sand, Cell::Air, Cell::Air });
    const World result = simulateStep(world);
    const World expected(3, { Cell::Sand, Cell::Air, Cell::Air,
                                // below:
                                Cell::Sand, Cell::Sand, Cell::Sand });
    REQUIRE(expected == result);
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
