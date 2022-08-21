rm -rf deps
mkdir -p deps
cd deps

DEPS_DIR=$PWD
echo "DEPS_DIR: ${DEPS_DIR}"


echo "Downloading libogg..."
wget http://downloads.xiph.org/releases/ogg/libogg-1.3.3.tar.xz
tar xvf libogg-1.3.3.tar.xz

echo "Downloading libvorbis..."
wget http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.6.tar.xz
tar xvf libvorbis-1.3.6.tar.xz

echo "Downloading libflac..."
wget http://downloads.xiph.org/releases/flac/flac-1.3.2.tar.xz
tar xvf flac-1.3.2.tar.xz

echo "Downloading libsndfile..."
wget http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.25.tar.gz
tar -xzf libsndfile-1.0.25.tar.gz
echo "Patching libsndfile..."
patch libsndfile-1.0.25/src/sndfile.c < ../patches/sndfile.c.patch
    
cd ..

ls -ll deps/libogg-1.3.3/
ls -ll deps/libvorbis-1.3.6/
ls -ll deps/flac-1.3.2/
ls -ll deps/libsndfile-1.0.25/
