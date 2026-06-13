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
# libsndfile for WebAssembly (isolated ExternalProject).
#
# Csound consumes libsndfile through the SndFile::sndfile target, both in the
# final link and in try_compile-based configure probes (CMAKE_REQUIRED_LIBRARIES,
# honored even by check_include_file via CMP0075). CMake can only forward an
# IMPORTED target into a try_compile project; it cannot forward an ALIAS to an
# in-tree target. libsndfile's own CMake unconditionally defines
# `add_library(SndFile::sndfile ALIAS sndfile)`, so building it in-tree
# (FetchContent_MakeAvailable) makes every Csound configure probe abort with
# "target ... was not found". The historical build avoided this by consuming a
# prebuilt libsndfile.a as a real IMPORTED target.
#
# Reproduce that cleanly: build libsndfile as an isolated ExternalProject (its
# alias stays in its own sub-project) and present Csound a real IMPORTED static
# library. A pre-created empty placeholder archive lets Csound's configure-time
# link probes succeed before the real archive is built; the real archive is
# produced before Csound compiles/links via add_dependencies(). No submodule
# files are modified.
# -----------------------------------------------------------------------------

include(ExternalProject)

set(CSOUND_WASM_SNDFILE_PREFIX      "${CMAKE_BINARY_DIR}/libsndfile")
set(CSOUND_WASM_SNDFILE_INSTALL     "${CSOUND_WASM_SNDFILE_PREFIX}/install")
set(CSOUND_WASM_SNDFILE_INCLUDE_DIR "${CSOUND_WASM_SNDFILE_INSTALL}/include")
set(CSOUND_WASM_SNDFILE_ARCHIVE
    "${CSOUND_WASM_SNDFILE_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sndfile${CMAKE_STATIC_LIBRARY_SUFFIX}")

ExternalProject_Add(libsndfile_external
    URL https://github.com/libsndfile/libsndfile/archive/refs/tags/1.2.2.tar.gz
    PREFIX "${CSOUND_WASM_SNDFILE_PREFIX}"
    INSTALL_DIR "${CSOUND_WASM_SNDFILE_INSTALL}"
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DCMAKE_INSTALL_LIBDIR=lib"
        "-DCMAKE_C_BYTE_ORDER=LITTLE_ENDIAN"
        "-DCMAKE_CXX_BYTE_ORDER=LITTLE_ENDIAN"
        "-DBUILD_SHARED_LIBS=OFF"
        "-DBUILD_PROGRAMS=OFF"
        "-DBUILD_EXAMPLES=OFF"
        "-DBUILD_TESTING=OFF"
        "-DBUILD_REGTEST=OFF"
        "-DENABLE_EXTERNAL_LIBS=OFF"
        "-DENABLE_MPEG=OFF"
        "-DINSTALL_PKGCONFIG_MODULE=OFF"
        "-DINSTALL_MANPAGES=OFF"
        "-DENABLE_CPACK=OFF"
        "-DENABLE_PACKAGE_CONFIG=OFF"
    BUILD_BYPRODUCTS "${CSOUND_WASM_SNDFILE_ARCHIVE}"
)

# The IMPORTED target's include directory must exist at configure time, and the
# configure-time link probes need a valid archive to link against before the
# real libsndfile.a exists. Create the include dir and an empty (but valid)
# placeholder archive; the real ExternalProject build overwrites the archive.
file(MAKE_DIRECTORY "${CSOUND_WASM_SNDFILE_INCLUDE_DIR}")
file(MAKE_DIRECTORY "${CSOUND_WASM_SNDFILE_INSTALL}/lib")
if(NOT EXISTS "${CSOUND_WASM_SNDFILE_ARCHIVE}")
    execute_process(COMMAND "${CMAKE_AR}" qc "${CSOUND_WASM_SNDFILE_ARCHIVE}")
endif()

# Real IMPORTED target: forwards cleanly into Csound's try_compile probes and is
# linked by the final wasm executables. Build ordering against the real archive
# is enforced with add_dependencies(... libsndfile_external) further below.
add_library(SndFile::sndfile STATIC IMPORTED GLOBAL)
set_target_properties(SndFile::sndfile PROPERTIES
    IMPORTED_LOCATION "${CSOUND_WASM_SNDFILE_ARCHIVE}"
    INTERFACE_INCLUDE_DIRECTORIES "${CSOUND_WASM_SNDFILE_INCLUDE_DIR}"
)
set(CSOUND_WASM_SNDFILE_TARGET SndFile::sndfile)

