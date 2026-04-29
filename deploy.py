#!/usr/bin/env python3
"""
Deploy files from csound-wasm/dist to dependent local projects.
"""

from pathlib import Path
import shutil

repo_root = Path(__file__).resolve().parent
source_directory = repo_root / "dist"

if not source_directory.is_dir():
    raise SystemExit(f"Missing {source_directory}. Build first with ./build-wasm.sh.")

targets = [
    Path.home() / "cloud-5",
    Path.home() / "csound-ac" / "silencio" / "js",
    Path.home() / "csound-examples" / "docs",
    Path.home() / "gogins.github.io",
]

names = [
    "CsoundAC.js",
    "CsoundAC.wasm",
    "CsoundAudioNode.js",
    "CsoundAudioProcessor.js",
    "csound_loader.js",
    "minimal.html",
    "trichord_space.html",
    "patches",
    "silencio",
]

for target in targets:
    target.mkdir(parents=True, exist_ok=True)
    for name in names:
        source = source_directory / name
        destination = target / name
        if source.is_file():
            print(f"copying {source} to {destination}")
            shutil.copy2(source, destination)
        elif source.is_dir():
            print(f"copying {source} to {destination}")
            shutil.copytree(source, destination, dirs_exist_ok=True)
