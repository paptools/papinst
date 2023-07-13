import pytest
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


def test_is_even_constant():
    sig = "_Bool demo::IsEven(int)"
    # All paths are constant.
    ctxs = ["0", "1", "2", "3", "4", "5"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(n)


def test_factorial():
    sig = "int demo::Factorial(int)"
    # Paths for x < 1 and x > 31 are constant.
    ctxs = ["0", "32"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(n)
    # Paths for 1 <= x <= 31 are linear.
    ctxs = ["1", "2", "3"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_linear(n)
        assert expr not in get_constant(n)
