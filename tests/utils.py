"""Collection of utilities for pathinst's tests."""
import pathlib


def get_exe_path():
    """Return the path to the pathinst binary."""
    return pathlib.Path(__file__).parent.parent / "build" / "pathinst"
