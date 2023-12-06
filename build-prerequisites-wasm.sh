#!/bin/bash
clear
echo "Clean build of csound-wasm prerequisites for WebAssembly..."
export WEBASSEMBLY_HOME=`pwd`}
echo "WEBASSEMBLY_HOME: ${WEBASSEMBLY_HOME}"
### echo "Completely re-installing total EMSDK..."
~/emsdk/emsdk install tot
source ~/emsdk/emsdk_env.sh
echo "Using EMSCRIPTEN_ROOT: ${EMSCRIPTEN_ROOT}."
echo "Deleting previous build..."
rm -rf build-wasm
emcc --clear-cache
bash download_libsndfile_wasm.sh
bash build_libsndfile_wasm.sh
echo "Completed clean build of csound-wasm prerequisites for WebAssembly."
echo
