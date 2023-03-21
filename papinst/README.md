# papinst

The path instrumenter executable.

## Building

To build the project, run:

```bash
cmake -S . -B build --install-prefix=$PWD/install
cmake --build build -j 8
```

## Installing

To install the project, run:

```bash
cmake --install build
```

## Usage

The provided examples use the project build directory.

### papinst

To see the `papinst` usage message, run the following from your build directory:


```bash
cmake -E chdir build/bin ./papinst --help
```

## Testing

To run the unit tests, run the following command from your build directory:

```bash
cmake -E chdir build/bin ./unit_tests
```

## Tools

### Formatting

To keep the style of the code consistent, you can run a repository wide format
with:

```bash
./tools/format.sh
```

## Limitations

- No support for parallel instances instrumentation of a single source file.
  - E.g., if process A instruments foo with contents "foo A" and process B
  instruments foo with contents "foo B", then by the time that process A
  attempts to compile foo it might contain contents "foo B" instead of "foo A".
- No support for header-only libraries.
