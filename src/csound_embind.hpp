/*

    Copyright (C) 2017 Michael Gogins

    The Csound Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Csound is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Csound; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA

    As a special exception, if other files instantiate templates or
    use macros or inline functions from this file, this file does not
    by itself cause the resulting executable or library to be covered
    by the GNU Lesser General Public License. This exception does not
    however invalidate any other reasons why the library or executable
    file might be covered by the GNU Lesser General Public License.
*/

#ifndef __CSOUND_EMBIND_HPP__
#define __CSOUND_EMBIND_HPP__

#if defined(__GNUC__)
#if __cplusplus <= 199711L
  #error To use csound_webaudio.hpp you need at least a C++11 compliant compiler.
#endif
#endif
#include "float-version.h"
#include <CsoundAC/csound_threaded.hpp>
#include <cstdio>
#include <deque>
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

extern "C" {
    int init_static_modules(CSOUND *csound);     
};

using namespace emscripten;

static emscripten::val csound_message_callback = val::undefined();

void csoundMessageCallback_(CSOUND *csound__, int attr, const char *format, va_list valist) {
    char buffer[0x1000];
    std::vsprintf(buffer, format, valist);
    if (csound_message_callback.isUndefined() || csound_message_callback.isNull()) {
        emscripten_console_log(buffer);
    } else {
        std::string string_buffer(buffer);
        emscripten::val val_buffer(string_buffer);
        csound_message_callback(val_buffer);
    }
}

struct MidiData {
    unsigned char status;
    unsigned char data1;
    unsigned char data2;
    unsigned char dummy;
};

static const int bytes_per_channel_message[8] = { 2, 2, 2, 2, 1, 1, 2, 0 };

/**
 * Provides a subset of the Csound C++ interface declared and defined in 
 * csound.hpp to the JavaScript context of Web browsers and other environments 
 * that support WebAssembly. Real-time audio is implemented using Web Audio. 
 * The semantics and run-time sequencing of this interface is almost identical 
 * to that of the corresponding subset of the Csound class.
 *
 * Member functions that take string parameters must, for Embind, take 
 * std::string instead of const char *. Please keep methods in alphabetical 
 * order by name.
 */
