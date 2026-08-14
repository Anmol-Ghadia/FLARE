1) clone:
```
git clone --recurse-submodules https://github.com/Anmol-Ghadia/FLARE.git
```

2) modify `captainrc` to configure magma parameters
3) modify `run.py` to specify the libraries beign fuzzed
4) execute:
```
python3 run.py && cd modules/magma/tools/captain/ && ./run.sh
```
