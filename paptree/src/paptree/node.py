import anytree


class Node(anytree.Node):
    def __init__(self, name, parent=None, children=None, **kwargs):
        super(Node, self).__init__(name, parent, children, **kwargs)

    @staticmethod
    def is_call_type(type_):
        return type_ in ["CallerExpr", "CalleeExpr"]

    @staticmethod
    def from_trace(trace):
        if Node.is_call_type(trace["type"]):
            return CallNode.from_trace(trace)
        else:
            return StmtNode.from_trace(trace)


class StmtNode(Node):
    def __init__(self, name, type_, desc, parent=None, children=None):
        super(StmtNode, self).__init__(
            name, parent, children, type=type_, desc=desc
        )
        self._type = type_
        self._desc = desc

    @property
    def type(self):
        return self._type

    @property
    def desc(self):
        return self._desc

    @staticmethod
    def from_trace(trace):
        if Node.is_call_type(type_ := trace["type"]):
            raise ValueError(f"Type '{type_}' is not a StmtNode type.")
        children = [Node.from_trace(child) for child in trace["children"]]
        return StmtNode(
            name=trace["id"],
            type_=type_,
            desc=trace["desc"],
            children=children,
        )


class CallNode(Node):
    def __init__(self, name, type_, sig, params, parent=None, children=None):
        super(CallNode, self).__init__(
            name, parent, children, type=type_, sig=sig, params=params
        )
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

    @staticmethod
    def from_trace(trace):
        if not Node.is_call_type(type_ := trace["type"]):
            raise ValueError(f"Type '{type_}' is not a CallNode type.")
        children = [Node.from_trace(child) for child in trace["children"]]
        return CallNode(
            name=trace["id"],
            type_=type_,
            sig=trace["sig"],
            params=trace["params"],
            children=children,
        )
