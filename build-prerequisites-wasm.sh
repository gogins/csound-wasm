#!/bin/bash
set -euo pipefail

echo "Clean build of csound-wasm prerequisites for WebAssembly..."
export WEBASSEMBLY_HOME="$(pwd)"
echo "WEBASSEMBLY_HOME: ${WEBASSEMBLY_HOME}"

~/emsdk/emsdk install latest
~/emsdk/emsdk activate latest
source ~/emsdk/emsdk_env.sh

echo "Using emcc: $(command -v emcc)"
echo "Deleting previous build..."
rm -rf build-wasm
emcc --clear-cache
bash download_libsndfile_wasm.sh
bash build_libsndfile_wasm.sh
echo "Completed clean build of csound-wasm prerequisites for WebAssembly."
echo