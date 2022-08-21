 #!/bin/bash
echo "Building csound-extended for WebAssembly..."

export EMSCRIPTEN_ALLOW_NEWER_PYTHON=1

source ~/emsdk/emsdk_env.sh

echo "Using EMSCRIPTEN_ROOT: $EMSCRIPTEN_ROOT."
echo "Python version (must be 3 or higher):"
python3 --version

# Total memory for a WebAssembly module must be a multiple of 64 KB so...
# 1024 * 64 = 65536 is 64 KB
# 65536 * 1024 * 4 is 268435456

CXX_FLAGS="-v -std=c++17 -O2 -Wno-implicit-int-float-conversion -DINIT_STATIC_MODULES=1"
export CXX_FLAGS

# Most emcc flags should be the same for both the 'compile' and the 'compile and link' passes.

EMCC_FLAGS='-s ENVIRONMENT="web,webview,worker,node,shell" -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -s DEMANGLE_SUPPORT=1 -s FORCE_FILESYSTEM=1 -s INITIAL_MEMORY=268435456 -s LINKABLE=1 -s NO_EXIT_RUNTIME=0 -s SAFE_HEAP=0 -s WASM=1'
export EMCC_FLAGS

rm -rf build-wasm
mkdir -p build-wasm
cp -f dependencies/csound-ac/CsoundAC/version.h dependencies/csound/include/version.h

cd build-wasm
rm -f CMakeCache.txt

# Haven't figured out how to get this work yet, may never.
# echo "Packaging resources..." 
# 
# cp -r ../csound-assets/ .
# python $EMSCRIPTEN_ROOT/tools/file_packager.py csound_assets.data --preload csound-assets --js-output=csound_assets.js

echo "Configuring to build static libraries..."

emcmake cmake -DBIG_ENDIAN=0 -DISBIGENDIAN=0 -DIS_BIG_ENDIAN=0 -G "Unix Makefiles" -DEIGEN_ROOT="/opt/homebrew/opt/eigen" -DBOOST_ROOT="/opt/homebrew/opt/boost" -DCSOUND_INCLUDE_DIR="../dependencies/csound" -DSNDFILE_H_PATH="../deps/include" -DSNDFILE_LIBRARY="../deps/lib/libsndfile.a" -Wno-dev ..

echo "Building Csound static library..."
emmake make csound-static -j6 VERBOSE=1

echo "Building CsoundAC static library..."
emmake make csoundac-static -j6 VERBOSE=1 

echo "Compiling csound_embind..."

em++ ${CXX_FLAGS} ${EMCC_FLAGS} -iquote ../src -I../dependencies -I../dependencies/csound/include -I../dependencies/csound/H -I../dependencies/csound/interfaces -I../deps/libsndfile-1.0.25/src -Iinclude -I/opt/homebrew/opt/eigen/include/eigen3 -c ../src/csound_embind.cpp --bind 

echo "Compiling CsoundAudioProcessor..."

em++ ${CXX_FLAGS} -O1 ${EMCC_FLAGS} --bind -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "FS"]' -s RESERVED_FUNCTION_POINTERS=2 -s SINGLE_FILE=1 -s WASM_ASYNC_COMPILATION=0 --source-map-base . --pre-js ../src/CsoundAudioProcessor_prejs.js --post-js ../src/CsoundAudioProcessor_postjs.js csound_embind.o dependencies/csound/libcsound.a ../deps/lib/libsndfile.a ../deps/lib/libogg.a ../deps/lib/libvorbis.a ../deps/lib/libvorbisenc.a ../deps/lib/libvorbisfile.a ../deps/lib/libFLAC.a -o CsoundAudioProcessor.js

echo "Compiling CsoundAC..."

em++ ${CXX_FLAGS} ${EMCC_FLAGS} -I../dependencies -I../dependencies/csound-extended --bind -s EXPORT_ES6=0 -s EXPORT_NAME="createCsoundAC" -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "FS"]' -s MODULARIZE=1 -s RESERVED_FUNCTION_POINTERS=1 -s SINGLE_FILE=1 -s USE_ES6_IMPORT_META=0 -s WASM_ASYNC_COMPILATION=1 --source-map-base . ../CsoundAC/csoundac_embind.cpp -I../dependencies/eigen -I/opt/homebrew/opt/boost/include -I../deps/libsndfile-1.0.25/src -I.. CsoundAC/libcsoundac-static.a dependencies/csound/libcsound.a ../deps/lib/libsndfile.a ../deps/lib/libogg.a ../deps/lib/libvorbis.a ../deps/lib/libvorbisenc.a ../deps/lib/libvorbisfile.a ../deps/lib/libFLAC.a -o CsoundAC.js

cd ..

find deps -name "*.a" -ls
find build-wasm -name "*.a" -ls

bash release-wasm.sh

echo "Finished building csound-extended for WebAssembly."
