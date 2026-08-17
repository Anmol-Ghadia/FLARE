# FLARE — Fuzzing Library Agent Rating / Evaluation

## Status of libraries
|Library|Magma support?|OpenCode Harness?|Promefuzz Harness?|
|---|---|---|---|
|tinycbor|Yes|Yes|Yes|
|sqlite3|Yes|No|Yes|
|libtiff|Yes|No|Yes|
|lhasa|Na|No|No|
|libpng|Yes|Yes|Yes|
|libsndfile|Yes|No|No|
|libxml2|Yes|No|No|
|lua|Yes|No|No|
|poppler|Yes|No|No|
|openssl|Yes|No|No|
|php|Yes|No|No|
|cjson|Yes|Yes|Yes|
|rapidcsv|No|No|Yes|
|pmp-library|No|No|No|

## How to run the experiments

1) Dependencies
```
sudo apt install docker.io
sudo usermod -aG docker $USER
newgrp docker
```

1) clone:
```
git clone --recurse-submodules https://github.com/Anmol-Ghadia/FLARE.git
```

2) modify `captainrc` to configure magma parameters  
3) modify `run.py` to specify the libraries being fuzzed  
4) execute:
```
python3 run.py && cd modules/magma/tools/captain/ && ./run.sh
```

Notes:
- Edit the CONFIG block in `run.py` (LIBRARIES list) to choose which libraries from `harness.yaml` should be prepared for Magma.
- `captainrc` contains a placeholder line for `aflplusplus_TARGETS` which will be patched by the script; ensure other Magma settings (WORKDIR, FUZZERS, TIMEOUT, POLL, etc.) are correct for your environment.

### Pushing base images
1) login with PAT from docker hub
2) execute `./run.sh` of magma after configuration with `BUILD_BASE=1`
```
cd modules/magma/tools/captain/ && BASE_BUILD=1 ./run.sh
```
3) tag and push the image with appropriate library name and version
```
docker image tag flare-tinycbor anmolg26/flare-tinycbor:0.1
docker image tag flare-tinycbor anmolg26/flare-tinycbor:latest
docker image push anmolg26/flare-tinycbor:0.1
docker image push anmolg26/flare-tinycbor:latest
```

## Repository layout

```
.gitmodules          - git submodule configuration (modules/magma)
captainrc            - Magma campaign configuration template (patched by run.py)
harness.yaml         - mapping: library -> {source_tool -> list of harness directories}
harnesses/           - local harness sources (expected to contain per-library subdirs)
run.py               - Python helper that patches captainrc, copies harnesses into Magma, and writes configrc
modules/             - contains the magma submodule (Magma fuzzing framework)
  magma/             - Magma checkout (submodule)
    targets/         - Magma targets; run.py will place harnesses under targets/<library>/custom/
    fuzzers/         - fuzzers supported by Magma (used by captain)
    tools/captain/   - driver scripts for running campaigns (run.sh)
    docs/            - Magma documentation
    docker/          - Docker-related helpers
```

