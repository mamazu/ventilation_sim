#include "simulation.hpp"
#include <benchmark/benchmark.h>

static void BM_GetIndexFromCoordinates(benchmark::State& state)
{
    for (auto _ : state) {
        getIndexFromCoordinates(Point(1, 2), Point(2, 2));
    }
}
BENCHMARK(BM_GetIndexFromCoordinates);

static void BM2_GetIndexFromCoordinates(benchmark::State& state)
{
    for (auto _ : state) {
        getIndexFromCoordinates2(Point(1, 2), Point(2, 2));
    }
}
BENCHMARK(BM2_GetIndexFromCoordinates);

static void BM_simulateStep(benchmark::State& state)
{
    const Point worldSize(500, 500);
    World input(worldSize.x * worldSize.y);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simulateStep(input.front(), worldSize));
    }
}
BENCHMARK(BM_simulateStep)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
