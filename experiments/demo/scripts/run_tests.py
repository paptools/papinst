#!/usr/bin/env python

import logging
import subprocess
import pathlib
import os


def run_tests(expr_file):
    logging.info(f"Running tests for: {expr_file.name}.")

    os.environ["TRACE_EXPR_DATA_PATH"] = str(expr_file.absolute())
    if subprocess.check_call(["pytest"]) != 0:
        raise RuntimeError("Test runner failed.")


def main():
    logging.basicConfig(format="%(asctime)s - %(message)s", level=logging.INFO)
    logging.info(f"Test runner has started.")

    # Get a list of expression files.
    exp_dir = pathlib.Path(__file__).parent.parent.absolute()
    data_dir = exp_dir / "data"
    if not data_dir.exists():
        raise RuntimeError(
            f"Experiment data directory {exp_data_dir} does not exist."
        )
    expr_files = list(data_dir.glob("trace_exprs.*.json"))
    if len(expr_files) == 0:
        raise RuntimeError(f"No trace data files found in {exp_data_dir}.")
    logging.info(f"Found {len(expr_files)} expression data files.")

    # Run tests for each expression file.
    os.chdir(exp_dir)
    for expr_file in expr_files:
        run_tests(expr_file)

    logging.info("Test runner is complete.")


if __name__ == "__main__":
    main()
