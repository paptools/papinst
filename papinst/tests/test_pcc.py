"""Collection of tests for papinst's wrapper script."""
import re
import subprocess
import pathlib

import pytest

from tests import utils


def _get_pcc_path():
    """Return the path to the pcc binary."""
    return pathlib.Path(__file__).parent.parent.parent / "build" / "bin" / "pcc"


@pytest.fixture(params=["--help", "-h"])
def help_cli_flag(request):
    """Pytest fixture to return each help flag."""
    return request.param


def test_help(help_cli_flag):
    """Test that invocation with the help flag displays the usage message."""
    result = subprocess.run(
        [_get_pcc_path(), help_cli_flag],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    assert result.returncode == 0
    assert "Usage:\n  pcc" in result.stdout
    assert "Options:" in result.stdout
    assert result.stderr == ""


def test_wrapper_opts_env_var():
    """Test that the PAPINST_WRAPPER_OPTS environment variable is respected."""
    result = subprocess.run(
        [_get_pcc_path(), "echo", "hello"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        env={"PAPINST_WRAPPER_OPTS": "--help"},
    )
    assert result.returncode == 0
    assert "Usage:\n  papinst" in result.stdout


def test_wrapper_opts_env_var_multi_value():
    """
    Test that the PAPINST_WRAPPER_OPTS environment variable is respected when it
    contains multiple values.
    """
    result = subprocess.run(
        [_get_pcc_path(), "echo", "hello"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        env={"PAPINST_WRAPPER_OPTS": "--verbose --version"},
    )
    assert result.returncode == 0
    assert "papinst 0.1.0" in result.stdout