# Variables consumed by the (unmodified) Csound submodule and by the shim
# FindSndFile module on CMAKE_MODULE_PATH (keeps USE_LIBSNDFILE enabled).
set(SNDFILE_H_PATH      "${CSOUND_WASM_SNDFILE_INCLUDE_DIR}" CACHE PATH     "sndfile.h directory" FORCE)
set(SndFile_INCLUDE_DIR "${CSOUND_WASM_SNDFILE_INCLUDE_DIR}" CACHE PATH     "libsndfile include directory" FORCE)
set(SndFile_LIBRARY     "${CSOUND_WASM_SNDFILE_ARCHIVE}"     CACHE FILEPATH "libsndfile archive" FORCE)
set(SNDFILE_LIBRARY     "${CSOUND_WASM_SNDFILE_ARCHIVE}"     CACHE FILEPATH "libsndfile archive" FORCE)
set(LIBSNDFILE_LIBRARY  "${CSOUND_WASM_SNDFILE_ARCHIVE}"     CACHE FILEPATH "libsndfile archive" FORCE)
set(SndFile_FOUND TRUE CACHE BOOL "libsndfile found" FORCE)
set(USE_LIBSNDFILE ON CACHE BOOL "Use libsndfile" FORCE)
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Csound's CMake defines many compiled targets (the core library plus numerous
# per-opcode-group / plugin libraries such as ipmidi, deprecated, ...), and many
# of them include Csound's soundfile.h -> <sndfile.h>. Every one must wait for
# libsndfile_external to install the header, or a parallel clean build races and
# fails with "sndfile.h file not found". Enumerate all buildsystem targets in a
# directory tree (recursing into subdirectories) and make each depend on the
# ExternalProject, rather than hand-listing Csound's internal target names.
function(csound_wasm_depend_on_sndfile dir)
    get_property(_subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(_sub IN LISTS _subdirs)
        csound_wasm_depend_on_sndfile("${_sub}")
    endforeach()
    get_property(_targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_tgt IN LISTS _targets)
        get_target_property(_type "${_tgt}" TYPE)
        if(NOT _type STREQUAL "INTERFACE_LIBRARY")
            add_dependencies("${_tgt}" libsndfile_external)
        endif()
    endforeach()
endfunction()

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
    "${CMAKE_SOURCE_DIR}/dependencies"
    "${CMAKE_SOURCE_DIR}/dependencies/csound"
    "${CMAKE_SOURCE_DIR}/dependencies/csound/include"
    "${CMAKE_SOURCE_DIR}/dependencies/csound/H"
    "${CMAKE_SOURCE_DIR}/dependencies/csound-ac"
    "${CMAKE_SOURCE_DIR}/dependencies/csound-ac/CsoundAC"
    "${CSOUND_WASM_SNDFILE_INCLUDE_DIR}"
)
# On Linux, Boost_INCLUDE_DIR is /usr/include. Adding that as a global -I pulls
# glibc stdint.h/limits.h before emscripten's sysroot (bits/libc-header-start.h).
# Eigen is under /usr/include/eigen3; some toolchains still follow into /usr/include
# for <stdint.h>, so do not add Eigen globally for Emscripten either.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    include_directories("${Boost_INCLUDE_DIR}")
    include_directories("${Eigen_INCLUDE_DIR}")
endif()

# Csound's CMake appends /usr to CMAKE_PREFIX_PATH; find_* can then add host
# libc headers to emcc. Ignore those paths for CMake searches (not for explicit
# target_include_directories below).
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    list(APPEND CMAKE_IGNORE_PATH /usr/include /usr/local/include)
endif()

# Pre-generate version.h before libcsound compiles so it wins over any stale copy on
# the global include path (e.g. csound-ac/CsoundAC/version.h from old workarounds).
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dependencies/csound/include")
if(NOT DEFINED CS_VERSION)
    set(CS_VERSION "7")
    set(CS_SUBVER "0")
    set(CS_PATCHLEVEL "0")
endif()
if(NOT DEFINED CS_PACKAGE_DATE_DEFINE)
    string(TIMESTAMP CS_PACKAGE_DATE_DEFINE "__DATE__")
endif()
configure_file(
    "${CMAKE_SOURCE_DIR}/dependencies/csound/include/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/dependencies/csound/include/version.h"
    @ONLY
)
include_directories(BEFORE "${CMAKE_CURRENT_BINARY_DIR}/dependencies/csound/include")

# Build Csound and CsoundAC.
add_subdirectory(dependencies/csound)
add_subdirectory(CsoundAC)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten" AND TARGET csoundac-static)
    target_include_directories(csoundac-static PRIVATE
        "${Boost_INCLUDE_DIR}"
        "${Eigen_INCLUDE_DIR}"
    )
endif()

if(TARGET csound-static)
    set(CSOUND_WASM_CSOUND_TARGET csound-static)
elseif(TARGET csound)
    set(CSOUND_WASM_CSOUND_TARGET csound)
else()
    message(FATAL_ERROR "Expected csound or csound-static target from dependencies/csound")
