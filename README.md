# csound-wasm

Michae Gogins, Edward Costello, Steven Yi, Henri Manson<br>
https://github.com/gogins<br>
http://michaelgogins.tumblr.com

## Important Note

In order for Csound to use audio, the microphone, and MIDI, the user must 
grant these permissions for the site hosting Csound to the Web browser 
(usually by right-clicking on the lock symbol to the left of the URL, or 
on a permissions icon to the left of the URL).

## Introduction

THis directory builds and packages csound-wasm (Csound for WebAssembly), 
which includes my WebAssembly build of [Csound](https://github.com/csound/csound) 
and a WebAssembly build of my [Csound Algorithmic Composition](https://github.com/gogins/csound-ac) 
library.

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
  implemented in CsoundOboe by `csound_oboe.hpp` for the Csound for Android
  app, and by `csound.node`.

* Additional Csound API methods exposed to JavaScript.

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

If you are simply going to _use_ csound-wasm, download the binary release 
hosted here. If you are going to _build_ csound-wasm, for example in order to 
debug or contribute to it, follow these instructions.

You will need to make sure that the boost header files and the Eigen library 
for matrix algebra are available to the Emscripten toolchain. The easiest way 
to do this is to install the `libeigen3-dev` and `libboost-dev` system 
packages, and make them available to the Emscripten toolchain. Only header 
files from these packages are used here.

The main build scripts are:

1. `build-prequisites-wasm.sh`, which re-installs the Emscripten SDK, 
   downloads libsndfile and its dependencies, and builds libsndfile. This step 
   is quite time-consuming, but you should only have to run it once. 
   
2. `build-wasm.sh`, which updates submodules, builds Csound for WASM, builds 
   CsoundAC for WASM, and creates a release package that also includes 
   examples, Csound instrument definitions, miscellaneous JavaScript files, 
   and so on.

## Release Notes

### [v0.3.3](https://github.com/gogins/csound-wasm/commits/v0.3.2)

 - Added a `CsoundAudioNode.GetFileData` method to enable users to get data 
 from ta file in the memory filesystem of the AudioWorkletGlobalScope, e.g. a 
 soundfile record by Csound's `fout` opcode, into the browser's JavaScript 
 context, where it can be downloaded.

 - Updated Csound.

### [v0.3.2](https://github.com/gogins/csound-wasm/commits/v0.3.2)

 - Updated Csound to version 6.19.0.

 - Improved the WebAssembly builds of Csound and CsoundAC to support 
   running either in NW.js with native Csound, or in Web browsers with 
   Csound for WebAssembly. This makes it possible, e.g., to compose pieces 
   using Strudel with native Csound, VST plugins, access to the local 
   filesystem, and so on.

