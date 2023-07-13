#include <demo/demo.h>

#include <benchmark/benchmark.h>

namespace benchmarks {
void IsEven(benchmark::State &state) {
  for (auto _ : state) {
    demo::IsEven(state.range(0));
  }
}

void Factorial(benchmark::State &state) {
  for (auto _ : state) {
    demo::Factorial(state.range(0));
  }
}

void Fibonacci(benchmark::State &state) {
  for (auto _ : state) {
    demo::Fibonacci(state.range(0));
  }
}
} // namespace benchmarks

BENCHMARK(benchmarks::IsEven)->DenseRange(0, 40, 8);
BENCHMARK(benchmarks::Factorial)->DenseRange(0, 30, 6);
BENCHMARK(benchmarks::Fibonacci)->DenseRange(0, 40, 8);
