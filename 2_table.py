#!/usr/bin/env python3
"""
Compare edge coverage of multiple evaluations against a single baseline.

Each evaluation is a directory containing a `union.txt` file (one edge/line
per row). The script computes, for each evaluation vs. the baseline:
  - its own edge count
  - union size
  - intersection size
  - edges only in the evaluation (eval - baseline)
  - edges only in the baseline (baseline - eval)

Usage:
    python3 compare_coverage.py --baseline D_DIR:D_NAME EVAL_DIR:EVAL_NAME [EVAL_DIR:EVAL_NAME ...]

Example:
    python3 compare_coverage.py \\
        --baseline temp-baseline:Baseline \\
        temp-claude:Claude \\
        temp-promefuzz:PromeFuzz \\
        temp-other:OtherTool
"""

import argparse
import os
import sys


def load_edges(directory):
    path = os.path.join(directory, "union.txt")
    if not os.path.isfile(path):
        sys.exit(f"Error: {path} not found")
    with open(path) as f:
        return {line.strip() for line in f if line.strip()}


def parse_spec(spec):
    """Parse DIR:NAME, defaulting NAME to the directory basename."""
    if ":" in spec:
        directory, name = spec.split(":", 1)
    else:
        directory, name = spec, os.path.basename(os.path.normpath(spec))
    return directory, name

def get_formatted_str(value, max):
    return f"{value} ({value*100/max:.2f}%)"

def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--baseline", required=True, help="Baseline evaluation as DIR:NAME")
    parser.add_argument("--total-edges", required=True, help="Total edge count")
    parser.add_argument("evals", nargs="+", help="Evaluations to compare, each as DIR:NAME")
    args = parser.parse_args()

    base_dir, base_name = parse_spec(args.baseline)
    total_edges = int(parse_spec(args.total_edges)[0])
    base_edges = load_edges(base_dir)

    rows = []
    for spec in args.evals:
        eval_dir, eval_name = parse_spec(spec)
        edges = load_edges(eval_dir)

        union = edges | base_edges
        intersection = edges & base_edges
        only_eval = edges - base_edges
        only_base = base_edges - edges

        rows.append({
            "name": eval_name,
            "count": get_formatted_str(len(edges), total_edges),
            "union": get_formatted_str(len(union), total_edges),
            "intersection": get_formatted_str(len(intersection), total_edges),
            "only_eval": get_formatted_str(len(only_eval), total_edges),
            "only_base": get_formatted_str(len(only_base), total_edges),
        })

    # --- Print table ---
    headers = [
        "Evaluation", "Count", f"Union w/ {base_name}", f"Intersect w/ {base_name}",
        f"Eval - {base_name}", f"{base_name} - Eval",
    ]
    col_widths = [max(len(h), 10) for h in headers]
    for r in rows:
        vals = [r["name"], r["count"], r["union"], r["intersection"], r["only_eval"], r["only_base"]]
        for i, v in enumerate(vals):
            col_widths[i] = max(col_widths[i], len(str(v)))

    def print_row(vals):
        print(" | ".join(str(v).ljust(w) for v, w in zip(vals, col_widths)))

    print(f"Total Edge Count: {total_edges}")
    print_row(headers)
    print_row(["-" * w for w in col_widths])
    for r in rows:
        print_row([r["name"], r["count"], r["union"], r["intersection"], r["only_eval"], r["only_base"]])


if __name__ == "__main__":
    main()
