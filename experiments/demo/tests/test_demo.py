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


def test_is_even():
    sig = "_Bool demo::IsEven(int)"
    ctxs = ["0", "1", "2", "3", "4", "5"]
    for ctx in ctxs:
        expr = get_expr(pytest.expr_data, sig, ctx)
        assert expr in get_constant(n)
