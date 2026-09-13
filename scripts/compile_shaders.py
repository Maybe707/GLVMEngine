#!/usr/bin/env python3
"""Rebuild stale SPIR-V assets using the checked-in source/output mappings."""
from pathlib import Path
import shlex
import subprocess

root = Path(__file__).resolve().parents[1]
for script in sorted((root / 'VKshaders').rglob('compile.sh')):
    for line in script.read_text().splitlines():
        args = shlex.split(line)
        if not args or args[0] != 'glslangValidator':
            continue
        source = script.parent / next(a for a in args if a.endswith(('.vert', '.frag', '.geom')))
        output = script.parent / args[args.index('-o') + 1]
        if not output.exists() or source.stat().st_mtime_ns > output.stat().st_mtime_ns:
            subprocess.run(['glslc', '--target-env=vulkan1.0', str(source), '-o', str(output)], check=True)
