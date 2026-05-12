cmake_minimum_required(VERSION 3.28)

project(csound-wasm LANGUAGES C CXX)

set(CSOUND_WEB_ASSEMBLY_VERSION "4.0.0-beta" CACHE STRING "Csound WebAssembly version")

include(FetchContent)

# -----------------------------------------------------------------------------
# Project layout.
# -----------------------------------------------------------------------------

set(CSOUND_WASM_DIST_DIR "${CMAKE_SOURCE_DIR}/dist" CACHE PATH "Generated runtime artifacts")
set(CSOUND_WASM_WEB_DIR  "${CMAKE_SOURCE_DIR}/web"  CACHE PATH "Served runtime directory")
set(CSOUND_WASM_ARTIFACT_DIR "${CMAKE_BINARY_DIR}/artifacts" CACHE PATH "Generated build artifacts before staging")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CSOUND_WASM_ARTIFACT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CSOUND_WASM_ARTIFACT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# WebAssembly is little-endian. These variables work around
# emscripten-core/emscripten#14346.
set(CMAKE_C_BYTE_ORDER LITTLE_ENDIAN)
set(CMAKE_CXX_BYTE_ORDER LITTLE_ENDIAN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CSOUND_VERSION_MAJOR 7 CACHE STRING "Csound major version")
add_compile_definitions(
    CSOUND_VERSION_MAJOR=${CSOUND_VERSION_MAJOR}
    USE_DOUBLE=0
)

# -----------------------------------------------------------------------------
# Host include roots.
# These remain overrideable from the command line.
# -----------------------------------------------------------------------------

if(NOT DEFINED EIGEN_ROOT)
    if(APPLE)
        set(EIGEN_ROOT "/opt/homebrew/opt/eigen" CACHE PATH "Eigen root")
    else()
        set(EIGEN_ROOT "/usr" CACHE PATH "Eigen root")
    endif()
endif()

if(NOT DEFINED BOOST_ROOT)
    if(APPLE)
        set(BOOST_ROOT "/opt/homebrew/opt/boost" CACHE PATH "Boost root")
    else()
        set(BOOST_ROOT "/usr" CACHE PATH "Boost root")
    endif()
endif()

set(Eigen_INCLUDE_DIR "${EIGEN_ROOT}/include/eigen3" CACHE PATH "Eigen include directory")
set(Boost_INCLUDE_DIR "${BOOST_ROOT}/include" CACHE PATH "Boost include directory")

