import pytest
import sympy
from sympy import O, oo

X0 = sympy.Symbol("X0")


def get_constant(x_):
    return O(1, (x_, oo))


def get_log(x_):
    return O(sympy.log(x_), (x_, oo))


def get_linear(x_):
    return O(x_, (x_, oo))


def get_exp(base, x_):
    return O(base**x_, (x_, oo))


def get_quad(x_):
    return O(x_**2, (x_, oo))


def get_cubic(x_):
    return O(x_**3, (x_, oo))


def get_expr(data, sig, ctx):
    sig_id = data["sigs"][sig]
    path_id = data["ctxs"][sig_id][ctx]
    expr = data["exprs"][sig_id][path_id]
    return sympy.sympify(expr)


def test_is_even():
    sig = "_Bool demo::IsEven(int)"
    # All paths are constant.
    ctxs = ["0", "2", "4", "1", "3", "5", "-2147483648", "2147483647"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)


def test_shifts_to_zero():
    sig = "int demo::ShiftsToZero(int)"
    # Path for x = 0 is constant.
    ctxs = ["0"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)

    # Paths for x > 0 are logarithmic.
    ctxs = ["1", "3", "7", "15", "31", "63", "127"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr not in get_constant(X0)
        assert expr in get_log(X0)


def test_factorial():
    sig = "int demo::Factorial(int)"
    # Paths for x < 1 and x > 31 are constant.
    ctxs = ["-1", "0", "32"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)

    # Paths for 1 <= x <= 31 are linear.
    ctxs = ["1", "2", "3", "4", "31"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr not in get_constant(X0)
        assert expr in get_linear(X0)


def test_is_prime():
    sig = "_Bool demo::IsPrime(int)"
    ctxs = [
        # Path 2
        "0",
        "1",
        # Path 0
        "2",
        "3",
        # Path 3
        "4",
        "6",
        "10",
        "16",
        "18",
        "20",
        "30",
        "32",
        "36",
        "42",
        "44",
        "48",
        "60",
        "64",
        "68",
        "72",
        # Path 4
        "25",
        "55",
    ]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)
    # Path 1: the primes.
    ctxs = [
        "5",
        "7",
        "11",
        "13",
        "17",
        "19",
        "23",
        "29",
        "31",
        "37",
        "41",
        "43",
        "47",
        "53",
        "59",
        "61",
        "67",
        "71",
    ]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_linear(X0)


def test_n_by_n_increments():
    sig = "int demo::NByNIncrements(int)"
    ctxs = ["0"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)
    ctxs = ["1", "2", "4", "8", "16", "32", "64"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr not in get_constant(X0)
        assert expr not in get_linear(X0)
        assert expr in get_quad(X0)


def test_n_by_n_by_n_increments():
    sig = "int demo::NByNByNIncrements(int)"
    ctxs = ["0"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)
    ctxs = ["1", "2", "4", "8", "16", "32"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr not in get_constant(X0)
        assert expr not in get_linear(X0)
        assert expr not in get_quad(X0)
        assert expr in get_cubic(X0)
