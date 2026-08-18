#!/usr/bin/env python3
"""
Set up Magma fuzzing targets: patch captainrc, place it, collect harnesses
from a YAML manifest, and generate configrc.

Assumptions (adjust if your Magma layout differs):
- MAGMA_ROOT/targets/<library>/ is the target dir for captainrc & configrc.
- MAGMA_ROOT/targets/<library>/harnesses/ is where harness sources land.
- captainrc has a line "TARGET_LIBRARY=" to fill in (change PLACEHOLDER
  below to match your actual captainrc template).
- HARNESS_YAML maps library -> source_tool (opencode/promefuzz/...) ->
  list of directories; direct-child .c/.cc/.cpp files in each directory
  are renamed with a "<source_tool>_" prefix and moved into harnesses/.

Edit the CONFIG block below, then just run:
    python setup_magma_target.py
"""
import re
import shutil
import yaml
from pathlib import Path

# ---------------- CONFIG (edit this) -------------------
LIBRARIES = ["tinycbor"]               # libraries being fuzzed
# -------------------------------------------------------

CAPTAINRC = Path("./captainrc")        # path to source captainrc
HARNESS_YAML = Path("./harness.yaml")  # path to harness manifest
MAGMA_ROOT = Path("./modules/magma")           # path to magma checkout
SRC_EXTS = {".c", ".cc", ".cpp", ".h"}       # harness file extensions to collect

PLACEHOLDER = re.compile(r"^aflplusplus_TARGETS=.*$", re.MULTILINE)


def patch_captainrc(captainrc: Path, libraries: list[str]) -> str:
    text = captainrc.read_text()
    replacement = f"aflplusplus_TARGETS=({" ".join(libraries)})"
    if PLACEHOLDER.search(text):
        text = PLACEHOLDER.sub(replacement, text)
    else:
        text = replacement + "\n" + text
    return text


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


def main():
    # 1) modify captainrc to specify the library being fuzzed
    patched = patch_captainrc(CAPTAINRC, LIBRARIES)

    # 2) move captainrc to the correct location
    (MAGMA_ROOT / "tools/captain/captainrc").write_text(patched)

    # 3) for each library, move harnesses and setup configrc
    for library in LIBRARIES:
        setup_target(library)


if __name__ == "__main__":
    main()
