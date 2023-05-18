import json
import pathlib
import pytest

from paptree import utils


class TestGroupFromTraceFile:
    def test_file_not_found(self):
        path = pathlib.Path("does_not_exist")
        with pytest.raises(FileNotFoundError):
            utils.from_file(path)

    def test_file_not_json(self, tmp_path):
        path = tmp_path / "not_json.txt"
        with open(path, "w") as f:
            f.write("not json")
        with pytest.raises(json.decoder.JSONDecodeError):
            utils.from_file(path)

    def test_success(self, tmp_path):
        data = {
            "traces": [
                {
                    "id": 123,
                    "type": "CallExpr",
                    "sig": "int foo(int, int)",
                    "params": [
                        {"name": "a", "value": "-1"},
                        {"name": "b", "value": "1"},
                    ],
                    "children": [],
                }
            ]
        }
        path = tmp_path / "trace.json"
        with open(path, "w") as f:
            json.dump(data, f)
        trees = utils.from_file(path)
        assert len(trees) == 1
        tree = trees[0]
        assert tree.name == "int foo(int, int)(a=-1, b=1)"
        assert tree.root.name == 123


class TestGroupFromJson:
    def test_no_traces_entry(self):
        with pytest.raises(KeyError, match="'traces'"):
            utils.from_json({"version": "0.1.0"})

    def test_non_list_traces_entry(self):
        with pytest.raises(TypeError, match="The traces entry is not a list."):
            utils.from_json({"version": "0.1.0", "traces": {}})

    def test_empty_traces_list(self):
        assert utils.from_json({"version": "0.1.0", "traces": []}) == []

    def test_single_trace(self, tmp_path):
        data = {
            "traces": [
                {
                    "id": 123,
                    "type": "CallExpr",
                    "sig": "int foo(int, int)",
                    "params": [
                        {"name": "a", "value": "-1"},
                        {"name": "b", "value": "1"},
                    ],
                    "children": [],
                }
            ]
        }
        trees = utils.from_json(data)
        assert len(trees) == 1
        tree = trees[0]
        assert tree.name == "int foo(int, int)(a=-1, b=1)"
        assert tree.root.name == 123

    # def test_trace_not_as_dict(self):
    #    data = {
    #        "traces": [
    #            [
    #                "children": [],
    #                "id": 1,
    #                "desc": "return 0",
    #                "type": "ReturnStmt",
    #            ],
    #        ]
    #    }

    # def test_trace_non_call_origin(self):
    #    data = {
    #        "traces": [
    #            {
    #                "children": [],
    #                "id": 1,
    #                "desc": "return 0",
    #                "type": "ReturnStmt",
    #            },
    #        ]
    #    }
    #    with pytest.raises(Exception):
    #        utils.from_json(data)


# def test_paptrace_file():
#    data_path = pathlib.Path(__file__).parent / "data/paptrace.json"
#    with open(data_path, "r") as f:
#        traces = json.load(f)["traces"]
#    trees = []
#    for trace in traces:
#        trees.append(trace["tree"])
