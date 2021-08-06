#include "simulation.hpp"
#include <benchmark/benchmark.h>

static void BM_simulateStep(benchmark::State& state)
{
    const Point worldSize(500, 500);
    World input(worldSize.x * worldSize.y, Cell::Sand);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simulateStep(input.front(), worldSize));
    }
}
BENCHMARK(BM_simulateStep)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
