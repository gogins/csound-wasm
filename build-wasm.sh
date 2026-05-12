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

cmake_config_args=(
    -S .
    -B build-wasm
    -G "Unix Makefiles"
    -DCMAKE_BUILD_TYPE=Release
    -DEIGEN_ROOT="${EIGEN_ROOT:-${eigen_root_default}}"
    -DBOOST_ROOT="${BOOST_ROOT:-${boost_root_default}}"
)

# Csound expects this generated/version header in the include tree for this build.
cp -f dependencies/csound-ac/CsoundAC/version.h dependencies/csound/include/version.h

emcmake cmake "${cmake_config_args[@]}"
cmake --build build-wasm --parallel "${BUILD_JOBS:-6}"

ls -ll dist
ls -ll web

echo "Built artifacts are in dist/ and have been copied to web/."