endif()

# Build the real libsndfile (headers + archive) before anything under Csound or
# CsoundAC that compiles against <sndfile.h>, and before the IMPORTED
# SndFile::sndfile archive is linked. Covers every Csound sub-target (core lib
# plus per-opcode-group libraries) and csoundac-static.
csound_wasm_depend_on_sndfile("${CMAKE_SOURCE_DIR}/dependencies/csound")
csound_wasm_depend_on_sndfile("${CMAKE_SOURCE_DIR}/CsoundAC")
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

# Emscripten 3.1.x: settings.js defines ENVIRONMENT as a string ('web,webview,...').
# Emscripten 5.0+: ENVIRONMENT is a list (['web', ...]); -sENVIRONMENT=web,worker
# then fails type-check ("expects list but got str"). JSON array works on 5.x;
# on 3.1.x JSON parses to a list and fails ("expects str but got list"). Branch
# on the toolchain version. SHELL: keeps each -s...=... as one linker argv.
set(CSOUND_WASM_ENVIRONMENT_LINK_FLAG "SHELL:-sENVIRONMENT=web,worker")
set(CSOUND_WASM_EXPORTED_RUNTIME_METHODS_FLAG "SHELL:-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"FS\"]")
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    if(NOT EMSCRIPTEN_VERSION AND DEFINED ENV{EMSCRIPTEN})
        set(_ems_ver_file "$ENV{EMSCRIPTEN}/emscripten-version.txt")
        if(EXISTS "${_ems_ver_file}")
            file(STRINGS "${_ems_ver_file}" EMSCRIPTEN_VERSION LIMIT_COUNT 1)
            string(STRIP "${EMSCRIPTEN_VERSION}" EMSCRIPTEN_VERSION)
        endif()
    endif()
    if(EMSCRIPTEN_VERSION VERSION_GREATER_EQUAL "5.0.0")
        set(CSOUND_WASM_ENVIRONMENT_LINK_FLAG "SHELL:-sENVIRONMENT=[\"web\",\"worker\"]")
    else()
        if(NOT EMSCRIPTEN_VERSION)
            message(WARNING "csound-wasm: EMSCRIPTEN_VERSION unset; using pre-5.0 ENVIRONMENT=-s form (comma string). If linking fails, use emcmake from emsdk 5.x or set EMSCRIPTEN in the environment.")
        endif()
        set(CSOUND_WASM_ENVIRONMENT_LINK_FLAG "SHELL:-sENVIRONMENT=web,worker")
    endif()
    set(CSOUND_WASM_EXPORTED_RUNTIME_METHODS_FLAG "SHELL:-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"FS\"]")
    message(STATUS "csound-wasm: Emscripten '${EMSCRIPTEN_VERSION}' — ${CSOUND_WASM_ENVIRONMENT_LINK_FLAG}")
endif()

set(CSOUND_WASM_COMMON_COMPILE_OPTIONS
    -O3
    -DNDEBUG
    -Wno-implicit-int-float-conversion
)

set(CSOUND_WASM_COMMON_LINK_OPTIONS
    "${CSOUND_WASM_ENVIRONMENT_LINK_FLAG}"
    "-sERROR_ON_UNDEFINED_SYMBOLS=0"
    "-sALLOW_MEMORY_GROWTH=1"
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
    "${CSOUND_WASM_SNDFILE_INCLUDE_DIR}"
    "${Eigen_INCLUDE_DIR}"
)

target_link_libraries(CsoundAudioProcessor PRIVATE
    ${CSOUND_WASM_CSOUND_TARGET}
    ${CSOUND_WASM_SNDFILE_TARGET}
)
add_dependencies(CsoundAudioProcessor libsndfile_external)

target_link_options(CsoundAudioProcessor PRIVATE
    --bind
    ${CSOUND_WASM_COMMON_LINK_OPTIONS}
    "${CSOUND_WASM_EXPORTED_RUNTIME_METHODS_FLAG}"
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
    "${CSOUND_WASM_SNDFILE_INCLUDE_DIR}"
    "${Eigen_INCLUDE_DIR}"
    "${Boost_INCLUDE_DIR}"
)

target_link_libraries(CsoundACWasm PRIVATE
    csoundac-static
    ${CSOUND_WASM_CSOUND_TARGET}
    ${CSOUND_WASM_SNDFILE_TARGET}
)
add_dependencies(CsoundACWasm libsndfile_external)

target_link_options(CsoundACWasm PRIVATE
    --bind
    ${CSOUND_WASM_COMMON_LINK_OPTIONS}
    "-sEXPORT_ES6=0"
    "-sEXPORT_NAME=createCsoundAC"
    "${CSOUND_WASM_EXPORTED_RUNTIME_METHODS_FLAG}"
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
