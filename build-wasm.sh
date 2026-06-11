#!/usr/bin/env bash
clear
set -euo pipefail

echo "Building csound-wasm for WebAssembly..."

export EMSCRIPTEN_ALLOW_NEWER_PYTHON=1
source "${EMSDK:-$HOME/emsdk}/emsdk_env.sh"

case "${OSTYPE}" in
    linux*)
        eigen_root_default="/usr"
        boost_root_default="/usr"
        ;;
    darwin*)
        eigen_root_default="/opt/homebrew/opt/eigen"
        boost_root_default="/opt/homebrew/opt/boost"
        ;;
    *)
        echo "Unsupported platform: ${OSTYPE}" >&2
        exit 1
        ;;
esac
bash update-dependency-submodules.sh
cmake_config_args=(
    -S .
    -B build-wasm
    -G "Unix Makefiles"
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    -DEIGEN_ROOT="${EIGEN_ROOT:-${eigen_root_default}}"
    -DBOOST_ROOT="${BOOST_ROOT:-${boost_root_default}}"
)

# Always do a clean build.
rm -rf build-wasm

# CMake generates version.h from version.h.in in the build tree. Remove stale copies
# (old workaround copied 6.18 from csound-ac; csound-ac/CsoundAC is on -I and
# would shadow the generated header during libcsound compile).
rm -f dependencies/csound/include/version.h
rm -f dependencies/csound-ac/CsoundAC/version.h

emcmake cmake "${cmake_config_args[@]}"
cmake --build build-wasm --parallel "${BUILD_JOBS:-6}"

ls -ll dist
ls -ll web

echo "Built artifacts are in dist/ and have been copied to web/."
