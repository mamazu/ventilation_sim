#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "simulation.hpp"

TEST_CASE("empty world")
{
    Cell dummyWorld;
    const std::vector<Cell> result = simulateStep(dummyWorld, sf::Vector2u(0, 0));
    const std::vector<Cell> expected;
    REQUIRE(expected == result);
}

TEST_CASE("simplest non-empty world")
{
    std::vector<Cell> world = { Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 1));
    const std::vector<Cell> expected = { Cell::Air };
    REQUIRE(expected == result);
}

TEST_CASE("wall does not fall")
{
    std::vector<Cell> world = { Cell::Wall, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 2));
    const std::vector<Cell> expected = { Cell::Wall, Cell::Air };
    REQUIRE(expected == result);
}

TEST_CASE("snow falls")
{
    std::vector<Cell> world = { Cell::Snow, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 2));
    const std::vector<Cell> expected = { Cell::Air, Cell::Snow };
    REQUIRE(expected == result);
}

TEST_CASE("snow falls with gaps")
{
    std::vector<Cell> world = { Cell::Snow, Cell::Snow, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 3));
    const std::vector<Cell> expected = { Cell::Snow, Cell::Air, Cell::Snow };
    REQUIRE(expected == result);
}

TEST_CASE("snow collects at the bottom")
{
    std::vector<Cell> world = { Cell::Snow };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 1));
    const std::vector<Cell> expected = { Cell::Snow };
    REQUIRE(expected == result);
}

TEST_CASE("wall stops snow")
{
    std::vector<Cell> world = { Cell::Snow, Cell::Wall };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 2));
    const std::vector<Cell> expected = { Cell::Snow, Cell::Wall };
    REQUIRE(expected == result);
}

TEST_CASE("snow stops snow")
{
    std::vector<Cell> world = { Cell::Snow, Cell::Snow };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 2));
    const std::vector<Cell> expected = { Cell::Snow, Cell::Snow };
    REQUIRE(expected == result);
}
