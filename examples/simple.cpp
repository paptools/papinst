namespace bar {
namespace {
const int foo(int &a, const int b) { return a + b; }
} // namespace
} // namespace bar

int main() {
  int a = 0;

  int b = 1;
  if (a == 1)
    b = 2;
  else
    b = 3;

  int c = bar::foo(a, b);
  if (c == 1) {
    return 0;
  } else {
    return 1;
  }
}
