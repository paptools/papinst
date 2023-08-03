#!/usr/bin/env python

import logging
import pathlib
import json

import sympy
from sympy import oo


def main():
    logging.basicConfig(format="%(asctime)s - %(message)s", level=logging.INFO)
    logging.info(f"Model summary script has started.")

    # Get a list of trace data files.
    script_dir = pathlib.Path(__file__).parent.parent.absolute()
    processed_data_dir = script_dir / "data" / "processed"
    expr_files = list(processed_data_dir.glob("trace_exprs.*.json"))
    if len(expr_files) == 0:
        raise RuntimeError(
            f"No trace data files found in {processed_data_dir}."
        )
    logging.info(f"Found {len(expr_files)} expr data files.")

    data_sets = {}
    for expr_file in expr_files:
        with open(expr_file, "r") as f:
            expr_data = json.load(f)
        data_sets[expr_file.stem.split(".")[1]] = expr_data

    summary = {}
    for platform, data_set in data_sets.items():
        sigs = data_set["sigs"]
        ctxs = data_set["ctxs"]
        exprs = data_set["exprs"]
        for sig, sig_id in sigs.items():
            sig_dict = summary.setdefault(sig, {})
            for path_id, expr in exprs[sig_id].items():
                ex = str(sympy.sympify(expr))
                path_dict = sig_dict.setdefault(path_id, {})
                path_dict[platform] = ex

    print(json.dumps(summary, indent=2))

    logging.info("Model summary script is complete.")


if __name__ == "__main__":
    main()
