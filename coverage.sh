python3 run.py
pushd modules/magma/tools/captain
BUILD_BASE=1 ./run.sh
popd

WORKDIR="modules/magma/tools/captain/workdir"

TOOLS=("promefuzz" "opencode")

# IMPORTANT: This script ignores trials 1,2,3....
TOTAL_EDGES="$(find "$PWD/$WORKDIR/ar/aflplusplus" | \
	grep "instrumented" | \
	head -1 | \
	xargs cat | \
	grep "[[:digit:]]")"
TABLE_GENERATION_ARGS="--total-edges $TOTAL_EDGES"
TABLE_GENERATION_ARGS="$TABLE_GENERATION_ARGS --baseline $PWD/$WORKDIR/ar/aflplusplus/libtiff/promefuzz:promefuzz"
for TOOL in "${TOOLS[@]}"; do
	echo "Current tool: $TOOL"

	rm -rf "$PWD/$WORKDIR/ar/aflplusplus/libtiff/$TOOL"
	mkdir -p "$PWD/$WORKDIR/ar/aflplusplus/libtiff/$TOOL"

	ls "$PWD/$WORKDIR/ar/aflplusplus/libtiff/$TOOL"*/0/coverage/map.sorted | \
		xargs cat | \
		sort -u > \
		"$PWD/$WORKDIR/ar/aflplusplus/libtiff/$TOOL/union.txt"
	TABLE_GENERATION_ARGS="$TABLE_GENERATION_ARGS $PWD/$WORKDIR/ar/aflplusplus/libtiff/$TOOL:$TOOL"
done

python3 2_table.py $TABLE_GENERATION_ARGS

