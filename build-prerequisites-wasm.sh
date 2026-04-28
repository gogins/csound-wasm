#!/usr/bin/env bash
set -euo pipefail

echo "Obtaining csound-wasm prerequisites for WebAssembly..."
export WEBASSEMBLY_HOME="$(pwd)"
echo "WEBASSEMBLY_HOME: ${WEBASSEMBLY_HOME}"

"${EMSDK:-$HOME/emsdk}/emsdk" install latest
"${EMSDK:-$HOME/emsdk}/emsdk" activate latest
source "${EMSDK:-$HOME/emsdk}/emsdk_env.sh"

echo "Using emcc: $(command -v emcc)"
rm -rf deps
emcc --clear-cache

echo "Completed prerequisites."