# -----------------------------------------------------------------------------
# libsndfile for WebAssembly.
#
# Build libsndfile from source with external compressed audio codecs disabled.
# This avoids the old shell-script chain for Ogg, Vorbis, FLAC, and libsndfile.
# -----------------------------------------------------------------------------

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(BUILD_PROGRAMS OFF CACHE BOOL "Build libsndfile programs" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build libsndfile examples" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
set(BUILD_REGTEST OFF CACHE BOOL "Build libsndfile regression tests" FORCE)
set(ENABLE_EXTERNAL_LIBS OFF CACHE BOOL "Enable Ogg/Vorbis/FLAC dependencies" FORCE)
set(ENABLE_MPEG OFF CACHE BOOL "Enable MPEG dependencies" FORCE)
set(INSTALL_PKGCONFIG_MODULE OFF CACHE BOOL "Install pkg-config module" FORCE)
set(INSTALL_MANPAGES OFF CACHE BOOL "Install man pages" FORCE)
set(ENABLE_CPACK OFF CACHE BOOL "Enable CPack support" FORCE)
set(ENABLE_PACKAGE_CONFIG OFF CACHE BOOL "Generate package config file" FORCE)

FetchContent_Declare(
    libsndfile
    URL https://github.com/libsndfile/libsndfile/archive/refs/tags/1.2.2.tar.gz
)

FetchContent_MakeAvailable(libsndfile)

if(TARGET sndfile)
    set(CSOUND_WASM_SNDFILE_TARGET sndfile)
elseif(TARGET SndFile::sndfile)
    set(CSOUND_WASM_SNDFILE_TARGET SndFile::sndfile)
else()
    message(FATAL_ERROR "libsndfile FetchContent succeeded, but no sndfile target was created")
endif()

set(SNDFILE_H_PATH
    "${libsndfile_SOURCE_DIR}/include"
    CACHE PATH "sndfile.h directory" FORCE
)

# Csound's CMake may expect either SNDFILE_LIBRARY or LIBSNDFILE_LIBRARY.
# Use the target name first; if upstream Csound later requires a file path,
# this can be changed in one place.
set(SNDFILE_LIBRARY
    ${CSOUND_WASM_SNDFILE_TARGET}
    CACHE STRING "libsndfile CMake target" FORCE
)

set(LIBSNDFILE_LIBRARY
    ${CSOUND_WASM_SNDFILE_TARGET}
    CACHE STRING "libsndfile CMake target" FORCE
)

set(SndFile_FOUND TRUE CACHE BOOL "libsndfile found" FORCE)
set(USE_LIBSNDFILE ON CACHE BOOL "Use libsndfile" FORCE)

# -----------------------------------------------------------------------------
# Csound configuration.
# -----------------------------------------------------------------------------

set(CPPSOUND_SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/dependencies/csound/interfaces" CACHE PATH "Csound C++ interface directory" FORCE)
set(CSOUND_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/dependencies/csound/include;." CACHE STRING "Csound include directories" FORCE)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Verbose makefile" FORCE)
set(USE_COMPILER_OPTIMIZATIONS 0 CACHE STRING "Csound compiler optimizations" FORCE)
set(WASM 1 CACHE BOOL "Build for WebAssembly" FORCE)
set(INIT_STATIC_MODULES OFF CACHE BOOL "Initialize static Csound modules" FORCE)
set(USE_DOUBLE OFF CACHE BOOL "Use doubles for audio sample words" FORCE)
set(BUILD_MULTI_CORE OFF CACHE BOOL "Build for multi-core performance" FORCE)
set(BUILD_JACK_OPCODES OFF CACHE BOOL "Build Jacko opcodes" FORCE)
set(EMSCRIPTEN ON CACHE BOOL "Using Emscripten" FORCE)
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
set(HAVE_BIG_ENDIAN OFF CACHE BOOL "Have big-endian architecture" FORCE)
set(CMAKE_16BIT_TYPE "unsigned short" CACHE STRING "16-bit type" FORCE)
set(HAVE_STRTOD_L OFF CACHE BOOL "Have strtod_l" FORCE)
# With Emscripten, Csound still adds the primary `csound` library; a second
# `csound-static` archive reuses OUTPUT_NAME "csound" and Ninja reports duplicate
# rules for libcsound.a. Use the main library target for final WASM links only.
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(BUILD_STATIC_LIBRARY OFF CACHE BOOL "Build Csound as a static library" FORCE)
else()
    set(BUILD_STATIC_LIBRARY ON CACHE BOOL "Build Csound as a static library" FORCE)
endif()
set(HAVE_ATOMIC_BUILTIN OFF CACHE BOOL "Have atomic builtins" FORCE)
set(HAVE_SPRINTF_L OFF CACHE BOOL "Have sprintf_l" FORCE)
set(USE_GETTEXT OFF CACHE BOOL "Use gettext" FORCE)
set(BUILD_UTILITIES OFF CACHE BOOL "Build Csound utility programs" FORCE)
set(BUILD_CSBEATS OFF CACHE BOOL "Build csbeats frontend" FORCE)
set(INSTALL_PYTHON_INTERFACE OFF CACHE BOOL "Install Python interface" FORCE)

include_directories(
    "${Boost_INCLUDE_DIR}"
    "${Eigen_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/dependencies"
    "${CMAKE_SOURCE_DIR}/dependencies/csound"
    "${CMAKE_SOURCE_DIR}/dependencies/csound/include"
    "${CMAKE_SOURCE_DIR}/dependencies/csound/H"
    "${CMAKE_SOURCE_DIR}/dependencies/csound-ac"
    "${CMAKE_SOURCE_DIR}/dependencies/csound-ac/CsoundAC"
    "${libsndfile_SOURCE_DIR}/include"
)

# Build Csound and CsoundAC.
add_subdirectory(dependencies/csound)
add_subdirectory(CsoundAC)

if(TARGET csound-static)
    set(CSOUND_WASM_CSOUND_TARGET csound-static)
elseif(TARGET csound)
    set(CSOUND_WASM_CSOUND_TARGET csound)
else()
    message(FATAL_ERROR "Expected csound or csound-static target from dependencies/csound")
endif()

if(TARGET csound-static)
    add_dependencies(csound-static ${CSOUND_WASM_SNDFILE_TARGET})
endif()
if(TARGET csoundac-static)
    target_compile_options(csoundac-static PRIVATE -O1)
endif()

# -----------------------------------------------------------------------------
# Fresh runtime output directories.
# -----------------------------------------------------------------------------

add_custom_target(prepare_runtime_dirs
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${CSOUND_WASM_DIST_DIR}" "${CSOUND_WASM_WEB_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CSOUND_WASM_DIST_DIR}" "${CSOUND_WASM_WEB_DIR}" "${CSOUND_WASM_ARTIFACT_DIR}"
    COMMENT "Removing and recreating dist/ and web/"
)

# -----------------------------------------------------------------------------
# Browser-facing Embind artifacts.
# -----------------------------------------------------------------------------

set(CSOUND_WASM_COMMON_COMPILE_OPTIONS
    -O3
    -DNDEBUG
    -Wno-implicit-int-float-conversion
)

set(CSOUND_WASM_COMMON_LINK_OPTIONS
    "-sENVIRONMENT=[\"web\",\"worker\"]"
    "-sERROR_ON_UNDEFINED_SYMBOLS=0"
    "-sALLOW_MEMORY_GROWTH=0"
    "-sASSERTIONS=0"
    "-sFORCE_FILESYSTEM=1"
    "-sINITIAL_MEMORY=268435456"
    "-sSAFE_HEAP=0"
    "-sWASM=1"
)

add_executable(CsoundAudioProcessor
    src/csound_embind.cpp
)


set_target_properties(CsoundAudioProcessor PROPERTIES
    OUTPUT_NAME "CsoundAudioProcessor"
    SUFFIX ".js"
    RUNTIME_OUTPUT_DIRECTORY "${CSOUND_WASM_ARTIFACT_DIR}"
)

target_compile_options(CsoundAudioProcessor PRIVATE
    ${CSOUND_WASM_COMMON_COMPILE_OPTIONS}
)

target_include_directories(CsoundAudioProcessor PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/dependencies"
    "${CMAKE_SOURCE_DIR}/dependencies/csound-ac"
    "${CMAKE_SOURCE_DIR}/dependencies/csound/include"
    "${CMAKE_SOURCE_DIR}/dependencies/csound/H"
    "${CMAKE_SOURCE_DIR}/dependencies/csound/interfaces"
    "${libsndfile_SOURCE_DIR}/include"
    "${Eigen_INCLUDE_DIR}"
)

target_link_libraries(CsoundAudioProcessor PRIVATE
    ${CSOUND_WASM_CSOUND_TARGET}
    ${CSOUND_WASM_SNDFILE_TARGET}
)

target_link_options(CsoundAudioProcessor PRIVATE
    --bind
    ${CSOUND_WASM_COMMON_LINK_OPTIONS}
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"FS\"]"
    "-sRESERVED_FUNCTION_POINTERS=2"
    "-sSINGLE_FILE=1"
    "-sWASM_ASYNC_COMPILATION=0"
    "--source-map-base=."
    "--pre-js=${CMAKE_SOURCE_DIR}/src/CsoundAudioProcessor_prejs.js"
    "--post-js=${CMAKE_SOURCE_DIR}/src/CsoundAudioProcessor_postjs.js"
)

add_executable(CsoundACWasm
    CsoundAC/csoundac_embind.cpp
)


set_target_properties(CsoundACWasm PROPERTIES
    OUTPUT_NAME "CsoundAC"
    SUFFIX ".js"
    RUNTIME_OUTPUT_DIRECTORY "${CSOUND_WASM_ARTIFACT_DIR}"
)

set(CSOUND_AC_WASM_OUTPUT "${CSOUND_WASM_ARTIFACT_DIR}/CsoundAC.wasm")

target_compile_options(CsoundACWasm PRIVATE
    -O3
    -DNDEBUG
    -Wno-implicit-int-float-conversion
)

# Keep this target at -O1 even when the global build type contributes -O2.
# set_property(TARGET CsoundACWasm APPEND_STRING PROPERTY COMPILE_FLAGS " -O1")

target_include_directories(CsoundACWasm PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/dependencies"
    "${CMAKE_SOURCE_DIR}/dependencies/csound-ac"
    "${libsndfile_SOURCE_DIR}/include"
    "${Eigen_INCLUDE_DIR}"
    "${Boost_INCLUDE_DIR}"
)

target_link_libraries(CsoundACWasm PRIVATE
    csoundac-static
    ${CSOUND_WASM_CSOUND_TARGET}
    ${CSOUND_WASM_SNDFILE_TARGET}
)

target_link_options(CsoundACWasm PRIVATE
    --bind
    ${CSOUND_WASM_COMMON_LINK_OPTIONS}
    "-sEXPORT_ES6=0"
    "-sEXPORT_NAME=createCsoundAC"
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"FS\"]"
    "-sMODULARIZE=1"
    "-sRESERVED_FUNCTION_POINTERS=1"
    # CsoundAC is intentionally emitted as JavaScript plus a separate
    # CsoundAC.wasm file. This avoids the large single-file async loader path
    # while retaining the single-file AudioWorklet processor build above.
    "-sSINGLE_FILE=0"
    "-sWASM_ASYNC_COMPILATION=1"
    "--source-map-base=."
)

# -----------------------------------------------------------------------------
# Dist and web staging.
#
# dist/ is the canonical staged runtime. web/ is a mirror of dist/ for local
# serving. Top-level files or directories prefixed with "--" are intentionally
# excluded from both locations.
# -----------------------------------------------------------------------------

set(CSOUND_WASM_STATIC_FILES
    "${CMAKE_SOURCE_DIR}/README.md"
    "${CMAKE_SOURCE_DIR}/src/csound_loader.js"
    "${CMAKE_SOURCE_DIR}/src/CsoundAudioNode.js"
    "${CMAKE_SOURCE_DIR}/src/dat.gui.js"
    "${CMAKE_SOURCE_DIR}/src/minimal.html"
    "${CMAKE_SOURCE_DIR}/src/sprintf.js"
    "${CMAKE_SOURCE_DIR}/src/tinycolor.js"
    "${CMAKE_SOURCE_DIR}/src/three.js"
    "${CMAKE_SOURCE_DIR}/src/TrackballControls.js"
    "${CMAKE_SOURCE_DIR}/src/trichord_space.html"
)

# web/ is an output directory and is removed at the start of each build.
# Optional browser helper files therefore must live in src/ or static/, not web/.
foreach(optional_static_file
    "${CMAKE_SOURCE_DIR}/src/ace.js"
    "${CMAKE_SOURCE_DIR}/src/favicon.ico"
    "${CMAKE_SOURCE_DIR}/src/csound-browser-test.html"
    "${CMAKE_SOURCE_DIR}/src/osc.min.js"
    "${CMAKE_SOURCE_DIR}/src/player.html"
    "${CMAKE_SOURCE_DIR}/static/ace.js"
    "${CMAKE_SOURCE_DIR}/static/csound-browser-test.html"
    "${CMAKE_SOURCE_DIR}/static/osc.min.js"
    "${CMAKE_SOURCE_DIR}/static/player.html"
)
    if(EXISTS "${optional_static_file}")
        list(APPEND CSOUND_WASM_STATIC_FILES "${optional_static_file}")
    endif()
endforeach()

add_custom_target(stage_dist ALL
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${CSOUND_WASM_STATIC_FILES} "${CSOUND_WASM_DIST_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:CsoundAudioProcessor>" "${CSOUND_WASM_DIST_DIR}/CsoundAudioProcessor.js"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:CsoundACWasm>" "${CSOUND_WASM_DIST_DIR}/CsoundAC.js"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CSOUND_AC_WASM_OUTPUT}" "${CSOUND_WASM_DIST_DIR}/CsoundAC.wasm"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/dependencies/csound-ac/patches" "${CSOUND_WASM_DIST_DIR}/patches"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/dependencies/csound-ac/silencio" "${CSOUND_WASM_DIST_DIR}/silencio"
    COMMAND /bin/sh -c "find '${CSOUND_WASM_DIST_DIR}' -maxdepth 1 -name '--*' -exec rm -rf {} +"
    DEPENDS
        prepare_runtime_dirs
        CsoundAudioProcessor
        CsoundACWasm
    COMMENT "Staging runtime files into dist/"
)

add_custom_target(stage_web ALL
    COMMAND rsync -a --delete --exclude='--*' "${CSOUND_WASM_DIST_DIR}/" "${CSOUND_WASM_WEB_DIR}/"
    COMMAND /bin/sh -c "find '${CSOUND_WASM_WEB_DIR}' -maxdepth 1 -name '--*' -exec rm -rf {} +"
    DEPENDS stage_dist
    COMMENT "Mirroring dist/ into web/"
)

add_custom_target(package_wasm ALL
    COMMAND "${CMAKE_COMMAND}" -E tar cfv "${CMAKE_SOURCE_DIR}/csound-wasm-${CSOUND_WEB_ASSEMBLY_VERSION}.zip" --format=zip .
    DEPENDS stage_web
    WORKING_DIRECTORY "${CSOUND_WASM_DIST_DIR}"
    COMMENT "Creating csound-wasm.zip from dist/"
)
