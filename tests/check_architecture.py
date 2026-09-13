#!/usr/bin/env python3
"""Guard the CPU/public API boundary against accidental Vulkan dependencies."""
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
headers = ['Engine.hpp', 'Assets/AssetLibrary.hpp', 'Rendering/RenderFrame.hpp',
           'Rendering/ScenePresenter.hpp', 'Rendering/CameraOrbit.hpp',
           'Runtime/AudioService.hpp', 'SystemManager.hpp']
for header in headers:
    result = subprocess.run(['clang++', '-std=c++20', '-Iinclude', '-MM', '-x', 'c++', '-'],
                            input=f'#include "{header}"\n', text=True, cwd=root,
                            check=True, capture_output=True)
    assert 'GraphicAPI/' not in result.stdout and 'VkStructs.hpp' not in result.stdout, result.stdout
    subprocess.run(['clang++', '-std=c++20', '-Iinclude', '-fsyntax-only', '-x', 'c++', '-'],
                   input=f'#include "{header}"\n#ifdef VULKAN_CORE_H_\n#error CPU header imports Vulkan\n#endif\n',
                   text=True, cwd=root, check=True)
print('PASS public API and CPU modules compile without Vulkan headers')
for path in [root / 'src/Engine.cpp', *(root / 'src/Rendering').glob('*.cpp'),
             root / 'src/ProceduralLevelGeneratingSystem.cpp']:
    assert 'Engine::GetInstance' not in path.read_text(), path
print('PASS no Engine singleton dependency in runtime, scene and generation')
