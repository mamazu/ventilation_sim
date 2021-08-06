#include "simulation.hpp"
#include <benchmark/benchmark.h>

static void BM_simulateStep(benchmark::State& state)
{
    const Point worldSize(500, 500);
    World input(worldSize, Cell::Snow);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simulateStep(input));
    }
}
BENCHMARK(BM_simulateStep)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
