# Shim find module for the csound-wasm WebAssembly build.
#
# The top-level CMakeLists builds libsndfile from source via FetchContent and
# exposes it through the SndFile_INCLUDE_DIR / SndFile_LIBRARY cache variables
# and the SndFile::sndfile target. The Csound submodule (which must not be
# modified) calls `find_package(SndFile MODULE)`; this module satisfies that
# request from the in-tree build instead of searching for a system install,
# which does not exist in the Emscripten sysroot. Without it, Csound's
# check_deps() disables USE_LIBSNDFILE and libcsound is compiled with no
# soundfile I/O ("not using libsndfile"), so fout and -o file.wav cannot open
# files.

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SndFile
    REQUIRED_VARS SndFile_LIBRARY SndFile_INCLUDE_DIR
)

if(SndFile_FOUND AND NOT TARGET SndFile::sndfile)
    if(TARGET "${SndFile_LIBRARY}")
        add_library(SndFile::sndfile ALIAS "${SndFile_LIBRARY}")
    else()
        add_library(SndFile::sndfile UNKNOWN IMPORTED)
        set_target_properties(SndFile::sndfile PROPERTIES
            IMPORTED_LOCATION "${SndFile_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SndFile_INCLUDE_DIR}"
        )
    endif()
endif()
