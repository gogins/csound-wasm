#!/usr/bin/env bash
set -euo pipefail

echo "Updating Emscripten toolchain..."
repo_root="$(pwd)"
cd "${EMSDK:-$HOME/emsdk}"
git pull
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd "${repo_root}"
echo "Updated Emscripten toolchain."
