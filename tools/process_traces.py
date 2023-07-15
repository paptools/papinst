#!/usr/bin/env python

import json

import argparse
import paptree


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_file", help="input file")
    parser.add_argument("output_file", help="output file")
    args = parser.parse_args()

    trees = paptree.utils.from_file(args.input_file)
    print(f"Loaded {len(trees)} traces.")

    # It is expected that we start with some known values.
    # NOTE: This currently uses signatures to generalize (i.e., it is not accounting for context).
    known = {
        "int operator-(unsigned short, int)": ["C_OP_INT_SUB"],
        "int operator+(unsigned short, int)": ["C_OP_INT_ADD"],
        "value_type operator=(value_type, int)": ["C_OP_INT_ASSIGN"],
        "reference std::vector<unsigned long long>::operator[](size_type)": [
            "C_OP_VEC_SUBSCRIPT"
        ],
        "value_type operator=(value_type, unsigned long long)": [
            "C_OP_VEC_ASSIGN"
        ],
        "unsigned long long fibonacci::(anonymous namespace)::RecursiveMemoImpl(unsigned short, std::vector<unsigned long long> &)": [
            "C_FIB_RECURSIVE_MEMO_IMPL"
        ],
        "unsigned long long operator+(unsigned long long, unsigned long long)": [
            "C_OP_ULL_ADD"
        ],
        "unsigned long long operator=(unsigned long long, unsigned long long)": [
            "C_OP_ULL_ASSIGN"
        ],
    }

    results = paptree.analyze.analyze(known, trees)
    print("\nResults:")
    print(json.dumps(results, indent=4))
    with open(args.output_file, "w") as f_out:
        f_out.write(json.dumps(results, indent=2))


if __name__ == "__main__":
    main()
