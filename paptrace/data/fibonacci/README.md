# fibonacci

Paptrace data generated from the
[iandinwoodie/fibonacci](https://github.com/iandinwoodie/fibonacci) library.

## Usage

Run the example using the command format:

```bash
../../tools/clitrace paptrace.json <request>
```

For example, the following commands will output results for
`fibonacci::RecursiveMemo(0)`, `fibonacci::RecursiveMemo(2)`, and
`fibonacci::RecursiveMemo(4)`:

```bash
../../tools/clitrace paptrace.json recursive_memo_0.json
../../tools/clitrace paptrace.json recursive_memo_2.json
../../tools/clitrace paptrace.json recursive_memo_4.json
```
