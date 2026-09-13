# Game Loop Versatile Modules (GLVM)

A small C++20 game engine with an archetype ECS, Vulkan renderer, basic AABB
physics, inventory, WAV audio and OBJ/glTF asset loading.

The maintained demo runs on **Windows x64 / Win32 + Vulkan** and
**Linux / Wayland + Vulkan** (including WSLg). OpenGL, X11/XCB and networking
sources are retained as legacy code; they are not supported demo configurations.
The glTF importer supports the bundled assets, not the complete glTF specification.

## Run on Windows

After building, double-click `Run-GLVM.cmd`. It launches
`build-win/GLVMEngine.exe` directly on the Windows GPU; WSL and a compiler are
not needed at runtime. Keep the executable inside the project so it can find
models, audio and shaders. Binaries are not checked into Git.

Controls: WASD movement, Space jump, mouse camera, I inventory,
O collision overlay, Esc exit.

[Windows setup and build instructions](docs/WINDOWS_RU.md)

## Build

GNU Make is the supported build system. All platforms use
`make_files/common_sources.mk`; old Makefiles forward to the root entry point.

```bash
# Ubuntu / WSL
bash scripts/install-ubuntu.sh
make -j4 CONFIG=Debug       # build/linGame; ASan/UBSan, Vulkan validation
make -j4 CONFIG=Release     # build-release/linGame
make test
bash scripts/run.sh        # builds Release, starts on Wayland/WSLg

# Cross-compile a native Windows executable in WSL
sudo apt-get install g++-mingw-w64-x86-64-posix mingw-w64-tools
bash scripts/build-windows.sh
```

Native MSYS2 UCRT64 also uses `make PLATFORM=windows CONFIG=Release`.
See the Windows instructions for packages. Debug and Release use separate output
directories. Header dependencies and changes to compiler/flags invalidate objects.

## Validation

`make test` covers containers, JSON, ECS chunks, ID reuse, spatial membership,
inventory, gameplay, camera math, audio concurrency and resource ownership without
opening a window or requiring a GPU. GitHub Actions builds Linux Debug/Release
and native Windows Release, runs CPU tests and compiles all shader sources.

Interactive GPU checks are separate:

```bash
python3 tests/check_shaders.py
bash tests/gpu_smoke.sh
python3 tests/renderer_probe.py
python3 tests/initialization_failure.py
```

Use `tests/windows_smoke.ps1` and the Windows test executables for native window,
resize, focus, camera and audio checks.

- [Installation and Linux launch](docs/INSTALL_RU.md)
- [Architecture and API migration](docs/ARCHITECTURE_RU.md)
- [Earlier fixes](docs/FIXES_RU.md)
- [Follow-up audit fixes and regression coverage](docs/RELIABILITY_RU.md)

Original engine by Maksim Manokhin (Yuriorkis_Scream), MIT license.
