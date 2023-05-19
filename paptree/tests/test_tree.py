import re

import anytree
import pytest

from paptree import Node, Tree


class TestGroupTree:
    def test_name_only(self):
        tree = Tree("foo")
        assert tree.name == "foo"
        assert tree.root == None
        assert tree.children == ()

    def test_root(self):
        root = Node("root")
        tree = Tree("foo", root=root)
        assert tree.name == "foo"
        assert tree.root == root
        assert tree.children == ()

    def test_children(self):
        child_a = Node("child_a")
        child_b = Node("child_b")
        root = Node("root", children=[child_a, child_b])
        tree = Tree("foo", root=root)
        assert tree.name == "foo"
        assert tree.root == root
        assert tree.children == (child_a, child_b)
