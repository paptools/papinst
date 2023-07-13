import json

import anytree
import sympy

from .node import Node
from .utils import from_file


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
    path_dict = {}
    for tree in trees:
        sig_paths = path_dict.setdefault(tree.root.sig, {})
        cf_tuple = tuple(tree.get_cf_nodes())
        sig_paths.setdefault(
            cf_tuple, {"path_id": len(sig_paths), "traces": []}
        )["traces"].append(tree)
    return path_dict


def get_expr_data(path_dict, known):
    exprs = {}
    call_path_ids = {}

    # Expr data should map: sig->path_id->ctx->expr
    expr_data = {}

    for sig, sig_paths in path_dict.items():
        for _, path_data in sig_paths.items():
            path_id = path_data["path_id"]
            for tree in path_data["traces"]:
                call_path_ids[to_call_str(tree.root)] = path_id
                expr = tree.root.to_expr(known)
                print(f"expr for {to_call_str(tree.root)}: {expr}")
                exprs[to_call_str(tree.root)] = expr
                sig_data = expr_data.setdefault(tree.root.sig, {})
                path_data = sig_data.setdefault(f"path_{path_id}", {})
                param_str = to_params_str(tree.root.params)
                # with sympy.evaluate(False):
                # sympy_expr = sympy.sympify(" + ".join(expr))
                path_data[param_str] = sympy.srepr(expr)
    return expr_data


def solve(path_exprs):
    # If there is only one expression, return it.
    if len(path_exprs) == 1:
        return list(path_exprs.values())[0]
    # Check if all expressions are equal. If they are, return the expression
    # since it is constant.
    expr_set = set([e for _, e in path_exprs.items()])
    if len(expr_set) == 1:
        return expr_set.pop()
    else:
        print("\nUnsolved exprs:")
        for expr in expr_set:
            print(expr)
    return None


def solve_exprs(expr_data):
    # Solve for each path.
    # We will want to query the results with a tuple of (signature, ctx). To do
    # this we will need to map the ctx for a signature to the correct path ID.
    # Therefore, the results will need to contain two sections:
    # 1. A mapping from (signature) to sig_id.
    # 2. A mapping from (sig_id, ctx) to path_id.
    # 3. A mapping from (sig_id, path_id) to the path expression.
    results = {"sigs": {}, "ctxs": {}, "exprs": {}}

    skip_sigs = ["unsigned long long fibonacci::RecursiveMemo(unsigned short)"]
    for sig, sig_data in expr_data.items():
        if sig in skip_sigs:
            continue
        sig_id = results["sigs"].setdefault(sig, f"sig_{len(results['sigs'])}")
        for path_id, path_data in sig_data.items():
            path_exprs = {}
            for ctx, expr in path_data.items():
                sexpr = sympy.sympify(expr)
                path_exprs[ctx] = sexpr
            result = solve(path_exprs)
            if result:
                results["exprs"].setdefault(sig_id, {})[path_id] = sympy.srepr(
                    result
                )
                result_ctxs = results["ctxs"].setdefault(sig_id, {})
                for ctx in path_exprs.keys():
                    result_ctxs[ctx] = path_id
    return results


def analyze(known, trees):
    link_recursive_nodes(trees)

    path_dict = get_path_partitions(trees)
    print("\nPaths per signature:")
    for k, v in path_dict.items():
        print(f"- {k}: {len(v)}")
    print("\nPaths:")
    for sig, sig_paths in path_dict.items():
        print(f"- {sig}:")
        for path_id, path_data in sig_paths.items():
            print(f"  - path_{path_id}:")

    expr_data = get_expr_data(path_dict, known)
    print("\nProcessed Traces:")
    print(json.dumps(expr_data, indent=2))

    results = solve_exprs(expr_data)
    return results
    print("\nResults:")
    print(json.dumps(results, indent=4))
    with open(args.output_file, "w") as f_out:
        f_out.write(json.dumps(results, indent=2))
