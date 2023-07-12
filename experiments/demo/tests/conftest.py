import json
import os

import pytest


def _get_expr_data():
    expr_data_path = os.getenv("TRACE_EXPR_DATA_PATH")
    with open(expr_data_path) as f_in:
        return json.load(f_in)


def pytest_configure():
    pytest.expr_data = _get_expr_data()


# def pytest_sessionstart(session):
#    """
#    Called after the Session object has been created and
#    before performing collection and entering the run test loop.
#    """
#    pass
