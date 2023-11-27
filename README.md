# csound-wasm

Edward Costello, Steven Yi, Henri Manson, Michael Gogins<br>
https://github.com/gogins<br>
http://michaelgogins.tumblr.com

## Important Note

In order for Csound to use audio, the microphone, and MIDI, the user must 
grant these permissions for the site hosting Csound to the Web browser 
(usually by right-clicking on the lock symbol to the left of the URL, or 
on a permissions icon to the left of the URL).

## Introduction

THis directory builds and packages csound-wasm (Csound for WebAssembly).
This build replaces `CsoundObj.js` from the core Csound repository with my 
own WebAssembly build of Csound, compiled using the Emscripten LLVM 
toolchain.

Although I have tried to use `CsoundObj` in my compositions, I find that 
my own build is more stable and easier to use for my purposes. That is because 
my WebAssembly version of the Csound API is much closer to the original C++ 
interface, supports printing Csound messages from JavaScript, and appears to 
be more resilient and reliable. As long as this project is easy to maintain, I 
will continue to support it and use it.

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

There are some working examples in the release zip file. In the directory where 
you have unzipped the release, execute:

```
python3 -m http.server
```

Then navigate to `http://localhost:8000` and view `minimal.html`. Click on 
the Play button to validate your installation. Do the same with 
`trichord_space.html` which uses CsoundAC in addition to Csound.

Some of the examples herein will run from 
https://gogins.github.io/csound-examples. For more information, see my 
[csound-examples](https://github.com/gogins/csound-examples) repository.

## Building

You will need to make sure that the boost header files and the Eigen library 
for matrix algebra are available to the Emscripten toolchain. The easiest way 
to do this is to install the `libeigen3-dev` and `libboost-dev` system 
packages, and make them available to the Emscripten toolchain. Only header 
files from these packages are used here.

The main build scripts are:

1. `build-prequisites-wasm.sh`, which re-installs the Emscripten SDK, 
   downloads libsndfile and its dependencies, and builds libsndfile. You 
   should only have to run this once.
   
2. `build-wasm.sh`, which updates submodules, builds Csound for WASM, builds 
   CsoundAC for WASM, and creates a release package that also includes 
   examples, Csound instrument definitions, miscellaneous JavaScript files, 
   and so on.



