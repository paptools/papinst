"""Collection of tests for pathinst's CLI."""
import pathlib
import subprocess

import pytest


def get_pathinst_bin_path():
    """Return the path to the pathinst binary."""
    return pathlib.Path(__file__).parent.parent / 'build' / 'pathinst'


@pytest.fixture(params=['--help', '-h'])
def help_cli_flag(request):
    """Pytest fixture to return each help flag."""
    return request.param


def test_cli_help(help_cli_flag):
    """Test that invocation with the help flag displays the usage message."""
    result = subprocess.run([get_pathinst_bin_path(), help_cli_flag],
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            universal_newlines=True)
    assert result.returncode == 0
    assert 'Usage:' in result.stdout
    assert 'Description:' in result.stdout
    assert 'Options:' in result.stdout
    assert result.stderr == ''


@pytest.fixture(params=['--version', '-V'])
def version_cli_flag(request):
    """Pytest fixture to return each version flag."""
    return request.param


def test_cli_version(version_cli_flag):
    """Test that invocation with the help flag displays the usage message."""
    result = subprocess.run([get_pathinst_bin_path(), version_cli_flag],
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            universal_newlines=True)
    assert result.returncode == 0
    assert result.stdout == "pathinst 0.1.0\n"
    assert result.stderr == ''
