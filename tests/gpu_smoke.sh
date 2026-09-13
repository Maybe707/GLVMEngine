#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build
make -f MakefileLin -j"${GLVM_BUILD_JOBS:-4}" > build/build.log 2>&1
export GLVM_MAX_FRAMES=${GLVM_SMOKE_FRAMES:-30}
export VK_LAYER_VALIDATE_SYNC=1
export UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1
if command -v llvm-symbolizer-21 >/dev/null; then
    export ASAN_SYMBOLIZER_PATH=$(command -v llvm-symbolizer-21)
fi
cd build
timeout "${GLVM_SMOKE_TIMEOUT:-180}s" ./linGame > gpu-smoke.log 2>&1
grep -q "Rendered $GLVM_MAX_FRAMES frames" gpu-smoke.log
if grep -E 'runtime error|ERROR:|hazard detected|The Vulkan spec states' gpu-smoke.log; then
    exit 1
fi
echo "PASS $GLVM_MAX_FRAMES Vulkan frames and shutdown"
