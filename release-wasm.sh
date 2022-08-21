#!/bin/sh
echo "Creating a release for Csound for WebAssembly..."
export CSOUND_WEB_ASSEMBLY_VERSION=3.0.0
export RELEASE_DIR=csound-wasm-${CSOUND_WEB_ASSEMBLY_VERSION}
rm -rf dist-wasm
mkdir dist-wasm
rm -rf $RELEASE_DIR
mkdir $RELEASE_DIR
#remove backup files ending with ~
find . -name "*~" -exec rm {} \;
cp README.md dist-wasm/
cp src/*.js dist-wasm/
cp src/httpd.py dist-wasm/
cp src/minimal.html dist-wasm/
cp build-wasm/CsoundAudio*.* dist-wasm/
cp build-wasm/CsoundAC.js dist-wasm/
#cp build-wasm/csound_assets.* dist-wasm/
#cp build-wasm/csound_samples.js dist-wasm/
cp -f CsoundAC/piano-roll.js dist-wasm/
cp -f src/csound_loader.js dist-wasm/
cp -f src/csound_loader.js ../silencio/js/
cp -f CsoundAC/piano-roll.js ../silencio/js/
pwd
ls -ll dist-wasm/
rm -f ${RELEASE_DIR}.zip
zip -rj ${RELEASE_DIR}.zip dist-wasm/ -x dist-wasm/*_*.js
unzip -v ${RELEASE_DIR}.zip 