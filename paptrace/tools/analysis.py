import time
import sympy
import numpy as np
from matplotlib import pyplot as plt
from pysr import PySRRegressor
from sklearn.model_selection import train_test_split


def fn1():
    # Dataset
    np.random.seed(0)
    X = 2 * np.random.randn(100, 5)
    y = 2.5382 * np.cos(X[:, 3]) + X[:, 0] ** 2 - 2

    default_pysr_params = dict(
        populations=30,
        model_selection="best",
    )

    # Learn equations
    model = PySRRegressor(
        # niterations=30,
        # turbo=True,
        binary_operators=["plus", "mult"],
        unary_operators=["cos", "exp", "sin"],
        **default_pysr_params,
    )

    model.fit(X, y)

    print(f"model:\n{model}")
    print(f"model.sympy():\n{model.sympy()}")

    ypredict = model.predict(X)
    ypredict_simpler = model.predict(X, 2)

    print("Default selection MSE:", np.power(ypredict - y, 2).mean())
    print(
        "Manual selection MSE for index 2:",
        np.power(ypredict_simpler - y, 2).mean(),
    )


def fn2():
    # Dataset

    # Create an np array from known values of f2(n)
    x = np.array([[0], [2], [4]])
    s1 = 1
    s2 = 2
    # y = np.array([s1, 2*s1+s2, 4*s1+3*s2])
    y = np.array([s1, 2 * s2 + s1, 4 * s2 + s1])

    default_pysr_params = dict(
        populations=10,
        model_selection="best",
    )

    # Learn equations
    model = PySRRegressor(
        # niterations=30,
        turbo=True,
        binary_operators=["plus", "mult"],
        unary_operators=["exp"],
        **default_pysr_params,
    )

    model.fit(x, y)

    print(f"model:\n{model}")
    print(f"model.sympy():\n{model.sympy()}")

    ypredict = model.predict(x)
    ypredict_simpler = model.predict(x, 2)

    print("Default selection MSE:", np.power(ypredict - y, 2).mean())
    print(
        "Manual selection MSE for index 2:",
        np.power(ypredict_simpler - y, 2).mean(),
    )


tic = time.perf_counter()
fn2()
toc = time.perf_counter()
print(f"Elapsed time: {toc - tic:0.4f} seconds")
