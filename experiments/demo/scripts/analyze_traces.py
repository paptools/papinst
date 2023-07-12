#!/usr/bin/env python

import logging
import subprocess
import pathlib
import platform
import os
import shutil
import datetime
import json

import paptree


def get_data_ext(compiler):
    """Return the file extension for the data file."""
    system = platform.system()
    machine = platform.machine()
    return f".{system}-{machine}-{compiler}.json".lower()


def process_traces(known_exprs, trace_file):
    logging.info(f"Analyzing traces for: {trace_file.name}.")

    trees = paptree.utils.from_file(trace_file)
    logging.info(f"Loaded {len(trees)} traces.")

    results = paptree.analyze.analyze(known_exprs, trees)
    output_name = f"trace_exprs{''.join(trace_file.suffixes)}"
    logging.info(f"Writing results to {output_name}.")


def main():
    logging.basicConfig(format="%(asctime)s - %(message)s", level=logging.INFO)
    logging.info(f"Trace analysis has started.")

    # Get a list of trace data files.
    script_dir = pathlib.Path(__file__).parent.parent.absolute()
    exp_data_dir = script_dir / "data"
    if not exp_data_dir.exists():
        raise RuntimeError(
            f"Experiment data directory {exp_data_dir} does not exist."
        )
    trace_files = list(exp_data_dir.glob("paptrace.*.json"))
    if len(trace_files) == 0:
        raise RuntimeError(f"No trace data files found in {exp_data_dir}.")
    logging.info(f"Found {len(trace_files)} trace data files.")

    # Get a list of known complexities.
    known_exprs = []
    with open(exp_data_dir / "known_exprs.json", "r") as f:
        known_exprs = json.load(f)

    # Process the traces for each data file.
    for trace_file in trace_files:
        process_traces(known_exprs, trace_file)

    logging.info("Trace analysis is complete.")


if __name__ == "__main__":
    main()
