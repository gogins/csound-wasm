cmake_minimum_required(VERSION 2.8)

project(csound-extended)

# WebAssembly is little-endian. These variables work around 
# emscripten-core/emscripten#14346

set(CMAKE_C_BYTE_ORDER LITTLE_ENDIAN)
set(CMAKE_CXX_BYTE_ORDER LITTLE_ENDIAN)

set(Boost_INCLUDE_DIR "${BOOST_ROOT}/include")
set(Eigen_INCLUDE_DIR "${EIGEN_ROOT}/include/eigen3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -frtti)
add_compile_definitions(USE_DOUBLE=0)
set(CPPSOUND_SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/dependencies/csound/interfaces")
set(CSOUND_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/dependencies/csound/include;.")
set(SNDFILE_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/deps/libsndfile-1.0.25/src")
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Verbose makefile" FORCE)
set(BUILD_PLUGINS_"plugins" CACHE DIR "plugins directory" FORCE)
set(USE_COMPILER_OPTIMIZATIONS 0)
set(WASM 1)
set(INIT_STATIC_MODULES ON CACHE BOOL "Initialize static modules" FORCE)
set(USE_DOUBLE OFF CACHE BOOL "Use doubles for audio sample words" FORCE)
set(BUILD_MULTI_CORE OFF CACHE BOOL "Build for multi-core performance" FORCE)
set(BUILD_JACK_OPCODES OFF CACHE BOOL "Build Jacko opcodes" FORCE)
set(EMSCRIPTEN ON CACHE BOOL "Using Emscripten to build" FORCE)
set(CMAKE_TOOLCHAIN_FILE "$EMSCRIPTEN_ROOT/cmake/Modules/Platform/Emscripten.cmake")
set(CMAKE_MODULE_PATH "$EMSCRIPTEN_ROOT/cmake")
set(CMAKE_BUILD_TYPE Release)
set(HAVE_BIG_ENDIAN OFF CACHE BOOL "Have big-endian architecture" FORCE)
set(CMAKE_16BIT_TYPE "unsigned short")
set(HAVE_STRTOD_L OFF CACHE BOOL "Have strtod_l "FORCE)
set(BUILD_STATIC_LIBRARY ON CACHE BOOL "Build as static library" FORCE)
set(HAVE_ATOMIC_BUILTIN OFF CACHE BOOL "Have atomic buildins" FORCE)
set(HAVE_SPRINTF_L OFF CACHE BOOL "Have sprintf_l" FORCE)
set(USE_GETTEXT OFF CACHE BOOL "Use gettext" FORCE)
set(BUILD_UTILITIES OFF CACHE "Build Csound utility programs" BOOL )
set(SNDFILE_H_PATH "${CMAKE_SOURCE_DIR}/deps/libsndfile-1.0.25/src" CACHE PATH "sndfile.h directory" FORCE)
set(LIBSNDFILE_LIBRARY 
    "${CMAKE_SOURCE_DIR}/deps/lib/libvorbisfile.a"
    "${CMAKE_SOURCE_DIR}/deps/lib/libvorbis.a"
    "${CMAKE_SOURCE_DIR}/deps/lib/libsndfile.a"
    "${CMAKE_SOURCE_DIR}/deps/lib/libogg.a"
    "${CMAKE_SOURCE_DIR}/deps/lib/libFLAC.a"
    "${CMAKE_SOURCE_DIR}/deps/lib/libFLAC++.a"
    "${CMAKE_SOURCE_DIR}/deps/lib/libvorbisenc.a"
    )
include_directories("${Boost_INCLUDE_DIR}")
include_directories("${Eigen_INCLUDE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/dependencies")
include_directories("${CMAKE_SOURCE_DIR}/dependencies/csound")
include_directories("${CMAKE_SOURCE_DIR}/dependencies/csound-ac")
#~ # Csound plugin opcodes, we only use some. Not doing now.
#~ set(CSOUND_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/dependencies/csound/include")
#~ add_subdirectory(dependencies/plugins)
# Just the regular Csound sources, which already are supposed to build with 
# Emscripten.
add_subdirectory(dependencies/csound)
# csound-ac sources, with some stuff added and some stuff left out.
add_subdirectory(CsoundAC)