class CsoundEmbind : public CsoundThreaded {
    std::deque<MidiData> midi_data_queue;
    bool midi_in_is_open = false;
public:
    virtual ~CsoundEmbind() {};
    CsoundEmbind() {
        Csound::SetHostData(this);
        csoundSetDefaultMessageCallback(csoundMessageCallback_);
    };
    virtual int32_t Cleanup() {
        return CsoundThreaded::Cleanup();
    }
    virtual int CompileCsd(const std::string &filename) {
        int result = 0;
#if defined(INIT_STATIC_MODULES) && INIT_STATIC_MODULES == 1
        result |= init_static_modules(csound);
#endif
        result |= CsoundThreaded::CompileCsd(filename.c_str());
        return result;
    }
    virtual int CompileCsdText(const std::string &csd) {
        int result = 0;
#if defined(INIT_STATIC_MODULES) && INIT_STATIC_MODULES == 1
        result |= init_static_modules(csound);
#endif
        result |= CsoundThreaded::CompileCsdText(csd.c_str());
        return result;
    }
    virtual int CompileOrc(const std::string &orc) {
        int result = 0;
#if defined(INIT_STATIC_MODULES) && INIT_STATIC_MODULES == 1
        result |= init_static_modules(csound);
#endif
        result |= Csound::CompileOrc(orc.c_str());
        return result;
    }
    virtual MYFLT EvalCode(const std::string &orc) {
        return Csound::EvalCode(orc.c_str());
    }
    virtual MYFLT GetControlChannel(const std::string &name) {
        int dummy_error = 0;
        return Csound::GetControlChannel(name.c_str(), &dummy_error);
    }
    virtual std::string GetEnv(const std::string &name) {
        return GetEnv(name.c_str());
    }
    virtual std::string GetInputName_() {
        const char *value = nullptr;    
#if defined(CSOUND_VERSION_MAJOR) && CSOUND_VERSION_MAJOR >= 7      
     auto params = GetParams();
        if (params == nullptr) {
            return "";
        }
        value = params->infilename;
        if (value == nullptr) {
            return "";
        }
#else
        value = GetInputName();
        if (value == nullptr) {
            return "";
        }
#endif
        return value;;
    }
    virtual int32_t GetKsmps()
    {
        return Csound::GetKsmps();
    }
    virtual int32_t GetNchnls() {
        return CsoundThreaded::GetNchnls();
    }
    virtual int32_t GetNchnlsInput() {
        return CsoundThreaded::GetNchnlsInput();
    }
    virtual MYFLT Get0dBFS() {
        return CsoundThreaded::Get0dBFS();
    }
    virtual int GetAPIVersion() {
#if defined(CSOUND_VERSION_MAJOR) && CSOUND_VERSION_MAJOR >= 7
        return Csound::GetVersion();
#else
        return Csound::GetAPIVersion();
#endif
    }
    virtual long GetCurrentTimeSamples() {
        return Csound::GetCurrentTimeSamples();
    }
    virtual std::string GetOutputName_() {
        const char *value = nullptr;    
#if defined(CSOUND_VERSION_MAJOR) && CSOUND_VERSION_MAJOR >= 7      
     auto params = GetParams();
        if (params == nullptr) {
            return "";
        }
        value = params->outfilename;
        if (value == nullptr) {
            return "";
        }
#else
        value = GetOutputName();
        if (value == nullptr) {
            return "";
        }
#endif
        return value;;
    }
    virtual MYFLT GetScoreOffsetSeconds() {
        return Csound::GetScoreOffsetSeconds();
    }
    virtual MYFLT GetScoreTime() {
        return Csound::GetScoreTime();
    }
    virtual MYFLT GetSr() {
        return Csound::GetSr();
    }
    virtual val GetSpinView() {
        int count = GetKsmps() * GetNchnlsInput();
        return val(typed_memory_view(count, GetSpin()));
    }
    virtual val GetSpoutView() {
        int count = GetKsmps() * GetNchnls();
        return val(typed_memory_view(count, GetSpout()));
    }
    virtual std::string GetStringChannel(const std::string &name) {
        char buffer[0x200];
        Csound::GetStringChannel(name.c_str(), buffer);
        return buffer;
    }
    virtual int GetVersion() {
        return Csound::GetVersion();
    }
    virtual int IsScorePending() {
        return Csound::IsScorePending();
    }
    virtual void InputMessage(const std::string &sco) {
        CsoundThreaded::InputMessage(sco.c_str());
    }
    virtual int KillInstance(MYFLT p1, const std::string &insname_, int mode, bool release) {
#if defined(CSOUND_VERSION_MAJOR) && CSOUND_VERSION_MAJOR >= 7
          Message("CsoundEmbind::KillInstance() is not implemented in Csound 7. Use Csound code to do this. The signature of this method is retained for API compatibility.\n"); 
          return 0;
#else  
        // Actually we ignore all string instrument names. The signature 
        // is retained for API compatibility.
        auto result = csoundKillInstance(GetCsound(), p1, nullptr, mode, release);
        return result;
#endif
    }
    virtual void Message(const std::string &message) {
        Csound::Message(message.c_str());
    }
    virtual int32_t Perform() {
        return CsoundThreaded::Perform();
    }
    virtual int32_t PerformKsmps() {
        return Csound::PerformKsmps();
    }
    virtual int ReadScore(const std::string &sco) {
        return CsoundThreaded::ReadScore(sco.c_str());
    }    
    virtual void Reset() {
        CsoundThreaded::Reset();
    }
    virtual void RewindScore() {
        Csound::RewindScore();
    }
    virtual void SetChannel(const std::string &name, MYFLT value) {
        return Csound::SetChannel(name.c_str(), value);
    }
    virtual void SetStringChannel(const std::string &name, const std::string &value) {
        Csound::SetStringChannel(name.c_str(), (char *)value.c_str());
    }
    virtual int SetInput(const std::string &input) {
        return CsoundThreaded::SetInput(input.c_str());
    }
    virtual void SetMessageCallback(const emscripten::val &csound_message_callback_) {
        csound_message_callback = csound_message_callback_;
    }
    virtual int SetOption(const std::string &value) {
        return Csound::SetOption(value.c_str());
    }
    virtual int SetOutput(const std::string &output, const std::string &type_, const std::string &format) {
        return CsoundThreaded::SetOutput(output.c_str(), type_.c_str(), format.c_str());
    }
    virtual void SetScoreOffsetSeconds(MYFLT seconds) {
        Csound::SetScoreOffsetSeconds(seconds);
    }
    virtual void SetScorePending(bool is_pending) {
        Csound::SetScorePending(is_pending);
    }
    virtual int Start() {
        int result = 0;
        result |= Csound::Start();
        return result;
    }
    virtual void Stop() {
        CsoundThreaded::Stop();
    }
    virtual void SetHostImplementedAudioIO(int state) {
        CsoundThreaded::SetHostImplementedAudioIO(state);
    }
    virtual MYFLT TableGet(int table, int index) {
        return CsoundThreaded::TableGet(table, index);
    }
    virtual int TableLength(int table) {
        return Csound::TableLength(table);
    }
    virtual void TableSet(int table, int index, MYFLT value) {
        CsoundThreaded::TableSet(table, index, value);
    }
    int MidiInOpenCallback(CSOUND *, void **, const char *) {
        midi_data_queue.clear();
        midi_in_is_open = true;
        return 0;
    }
    static int MidiInOpenCallback_(CSOUND *csound_, void **user_data, const char *device_name) {
        void *host_data = csoundGetHostData(csound_);
        return static_cast<CsoundEmbind *>(host_data)->MidiInOpenCallback(csound_, user_data, device_name);
    }
    int MidiInCloseCallback(CSOUND *, void *) {
        midi_in_is_open = false;
        midi_data_queue.clear();
        return 0;
    }
    static int MidiInCloseCallback_(CSOUND *csound_, void *user_data) {
        void *host_data = csoundGetHostData(csound_);
        return static_cast<CsoundEmbind *>(host_data)->MidiInCloseCallback(csound_, user_data);
    }
    int MidiReadCallback(CSOUND *, void *, unsigned char *buffer, int bytes_to_read) {
        int bytes_read = 0;
        while (midi_data_queue.empty() == false) {
            MidiData midi_data = midi_data_queue.front();
            int st = (int) midi_data.status;
            int d1 = (int) midi_data.data1;
            int d2 = (int) midi_data.data2;
            if (st < 0x80)
                goto next;
            if (st >= 0xF0 &&
                    !(st == 0xF8 || st == 0xFA || st == 0xFB ||
                      st == 0xFC || st == 0xFF))
                goto next;
            bytes_to_read -= (bytes_per_channel_message[(st - 0x80) >> 4] + 1);
            if (bytes_to_read < 0) break;
            // Write to Csound's MIDI input buffer.
            bytes_read += (bytes_per_channel_message[(st - 0x80) >> 4] + 1);
            switch (bytes_per_channel_message[(st - 0x80) >> 4]) {
            case 0:
                *buffer++ = (unsigned char) st;
                break;
            case 1:
                *buffer++ = (unsigned char) st;
                *buffer++ = (unsigned char) d1;
                break;
            case 2:
                *buffer++ = (unsigned char) st;
                *buffer++ = (unsigned char) d1;
                *buffer++ = (unsigned char) d2;
                break;
            }
    next:
            midi_data_queue.pop_front();
        }
        return bytes_read;
    }
    static int MidiReadCallback_(CSOUND * csound_, void *user_data, unsigned char *buffer, int bytes_to_read) {
        void *host_data = csoundGetHostData(csound_);        
        return static_cast<CsoundEmbind *>(host_data)->MidiReadCallback(csound_, user_data, buffer, bytes_to_read);
    }
    virtual void InitializeHostMidi() {
        CsoundThreaded::SetHostImplementedMIDIIO(1);
        CsoundThreaded::SetExternalMidiInOpenCallback(&CsoundEmbind::MidiInOpenCallback_);
        CsoundThreaded::SetExternalMidiReadCallback(&CsoundEmbind::MidiReadCallback_);
        CsoundThreaded::SetExternalMidiInCloseCallback(&CsoundEmbind::MidiInCloseCallback_);
    }
    virtual void MidiEventIn(unsigned char status, unsigned char data1, unsigned char data2) {
        if (midi_in_is_open) {
            MidiData midi_data;
            midi_data.status = status;
            midi_data.data1 = data1;
            midi_data.data2 = data2;
            midi_data_queue.push_back(midi_data);
        }
    }
};

