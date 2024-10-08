
#    Copyright (C) 2018 Michael Gogins#
#
#    This file is part of csound-extended.
#
#    The Csound Library is free software; you can redistribute it
#    and/or modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    Csound is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with Csound; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
#    02111-1307 USA
#
#    As a special exception, if other files instantiate templates or
#    use macros or inline functions from this file, this file does not
#    by itself cause the resulting executable or library to be covered
#    by the GNU Lesser General Public License. This exception does not
#    however invalidate any other reasons why the library or executable
#    file might be covered by the GNU Lesser General Public License.

cmake_minimum_required(VERSION 3.7)
project(CsoundAC)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER "emcc")
set(CMAKE_CXX_COMPILER "em++")
set(CSOUNDAC_SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/dependencies/csound-ac/CsoundAC")
set(CPPSOUND_SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/dependencies/csound/interfaces")
set(CSOUND_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/dependencies/csound/include")
set(PORTSMF_DIRECTORY "${CMAKE_SOURCE_DIR}/dependencies/csound-ac/dependencies/portsmf")
message("PORTSMF_DIRECTORY: ${PORTSMF_DIRECTORY}")
## set(SNDFILE_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/deps/libsndfile-1.0.25/src")
# ${CSOUNDAC_SOURCE_DIRECTORY}/ImageToScore.cpp
add_library(csoundac-static 
${CPPSOUND_SOURCE_DIRECTORY}/CppSound.cpp
${CPPSOUND_SOURCE_DIRECTORY}/CsoundFile.cpp
${CPPSOUND_SOURCE_DIRECTORY}/cs_glue.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Cell.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/ChordLindenmayer.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/ChordSpace.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Composition.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Conversions.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Counterpoint.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/CounterpointNode.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Event.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Lindenmayer.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/MCRM.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Midifile.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Node.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Random.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Rescale.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Score.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/ScoreModel.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/ScoreNode.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Sequence.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Shell.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Soundfile.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/StrangeAttractor.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/System.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/Voicelead.cpp
${CSOUNDAC_SOURCE_DIRECTORY}/VoiceleadingNode.cpp
${PORTSMF_DIRECTORY}/allegro.cpp
${PORTSMF_DIRECTORY}/allegrord.cpp
${PORTSMF_DIRECTORY}/allegroserial.cpp
${PORTSMF_DIRECTORY}/allegrosmfrd.cpp
${PORTSMF_DIRECTORY}/allegrosmfwr.cpp
${PORTSMF_DIRECTORY}/allegrowr.cpp
${PORTSMF_DIRECTORY}/mfmidi.cpp
${PORTSMF_DIRECTORY}/strparse.cpp
)
target_include_directories(csoundac-static PRIVATE ${CSOUNDAC_SOURCE_DIRECTORY} ${CPPSOUND_SOURCE_DIRECTORY} ${CSOUND_INCLUDE_DIRECTORY} ${CSOUND_INCLUDE_DIRECTORY}/../H ${Eigen_INCLUDE_DIR} ${PORTSMF_DIRECTORY} ${SNDFILE_INCLUDE_DIRECTORY})
set(EMCC_LINKER_FLAGS "-s LINKABLE=1 -s SAFE_HEAP=1 --bind -s WASM=1 -O2 -o ../index.js -s ASSERTIONS=1 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 -s INVOKE_RUN=0 -std=c++17 --bind")
set(CMAKE_REQUIRED_FLAGS "${EMCC_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${EMCC_LINKER_FLAGS}")

