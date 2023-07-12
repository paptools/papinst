#include <demo/demo.h>

#include <benchmark/benchmark.h>

namespace benchmarks {
void IsSuccessExitCode(benchmark::State &state) {
  for (auto _ : state) {
    demo::IsSuccessExitCode(state.range(0));
  }
}
} // namespace benchmarks

BENCHMARK(benchmarks::IsSuccessExitCode)->DenseRange(0, 40, 8);
