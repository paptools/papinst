import json
import argparse

import sympy
from sympy import O, oo
from sympy.abc import n


def get_constant(x_):
    return O(1, (x_, oo))


def get_linear(x_):
    return O(x_, (x_, oo))


def get_exp(base, x_):
    return O(base**x_, (x_, oo))


def get_quad(x_):
    return O(x_**2, (x_, oo))


def get_expr(data, sig, ctx):
    sig_id = data["sigs"][sig]
    path_id = data["ctxs"][sig_id][ctx]
    expr = data["exprs"][sig_id][path_id]
    return sympy.sympify(expr)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_file", help="input file")
    args = parser.parse_args()

    with open(args.input_file) as f_in:
        data = json.load(f_in)

    # Start with constant time paths.
    print("Testing constant time paths...")
    sig = "unsigned long long fibonacci::LookupTable(unsigned short)"
    for ctx in ["0", "1", "2", "3", "4", "5", "6", "7", "94"]:
        expr = get_expr(data, sig, ctx)
        assert expr in get_constant(n)
    sig = "unsigned long long fibonacci::Iterative(unsigned short)"
    for ctx in ["0", "1", "94"]:
        expr = get_expr(data, sig, ctx)
        assert expr in get_constant(n)
    sig = "unsigned long long fibonacci::RecursiveNaive(unsigned short)"
    for ctx in ["0", "1", "94"]:
        expr = get_expr(data, sig, ctx)
        assert expr in get_constant(n)
    print(f"O(1) tests passed.")

    # Next test linear time paths.
    print("\nTesting linear time paths...")
    sig = "unsigned long long fibonacci::Iterative(unsigned short)"
    for ctx in ["2", "3", "4", "5", "6", "7"]:
        expr = get_expr(data, sig, ctx)
        assert expr in get_constant(n)
    print(f"O(n) tests passed.")

    # Next test exponential time paths.
    print("\nTesting exponential time paths...")
    sig = "unsigned long long fibonacci::RecursiveNaive(unsigned short)"
    for ctx in ["2", "3", "4", "5", "6", "7"]:
        expr = get_expr(data, sig, ctx)
        assert expr in get_exp(2, n)
    print(f"O(2^n) tests passed.")


if __name__ == "__main__":
    main()
