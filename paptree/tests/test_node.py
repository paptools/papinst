import paptree


class TestGroupNode:
    def test_name(self):
        node = paptree.Node("foo")
        assert node.name == "foo"

    def test_no_parent(self):
        node = paptree.Node("foo")
        assert node.parent == None

    def test_parent(self):
        parent = paptree.Node("parent")
        child = paptree.Node("child", parent=parent)
        assert child.parent == parent

    def test_no_children(self):
        node = paptree.Node("foo")
        assert node.children == ()

    def test_children(self):
        parent = paptree.Node("parent")
        child = paptree.Node("child", parent=parent)
        assert parent.children == (child,)

    def test_children_order(self):
        parent = paptree.Node("parent")
        child1 = paptree.Node("child1", parent=parent)
        child2 = paptree.Node("child2", parent=parent)
        assert parent.children == (child1, child2)


class TestGroupStmtNode:
    def test_properties(self):
        type_ = "Return"
        desc = "return 0"
        node = paptree.StmtNode("foo", type_, desc)
        assert node.type == type_
        assert node.desc == desc


class TestGroupCallNode:
    def test_properties(self):
        type_ = "Callee"
        sig = "void foo(int x)"
        params = [{"name": "x", "value": "1"}]
        node = paptree.CallNode("foo", type_, sig, params)
        assert node.type == type_
        assert node.sig == sig
        assert node.params == params
