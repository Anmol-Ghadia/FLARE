#!/usr/bin/env python3
"""
Plot Venn diagrams of fuzzer edge coverage from CSV files, combining
multiple trial runs (one CSV per trial) and annotating each region with
the mean and the min-max range across trials to convey variability.

Expected CSV columns:
    library,tools,n_tools,union,intersect,exclusive

Row types:
  - tools == "library", n_tools == 0: 'union' is the total number of
    edges in the whole library (the universe size). Used only for the
    coverage % annotation.
  - n_tools == 1 (e.g. tools == "opencode"): 'exclusive' is the count
    of edges covered ONLY by that tool and no other tool tested.
  - n_tools == k (e.g. tools == "opencode+promefuzz"): 'exclusive' is
    the count of edges covered by exactly that combination of tools
    (all listed tools, and no tool outside the list).

These 'exclusive' values map 1:1 onto Venn diagram regions, so no
inclusion-exclusion math is needed -- we just place each row's
'exclusive' value into the region defined by which tools are present.

Expected file layout (one CSV per trial):
    csv/<library>_<run>.csv
    e.g. csv/libtiff_0.csv, csv/libtiff_1.csv, csv/libtiff_2.csv

Requires: pandas, matplotlib, matplotlib-venn
    pip install pandas matplotlib matplotlib-venn

Usage:
    python plot_venn.py --csv-dir csv --out-dir plots
    python plot_venn.py --csv-dir csv --library libtiff
"""
import argparse
import glob
import os
import re

import pandas as pd
import matplotlib.pyplot as plt
from matplotlib_venn import venn2, venn3

LIBRARY_ROW_TOOLS = "library"

# venn3/venn2 region-id ordering used by matplotlib_venn's subsets tuple
VENN3_ORDER = ['100', '010', '110', '001', '101', '011', '111']
VENN2_ORDER = ['10', '01', '11']


def parse_tools(tools_str):
    return frozenset(t.strip() for t in tools_str.split('+'))


def discover_libraries(csv_dir):
    """Find distinct '<library>' prefixes from '<library>_<run>.csv' files."""
    libs = set()
    for f in glob.glob(os.path.join(csv_dir, "*.csv")):
        base = os.path.basename(f)
        m = re.match(r"(.+)_(\w+)\.csv$", base)
        if m:
            libs.add(m.group(1))
    return sorted(libs)


def load_library_data(csv_dir, library):
    """Load all trial CSVs for a library.

    Returns:
        per_run: dict  run -> {frozenset(tools): exclusive_value}
        universe_by_run: dict run -> total edges in library (from the
                          'library' row), or None if absent
    """
    pattern = os.path.join(csv_dir, f"{library}_*.csv")
    files = sorted(glob.glob(pattern))
    if not files:
        raise FileNotFoundError(f"No CSV files found for library '{library}' in {csv_dir}")

    per_run = {}
    universe_by_run = {}
    for f in files:
        base = os.path.basename(f)
        m = re.match(rf"{re.escape(library)}_(\w+)\.csv$", base)
        run = m.group(1) if m else base

        df = pd.read_csv(f)
        if 'library' in df.columns:
            df = df[df['library'] == library]

        combos = {}
        universe = None
        for _, row in df.iterrows():
            tools_str = str(row['tools'])
            if tools_str == LIBRARY_ROW_TOOLS or int(row['n_tools']) == 0:
                universe = row['union']
                continue
            tools = parse_tools(tools_str)
            combos[tools] = row['exclusive']

        per_run[run] = combos
        universe_by_run[run] = universe

    return per_run, universe_by_run


def region_id_for_combo(tools_present, tool_names):
    """matplotlib_venn region id: string of '1'/'0' per tool_names order."""
    return ''.join('1' if t in tools_present else '0' for t in tool_names)


def fmt_region(mean, lo, hi):
    if round(lo) == round(hi) == round(mean):
        return f"{int(round(mean))}"
    return f"{int(round(mean))}\n[{int(round(lo))}\u2013{int(round(hi))}]"


def plot_library(library, per_run, universe_by_run, out_dir):
    all_tools = sorted({t for combos in per_run.values() for tools in combos for t in tools})
    n = len(all_tools)

    if n not in (2, 3):
        print(f"[skip] {library}: found {n} tools {all_tools}; "
              f"only 2- or 3-tool Venn diagrams are supported.")
        return

    order = VENN3_ORDER if n == 3 else VENN2_ORDER

    # per_run_regions: run -> {region_id: exclusive_value}
    per_run_regions = {}
    for run, combos in per_run.items():
        regions = {}
        for tools, exclusive_val in combos.items():
            rid = region_id_for_combo(tools, all_tools)
            regions[rid] = exclusive_val
        missing = [rid for rid in order if rid not in regions]
        if missing:
            print(f"[warn] {library} run {run}: missing region(s) {missing}, skipping this run")
            continue
        per_run_regions[run] = regions

    if not per_run_regions:
        print(f"[skip] {library}: no complete trial had all required tool combos")
        return

    stats = {}
    for rid in order:
        vals = [regions[rid] for regions in per_run_regions.values()]
        stats[rid] = (sum(vals) / len(vals), min(vals), max(vals))

    fig, ax = plt.subplots(figsize=(7, 7))

    subsets = tuple(max(stats[rid][0], 0) for rid in order)
    if n == 3:
        v = venn3(subsets=subsets, set_labels=all_tools, ax=ax)
    else:
        v = venn2(subsets=subsets, set_labels=all_tools, ax=ax)

    for rid in order:
        label = v.get_label_by_id(rid)
        if label is not None:
            mean, lo, hi = stats[rid]
            label.set_text(fmt_region(mean, lo, hi))
            label.set_fontsize(9)

    n_runs = len(per_run_regions)
    title = f"{library} \u2014 edge coverage overlap\n(mean of {n_runs} trials; bracket = [min\u2013max])"

    universes = [u for u in universe_by_run.values() if u is not None]
    if universes:
        mean_universe = sum(universes) / len(universes)
        total_covered = sum(stats[rid][0] for rid in order)
        pct = 100 * total_covered / mean_universe if mean_universe else 0
        title += f"\nTotal library edges: {mean_universe:.0f}  |  Covered by any tool: {total_covered:.0f} ({pct:.1f}%)"

    ax.set_title(title, fontsize=11)

    out_path = os.path.join(out_dir, f"{library}_venn.png")
    fig.tight_layout()
    fig.savefig(out_path, dpi=200)
    plt.close(fig)
    print(f"[ok] wrote {out_path}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--csv-dir', default='csv', help="Directory with '<library>_<run>.csv' files (default: csv)")
    parser.add_argument('--out-dir', default='plots', help='Directory to write output PNGs (default: plots)')
    parser.add_argument('--library', action='append',
                         help='Restrict to specific library name(s); repeatable. Default: auto-discover all.')
    args = parser.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)

    libraries = args.library if args.library else discover_libraries(args.csv_dir)
    if not libraries:
        print(f"No files matching '<library>_<run>.csv' found in {args.csv_dir}")
        return

    for library in libraries:
        try:
            per_run, universe_by_run = load_library_data(args.csv_dir, library)
        except FileNotFoundError as e:
            print(f"[skip] {e}")
            continue
        plot_library(library, per_run, universe_by_run, args.out_dir)


if __name__ == '__main__':
    main()
