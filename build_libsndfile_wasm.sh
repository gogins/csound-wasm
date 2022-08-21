source ~/emsdk/emsdk_env.sh

export CFLAGS="-O3 -flto"
export CXXFLAGS="-O3 -flto"

mkdir -p deps
cd deps

DEPS_DIR=$PWD

echo "DEPS_DIR: ${DEPS_DIR}"

echo "Building libogg..."
cd libogg-1.3.3
emconfigure ./configure --enable-static --disable-shared --prefix=$DEPS_DIR --libdir=$DEPS_DIR/lib --includedir=$DEPS_DIR/include 
#emconfigure ./configure --enable-static --disable-shared
emmake make
emmake make install
cd ..

echo "Building libvorbis..."
cd libvorbis-1.3.6
emconfigure ./configure --enable-static --disable-shared --prefix=$DEPS_DIR --libdir=$DEPS_DIR/lib --includedir=$DEPS_DIR/include 
#emconfigure ./configure --enable-static --disable-shared 
emmake make CPPFLAGS=$DEPS_DIR/libogg-1.3.3/include
emmake make install
cd ..

echo "Building libflac..."
cd flac-1.3.2
emconfigure ./configure --enable-static --disable-shared --prefix=$DEPS_DIR --libdir=$DEPS_DIR/lib --includedir=$DEPS_DIR/include --with-ogg-libraries=$DEPS_DIR/lib --with-ogg-includes=$DEPS_DIR/include --host=asmjs
emmake make
emmake make install
cd ..

echo "Building libsndfile..."
cd libsndfile-1.0.25
#emconfigure ./configure --enable-static --disable-shared --disable-libtool-lock --disable-cpu-clip --disable-sqlite --disable-alsa --enable-external-libs --disable-full-suite 

emconfigure ./configure --enable-static --disable-shared --prefix=$DEPS_DIR --disable-libtool-lock --disable-cpu-clip --disable-sqlite --disable-alsa --disable-full-suite --enable-external-libs --includedir=$DEPS_DIR/include LD_FLAGS=-L${DEPS_DIR}/lib --libdir=${DEPS_DIR}/lib OGG_LIBS="-lm -logg" OGG_CFLAGS="-I${DEPS_DIR}/include" VORBIS_LIBS="-lm -lvorbis" VORBIS_CFLAGS="-I${DEPS_DIR}/include" VORBISENC_LIBS="-lvorbisenc" VORBISENC_CFLAGS=-"I${DEPS_DIR}/include" FLAC_LIBS="-lFLAC" FLAC_CFLAGS="-I${DEPS_DIR}/include" CPPFLAGS="-I${DEPS_DIR}/libogg-1.3.3/include"
emmake make 
emmake make install
cd ..

echo "What was built:"
find . -name "*.a" -ls
find . -name "*.la" -ls




