#!/usr/bin/env python3
from pathlib import Path
import os
import subprocess

root = Path(__file__).resolve().parents[1]
subprocess.run(['make', '-f', 'MakefileLin', '-j4'], cwd=root, check=True, stdout=subprocess.DEVNULL)
objects = [root / 'build/src/EngineMain.o', root / 'build/src/Demo/DemoScene.o']
objects += sorted((root / 'build/textures').glob('*.o')) + [root / 'build/libglvm.a']
libraries = ['X11', 'asound', 'vulkan', 'dl', 'pthread', 'Xxf86vm', 'Xrandr', 'Xi',
             'xcb', 'xcb-keysyms', 'xcb-xfixes', 'xcb-cursor', 'X11-xcb', 'wayland-client']
subprocess.run(['clang++', '-std=c++20', '-g', '-fsanitize=address,undefined',
                'tests/initialization_failure.cpp', *(str(p) for p in objects),
                *('-l' + lib for lib in libraries), '-Wl,--wrap=vkCreateGraphicsPipelines',
                '-o', 'build/initialization_failure'], cwd=root, check=True)
env = os.environ.copy()
env.update(GLVM_MAX_FRAMES='1', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
cases = [('./linGame', {'GLVM_MAX_FRAMES': '-1'}, 'GLVM_MAX_FRAMES'),
         ('./linGame', {'VK_DRIVER_FILES': '/nonexistent-glvm-driver.json'}, 'failed to create instance'),
         ('./initialization_failure', {}, 'failed to create graphics pipeline')]
for index, (binary, extra, expected) in enumerate(cases):
    with (root / f'build/init-failure-{index}.log').open('w') as log:
        result = subprocess.run([binary], cwd=root / 'build', env=env | extra,
                                stdout=log, stderr=subprocess.STDOUT, timeout=90)
    text = (root / f'build/init-failure-{index}.log').read_text()
    assert result.returncode == 1 and expected in text, text[-6000:]
    assert not any(error in text for error in ['ERROR:', 'runtime error:', 'The Vulkan spec states', 'LeakSanitizer']), text[-6000:]
    print(f'PASS clean shutdown after {expected}')
