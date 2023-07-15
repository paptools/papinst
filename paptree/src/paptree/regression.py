import operator
import random

from deap import creator, base, tools
from gplearn.genetic import SymbolicRegressor
import geppy as gep
import gplearn
import numpy as np
import sympy
from sympy import oo

LINEAR_SCALING = True

# for reproduction
s = 0
random.seed(s)
np.random.seed(s)

x = sympy.Symbol("X0")


def protected_log(b):
    if np.isscalar(b):
        if b < 1e-6:
            b = 1
    else:
        b[b < 1e-6] = 1
    return np.log(b)


def protected_sqrt(b):
    if np.isscalar(b):
        if b < 1e-6:
            b = 1
    else:
        b[b < 1e-6] = 1
    return np.sqrt(b)


def geppy_lscale_symreg(data):
    X = np.array([item[0] for item in data])
    Y = np.array([item[1] for item in data])

    pset = gep.PrimitiveSet("Main", input_names=["X0"])
    pset.add_function(operator.add, 2)
    pset.add_function(operator.mul, 2)
    pset.add_function(protected_log, 1, name="log")
    pset.add_function(protected_sqrt, 1, name="sqrt")
    pset.add_ephemeral_terminal(name="enc", gen=lambda: random.uniform(-5, 5))

    creator.create(
        "FitnessMin", base.Fitness, weights=(-1,)
    )  # to minimize the objective (fitness)
    creator.create(
        "Individual",
        gep.Chromosome,
        fitness=creator.FitnessMin,
        a=float,
        b=float,
    )

    h = 2  # head length
    n_genes = 2  # number of genes in a chromosome
    r = 5  # length of the RNC array

    toolbox = gep.Toolbox()
    toolbox.register("gene_gen", gep.Gene, pset=pset, head_length=h)
    toolbox.register(
        "individual",
        creator.Individual,
        gene_gen=toolbox.gene_gen,
        n_genes=n_genes,
        linker=operator.add,
    )
    toolbox.register("population", tools.initRepeat, list, toolbox.individual)

    # compile utility: which translates an individual into an executable function (Lambda)
    toolbox.register("compile", gep.compile_, pset=pset)

    def evaluate_linear_scaling(individual):
        """Evaluate the fitness of an individual with linearly scaled MSE.
        Get a and b by minimizing (a*Yp + b - Y)"""
        func = toolbox.compile(individual)
        Yp = func(X)

        # special cases: (1) individual has only a terminal
        #  (2) individual returns the same value for all test cases, like 'x - x + 10'. np.linalg.lstsq will fail in such cases.

        if isinstance(Yp, np.ndarray):
            Q = np.hstack((np.reshape(Yp, (-1, 1)), np.ones((len(Yp), 1))))
            (individual.a, individual.b), residuals, _, _ = np.linalg.lstsq(
                Q, Y, rcond=None
            )
            # residuals is the sum of squared errors
            if residuals.size > 0:
                return (residuals[0] / len(Y),)  # MSE

        # for the above special cases, the optimal linear scaling is just the mean of true target values
        individual.a = 0
        individual.b = np.mean(Y)
        return (np.mean((Y - individual.b) ** 2),)

    if LINEAR_SCALING:
        toolbox.register("evaluate", evaluate_linear_scaling)
    else:
        toolbox.register("evaluate", evaluate)

    # toolbox.register('select', tools.selTournament, tournsize=3)
    toolbox.register(
        "select",
        tools.selDoubleTournament,
        fitness_size=7,
        parsimony_size=1.4,
        fitness_first=True,
    )

    # 1. general operators
    toolbox.register(
        "mut_uniform", gep.mutate_uniform, pset=pset, ind_pb=0.05, pb=1
    )
    toolbox.register("mut_invert", gep.invert, pb=0.1)
    toolbox.register("mut_is_transpose", gep.is_transpose, pb=0.1)
    toolbox.register("mut_ris_transpose", gep.ris_transpose, pb=0.1)
    toolbox.register("mut_gene_transpose", gep.gene_transpose, pb=0.1)
    toolbox.register("cx_1p", gep.crossover_one_point, pb=0.4)
    toolbox.register("cx_2p", gep.crossover_two_point, pb=0.2)
    toolbox.register("cx_gene", gep.crossover_gene, pb=0.1)
    toolbox.register(
        "mut_ephemeral", gep.mutate_uniform_ephemeral, ind_pb="1p"
    )  # 1p: expected one point mutation in an individual
    toolbox.pbs["mut_ephemeral"] = (
        1  # we can also give the probability via the pbs property
    )

    stats = tools.Statistics(key=lambda ind: ind.fitness.values[0])
    stats.register("avg", np.mean)
    stats.register("std", np.std)
    stats.register("min", np.min)
    stats.register("max", np.max)

    # size of population and number of generations
    n_pop = 100
    n_gen = 100

    pop = toolbox.population(n=n_pop)
    hof = tools.HallOfFame(
        3
    )  # only record the best three individuals ever found in all generations

    # start evolution
    pop, log = gep.gep_simple(
        pop,
        toolbox,
        n_generations=n_gen,
        n_elites=1,
        stats=stats,
        hall_of_fame=hof,
        verbose=False,
    )
    best_ind = hof[0]
    renames = {
        "log": sympy.log,
        "sqrt": sympy.sqrt,
        "add": sympy.Add,
        "mul": sympy.Mul,
    }
    symplified_best = gep.simplify(best_ind, renames)
    ex2 = symplified_best
    for a in sympy.preorder_traversal(symplified_best):
        if isinstance(a, sympy.Float):
            ex2 = ex2.subs(a, round(a, 1))
    return ex2


def gplearn_symreg(data):
    np.random.seed(0)  # for reproduction

    # Extract x and y from the data
    x = np.array([item[0] for item in data]).reshape(-1, 1)
    y = np.array([item[1] for item in data])

    # Create symbolic regressor
    # def _pow_exp(x1):
    #    with np.errstate(divide='ignore', invalid='ignore'):
    #        if (x1 > 10).any():
    #            return 0.
    #        try:
    #            result = np.power(2, x1)
    #            #print(result)
    #            return result
    #        except OverflowError:
    #            return 0.
    #        except ValueError:  # The math domain error
    #            return 0.
    #        except RuntimeWarning:
    #            return 0.

    # pow_exp = gplearn.functions.make_function(
    #    function=_pow_exp,
    #    name='pow_exp',
    #    arity=1
    # )
    # function_set=['add', 'mul', 'log', 'sqrt', pow_exp]
    function_set = ["add", "mul", "log", "sqrt"]
    sr = SymbolicRegressor(
        population_size=5000,
        generations=20,
        function_set=function_set,
        stopping_criteria=0.01,
        p_crossover=0.7,
        p_subtree_mutation=0.1,
        p_hoist_mutation=0.05,
        p_point_mutation=0.1,
        max_samples=0.9,
        verbose=0,
        parsimony_coefficient="auto",
        random_state=0,
        n_jobs=1,
        init_method="grow",
    )

    # Fit the data
    sr.fit(x, y)

    def to_sympy_expr(prog):
        """Convert a program to a sympy expression."""
        locals = {
            "sub": lambda x, y: x - y,
            "div": lambda x, y: x / y,
            "mul": lambda x, y: x * y,
            "add": lambda x, y: x + y,
            "neg": lambda x: -x,
            "pow": lambda x, y: x**y,
            "cos": lambda x: sympy.cos(x),
            "sqrt": lambda x: sympy.sqrt(x),
        }
        return sympy.simplify(sympy.sympify(str(prog), locals=locals))

    return to_sympy_expr(sr._program)
