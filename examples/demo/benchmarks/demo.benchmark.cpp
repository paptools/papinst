#include <demo/demo.h>

#include <benchmark/benchmark.h>

namespace benchmarks {
void IsEven(benchmark::State &state) {
  for (auto _ : state) {
    demo::IsEven(state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void Factorial(benchmark::State &state) {
  for (auto _ : state) {
    demo::Factorial(state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void IsPrime(benchmark::State &state) {
  for (auto _ : state) {
    demo::IsPrime(state.range(0));
  }
  state.SetComplexityN(state.range(0));
} // namespace benchmarks

BENCHMARK(benchmarks::IsEven)
    ->Name("IsEven")
    ->Complexity()
    ->DenseRange(1, 10, 1);
BENCHMARK(benchmarks::Factorial)
    ->Name("Factorial")
    ->Complexity()
    ->DenseRange(1, 30, 3);
BENCHMARK(benchmarks::IsPrime)
    ->Name("IsPrime")
    ->Complexity()
    ->DenseRange(1, 10, 1);
} // namespace benchmarks
