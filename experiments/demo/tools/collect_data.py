#!/usr/bin/env python

import logging
import subprocess
import pathlib
import platform
import os
import shutil


def get_data_ext(compiler):
    """Return the file extension for the data file."""
    system = platform.system()
    machine = platform.machine()
    return f".{system}-{machine}-{compiler}.json".lower()


def collect_data(compiler, exp_data_dir):
    logging.info(f"Collecting data for {compiler}.")
    # Remove the existing build directory if it exists.
    build_dir = pathlib.Path("build")
    shutil.rmtree(build_dir, ignore_errors=True)

    # Build the target source code with the specified compiler.
    build_cmd = ["./tools/build", compiler, "--perf"]
    if subprocess.check_call(build_cmd) != 0:
        raise RuntimeError("Build failed.")
    # rv = subprocess.run(
    #    build_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    # )
    # if rv.returncode != 0:
    #    raise RuntimeError(
    #        "Build failed with the following"
    #        f" output:\n{rv.stdout.decode('utf-8')}"
    #    )

    # Run the target binary unit tests to generate the trace data.
    test_cmd = ["./tools/test"]
    if subprocess.check_call(test_cmd) != 0:
        raise RuntimeError("Test runner failed.")
    # rv = subprocess.run(
    #    build_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    # )
    # if rv.returncode != 0:
    #    raise RuntimeError(
    #        "Test runner failed with the following"
    #        f" output:\n{rv.stdout.decode('utf-8')}"
    #    )

    # Copy the generated data file to the experiment directory.
    data_file = build_dir / "paptrace.json"
    if not data_file.exists():
        raise RuntimeError(f"Data file {data_file} does not exist")
    exp_data_file = (
        exp_data_dir / data_file.with_suffix(get_data_ext(compiler)).name
    )
    shutil.copy(data_file, exp_data_file)
    with open(exp_data_file, "rb") as f:
        print(f.read())
    logging.info(f"Trace data saved to {exp_data_file}.")


def main():
    logging.basicConfig(format="%(asctime)s - %(message)s", level=logging.INFO)
    logging.info(f"Data collection has started.")
    logging.info(f"Platform: {platform.platform()}.")

    # Create the experiment data directory if it does not exist.
    script_dir = pathlib.Path(__file__).parent.parent.absolute()
    exp_data_dir = script_dir / "data"
    if not exp_data_dir.exists():
        exp_data_dir.mkdir()

    # Change the working directory to the demo directory.
    repo_dir = script_dir.parent.parent.absolute()
    demo_dir = repo_dir / "examples" / "demo"
    os.chdir(demo_dir)

    # Collect data for each compiler.
    compilers = ["clang", "gcc"]
    for compiler in compilers:
        collect_data(compiler, exp_data_dir)

    logging.info("Data collection is complete.")


if __name__ == "__main__":
    main()
