"""Collection of utilities for pathinst's tests."""
import pathlib


def get_exe_path():
    """Return the path to the pathinst binary."""
    return pathlib.Path(__file__).parent.parent / "build" / "pathinst"


def get_timestamp_pattern():
    """Return the pattern for a timestamp."""
    return r"^\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}\] "


def get_debug_pattern():
    """Return the pattern for a debug message."""
    return get_timestamp_pattern() + r"\[debug\] "
