#!/bin/sh
echo "Creating a release for Csound for WebAssembly..."
export CSOUND_WEB_ASSEMBLY_VERSION=1.0.0-beta
export RELEASE_DIR=csound-wasm-${CSOUND_WEB_ASSEMBLY_VERSION}
rm -rf dist
mkdir dist
rm -rf $RELEASE_DIR
mkdir $RELEASE_DIR
#remove backup files ending with ~
find . -name "*~" -exec rm {} \;
cp README.md dist/
cp src/*.js dist/
cp src/*.html dist/
cp build-wasm/CsoundAudio*.* dist/
cp build-wasm/CsoundAC.js dist/
#cp build-wasm/csound_assets.* dist/
#cp build-wasm/csound_samples.js dist/
cp -rf dependencies/csound-ac/patches dist/
cp -rf dependencies/csound-ac/silencio dist/
pwd
ls -ll dist/
rm -f ${RELEASE_DIR}.zip
cd dist
zip -r ../${RELEASE_DIR}.zip . -x *_*.js
echo "Contents of release archive:"
cd ..
unzip -v ${RELEASE_DIR}.zip 