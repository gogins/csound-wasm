# csound-wasm

Edward Costello, Steven Yi, Henri Manson, Michael Gogins<br>
https://github.com/gogins<br>
http://michaelgogins.tumblr.com

## Important Note

In order for Csound to use audio, the microphone, and MIDI, the user must 
grant these permissions for the site hosting Csound to the Web browser 
(usually by right-clicking on the lock symbol to the left of the URL, or 
on a permissions icon to the left of the URL).

At this time, for reasons not yet known to me, the csound-wasm build 
of Csound does not always run in Google Chrome, even after granting 
permissions. I have opened an issue to fix this.

In the meantime, after granting permissions, everything works fine in 
Firefox.

## Introduction

THis directory builds and packages csound-wasm for WebAssembly.
This build replaces `CsoundObj.js` from the core Csound repository with a 
WebAssembly build of Csound, compiled using the Emscripten LLVM toolchain.
This build uses the new WebAudio AudioWorklet for superior performance with 
fewer audio issues. Features include:

* A number of C++ plugin opcodes (here, statically linked).

* A new JavaScript interface to Csound that follows, as exactly as possible,
  the interface defined by the Csound class in `csound.hpp` and also
  implemented in CsoundOboe in `csound_oboe.hpp` for the Csound for Android
  app, and in `csound.node`.

* Additional Csound API methods exposed to JavaScript.

* A new WebAssembly build of the C++ CsoundAC ("Csound Algorithmic 
  Composition") library.

Please log any bug reports or requests for enhancements at
https://github.com/gogins/csound-extended/issues.

## Changes

See https://github.com/gogins/csound-wasm/commits/develop for the commit log.

## Installation

Download the latest version of `csound-wasm-{version.zip}` from the 
[release page here](https://github.com/gogins/csound-extended/releases). Unzip 
it and it is ready to use.

## Examples

There is a local Web server and a minimal working example in the release zip 
file. In the directory where you have unzipped the release, execute:

```
python3 -m http.server
```

Then navigate to `http://localhost:8000` and view `minimal.html`. Click on 
the Play button to validate your installation.

Do the same with `trichord_space.html` which also uses CsoundAC.

Some of the examples herein will run from 
https://gogins.github.io/csound-examples. For more information, see my 
[csound-examples](https://github.com/gogins/csound-examples) repository.

## Building

You will need to make sure that the boost header files and the Eigen library 
for matrix algebra are available to the Emscripten toolchain. The easiest way 
to do this is to install the `libeigen3-dev` and `libboost-dev` system 
packages, and make them available to the Emscripten toolchain. Only header 
files from these packages are used here.

For your first build, run...

### `fresh-build-wasm.sh`

This updates the Git submodules, updates the Emscripten toolchain, and then 
calls... 

### `clean-build-wasm.sh`

This actives the latest Emscripten SDK and sets up the shell environment for 
building, deletes any previous build directory, creates a clean build 
directory, clears the Emscripten compiler cache, downloads libsndfile 
dependencies and builds them, and then calls...

### `build-wasm.sh`

This does the actual build of WASM binaries for csound, cmask, and CsoundAC 
with a mixture of CMake and build scripts, and then calls...

### `release-wasm.sh`

This marshals the release into the `dist-wasm` directory, and then zips up the 
contents of `dist-wasm` into a release package in the form of a zip archive 
that provides WebAssembly builds of Csound and CsoundAC.




