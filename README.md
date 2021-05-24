# csound-extended-wasm

Edward Costello, Steven Yi, Henri Manson, Michael Gogins<br>
https://github.com/gogins<br>
http://michaelgogins.tumblr.com

## Introduction

THis directory builds and packages csound-extended for WebAssembly.
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

See https://github.com/gogins/csound-extended/commits/develop for the commit log.

## Installation

Download the latest version of `csound-extended-wasm-{version.zip}` from the 
[release page here](https://github.com/gogins/csound-extended/releases). Unzip 
it and it is ready to use.

## Examples

There is a local Web server and a minimal working example in the release zip 
file. In the directory where you have unzipped the release, execute:

```
python2 httpd.py
```

Then navigate to `http://localhost:5103` and view `minimal.html`. Click on 
the Play button to validate your installation.

Some of the examples herein will run from 
https://gogins.github.io/csound-examples. For more information, see my 
[csound-examples](https://github.com/gogins/csound-examples) repository.

## Building

You will need to make sure that the Eigen library for matrix algebra is 
available to the Emscripten toolchain. The easiest way to do this is to 
install the `libeigen3-dev` system package, and make it available to the 
Emscripten toolchain. You can create symbolic link to do this, e.g.
```
/home/mkg/emsdk/upstream/emscripten/cache/sysroot/include/eigen3 -> /usr/include/eigen3
```

Similarly there is a problem with the `csound/include/float-version.h` file. 
Copy `csoound/include/float-version.h.in` to `csound/include/float-version.h`, 
and in that file, change 
```
#cmakedefine USE_DOUBLE
```
to
```
#define USE_DOUBLE
```

