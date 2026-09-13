#!/usr/bin/env bash
set -euo pipefail
if ((EUID != 0)); then
    exec sudo bash "$0" "$@"
fi
apt-get update
apt-get install -y build-essential clang llvm-21 libx11-dev libxi-dev libxrandr-dev \
    libxxf86vm-dev libxcb-keysyms1-dev libxcb-xfixes0-dev libxcb-cursor-dev \
    libx11-xcb-dev libwayland-dev libasound2-dev libasound2-plugins libpulse-dev \
    libvulkan-dev mesa-vulkan-drivers vulkan-tools vulkan-validationlayers glslc
