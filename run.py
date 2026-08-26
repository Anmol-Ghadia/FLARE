#!/usr/bin/env python3
"""
Set up Magma fuzzing targets: patch captainrc, place it, collect harnesses
from a YAML manifest, and generate configrc.

Edit the CONFIG block below, then just run:
    python setup_magma_target.py
"""
import re
import shutil
import yaml
import subprocess
import os
import sys
import glob
import csv
from pathlib import Path

# ---------------- CONFIG (edit this) -------------------
# libraries being fuzzed as a python list of strings
# note the capitalization
LIBRARIES = ["libtiff"]

# running in development environment?
# set False if unsure
DEV=False
# -------------------------------------------------------

#RAW_RESULTS_DIR = Path("/scratch-ssd/anmol/experiment-findings/workdir")
EXECUTE_MAGMA = True            # True if you want to run mamga fuzzing containers
SETUP_LIBRARY = True            # True if you want to move the harnesses to magma
RUNS = 2        # 2 = _0, _1 folders
CSV_DIR = Path("./csv")
CAPTAINRC = Path("./captainrc")        # path to source captainrc
HARNESS_YAML = Path("./harness.yaml")  # path to harness manifest
MAGMA_ROOT = Path("./modules/magma")           # path to magma checkout
SRC_EXTS = {".c", ".cc", ".cpp", ".h"}       # harness file extensions to collect

PLACEHOLDER = re.compile(r"^aflplusplus_TARGETS=.*$", re.MULTILINE)

# --- Helpers for step 1 ---
def patch_captainrc(captainrc: Path, libraries: list[str]) -> str:
    text = captainrc.read_text()
    replacement = f"aflplusplus_TARGETS=({" ".join(libraries)})"
    if PLACEHOLDER.search(text):
        text = PLACEHOLDER.sub(replacement, text)
    else:
        text = replacement + "\n" + text
    return text
# --- END of Helpers for step 1 ---


# --- Helpers for step 3 ---
def move_harnesses(yaml_path: Path, library: str, target_dir: Path) -> list[str]:
    data = yaml.safe_load(yaml_path.read_text())
    dest = target_dir / "custom"
    if dest.exists():
       shutil.rmtree(dest)
    dest.mkdir(parents=True, exist_ok=True)
    names = []
    for source_tool, dirs in data.get(library, {}).items():
        for dir_index in range(len(dirs)):
            d = dirs[dir_index]
            src_dir = Path("harnesses") / Path(d)
            if not src_dir.is_dir():
                continue
            for f in src_dir.iterdir():
                if f.is_file() and f.suffix in SRC_EXTS:
                    new_name = f"{source_tool}_{dir_index+1}_{f.name}"
                    shutil.copy(str(f), dest / new_name)
                    names.append(Path(new_name).stem)
    return names

def write_configrc(target_dir: Path, harness_names: list[str]) -> None:
    lines = [f'PROGRAMS=({" ".join(harness_names)})']
    (target_dir / "configrc").write_text("\n".join(lines) + "\n")


def setup_target(library: str) -> None:
    target_dir = MAGMA_ROOT / "targets" / library

    # A) move the harnesses to magma
    harness_names = move_harnesses(HARNESS_YAML, library, target_dir)

    # B) generate configrc specifying names of harness files
    write_configrc(target_dir, harness_names)

    print(f"Target set up at {target_dir}")
    print(f"Harness count: {len(harness_names)}")
# --- END of Helpers for step 3 ---

# --- Helpers for step 5 ---
def get_total_edges(library, output_dir):
    instrumented_file = next((output_dir / f"ar/aflplusplus/{library}").rglob("*instrumented*"))
    return int(instrumented_file.read_text().strip())
# --- END of Helpers for step 5 ---

# --- Helpers for step 6 ---
def generate_union_edges_file(library, tool, output_dir, run_number):
    tool_dir = output_dir / "ar/aflplusplus" / library / f"{tool}_{run_number}"
    tool_dir.mkdir(parents=True, exist_ok=True)

    lines = set()
    for map_file in glob.glob(str(output_dir / f"ar/aflplusplus/{library}/{tool}*/{run_number}/coverage/map.sorted")):
        lines.update(Path(map_file).read_text().splitlines())

    (tool_dir / "union.txt").write_text("\n".join(sorted(lines)))
# --- END of Helpers for step 6 ---

