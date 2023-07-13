#!/usr/bin/env python

import logging
import pathlib
import shutil
import json

import paptree


def process_traces(known_exprs, trace_file, process_data_dir):
    logging.info(f"Analyzing traces for: {trace_file}.")

    trees = paptree.utils.from_file(trace_file)
    logging.info(f"Loaded {len(trees)} traces.")

    results = paptree.analyze.analyze(known_exprs, trees)
    output_name = f"trace_exprs{''.join(trace_file.suffixes)}"
    output_path = process_data_dir / output_name
    with open(output_path, "w") as f_out:
        f_out.write(json.dumps(results, indent=4))
    logging.info(f"Trace expressions saved to {output_path}.")


def main():
    logging.basicConfig(format="%(asctime)s - %(message)s", level=logging.INFO)
    logging.info(f"Trace analysis has started.")

    # Get a list of trace data files.
    script_dir = pathlib.Path(__file__).parent.parent.absolute()
    data_dir = script_dir / "data"
    raw_data_dir = data_dir / "raw"
    if not raw_data_dir.exists():
        raise RuntimeError(f"Raw data directory {raw_data_dir} does not exist.")
    trace_files = list(raw_data_dir.glob("paptrace.*.json"))
    if len(trace_files) == 0:
        raise RuntimeError(f"No trace data files found in {raw_data_dir}.")
    logging.info(f"Found {len(trace_files)} trace data files.")

    # Get a list of known complexities.
    known_exprs = []
    with open(raw_data_dir / "known_exprs.json", "r") as f:
        known_exprs = json.load(f)

    # Process the traces for each data file.
    process_data_dir = data_dir / "processed"
    for trace_file in trace_files:
        process_traces(known_exprs, trace_file, process_data_dir)

    logging.info("Trace analysis is complete.")


if __name__ == "__main__":
    main()
