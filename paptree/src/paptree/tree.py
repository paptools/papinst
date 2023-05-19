from paptree import CallNode


class Tree:
    def __init__(self, name, root=None):
        self.name = name
        self.root = root

    @property
    def children(self):
        return self.root.children if self.root else ()

    @staticmethod
    def from_trace(trace):
        """Create a tree from a paptrace trace entry."""
        if not isinstance(trace, dict):
            raise TypeError("The JSON object is not a dict.")
        root = CallNode.from_trace(trace)
        param_str = ", ".join(
            [f"{p['name']}={p['value']}" for p in root.params]
        )
        name = f"{root.sig}({param_str})"
        return Tree(name, root)
