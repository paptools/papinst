import re

import anytree
import pytest

from paptree import Node, Tree


class TestGroupTree:
    def test_name(self):
        tree = Tree("foo")
        assert tree.name == "foo"

    def test_no_root(self):
        tree = Tree("foo")
        assert tree.root == None

    def test_root(self):
        root = Node("root")
        tree = Tree("foo", root=root)
        assert tree.root == root

    # def test_from_json_traces_entry_missing(self):
    #    with pytest.raises(ValueError, match="No traces found in JSON object."):
    #        Tree.from_trace("foo", {})

    # def test_from_json_traces_entry_empty(self):
    #    with pytest.raises(ValueError, match="No traces found in JSON object."):
    #        Tree.from_json("foo", {"traces": []})
