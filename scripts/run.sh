#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
build=build-release
debug=0
while (($#)); do
    case "$1" in
        --debug) debug=1; build=build; shift ;;
        --frames)
            [[ ${2:-} =~ ^[1-9][0-9]*$ ]] || { echo '--frames requires a positive integer' >&2; exit 2; }
            export GLVM_MAX_FRAMES=$2; shift 2 ;;
        *) echo "Unknown argument: $1 (supported: --debug, --frames N)" >&2; exit 2 ;;
    esac
done
if ! command -v clang++ >/dev/null || ! command -v make >/dev/null; then
    echo 'Install dependencies first: bash scripts/install-ubuntu.sh' >&2
    exit 1
fi
if [[ -z ${WAYLAND_DISPLAY:-} ]]; then
    echo 'A Wayland desktop or WSLg is required to display the game window.' >&2
    exit 1
fi
mkdir -p "$build"
jobs=${GLVM_BUILD_JOBS:-4}
if ((debug)); then
    make -f MakefileLin -j"$jobs" > "$build/build.log" 2>&1 || { tail -60 "$build/build.log"; exit 1; }
    if command -v llvm-symbolizer-21 >/dev/null; then
        export ASAN_SYMBOLIZER_PATH=$(command -v llvm-symbolizer-21)
    fi
else
    make -f MakefileLin BUILD="$build" SANITIZE= OPT='-O2 -DGLVM_DISABLE_VALIDATION' -j"$jobs" \
        > "$build/build.log" 2>&1 || { tail -60 "$build/build.log"; exit 1; }
fi
cd "$build"
exec ./linGame
