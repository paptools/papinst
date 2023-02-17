# pathinst

Repository for the path instrumenter.

## Building

To build the project, run:

```bash
mkdir build && cd build
cmake -DCMAKE_C_COMPILER=$(which gcc) -DCMAKE_CXX_COMPILER=$(which g++) ..
make -j8
```

## Usage

### pathinst

To see the `pathinst` usage message, run the following from your build directory:


```bash
./pathinst --help
```

## Testing

To run the unit tests, run the following command from your build directory:

```bash
./unit_tests
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
