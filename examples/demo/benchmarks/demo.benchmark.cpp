#include <demo/demo.h>

#include <benchmark/benchmark.h>

namespace benchmarks {
namespace {
// The following function is used to add arguments to a benchmark when you want
// to run on a mix of prime and non-prime values.
void PrimeAndAdjacentArgs(benchmark::internal::Benchmark *b) {
  std::vector<int> primes = {2, 5, 11, 17, 23};
  for (int prime : primes) {
    b->Arg(prime - 1);
    b->Arg(prime);
  }
}
} // namespace

void IsEven(benchmark::State &state) {
  for (auto _ : state) {
    demo::IsEven(state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void ShiftsToZero(benchmark::State &state) {
  for (auto _ : state) {
    demo::ShiftsToZero(state.range(0));
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
    ->DenseRange(1, 30, 3);
BENCHMARK(benchmarks::ShiftsToZero)
    ->Name("ShiftsToZero")
    ->Complexity()
    ->DenseRange(1, 30, 3);
BENCHMARK(benchmarks::Factorial)
    ->Name("Factorial")
    ->Complexity()
    ->DenseRange(1, 30, 3);
BENCHMARK(benchmarks::IsPrime)
    ->Name("IsPrime")
    ->Complexity()
    ->Apply(PrimeAndAdjacentArgs);
} // namespace benchmarks
