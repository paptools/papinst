import pytest

from paptree import Node, StmtNode, CallNode


class TestGroupNode:
    def test_properties(self):
        node = Node("foo")
        assert node.name == "foo"

    def test_no_parent(self):
        node = Node("foo")
        assert node.parent == None

    def test_parent(self):
        parent = Node("parent")
        child = Node("child", parent=parent)
        assert child.parent == parent

    def test_no_children(self):
        node = Node("foo")
        assert node.children == ()

    def test_children(self):
        parent = Node("parent")
        child = Node("child", parent=parent)
        assert parent.children == (child,)

    def test_children_order(self):
        parent = Node("parent")
        child1 = Node("child1", parent=parent)
        child2 = Node("child2", parent=parent)
        assert parent.children == (child1, child2)

    def test_is_call_type(self):
        # Exhaustive test of call node types.
        assert Node.is_call_type("CallExpr") == True
        assert Node.is_call_type("CalleeExpr") == True
        # Non-exhaustive test of non-call node types.
        assert Node.is_call_type("ReturnStmt") == False
        assert Node.is_call_type("IfThenStmt") == False
        assert Node.is_call_type("ForStmt") == False


class TestGroupStmtNode:
    def test_properties(self):
        type_ = "Return"
        desc = "return 0"
        node = StmtNode("foo", type_, desc)
        assert node.type == type_
        assert node.desc == desc

    def test_from_trace_incomplete_entry(self):
        with pytest.raises(KeyError, match="'type'"):
            StmtNode.from_trace({})

    def test_from_trace_invalid_type(self):
        with pytest.raises(
            ValueError, match="Type 'CallExpr' is not a StmtNode type."
        ):
            StmtNode.from_trace({"type": "CallExpr"})

    def test_from_trace_without_children(self):
        trace = {
            "id": 123,
            "type": "ReturnStmt",
            "desc": "return 0",
            "children": [],
        }
        node = StmtNode.from_trace(trace)
        assert node.name == trace["id"]
        assert node.type == trace["type"]
        assert node.desc == trace["desc"]
        assert node.children == ()

    def test_from_trace_with_children(self):
        trace = {
            "id": 123,
            "type": "ReturnStmt",
            "desc": "return 0",
            "children": [
                {
                    "id": 456,
                    "type": "ReturnStmt",
                    "desc": "return 1",
                    "children": [],
                }
            ],
        }
        node = StmtNode.from_trace(trace)
        assert node.name == trace["id"]
        assert node.type == trace["type"]
        assert node.desc == trace["desc"]
        assert len(node.children) == 1
        child = node.children[0]
        assert child.name == trace["children"][0]["id"]
        assert child.type == trace["children"][0]["type"]
        assert child.desc == trace["children"][0]["desc"]
        assert child.children == ()


class TestGroupCallNode:
    def test_properties(self):
        type_ = "Callee"
        sig = "void foo(int x)"
        params = [{"name": "x", "value": "1"}]
        node = CallNode("foo", type_, sig, params)
        assert node.type == type_
        assert node.sig == sig
        assert node.params == params

    def test_from_trace_incomplete_entry(self):
        with pytest.raises(KeyError, match="'type'"):
            CallNode.from_trace({})

    def test_from_trace_invalid_type(self):
        with pytest.raises(
            ValueError, match="Type 'ReturnStmt' is not a CallNode type."
        ):
            CallNode.from_trace({"type": "ReturnStmt"})

    def test_from_trace_without_children(self):
        trace = {
            "id": 123,
            "type": "CallExpr",
            "sig": "int foo(int, int)",
            "params": [
                {"name": "a", "value": "-1"},
                {"name": "b", "value": "1"},
            ],
            "children": [],
        }
        node = CallNode.from_trace(trace)
        assert node.name == trace["id"]
        assert node.type == trace["type"]
        assert node.sig == trace["sig"]
        assert node.params == trace["params"]
        assert node.children == ()

    def test_from_trace_with_children(self):
        trace = {
            "id": 123,
            "type": "CallExpr",
            "sig": "int foo(int, int)",
            "params": [
                {"name": "a", "value": "-1"},
                {"name": "b", "value": "1"},
            ],
            "children": [
                {
                    "id": 456,
                    "type": "ReturnStmt",
                    "desc": "return 1",
                    "children": [],
                },
            ],
        }
        node = CallNode.from_trace(trace)
        assert node.name == trace["id"]
        assert node.type == trace["type"]
        assert node.sig == trace["sig"]
        assert node.params == trace["params"]
        assert len(node.children) == 1
        child = node.children[0]
        assert child.name == trace["children"][0]["id"]
        assert child.type == trace["children"][0]["type"]
        assert child.desc == trace["children"][0]["desc"]
        assert child.children == ()
