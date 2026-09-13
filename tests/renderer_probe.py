#!/usr/bin/env python3
from pathlib import Path
import os
import subprocess

root = Path(__file__).resolve().parents[1]
subprocess.run(['make', '-f', 'MakefileLin', '-j4'], cwd=root, check=True, stdout=subprocess.DEVNULL)
objects = [root / 'build/src/EngineMain.o', root / 'build/src/Demo/DemoScene.o']
objects += sorted((root / 'build/textures').glob('*.o')) + [root / 'build/libglvm.a']
libraries = ['X11', 'asound', 'vulkan', 'dl', 'pthread', 'Xxf86vm', 'Xrandr', 'Xi', 'xcb', 'xcb-keysyms', 'xcb-xfixes', 'xcb-cursor', 'X11-xcb', 'wayland-client']
subprocess.run(['clang++', '-std=c++20', '-g', '-fsanitize=address,undefined', '-Iinclude',
                'tests/renderer_probe.cpp', *(str(p) for p in objects), *('-l' + lib for lib in libraries),
                '-Wl,--wrap=_ZN4GLVM4core15CVulkanRenderer4drawEv', '-o', 'build/renderer_probe'], cwd=root, check=True)
env = os.environ.copy()
env.update(GLVM_MAX_FRAMES='18', VK_LAYER_VALIDATE_SYNC='1', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
with (root / 'build/renderer-probe.log').open('w') as log:
    result = subprocess.run(['./renderer_probe'], cwd=root / 'build', env=env, stdout=log, stderr=subprocess.STDOUT, timeout=120)
text = (root / 'build/renderer-probe.log').read_text()
assert result.returncode == 0, text[-6000:]
assert 'Rendered 18 frames' in text and text.count('PASS resize') == 2, text[-6000:]
assert text.count('PASS inventory event') == 2, text[-6000:]
assert not any(problem in text for problem in ['hazard detected', 'The Vulkan spec states', 'runtime error', 'ERROR:']), text[-6000:]
print('PASS resize twice, inventory, collision overlay and shutdown')
