"""Collection of utilities for papinst's tests."""
import pathlib


def get_exe_path():
    """Return the path to the papinst binary."""
    return pathlib.Path(__file__).parent.parent.parent / "build" / "bin" / "papinst"


def get_timestamp_pattern():
    """Return the pattern for a timestamp."""
    return r"^\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}\] "


def get_pid_pattern():
    """Return the pattern for a PID."""
    return r"\[\d{1,5}\] "


def get_debug_pattern():
    """Return the pattern for a debug message."""
    return get_timestamp_pattern() + get_pid_pattern() + r"\[debug\] "


def get_error_pattern():
    """Return the pattern for an error message."""
    return get_timestamp_pattern() + get_pid_pattern() + r"\[error\] "
