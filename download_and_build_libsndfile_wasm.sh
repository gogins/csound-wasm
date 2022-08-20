# Downloads and build ogg, vorbis, flac, and libsndfile
# Notes: 
# * Using pkgconfig did not work with emconfigure ./configure ..., so hardcoded paths are used
# * All artifacts are installed into deps/lib and deps/include

export CFLAGS="-O3 -flto"
export CXXFLAGS="-O3 -flto"

mkdir -p deps
cd deps

DEPS_DIR=$PWD

echo "DEPS_DIR: ${DEPS_DIR}"

# DOWNLOAD DEPS AND UNPACK

echo "Downloading libogg..."
if [ ! -d "libogg-1.3.3" ]; then
    wget http://downloads.xiph.org/releases/ogg/libogg-1.3.3.tar.xz
    tar xvf libogg-1.3.3.tar.xz

    echo "Building libogg..."
    cd libogg-1.3.3
    emconfigure ./configure --enable-static --disable-shared --prefix=$DEPS_DIR --libdir=$DEPS_DIR/lib --includedir=$DEPS_DIR/include 
    #emconfigure ./configure --enable-static --disable-shared
    emmake make
    emmake make install
    cd ..
fi

echo "Downloading libvorbis..."
if [ ! -d "libvorbis-1.3.6" ]; then
    wget http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.6.tar.xz
    tar xvf libvorbis-1.3.6.tar.xz

    echo "Building libvorbis..."
    cd libvorbis-1.3.6
    emconfigure ./configure --enable-static --disable-shared --prefix=$DEPS_DIR --libdir=$DEPS_DIR/lib --includedir=$DEPS_DIR/include 
    #emconfigure ./configure --enable-static --disable-shared 
    emmake make CPPFLAGS=$DEPS_DIR/libogg-1.3.3/include
    emmake make install
    cd ..
fi

echo "Downloading libflac..."
if [ ! -d "flac-1.3.2" ]; then
    wget http://downloads.xiph.org/releases/flac/flac-1.3.2.tar.xz
    tar xvf flac-1.3.2.tar.xz
    echo "Building libflac..."
    cd flac-1.3.2
    emconfigure ./configure --enable-static --disable-shared --prefix=$DEPS_DIR --libdir=$DEPS_DIR/lib --includedir=$DEPS_DIR/include --with-ogg-libraries=$DEPS_DIR/lib --with-ogg-includes=$DEPS_DIR/include --host=asmjs
    emmake make
    emmake make install
    cd ..
fi

echo "Downloading libsndfile..."

if [ ! -d "libsndfile-1.0.25" ]; then

    wget http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.25.tar.gz
    tar -xzf libsndfile-1.0.25.tar.gz
    echo "Patching libsndfile..."
    patch libsndfile-1.0.25/src/sndfile.c < ../patches/sndfile.c.patch
    echo "Building libsndfile..."
    cd libsndfile-1.0.25
    #emconfigure ./configure --enable-static --disable-shared --disable-libtool-lock --disable-cpu-clip --disable-sqlite --disable-alsa --enable-external-libs --disable-full-suite 
    emconfigure ./configure --enable-static --disable-shared --prefix=$DEPS_DIR --disable-libtool-lock --disable-cpu-clip --disable-sqlite --disable-alsa --enable-external-libs --includedir=$DEPS_DIR/include LD_FLAGS="-L${DEPS_DIR}/lib" --libdir="${DEPS_DIR}/lib" OGG_LIBS="-lm -logg" OGG_CFLAGS="-I${DEPS_DIR}/include"  VORBIS_LIBS="-lm -lvorbis" VORBIS_CFLAGS="-I${DEPS_DIR}/include" VORBISENC_LIBS="-lvorbisenc" VORBISENC_CFLAGS="-I${DEPS_DIR}/include" FLAC_LIBS="-lFLAC" FLAC_CFLAGS="-I${DEPS_DIR}/include" CPPFLAGS=$DEPS_DIR/libogg-1.3.3/include

    emmake make 
    emmake make install

fi

echo "What was built:"
find . -name "*.a" -ls
find . -name "*.la" -ls




