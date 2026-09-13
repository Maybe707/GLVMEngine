#!/usr/bin/env python3
"""Compile every shader listed by the repository's shader build scripts."""
from pathlib import Path
import shlex
import subprocess

root = Path(__file__).resolve().parents[1]
output = root / "build" / "shader-check"
count = 0
for script in sorted((root / "VKshaders").rglob("compile.sh")):
    for line in script.read_text().splitlines():
        args = shlex.split(line)
        if not args or args[0] != "glslangValidator":
            continue
        source = next(arg for arg in args if arg.endswith((".vert", ".frag", ".geom")))
        destination = output / script.parent.relative_to(root / "VKshaders") / args[args.index("-o") + 1]
        destination.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(["glslc", "--target-env=vulkan1.0", source, "-o", str(destination)], cwd=script.parent, check=True)
        count += 1
print(f"PASS {count} shader sources")
