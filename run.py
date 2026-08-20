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
DEV=True
# -------------------------------------------------------

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
def generate_union_edges_file(library, tool, output_dir):
    tool_dir = output_dir / "ar/aflplusplus" / library / tool
    tool_dir.mkdir(parents=True, exist_ok=True)

    lines = set()
    for map_file in glob.glob(str(output_dir / f"ar/aflplusplus/{library}/{tool}*/0/coverage/map.sorted")):
        lines.update(Path(map_file).read_text().splitlines())

    (tool_dir / "union.txt").write_text("\n".join(sorted(lines)))
# --- END of Helpers for step 6 ---

# --- Helpers for step 7 ---
def generate_table_csv(library, total_edges, csv_dir):
    csv_dir.mkdir(parents=True, exist_ok=True)
    def load_edges(directory):
        path = os.path.join(directory, "union.txt")
        if not os.path.isfile(path):
            sys.exit(f"Error: {path} not found")
        with open(path) as f:
            return {line.strip() for line in f if line.strip()}

    def get_formatted_str(value, max):
        return f"{value} ({value*100/max:.2f}%)"

    evals = ["promefuzz", "opencode"]

    base_name = "promefuzz"
    base_dir = Path(f"modules/magma/tools/captain/workdir/ar/aflplusplus/{library}/{base_name}/")
    base_edges = load_edges(base_dir)

    headers = [
        "Evaluation", "Count", f"Union w/ {base_name}", f"Intersect w/ {base_name}",
        f"Eval - {base_name}", f"{base_name} - Eval",
    ]

    csv_path = csv_dir / f"{library}.csv"
    with open(csv_path, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["Total Edge Count", total_edges])
        writer.writerow(headers)

        for eval_name in evals:
            eval_dir = Path(f"modules/magma/tools/captain/workdir/ar/aflplusplus/{library}/{eval_name}/")
            edges = load_edges(eval_dir)

            union = edges | base_edges
            intersection = edges & base_edges
            only_eval = edges - base_edges
            only_base = base_edges - edges

            writer.writerow([
                eval_name,
                get_formatted_str(len(edges), total_edges),
                get_formatted_str(len(union), total_edges),
                get_formatted_str(len(intersection), total_edges),
                get_formatted_str(len(only_eval), total_edges),
                get_formatted_str(len(only_base), total_edges),
            ])
    return csv_path

def print_table_from_csv(csv_path):
    with open(csv_path, newline="") as f:
        reader = list(csv.reader(f))

    total_edge_line, headers, *rows = reader

    col_widths = [max(len(h), 10) for h in headers]
    for r in rows:
        for i, v in enumerate(r):
            col_widths[i] = max(col_widths[i], len(str(v)))

    def print_row(vals):
        print(" | ".join(str(v).ljust(w) for v, w in zip(vals, col_widths)))

    print(f"{total_edge_line[0]}: {total_edge_line[1]}")
    print_row(headers)
    print_row(["-" * w for w in col_widths])
    for r in rows:
        print_row(r)
# --- END of Helpers for step 7 ---

def main():
    # 1) modify captainrc to specify the library being fuzzed
    patched = patch_captainrc(CAPTAINRC, LIBRARIES)

    # 2) move captainrc to the correct location
    (MAGMA_ROOT / "tools/captain/captainrc").write_text(patched)

    # 3) for each library, move harnesses and setup configrc
    for library in LIBRARIES:
        setup_target(library)

    # 4) run magma
    if DEV:
        env = os.environ | {"BUILD_BASE": "1"}
    #subprocess.run(["./run.sh"], cwd=(MAGMA_ROOT / "tools/captain/captainrc"), env=env, check=True)

    output_dir = (MAGMA_ROOT / "tools/captain/workdir")

    for library in LIBRARIES:
        # 5) get total number of edges
        total_edges = get_total_edges(library,output_dir)

        # 6) build union.txt per tool
        for tool in ["promefuzz", "opencode"]: # TODO: instead read this from YAML
            generate_union_edges_file(library, tool, output_dir)

        # 7) print the table with coverage numbers
        csv_path = generate_table_csv(library, total_edges, CSV_DIR)
        print_table_from_csv(csv_path)

if __name__ == "__main__":
    main()
