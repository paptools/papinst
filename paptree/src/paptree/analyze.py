import json

import anytree
import sympy
import gplearn
from gplearn.genetic import SymbolicRegressor
import numpy as np

from .node import Node
from .utils import from_file


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


def symbolic_regression(data):
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
        verbose=1,
        parsimony_coefficient="auto",
        random_state=0,
        n_jobs=1,
        init_method="grow",
    )

    # Fit the data
    sr.fit(x, y)
    return sr._program


def to_params_str(params):
    # return ", ".join([f"{param['name']}={param['value']}" for param in params])
    return ", ".join([f"{param['value']}" for param in params])


def to_call_str(node):
    return f"{node.sig}: ({to_params_str(node.params)})"


def link_recursive_nodes(trees):
    # Let's start by putting our trace root nodes in a container that allows us
    # to lookup the trace by context.
    # NOTE: This does not account for complete non-root traces (e.g., RecursiveMemoImpl).
    trace_roots = {}
    for tree in trees:
        call_str = to_call_str(tree.root)
        if call_str not in trace_roots:
            trace_roots[call_str] = tree.root
        else:
            if tree.root != trace_roots[call_str]:
                raise RuntimeError(
                    "Unhandled scenario: 2 traces with the same context have"
                    " differing trees."
                )

    # Next we walk the trees looking for child nodes that can be substituted with a
    # trace root node.
    for tree in trees:
        for node in anytree.PreOrderIter(tree.root):
            if node.is_root:
                continue
            replace_children = False
            new_children = []
            for child in node.children:
                if child.type == "CalleeExpr":
                    child_call_str = to_call_str(child)
                    if child_call_str in trace_roots:
                        replace_children = True
                        child = anytree.SymlinkNode(child)
                new_children.append(child)
            if replace_children:
                node.children = tuple(new_children)


def get_path_partitions(trees):
    # Schema: {sig: {cf_tuple: {path_id: int, traces: [tree]}}}
    path_dict = {}
    for tree in trees:
        sig_paths = path_dict.setdefault(tree.root.sig, {})
        cf_tuple = tuple(tree.get_cf_nodes())
        sig_paths.setdefault(
            cf_tuple, {"path_id": len(sig_paths), "traces": []}
        )["traces"].append(tree)
    return path_dict


def is_constant(exprs):
    """Returns True if all expressions are equal."""
    return len(set(exprs)) == 1


def find_repr_exprs(path_dict, known):
    # We will want to query the results with a tuple of (signature, ctx). To do
    # this we will need to map the ctx for a signature to the correct path ID.
    # Therefore, the results will need to contain two sections:
    # 1. A mapping from (signature) to sig_id.
    # 2. A mapping from (sig_id, ctx) to path_id.
    # 3. A mapping from (sig_id, path_id) to the path expression.
    results = {"sigs": {}, "ctxs": {}, "exprs": {}}

    def add_result(sig, path_id, expr, trees):
        print(f"  Found expr.: {expr}")
        sig_id = results["sigs"].setdefault(sig, f"sig_{len(results['sigs'])}")
        path_id_str = f"path_{path_id}"
        results["exprs"].setdefault(sig_id, {})[path_id_str] = sympy.srepr(expr)
        result_ctxs = results["ctxs"].setdefault(sig_id, {})
        for tree in trees:
            result_ctxs[to_params_str(tree.root.params)] = path_id_str

    for sig, sig_entry in path_dict.items():
        for cf_tuple, path_entry in sig_entry.items():
            path_id = path_entry["path_id"]
            trees = path_entry["traces"]
            print(f"\nFinding general expr. for: {sig}: ({path_id})")

            # Get the expressions for each trace.
            exprs = [tree.to_expr(known) for tree in trees]

            if is_constant(exprs):
                add_result(sig, path_id, exprs[0], trees)
                continue

            if trees[0].has_loop():
                ctxs = [int(to_params_str(tree.root.params)) for tree in trees]
                loop_nodes = [tree.get_loop_nodes() for tree in trees]
                ref_nodes = loop_nodes[0]
                for i, ref_node in enumerate(ref_nodes):
                    iter_cnts = [ref_node.iter_count]
                    # Get the expressions for each entry in loop_nodes except the 0th.
                    for j in range(1, len(loop_nodes)):
                        alt_node = loop_nodes[j][i]
                        iter_cnts.append(alt_node.iter_count)
                    data = []
                    for j in range(0, len(ctxs)):
                        data.append((ctxs[j], iter_cnts[j]))
                    prog = symbolic_regression(data)
                    loop_expr = to_sympy_expr(prog)
                    ref_node.loop_expr = loop_expr
                add_result(sig, path_id, trees[0].to_expr(known), trees)
                continue

            print("  No general expr. found for exprs:")
            for expr in exprs:
                print(f"    {expr}")

    return results


def analyze(known, trees):
    link_recursive_nodes(trees)

    path_dict = get_path_partitions(trees)
    print("Path summary:")
    for sig, sig_entry in path_dict.items():
        print(f"- {sig}: {len(sig_entry)} paths")
        for cf_tuple, path_entry in sig_entry.items():
            print(
                f"  - [path_{path_entry['path_id']}] {cf_tuple}:"
                f" {len(path_entry['traces'])} traces"
            )

    results = find_repr_exprs(path_dict, known)
    return results
    print("\nResults:")
    print(json.dumps(results, indent=4))
    with open(args.output_file, "w") as f_out:
        f_out.write(json.dumps(results, indent=2))
