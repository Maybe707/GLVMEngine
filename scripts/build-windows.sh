#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
for tool in x86_64-w64-mingw32-g++ x86_64-w64-mingw32-dlltool gendef glslc; do
    command -v "$tool" >/dev/null || { echo "Missing build tool: $tool" >&2; exit 1; }
done
sdk=build-win/sdk
mkdir -p "$sdk/include"
cp -ru /usr/include/vulkan "$sdk/include/"
cp -ru /usr/include/vk_video "$sdk/include/"
loader=/mnt/c/Windows/System32/vulkan-1.dll
if [[ ! -f "$sdk/libvulkan-1.a" || "$loader" -nt "$sdk/libvulkan-1.a" ]]; then
    [[ -f "$loader" ]] || { echo 'Install the Windows GPU Vulkan driver first.' >&2; exit 1; }
    (cd "$sdk" && gendef "$loader" && x86_64-w64-mingw32-dlltool \
        -d vulkan-1.def -D vulkan-1.dll -l libvulkan-1.a)
fi
make -f MakefileWindows -j"${GLVM_BUILD_JOBS:-4}" "$@"