/** 
 * Mounts the current directory in the JavaScript sandbox 
 * to a "/csound" mountpoint in the node filesystem 
 * (i.e., the physical directory that node.js or NW.js is 
 * running in). Does not work in browsers!
 */
void nodefs_mount() {
  EM_ASM(
    FS.mkdir('/csound');
    FS.mount(NODEFS, { root: '.' }, '/csound');
  );
}

/**
 * For the sake of backwards compatibility, all method names are declared with 
 * both initial capitals and camel case. Please keep bindings in 
 * alphabetical order. If the method is new, the case is the same as in the 
 * existing C/C++ API.
 */
EMSCRIPTEN_BINDINGS(csound_web_audio) {  
    function("nodefs_mount", &nodefs_mount);
#if 0
    class_<Csound>("Csound")
        .function("Cleanup", &CsoundThreaded::Cleanup)
        .function("cleanup", &CsoundThreaded::Cleanup)
        .function("Get0dBFS", &Csound::Get0dBFS)
        .function("get0dBFS", &Csound::Get0dBFS)
#if defined(CSOUND_VERSION_MAJOR) && CSOUND_VERSION_MAJOR >= 7  
        .function("GetAPIVersion", &Csound::GetVersion)
        .function("getAPIVersion", &Csound::GetVersion)
#else
        .function("GetAPIVersion", &Csound::GetAPIVersion)
        .function("getAPIVersion", &Csound::GetAPIVersion)
#endif
        .function("GetCurrentTimeSamples", &Csound::GetCurrentTimeSamples)
        .function("getCurrentTimeSamples", &Csound::GetCurrentTimeSamples)
        .function("GetKsmps", &Csound::GetKsmps)
        .function("getKsmps", &Csound::GetKsmps)
        .function("GetNchnls", &CsoundThreaded::GetNchnls)
        .function("getNchnls", &CsoundThreaded::GetNchnls)
        .function("GetNchnlsInput", &CsoundThreaded::GetNchnlsInput)
        .function("getNchnlsInput", &CsoundThreaded::GetNchnlsInput)
        .function("getNchnlsInput", &CsoundThreaded::GetNchnlsInput)
        .function("GetScoreOffsetSeconds", &Csound::GetScoreOffsetSeconds)
        .function("getScoreOffsetSeconds", &Csound::GetScoreOffsetSeconds)
        .function("GetScoreTime", &Csound::GetScoreTime)
        .function("getScoreTime", &Csound::GetScoreTime)
        .function("GetSr", &Csound::GetSr)
        .function("getSr", &Csound::GetSr)
        .function("GetVersion", &Csound::GetVersion)
        .function("getVersion", &Csound::GetVersion)
        .function("IsScorePending", &Csound::IsScorePending)
        .function("isScorePending", &Csound::IsScorePending)
        .function("Perform", select_overload<int()>(&CsoundThreaded::Perform))
        .function("perform", select_overload<int()>(&CsoundThreaded::Perform))
        .function("PerformKsmps", select_overload<int()>(&Csound::PerformKsmps))
        .function("performKsmps", select_overload<int()>(&Csound::PerformKsmps))
        .function("Reset", &Csound::Reset)
        .function("reset", &Csound::Reset)
        .function("RewindScore", &Csound::RewindScore)
        .function("rewindScore", &Csound::RewindScore)
        .function("SetHostImplementedAudioIO", &CsoundThreaded::SetHostImplementedAudioIO)
        .function("setHostImplementedAudioIO", &CsoundThreaded::SetHostImplementedAudioIO)
        .function("SetScoreOffsetSeconds", &Csound::SetScoreOffsetSeconds)
        .function("setScoreOffsetSeconds", &Csound::SetScoreOffsetSeconds)
        .function("SetScorePending", &Csound::SetScorePending)
        .function("setScorePending", &Csound::SetScorePending)
        .function("Start", &Csound::Start)
        .function("start", &Csound::Start)
        .function("Stop", &CsoundThreaded::Stop)
        .function("stop", &CsoundThreaded::Stop)
        .function("TableGet", &CsoundThreaded::TableGet)
        .function("tableGet", &CsoundThreaded::TableGet)
        .function("TableLength", &Csound::TableLength)
        .function("tableLength", &Csound::TableLength)
        .function("TableSet", &CsoundThreaded::TableSet)
        .function("tableSet", &CsoundThreaded::TableSet)
        ;
#endif
    class_<CsoundEmbind>("CsoundEmbind")
        .constructor<>()
        .function("Cleanup", &CsoundEmbind::Cleanup )
        .function("cleanup", &CsoundEmbind::Cleanup)
        .function("CompileCsd", &CsoundEmbind::CompileCsd)
        .function("compileCsd", &CsoundEmbind::CompileCsd)
        .function("CompileCsdText", &CsoundEmbind::CompileCsdText)
        .function("compileCsdText", &CsoundEmbind::CompileCsdText)
        .function("CompileOrc", &CsoundEmbind::CompileOrc)
        .function("compileOrc", &CsoundEmbind::CompileOrc)
        .function("EvalCode", &CsoundEmbind::EvalCode)
        .function("evalCode", &CsoundEmbind::EvalCode)
        .function("GetAPIVersion", &CsoundEmbind::GetAPIVersion)
        .function("getAPIVersion", &CsoundEmbind::GetAPIVersion)
        .function("GetChannel", &CsoundEmbind::GetControlChannel)
        .function("getChannel", &CsoundEmbind::GetControlChannel)
        .function("GetControlChannel", &CsoundEmbind::GetControlChannel)
        .function("getControlChannel", &CsoundEmbind::GetControlChannel)
        .function("GetCurrentTimeSamples", &CsoundEmbind::GetCurrentTimeSamples)
        .function("getCurrentTimeSamples", &CsoundEmbind::GetCurrentTimeSamples)
        .function("GetEnv", &CsoundEmbind::GetEnv)
        .function("getEnv", &CsoundEmbind::GetEnv)
        .function("GetInputName", &CsoundEmbind::GetInputName_)
        .function("getInputName", &CsoundEmbind::GetInputName_)
        .function("Get0dBFS", &CsoundEmbind::Get0dBFS)
        .function("get0dBFS", &CsoundEmbind::Get0dBFS)
        .function("GetOutputName", &CsoundEmbind::GetOutputName_)
        .function("getOutputName", &CsoundEmbind::GetOutputName_)
        .function("GetScoreOffsetSeconds", &CsoundEmbind::GetScoreOffsetSeconds)
        .function("getScoreOffsetSeconds", &CsoundEmbind::GetScoreOffsetSeconds)
        .function("GetScoreTime", &CsoundEmbind::GetScoreTime)
        .function("getScoreTime", &CsoundEmbind::GetScoreTime)
        .function("GetSr", &CsoundEmbind::GetSr)
        .function("getSr", &CsoundEmbind::GetSr)
        .function("GetSpin", &CsoundEmbind::GetSpinView)
        .function("GetSpout", &CsoundEmbind::GetSpoutView)
        .function("GetKsmps", &CsoundEmbind::GetKsmps)
        .function("getKsmps", &CsoundEmbind::GetKsmps)
        .function("GetNchnls", &CsoundEmbind::GetNchnls)
        .function("getNchnls", &CsoundEmbind::GetNchnls)
        .function("GetNchnlsInput", &CsoundEmbind::GetNchnlsInput)
        .function("getNchnlsInput", &CsoundEmbind::GetNchnlsInput)
        .function("GetStringChannel", &CsoundEmbind::GetStringChannel)
        .function("getStringChannel", &CsoundEmbind::GetStringChannel)
        .function("GetVersion", &CsoundEmbind::GetVersion)
        .function("getVersion", &CsoundEmbind::GetVersion)
        .function("InitializeHostMidi", &CsoundEmbind::InitializeHostMidi)
        .function("IsScorePending", &CsoundEmbind::IsScorePending)
        .function("isScorePending", &CsoundEmbind::IsScorePending)
        .function("InputMessage", &CsoundEmbind::InputMessage)
        .function("inputMessage", &CsoundEmbind::InputMessage)
        .function("Message", &CsoundEmbind::Message)
        .function("KillInstance", &CsoundEmbind::KillInstance)
        .function("killInstance", &CsoundEmbind::KillInstance)
        .function("message", &CsoundEmbind::Message)
        .function("MidiEventIn", &CsoundEmbind::MidiEventIn)
        .function("Perform", &CsoundEmbind::Perform)
        .function("perform", &CsoundEmbind::Perform)
        .function("PerformKsmps", &CsoundEmbind::PerformKsmps)
        .function("performKsmps", &CsoundEmbind::PerformKsmps)
        .function("ReadScore", &CsoundEmbind::ReadScore)
        .function("readScore", &CsoundEmbind::ReadScore)
        .function("Reset", &CsoundEmbind::Reset)
        .function("reset", &CsoundEmbind::Reset)
        .function("RewindScore", &CsoundEmbind::RewindScore)
        .function("rewindScore", &CsoundEmbind::RewindScore)
        .function("SetChannel", &CsoundEmbind::SetChannel)
        .function("setChannel", &CsoundEmbind::SetChannel)
        .function("SetControlChannel", &CsoundEmbind::SetChannel)
        .function("setControlChannel", &CsoundEmbind::SetChannel)
        .function("SetHostImplementedAudioIO", &CsoundEmbind::SetHostImplementedAudioIO)
        .function("setHostImplementedAudioIO", &CsoundEmbind::SetHostImplementedAudioIO)
        .function("SetInput", &CsoundEmbind::SetInput)
        .function("setInput", &CsoundEmbind::SetInput)
        .function("SetMessageCallback", &CsoundEmbind::SetMessageCallback)
        .function("setMessageCallback", &CsoundEmbind::SetMessageCallback)
        .function("SetOption", &CsoundEmbind::SetOption)
        .function("setOption", &CsoundEmbind::SetOption)
        .function("SetOutput", &CsoundEmbind::SetOutput)
        .function("setOutput", &CsoundEmbind::SetOutput)
        .function("SetScoreOffsetSeconds", &CsoundEmbind::SetScoreOffsetSeconds)
        .function("setScoreOffsetSeconds", &CsoundEmbind::SetScoreOffsetSeconds)
        .function("SetScorePending", &CsoundEmbind::SetScorePending)
        .function("setScorePending", &CsoundEmbind::SetScorePending)
        .function("SetStringChannel", &CsoundEmbind::SetStringChannel)
        .function("setStringChannel", &CsoundEmbind::SetStringChannel)
        .function("Start", &CsoundEmbind::Start)
        .function("start", &CsoundEmbind::Start)
        .function("Stop", &CsoundEmbind::Stop)
        .function("stop", &CsoundEmbind::Stop)
        .function("TableGet", &CsoundEmbind::TableGet)
        .function("tableGet", &CsoundEmbind::TableGet)
        .function("TableLength", &CsoundEmbind::TableLength)
        .function("tableLength", &CsoundEmbind::TableLength)
        .function("TableSet", &CsoundEmbind::TableSet)
        .function("tableSet", &CsoundEmbind::TableSet)
        ;
}

#endif  // __CSOUND_EMBIND_HPP__
