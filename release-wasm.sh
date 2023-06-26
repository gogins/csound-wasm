#!/bin/sh
echo "Creating a release for Csound for WebAssembly..."
export CSOUND_WEB_ASSEMBLY_VERSION=3.1.0
export RELEASE_DIR=csound-wasm-${CSOUND_WEB_ASSEMBLY_VERSION}
rm -rf dist-wasm
mkdir dist-wasm
rm -rf $RELEASE_DIR
mkdir $RELEASE_DIR
#remove backup files ending with ~
find . -name "*~" -exec rm {} \;
cp README.md dist-wasm/
cp src/*.js dist-wasm/
cp src/*.html dist-wasm/
cp build-wasm/CsoundAudio*.* dist-wasm/
cp build-wasm/CsoundAC.js dist-wasm/
#cp build-wasm/csound_assets.* dist-wasm/
#cp build-wasm/csound_samples.js dist-wasm/
cp -rf dependencies/csound-ac/patches dist-wasm/
cp -rf dependencies/csound-ac/silencio dist-wasm/
pwd
ls -ll dist-wasm/
rm -f ${RELEASE_DIR}.zip
cd dist-wasm
zip -r ../${RELEASE_DIR}.zip . -x *_*.js
echo "Contents of release archive:"
cd ..
unzip -v ${RELEASE_DIR}.zip 