# --- Helpers for step 7 ---
import os
import sys
import csv
import itertools


def generate_table_csv(library, total_edges, csv_dir, run_number, output_dir, tools):
    csv_dir.mkdir(parents=True, exist_ok=True)

    def load_edges(directory):
        path = os.path.join(directory, "union.txt")
        if not os.path.isfile(path):
            sys.exit(f"Error: {path} not found")
        with open(path) as f:
            return {line.strip() for line in f if line.strip()}

    headers = [
        "library","tools", "n_tools",
        "union", "intersect", "exclusive"
    ]

    csv_path = csv_dir / f"{library}_{run_number}.csv"
    with open(csv_path, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        # row 0: the library's total instrumented edge count
        writer.writerow([library, "library", 0, total_edges, None, None])

        # load each tool's raw edge set for this library/run once
        tool_edges = {}
        for tool_name in tools:
            tool_dir = output_dir / f"ar/aflplusplus/{library}/{tool_name}_{run_number}/"
            tool_edges[tool_name] = load_edges(tool_dir)

        # every non-empty combination of tools -> union/intersect/exclusive
        for k in range(1, len(tools) + 1):
            for combo in itertools.combinations(tools, k):
                sets = [tool_edges[t] for t in combo]
                union = set.union(*sets)
                intersection = set.intersection(*sets)

                # exclusive region: in all of `combo`, in none of the rest
                others = set(tools) - set(combo)
                exclusive = set(intersection)
                for o in others:
                    exclusive -= tool_edges[o]

                writer.writerow([
                    library,
                    "+".join(combo),
                    k,
                    len(union),
                    len(intersection),
                    len(exclusive),
                ])
    return csv_path

def print_table_from_csv(csv_path):
    def get_formatted_str(value, total_edges):
        return f"{value} ({int(value)*100/int(total_edges):.2f}%)"

    with open(csv_path, newline="") as f:
        reader = list(csv.reader(f))

    headers, total_edge_line, *rows = reader
    total_edges = total_edge_line[1]

    formatted_rows = [
        [r[0]] + [get_formatted_str(v, total_edges) for v in r[1:]]
        for r in rows
    ]

    col_widths = [max(len(h), 10) for h in headers]
    for r in formatted_rows:
        for i, v in enumerate(r):
            col_widths[i] = max(col_widths[i], len(str(v)))

    def print_row(vals):
        print(" | ".join(str(v).ljust(w) for v, w in zip(vals, col_widths)))

    print(f"Total Edges: {total_edges}")
    print_row(headers)
    print_row(["-" * w for w in col_widths])
    for r in formatted_rows:
        print_row(r)
# --- END of Helpers for step 7 ---

def get_tools(library):
    data = yaml.safe_load(HARNESS_YAML.read_text())
    names = []
    for tool, _ in data.get(library, {}).items():
        names.append(tool)
    return names

def combine_results_from_same_tool(tools, library: str, output_dir: Path, run_number: int):
    for tool in tools:
        generate_union_edges_file(library, tool, output_dir, run_number)

def main():
    # 1) modify captainrc to specify the library being fuzzed
    patched = patch_captainrc(CAPTAINRC, LIBRARIES)

    # 2) move captainrc to the correct location
    (MAGMA_ROOT / "tools/captain/captainrc").write_text(patched)

    # 3) for each library, move harnesses and setup configrc
    for library in LIBRARIES:
        if SETUP_LIBRARY:
            setup_target(library)
        pass

    # 4) run magma
    env = os.environ
    if DEV:
        env |= {"BUILD_BASE": "1"}
    if EXECUTE_MAGMA:
        subprocess.run(["./run.sh"], cwd=(MAGMA_ROOT / "tools/captain"), env=env, check=True)

    try:
        output_dir = RAW_RESULTS_DIR
    except:
        output_dir = (MAGMA_ROOT / "tools/captain/workdir")

    total_edges=0
    for library in LIBRARIES:
        tools = get_tools(library)

        # 5) get total number of edges
        total_edges = get_total_edges(library,output_dir)

        for run_number in range(RUNS):

            # 6) build union.txt per tool
            combine_results_from_same_tool(tools, library, output_dir, run_number)

            # 7) print the table with coverage numbers
            csv_path = generate_table_csv(library, total_edges, CSV_DIR, run_number, output_dir, tools)

if __name__ == "__main__":
    main()
