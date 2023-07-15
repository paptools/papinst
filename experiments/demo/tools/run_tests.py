#!/usr/bin/env python

import logging
import subprocess
import pathlib
import os


def run_tests(expr_file):
    logging.info(f"Running tests for: {expr_file.name}.")

    os.environ["TRACE_EXPR_DATA_PATH"] = str(expr_file.absolute())
    try:
        subprocess.check_call(["pytest", "-s"])
        return 0
    except:
        return 1


def main():
    logging.basicConfig(format="%(asctime)s - %(message)s", level=logging.INFO)
    logging.info(f"Test runner has started.")

    # Get a list of expression files.
    exp_dir = pathlib.Path(__file__).parent.parent.absolute()
    processed_data_dir = exp_dir / "data/processed"
    expr_files = list(processed_data_dir.glob("trace_exprs.*.json"))
    if len(expr_files) == 0:
        raise RuntimeError(f"No expr data files found in {processed_data_dir}.")
    logging.info(f"Found {len(expr_files)} expression data files.")

    # Run tests for each expression file.
    fail_cnt = 0
    os.chdir(exp_dir)
    for expr_file in expr_files:
        fail_cnt += run_tests(expr_file)

    logging.info("Tests failed for {fail_cnt}/{len(expr_files)} configs.")
    logging.info("Test runner is complete.")


if __name__ == "__main__":
    main()
