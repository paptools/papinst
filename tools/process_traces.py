import argparse
import operator
import json

import anytree
import sympy

import paptree


def to_params_str(params):
    # return ", ".join([f"{param['name']}={param['value']}" for param in params])
    return ", ".join([f"{param['value']}" for param in params])


def to_call_str(node):
    return f"{node.sig}: ({to_params_str(node.params)})"


def to_simple_node_view(node):
    sym = "sym @ " if hasattr(node, "target") else ""
    if paptree.Node.is_call_type(node.type):
        desc = f" {to_call_str(node)}"
    else:
        desc = f" {node.type}: {node.desc}"
    return f"({sym}{node.name}){desc}"


def eliminate_operator_children(trees):
    # Let's eliminate the children of the operator nodes. They are there by mistake.
    for tree in trees:
        for node in anytree.PreOrderIter(tree.root):
            if node.type == "CallerExpr":
                if (
                    "operator+" in node.sig
                    or "operator-" in node.sig
                    or "operator=" in node.sig
                ):
                    node.children = ()


def is_cf_node(node):
    return node.type in [
        "IfThenStmt",
        "ReturnStmt",
        "CXXThrowExpr",
        "ForStmt",
        "LoopIter",
    ]


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
        cf_nodes = [
            node.name
            for node in anytree.PreOrderIter(tree.root, filter_=is_cf_node)
        ]
        cf_tuple = tuple(cf_nodes)
        sig_paths.setdefault(
            cf_tuple, {"path_id": len(sig_paths), "traces": []}
        )["traces"].append(tree)
    return path_dict


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_file", help="input file")
    parser.add_argument("output_file", help="output file")
    args = parser.parse_args()

    trees = paptree.utils.from_file(args.input_file)
    print(f"Loaded {len(trees)} traces.")

    # Bin traces by signature.
    bins = {}
    for tree in trees:
        bins.setdefault(tree.root.sig, []).append(tree)
    print("\nTraces per signature:")
    for k, v in bins.items():
        print(f"- {k}: {len(v)}")

    eliminate_operator_children(trees)
    link_recursive_nodes(trees)

    path_dict = get_path_partitions(trees)
    print("\nPaths per signature:")
    for k, v in path_dict.items():
        print(f"- {k}: {len(v)}")

    # It is expected that we start with some known values.
    # NOTE: This currently uses signatures to generalize (i.e., it is not accounting for context).
    known = {
        "int operator-(unsigned short, int)": ["C_OP_INT_SUB"],
        "int operator+(unsigned short, int)": ["C_OP_INT_ADD"],
        "value_type operator=(value_type, int)": ["C_OP_INT_ASSIGN"],
        "reference std::vector<unsigned long long>::operator[](size_type)": [
            "C_OP_VEC_SUBSCRIPT"
        ],
        "value_type operator=(value_type, unsigned long long)": [
            "C_OP_VEC_ASSIGN"
        ],
        "unsigned long long fibonacci::(anonymous namespace)::RecursiveMemoImpl(unsigned short, std::vector<unsigned long long> &)": [
            "C_FIB_RECURSIVE_MEMO_IMPL"
        ],
        "unsigned long long operator+(unsigned long long, unsigned long long)": [
            "C_OP_ULL_ADD"
        ],
        "unsigned long long operator=(unsigned long long, unsigned long long)": [
            "C_OP_ULL_ASSIGN"
        ],
    }
    exprs = {}
    call_path_ids = {}

    # Expr data should map: sig->path_id->ctx->expr
    expr_data = {}

    def to_expr(node, level=0):
        subexpr = []
        for child in node.children:
            subexpr.extend(to_expr(child, level + 1))
        if node.is_root:
            subexpr.append(
                f"C_F{node.name}_P{call_path_ids[to_call_str(node)]}"
            )
        elif paptree.Node.is_call_type(node.type):
            # print(f"{' ' * 2 * level}including node {to_simple_node_view(node)}")
            if node.sig in known:
                subexpr.extend(known[node.sig])
            else:
                subexpr.extend(exprs[to_call_str(node)])
        else:
            # print(f"{' ' * 2 * level}excluding node {to_simple_node_view(node)}")
            pass
        return subexpr

    for sig, sig_paths in path_dict.items():
        for _, path_data in sig_paths.items():
            path_id = path_data["path_id"]
            for tree in path_data["traces"]:
                call_path_ids[to_call_str(tree.root)] = path_id
                expr = to_expr(tree.root)
                exprs[to_call_str(tree.root)] = expr
                sig_data = expr_data.setdefault(tree.root.sig, {})
                path_data = sig_data.setdefault(f"path_{path_id}", {})
                param_str = to_params_str(tree.root.params)
                # with sympy.evaluate(False):
                sympy_expr = sympy.sympify(" + ".join(expr))
                path_data[param_str] = sympy.srepr(sympy_expr)

    print("\nProcessed Traces:")
    print(json.dumps(expr_data, indent=2))
    with open(args.output_file, "w") as f_out:
        f_out.write(json.dumps(expr_data, indent=2))


if __name__ == "__main__":
    main()
