#!/usr/bin/env bash
set -euo pipefail

version="${CSOUND_WEB_ASSEMBLY_VERSION:-4.0.0-beta}"
archive="csound-wasm-${version}.zip"

cmake --build build-wasm --target stage_dist
cmake --build build-wasm --target wasm_zip

if [[ -f "csound-wasm-4.0.0-beta.zip" && "${archive}" != "csound-wasm-4.0.0-beta.zip" ]]; then
    mv "csound-wasm-4.0.0-beta.zip" "${archive}"
fi

unzip -v "${archive}"
