"""Collection of tests for papinst's CLI."""
import re
import subprocess

import pytest

from tests import utils


@pytest.fixture(params=["--help", "-h"])
def help_cli_flag(request):
    """Pytest fixture to return each help flag."""
    return request.param


def test_cli_help(help_cli_flag):
    """Test that invocation with the help flag displays the usage message."""
    result = subprocess.run(
        [utils.get_exe_path(), help_cli_flag],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    assert result.returncode == 0
    assert "Usage:" in result.stdout
    assert "Description:" in result.stdout
    assert "Options:" in result.stdout
    assert result.stderr == ""


@pytest.fixture(params=["--version", "-V"])
def version_cli_flag(request):
    """Pytest fixture to return each version flag."""
    return request.param


def test_cli_version(version_cli_flag):
    """
    Test that invocation with the version flag displays the version message.
    """
    result = subprocess.run(
        [utils.get_exe_path(), version_cli_flag],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    assert result.returncode == 0
    assert result.stdout == "papinst 0.1.0\n"
    assert result.stderr == ""


@pytest.fixture(params=["--verbose", "-v"])
def verbose_cli_flag(request):
    """Pytest fixture to return each verbose flag."""
    return request.param


def test_cli_verbose(verbose_cli_flag):
    """Test that invocation with the verbose flag enabled verbose logging."""
    result = subprocess.run(
        [utils.get_exe_path(), verbose_cli_flag, "--", "echo", "hello"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    assert result.returncode == 0
    patterns = [
        utils.get_debug_pattern() + r"Parsing command 'echo hello'.$",
        utils.get_debug_pattern()
        + r"Executable 'echo' is not a supported compiler. Skipping parse.$",
        utils.get_debug_pattern() + r"Executing command 'echo hello'.$",
        r"^hello$",
    ]
    for idx, line in enumerate(result.stdout.splitlines()):
        assert re.match(patterns[idx], line)
    assert result.stderr == ""


def test_cli_no_args():
    """Test that invocation without any arguments displays the usage message."""
    result = subprocess.run(
        [utils.get_exe_path()],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    assert result.returncode == 1
    assert "Usage:" in result.stdout
    assert result.stderr == ""


def test_cli_invalid_flag():
    """Test that invocation with an invalid flag displays the usage message."""
    result = subprocess.run(
        [utils.get_exe_path(), "--invalid"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    assert result.returncode == 1
    patterns = [
        utils.get_error_pattern() + r"Error: unrecognised option '--invalid'$",
    ]
    for idx, line in enumerate(result.stdout.splitlines()):
        assert re.match(patterns[idx], line)
    assert result.stderr == ""


def test_cli_invalid_command():
    """
    Test that invocation with an invalid command displays the error message.
    """
    result = subprocess.run(
        [utils.get_exe_path(), "--", "invalid"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    assert result.returncode == 127
    assert result.stdout == ""
    assert result.stderr == "sh: invalid: command not found\n"


def test_cli_valid_command():
    """Test that invocation with a valid command executes the command."""
    result = subprocess.run(
        [utils.get_exe_path(), "--", "echo", "hello"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    assert result.returncode == 0
    assert result.stdout == "hello\n"
    assert result.stderr == ""
