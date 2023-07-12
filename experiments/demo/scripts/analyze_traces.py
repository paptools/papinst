#!/usr/bin/env python

import logging
import subprocess
import pathlib
import platform
import os
import shutil
import datetime


def get_data_ext(compiler):
    """Return the file extension for the data file."""
    system = platform.system()
    machine = platform.machine()
    return f".{system}-{machine}-{compiler}.json".lower()


def process_traces(analysis_script, trace_file):
    logging.info(f"Analyzing traces for: {trace_file.name}.")

    output_name = f"trace_exprs{''.join(trace_file.suffixes)}"
    output_file = trace_file.parent / output_name
    cmd = [analysis_script, trace_file, output_file]
    if subprocess.check_call(cmd) != 0:
        raise RuntimeError("Trace analysis failed.")


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

    # Get the analysis script directory.
    repo_dir = script_dir.parent.parent.absolute()
    analysis_script = repo_dir / "tools" / "process_traces.py"

    # Process the traces for each data file.
    for trace_file in trace_files:
        process_traces(analysis_script, trace_file)

    logging.info("Trace analysis is complete.")


if __name__ == "__main__":
    main()
