import subprocess


def test_paptree_attrs(tmp_path):
    script_path = tmp_path / "script.py"
    script_path.write_text("""
import paptree

# Node classes are attributes of the paptree.
assert hasattr(paptree, 'Node')
assert hasattr(paptree, 'StmtNode')
assert hasattr(paptree, 'CallNode')

# The utils module is also an attribute of the paptree.
assert hasattr(paptree, 'utils')
""")
    proc = subprocess.run(["python", str(script_path)])
    assert proc.returncode == 0
