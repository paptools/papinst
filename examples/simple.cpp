int foo(int a, int b) { return a + b; }

int main() {
  int a = 0, b = 1;
  int c = foo(a, b);
  return (c == 1) ? 0 : 1;
}
