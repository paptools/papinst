#!/usr/bin/env python

import logging
import subprocess
import pathlib
import os


def profile_tool_build():
    logging.info("Profiling tool build.")
    command = [
        "hyperfine",
        "--prepare",
        "'rm -rf build install'",
        f"'./tools/build'",
    ]
    command_str = " ".join(command)
    subprocess.check_call(command_str, shell=True)


def profile_demo_example(enable_perf):
    perf_status = "ON" if enable_perf else "OFF"
    logging.info("Profiling full demo build (perf={perf_status}).")
    perf_flag = "--perf" if enable_perf else ""
    command = [
        "hyperfine",
        "--prepare",
        "'rm -rf build'",
        f"'./tools/build gcc {perf_flag}'",
    ]
    command_str = " ".join(command)
    subprocess.check_call(command_str, shell=True)

    os.chdir("build")
    logging.info("Profiling demo library build (perf={perf_status}).")
    command = ["hyperfine", "--warmup", "10", "'make -B -j1 demo'"]
    command_str = " ".join(command)
    subprocess.check_call(command_str, shell=True)

    logging.info("Profiling demo unit tests build (perf={perf_status}).")
    command = ["hyperfine", "--warmup", "10", "'make -B -j1 demo_unit_tests'"]
    command_str = " ".join(command)
    subprocess.check_call(command_str, shell=True)

    logging.info("Profiling demo unit tests execution (perf={perf_status}).")
    command = ["hyperfine", "--warmup", "10", "'./demo_unit_tests'"]
    command_str = " ".join(command)
    subprocess.check_call(command_str, shell=True)


def main():
    logging.basicConfig(format="%(asctime)s - %(message)s", level=logging.INFO)
    logging.info(f"Time profiling has started.")

    # Change the working directory to the demo directory.
    script_dir = pathlib.Path(__file__).parent.parent.absolute()
    repo_dir = script_dir.parent.parent.absolute()
    demo_dir = repo_dir / "examples" / "demo"

    profile_tool_build()
    for x in [True, False]:
        os.chdir(demo_dir)
        profile_demo_example(x)

    logging.info("Time profiling is complete.")


if __name__ == "__main__":
    main()
