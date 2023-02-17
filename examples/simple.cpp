int foo(int a, int b) { return a + b; }

int main() {
  int a = 0, b = 1;
  int c = foo(a, b);
  if (c == 1) {
    return 0;
  } else {
    return 1;
  }
}
