import anytree


class Node(anytree.NodeMixin):
    def __init__(self, name, parent=None, children=None):
        super(Node, self).__init__()
        self.name = name
        self.parent = parent
        if children:
            self.children = children


class StmtNode(Node):
    def __init__(self, name, type_, desc, parent=None, children=None):
        super(StmtNode, self).__init__(name, parent, children)
        self._type = type_
        self._desc = desc

    @property
    def type(self):
        return self._type

    @property
    def desc(self):
        return self._desc


class CallNode(Node):
    def __init__(self, name, type_, sig, params, parent=None, children=None):
        super(CallNode, self).__init__(name, parent, children)
        self._type = type_
        self._sig = sig
        self._params = params

    @property
    def type(self):
        return self._type

    @property
    def sig(self):
        return self._sig

    @property
    def params(self):
        return self._params
