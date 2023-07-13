import anytree
import sympy


class Node(anytree.AnyNode):
    def __init__(self, name, type_, parent=None, children=None, **kwargs):
        super(Node, self).__init__(
            name=name, _type=type_, parent=parent, children=children, **kwargs
        )

    def __eq__(self, other):
        if not isinstance(other, Node):
            # don't attempt to compare against unrelated types
            return NotImplemented
        return repr(self) == repr(other)

    @property
    def type(self):
        return self._type

    @staticmethod
    def is_call_type(type_):
        return type_ in ["CallerExpr", "CalleeExpr"]

    def is_call_node(self):
        return Node.is_call_type(self.type)

    @staticmethod
    def is_cf_type(type_):
        return type_ in [
            "IfThenStmt",
            "ReturnStmt",
            "CXXThrowExpr",
            "ForStmt",
            "LoopIter",
        ]

    def is_cf_node(self):
        return Node.is_cf_type(self.type)

    @staticmethod
    def is_loop_type(type_):
        return type_ in ["ForStmt"]

    def is_loop_node(self):
        return Node.is_loop_type(self.type)

    @staticmethod
    def from_trace(trace):
        if Node.is_call_type(trace["type"]):
            return CallNode.from_trace(trace)
        elif Node.is_loop_type(trace["type"]):
            return LoopNode.from_trace(trace)
        else:
            return StmtNode.from_trace(trace)

    def get_cf_nodes(self):
        raise NotImplementedError

    def to_expr(self, known_exprs):
        raise NotImplementedError

    def get_loop_nodes(self):
        """Returns a list of loop nodes."""
        loop_nodes = []
        for child in self.children:
            loop_nodes.extend(child.get_loop_nodes())
        if self.is_loop_node():
            loop_nodes.append(self)
        return loop_nodes


class StmtNode(Node):
    def __init__(self, name, type_, desc, parent=None, children=None):
        super(StmtNode, self).__init__(name, type_, parent, children, desc=desc)
        self._desc = desc

    @property
    def desc(self):
        return self._desc

    @staticmethod
    def from_trace(trace):
        if Node.is_call_type(type_ := trace["type"]):
            raise ValueError(f"Type '{type_}' is not a StmtNode type.")
        children = [Node.from_trace(child) for child in trace["children"]]
        desc = (
            trace["sig"] if "sig" in trace else trace["desc"]
        )  # For op nodes.
        return StmtNode(
            name=trace["id"],
            type_=type_,
            desc=desc,
            children=children,
        )

    def get_cf_nodes(self):
        """Return a list of control flow nodes."""
        cf_nodes = [self.name] if self.is_cf_node() else []
        for child in self.children:
            cf_nodes.extend(child.get_cf_nodes())
        return cf_nodes

    def to_expr(self, known_exprs):
        """Returns a symbolic expression of the tree."""
        if self.desc in known_exprs:
            expr = sympy.sympify(known_exprs[self.desc])
        else:
            expr = (
                sympy.sympify(f"T_{self.name}")
                if not self.is_cf_node()
                else None
            )
        for child in self.children:
            child_expr = child.to_expr(known_exprs)
            if child_expr is None:
                continue
            expr = child_expr if expr is None else expr + child_expr
        return expr


class LoopNode(StmtNode):
    def __init__(self, name, type_, desc, parent=None, children=None):
        super(LoopNode, self).__init__(name, type_, desc, parent, children)
        # TODO: Iter count may need to be a list to support complex loops.
        self.iter_count = None
        self._cf_nodes = None
        self.loop_expr = None

    def get_iter_count(self):
        """Return the number of iterations of this loop."""
        if self.iter_count is None:
            self.get_cf_nodes()
        return self.iter_count

    @staticmethod
    def from_trace(trace):
        if not Node.is_loop_type(type_ := trace["type"]):
            raise ValueError(f"Type '{type_}' is not a LooNode type.")
        children = [Node.from_trace(child) for child in trace["children"]]
        desc = (
            trace["sig"] if "sig" in trace else trace["desc"]
        )  # For op nodes.
        return LoopNode(
            name=trace["id"],
            type_=type_,
            desc=desc,
            children=children,
        )

    def get_cf_nodes(self):
        """Return a list of control flow nodes."""
        if self._cf_nodes is not None:
            return self._cf_nodes

        self.iter_count = 0
        cf_nodes = [self.name] if self.is_cf_node() else []
        prev_cf_nodes = [None, None]  # 1. body, 2. inc
        for idx, child in enumerate(self.children):
            curr_cf_nodes = child.get_cf_nodes()
            pos = idx % 2
            if prev_cf_nodes[pos] is None:
                prev_cf_nodes[pos] = curr_cf_nodes
            else:
                if prev_cf_nodes[pos] != curr_cf_nodes:
                    raise ValueError(
                        "Loop children have different control flow nodes."
                    )
            if pos == 1:  # Increment every time the body is executed.
                self.iter_count += 1
        for child_cf_nodes in prev_cf_nodes:
            if child_cf_nodes is not None:
                cf_nodes.extend(child_cf_nodes)
        return cf_nodes

    def to_expr(self, known_exprs):
        """Returns a symbolic expression of the tree."""
        expr = None
        for idx, child in enumerate(self.children):
            child_expr = child.to_expr(known_exprs)
            if child_expr is None:
                continue
            if expr is None:
                expr = child_expr
            else:
                expr += child_expr
            if self.loop_expr and idx == 1:
                print(f"Found established loop expr: {self.loop_expr}")
                expr = sympy.Mul(self.loop_expr, expr)
                break
        return expr


class CallNode(Node):
    def __init__(self, name, type_, sig, params, parent=None, children=None):
        super(CallNode, self).__init__(
            name, type_, parent, children, sig=sig, params=params
        )
        self._sig = sig
        self._params = params

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

    def get_cf_nodes(self):
        """Return a list of control flow nodes."""
        cf_nodes = [self.name] if self.is_cf_node() else []
        for child in self.children:
            cf_nodes.extend(child.get_cf_nodes())
        return cf_nodes

    def to_expr(self, known_exprs):
        """Returns a symbolic expression of the tree."""
        if self.type == "CallerExpr":
            if self.sig in known_exprs:
                expr = sympy.sympify(known_exprs[self.sig])
            else:
                expr = sympy.sympify(f"T_{self.name}")
        else:
            expr = sympy.sympify(f"C_{self.name}")
            for child in self.children:
                child_expr = child.to_expr(known_exprs)
                if child_expr is None:
                    continue
                expr += child_expr
        return expr
