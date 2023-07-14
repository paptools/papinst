import pytest
import sympy
from sympy import O, oo

X0 = sympy.Symbol("X0")


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


def test_is_even():
    sig = "_Bool demo::IsEven(int)"
    # All paths are constant.
    ctxs = ["0", "1", "2", "3", "4", "5"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)


def test_factorial():
    sig = "int demo::Factorial(int)"
    # Paths for x < 1 and x > 31 are constant.
    ctxs = ["0", "32"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)

    # Paths for 1 <= x <= 31 are linear.
    ctxs = ["1", "2", "3"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr not in get_constant(X0)
        assert expr in get_linear(X0)


def test_is_prime():
    pass


def test_n_by_n_increments():
    sig = "int demo::NByNIncrements(int)"
    ctxs = ["0"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(X0)
    ctxs = ["1", "2", "3", "4", "5"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr not in get_constant(X0)
        assert expr not in get_linear(X0)
        assert expr in get_quad(X0)
