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
from pathlib import Path

# ---------------- CONFIG (edit this) -------------------
# libraries being fuzzed as a python list of strings
# note the capitalization
LIBRARIES = ["libtiff"]

# running in development environment?
# set False if unsure
DEV=True
# -------------------------------------------------------

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
            print(d)
            src_dir = Path("harnesses") / Path(d)
            print(src_dir)
            if not src_dir.is_dir():
                continue
            for f in src_dir.iterdir():
                print(f)
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
    print(f"Harnesses: {harness_names}")
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
def print_table(library, total_edges):

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

    rows = []
    for eval_name in evals:
        eval_dir = Path(f"modules/magma/tools/captain/workdir/ar/aflplusplus/{library}/{eval_name}/")
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
    subprocess.run(["./run.sh"], cwd=(MAGMA_ROOT / "tools/captain/captainrc"), env=env, check=True)

    output_dir = (MAGMA_ROOT / "tools/captain/workdir")

    for library in LIBRARIES:
        # 5) get total number of edges
        total_edges = get_total_edges(library,output_dir)

        # 6) build union.txt per tool
        for tool in ["promefuzz", "opencode"]: # TODO: instead read this from YAML
            generate_union_edges_file(library, tool, output_dir)

        # 7) print the table with coverage numbers
        print_table(library, total_edges)

if __name__ == "__main__":
    main()
