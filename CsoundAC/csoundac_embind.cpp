/*
    Copyright (C) 2018 Michael Gogins

    This file is part of csound-extended.

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
#include <cstdio>
#include <ctime>
#include <Eigen/Dense>
#include <emscripten/bind.h>
#include <fstream>
#include <iostream>
#include <CsoundAC/ChordSpace.hpp>
#include <CsoundAC/Silence.hpp>
#include <string>
#include <sstream>

static std::string hello() {
    static std::string hello_ = "Hello, World, this is CsoundAC!\n";
    csound::System::message(hello_.c_str());
    return hello_;
}

/*
The objective here is not completeness, but usefulness. As much must be 
exposed as is needed for freely composing with CsoundAC. The situation 
is complicated by the fact that we do not use MusicModel with its 
built-in C++ interface to Csound, but defer to the WebAssembly build of 
Csound, which means we use ScoreModel rather than MusicModel to compose.

Filling out the Embind definitions is a big job that needs to be kept in order 
and to enable keeping track of work completed. This is done by following the 
order in the Doxygen documentation for the `csound` namespace by structure 
name and method name, and marking finished classes with a // FINISHED comment.
In other words, classes and their methods are more or less in alphabetical 
order. Other functions, constants, and such should however put in, in 
declaration order, just above the class that appears in the same header file.

The order of definitions does not matter when compiling, so just keep these 
in the order described above. 

In place of overloads use distinguished names, but for the Embind name, use 
the name that would go to the most commonly used overload.
*/

/**
 * Basic matrix arithmetic for CsoundAC notes and transformation matrices,
 * in code that accesses CsoundAC through JavaScript.
 */
static Eigen::MatrixXd zeros(int dimensions) {
    return Eigen::MatrixXd::Zero(dimensions, dimensions);
}

static Eigen::MatrixXd identity(int dimensions) {
    return Eigen::MatrixXd::Identity(dimensions, dimensions);
}

static Eigen::MatrixXd add(const Eigen::MatrixXd &m, double s) {
    return (m.array() + s);
}

static Eigen::MatrixXd subtract(const Eigen::MatrixXd &m, double s) {
    return (m.array() - s);
}

static Eigen::MatrixXd scalar_multiply(const Eigen::MatrixXd &m, double s) {
    return (m.array() * s);
}

static Eigen::MatrixXd scalar_divide(const Eigen::MatrixXd &m, double s) {
    return (m.array() / s);
}

static double norm(const Eigen::MatrixXd &m) {
    return m.norm();
}

static Eigen::MatrixXd matrix_multiply(const Eigen::MatrixXd &a, const Eigen::MatrixXd &b) {
    return (a * b);
}

static Eigen::MatrixXd create_rotation(int from_axis, int to_axis, double radians) {
    Eigen::MatrixXd rotation = Eigen::MatrixXd::Identity(12, 12);
    rotation(from_axis, from_axis) = std::cos(radians);
    rotation(from_axis, to_axis) = -std::sin(radians);
    rotation(to_axis, from_axis) = std::sin(radians);
    rotation(to_axis, to_axis) = std::cos(radians);
    return rotation;
}

EMSCRIPTEN_BINDINGS(csoundac) {  
    emscripten::function("create", &zeros);
    emscripten::function("identity", &identity);
    emscripten::function("add", &add);
    emscripten::function("subtract", &subtract);
    emscripten::function("scalar_multiply", &scalar_multiply);
    emscripten::function("scalar_divide", &scalar_divide);
    emscripten::function("matrix_multiply", &matrix_multiply);
    emscripten::function("norm", &norm);
    emscripten::function("create_rotation", &create_rotation);
    
    emscripten::register_map<std::string,std::string>("StringToStringMap");
    emscripten::register_vector<double>("DoubleVector");
    emscripten::register_vector<int>("IntVector");
    emscripten::register_vector<csound::Chord>("ChordVector");
    emscripten::register_vector<csound::Scale>("ScaleVector");
    emscripten::register_vector<csound::Event>("EventVector");
    emscripten::register_vector<std::string>("StringVector");
     // FINISHED
    emscripten::class_<csound::Cell, emscripten::base<csound::ScoreNode> >("Cell")
        .constructor<>()
        .property("durationSeconds", &csound::Cell::getDurationSeconds, &csound::Cell::setDurationSeconds)
        .property("importFilename", &csound::Cell::getImportFilename, &csound::Cell::setImportFilename)
        .property("relativeDuration", &csound::Cell::getRelativeDuration, &csound::Cell::setRelativeDuration)
        .property("repeatCount", &csound::Cell::getRepeatCount, &csound::Cell::setRepeatCount)
    ;
    // FINISHED
    emscripten::class_<csound::CellAdd, emscripten::base<csound::Node> >("CellAdd")
        .constructor<>()
        .function("add", &csound::CellAdd::add)
    ;
    // FINISHED
    emscripten::class_<csound::CellChord, emscripten::base<csound::Node> >("CellChord")
        .constructor<>()
        .function("chord", &csound::CellChord::chord)
    ;
    // FINISHED
    emscripten::class_<csound::CellMultiply, emscripten::base<csound::Node> >("CellMultiply")
        .constructor<>()
        .function("multiply", &csound::CellChord::chord)
    ;
    // FINISHED
    emscripten::class_<csound::CellRandom, emscripten::base<csound::Random> >("CellRandom")
        .constructor<>()
        .function("random", &csound::CellRandom::random)
    ;
    // FINISHED
    emscripten::class_<csound::CellReflect, emscripten::base<csound::Node> >("CellReflect")
        .constructor<>()
        .function("reflect", &csound::CellReflect::reflect)
    ;
    // FINISHED
    emscripten::class_<csound::CellRemove, emscripten::base<csound::Node> >("CellRemove")
        .constructor<>()
        .function("remove", &csound::CellRemove::remove)
    ;
    // FINISHED
    emscripten::class_<csound::CellRepeat, emscripten::base<csound::Node> >("CellRepeat")
        .constructor<>()
        .function("repeat", &csound::CellRepeat::repeat)
    ;
    // FINISHED
    emscripten::class_<csound::CellSelect, emscripten::base<csound::Node> >("CellSelect")
        .constructor<>()
        .function("select", &csound::CellSelect::select)
    ;
    // FINISHED
    emscripten::class_<csound::CellShuffle, emscripten::base<csound::Random> >("CellShuffle")
        .constructor<>()
        .function("shuffle", &csound::CellShuffle::shuffle)
    ;
    // NOT SUPPORTED
    //~ // Can't seem to do scoped enums.
    //~ emscripten::enum_<csound::Chord::CHORD_DIMENSION>("CHORD_DIMENSION")
        //~ .value("PITCH", CHORD_DIMENSION::PITCH)
        //~ .value("DURATION", CHORD_DIMENSION::DURATION)
        //~ .value("LOUDNESS", CHORD_DIMENSION::LOUDNESS)
        //~ .value("INSTRUMENT", CHORD_DIMENSION::INSTRUMENT)
        //~ .value("PAN", CHORD_DIMENSION::PAN)
        //~ .value("COUNT", CHORD_DIMENSION::COUNT)
    //~ ;
    emscripten::function("allOfEquivalenceClass", &csound::allOfEquivalenceClass);
    emscripten::function("chord_space_version", &csound::chord_space_version);
    emscripten::function("euclidean", &csound::euclidean);
    emscripten::function("factorial", &csound::factorial);
    emscripten::function("midpoint", &csound::midpoint);
    emscripten::function("eq_tolerance", &csound::eq_tolerance);
    emscripten::function("gt_tolerance", &csound::gt_tolerance);
    emscripten::function("lt_tolerance", &csound::lt_tolerance);
    emscripten::function("ge_tolerance", &csound::ge_tolerance);
    emscripten::function("le_tolerance", &csound::le_tolerance);
    emscripten::function("OCTAVE", &csound::OCTAVE);
    emscripten::function("MIDDLE_C", &csound::MIDDLE_C);
    emscripten::function("C4", &csound::C4);
    emscripten::function("T", &csound::T);
    emscripten::function("I", &csound::I);
    emscripten::function("modulo", &csound::modulo);
    emscripten::function("epc", &csound::epc);
    emscripten::function("pitchClassForName", &csound::pitchClassForName);
    emscripten::function("nameForPitchClass", &csound::nameForPitchClass);
    emscripten::function("nameForChord", &csound::nameForChord);
    emscripten::function("chordForName", &csound::chordForName);
    emscripten::function("nameForScale", &csound::nameForScale);
    emscripten::function("scaleForName", &csound::scaleForName);
    emscripten::function("add_chord", &csound::add_chord);
    emscripten::function("add_scale", &csound::add_scale);
    emscripten::function("iterator", &csound::iterator);
    emscripten::function("next", &csound::next);
    emscripten::function("note", &csound::note);
    emscripten::function("notes", &csound::notes);
    emscripten::function("numerics_information", &csound::numerics_information);
    emscripten::function("operator==", &csound::operator==);
    emscripten::function("chord_lt", emscripten::select_overload<bool(const csound::Chord&,const csound::Chord&)>(&csound::operator<));
    emscripten::function("scale_lt", emscripten::select_overload<bool(const csound::Scale&,const csound::Scale&)>(&csound::operator<));
    emscripten::function("harmony_point_2_lt", emscripten::select_overload<bool(const csound::HarmonyPoint2&,const csound::HarmonyPoint2&)>(&csound::operator<));
    emscripten::function("operator<=", &csound::operator<=);
    emscripten::function("operator>", &csound::operator>);
    emscripten::function("operator>=", &csound::operator>=);
    emscripten::function("voiceleading", &csound::voiceleading);
    emscripten::function("parallelFifth", &csound::parallelFifth);
    emscripten::function("toScore", &csound::toScore);
    emscripten::function("voiceleadingSmoothness", &csound::voiceleadingSmoothness);
    emscripten::function("voiceleadingSmoother", &csound::voiceleadingSmoother);
    emscripten::function("voiceleadingSimpler", &csound::voiceleadingSimpler);
    emscripten::function("voiceleadingCloser", &csound::voiceleadingCloser);
    emscripten::function("voiceleadingClosestRange", &csound::voiceleadingClosestRange);
    emscripten::function("closestPitch", &csound::closestPitch);
    emscripten::function("conformToPitchClassSet", &csound::conformToPitchClassSet);
    emscripten::function("conformToChord", &csound::conformToChord, emscripten::allow_raw_pointers());
    emscripten::function("conformToChord_equivalence", &csound::conformToChord_equivalence, emscripten::allow_raw_pointers());
    emscripten::function("insert", emscripten::select_overload<void(csound::Score&,const csound::Chord&, double)>(&csound::insert));
    emscripten::function("slice", &csound::slice);
    emscripten::function("apply", &csound::apply);
    emscripten::function("gather", &csound::gather);
    emscripten::function("octavewiseRevoicings", &csound::octavewiseRevoicings);
    emscripten::function("octavewiseRevoicing", &csound::octavewiseRevoicing);
    emscripten::function("indexForOctavewiseRevoicing", emscripten::select_overload<int(const csound::Chord&,const csound::Chord&, double)>(&csound::indexForOctavewiseRevoicing));
    emscripten::function("scale", &csound::scale);
    emscripten::function("chord", &csound::chord);
    emscripten::function("transpose_degrees", &csound::transpose_degrees);
    emscripten::function("SET_CHORD_SPACE_DEBUGGING", &csound::SET_CHORD_SPACE_DEBUGGING);
    emscripten::function("SET_SCOPED_DEBUGGING", &csound::SET_SCOPED_DEBUGGING);
    /**
     * Basic matrix arithmetic for CsoundAC notes and transformation matrices,
     * in code that accesses CsoundAC through JavaScript; s is a scalar, m is a 
     * matrix.
     * m = identity(int dimensions)
     * m = add(m, s)
     * m = subtract(m, s)
     * m = scalar_multiply(m, s)
     * m = scalar_divide(m, s)
     * s = norm(m)
     * m = multiply(m, m)
     * m = create_rotation(dimension1, dimension2, radians)
     */
    emscripten::class_<csound::Chord>("Chord")
        .constructor<>()
        .constructor<csound::Chord>()
        // NOT SUPPORTED
        //.function("a", &csound::Chord::a)
        .function("ceiling", &csound::Chord::ceiling)
        .function("contains", &csound::Chord::contains)
        .function("count", &csound::Chord::count)
        .function("cycle", &csound::Chord::cycle)
        .function("distanceToOrigin", &csound::Chord::distanceToOrigin)
        .function("distanceToUnisonDiagonal", &csound::Chord::distanceToUnisonDiagonal)
        .function("eI", &csound::Chord::eI)
        .function("eO", &csound::Chord::eO)
        .function("eOP", &csound::Chord::eOP)
        .function("eOPI", &csound::Chord::eOPI)
        .function("eOPT", &csound::Chord::eOPT)
        .function("eOPTI", &csound::Chord::eOPTI)
        .function("eOPTT", &csound::Chord::eOPTT)
        .function("eOPTTI", &csound::Chord::eOPTTI)
        .function("eP", &csound::Chord::eP)
        .function("epcs", &csound::Chord::epcs)
        .function("equals", &csound::Chord::equals)
        .function("eR", &csound::Chord::eR)
        .function("eRP", &csound::Chord::eRP)
        .function("eRPI", &csound::Chord::eRPI)
        .function("eRPT", &csound::Chord::eRPT)
        .function("eRPTI", &csound::Chord::eRPTI)
        .function("eRPTT", &csound::Chord::eRPTT)
        .function("eRPTTI", &csound::Chord::eRPTTI)
        .function("et", &csound::Chord::et)
        .function("eT", &csound::Chord::eT)
        .function("eTT", &csound::Chord::eTT)
        .function("floor", &csound::Chord::floor)
        .function("fromString", &csound::Chord::fromString)
        .function("getDuration", &csound::Chord::getDuration)
        .function("getInstrument", &csound::Chord::getInstrument)
        .function("getLoudness", &csound::Chord::getLoudness)
        .function("getPan", &csound::Chord::getPan)
        .function("getPitch", &csound::Chord::getPitch)
        .function("greater", &csound::Chord::greater)
        .function("greater_equals", &csound::Chord::greater_equals)
        .function("I", &csound::Chord::I)
        .function("Iform", &csound::Chord::Iform)
        .function("information",&csound::Chord::information)
        .function("information_debug",&csound::Chord::information_debug)
        .function("information_sector",&csound::Chord::information_sector)
        .function("iseI", &csound::Chord::iseI)
        .function("iseI_chord", &csound::Chord::iseI_chord, emscripten::allow_raw_pointers())
        .function("iseOP", &csound::Chord::iseOP)
        .function("iseOPI", &csound::Chord::iseOPI)
        .function("iseOPT", &csound::Chord::iseOPT)
        .function("iseOPTI", &csound::Chord::iseOPTI)
        .function("iseOPTT", &csound::Chord::iseOPTT)
        .function("iseOPTTI", &csound::Chord::iseOPTTI)
        .function("iseP", &csound::Chord::iseP)
        .function("isepcs", &csound::Chord::isepcs)
        .function("iseR", &csound::Chord::iseR)
        .function("iseRP", &csound::Chord::iseRP)
        .function("iseRPI", &csound::Chord::iseRPI)
        .function("iseRPT", &csound::Chord::iseRPT)
        .function("iseRPTI", &csound::Chord::iseRPTI)
        .function("iseRPTT", &csound::Chord::iseRPTT)
        .function("iseRPTTI", &csound::Chord::iseRPTTI)
        .function("iset", &csound::Chord::iset)
        .function("iseT", &csound::Chord::iseT)
        .function("iseTT", &csound::Chord::iseTT)
        .function("K", &csound::Chord::K)
        .function("K_range", &csound::Chord::K_range)
        .function("layer", &csound::Chord::layer)
        .function("lesser", &csound::Chord::lesser)
        .function("lesser_equals", &csound::Chord::lesser_equals)
        .function("max", &csound::Chord::max)
        .function("center", &csound::Chord::center)
        .function("maximumInterval", &csound::Chord::maximumInterval)
        .function("min", &csound::Chord::min)
        .function("minimumInterval", &csound::Chord::minimumInterval)
        .function("move", &csound::Chord::move)
        .function("name", &csound::Chord::name)
        .function("normal_form", &csound::Chord::normal_form)
        .function("normal_order", &csound::Chord::normal_order)
        .function("nrD", &csound::Chord::nrD)
        .function("nrH", &csound::Chord::nrH)
        .function("nrL", &csound::Chord::nrL)
        .function("nrN", &csound::Chord::nrN)
        .function("nrP", &csound::Chord::nrP)
        .function("nrR", &csound::Chord::nrR)
        .function("nrS", &csound::Chord::nrS)
        .function("operator=", emscripten::select_overload<csound::Chord &(const csound::Chord &)>(&csound::Chord::operator=))
        ///.function("operator<", emscripten::select_overload<csound::Chord &(const csound::Chord &)>(&csound::Chord::operator<))
        .function("opt_domain_sectors", &csound::Chord::opt_domain_sectors)
        .function("opti_domain_sectors", &csound::Chord::opti_domain_sectors)
        .function("origin", &csound::Chord::origin)
        .function("permutations", &csound::Chord::permutations)
        .function("prime_form", &csound::Chord::prime_form)
        .function("Q", &csound::Chord::Q)
        .function("reflect", &csound::Chord::reflect)
        .function("resize", &csound::Chord::resize)
        .function("setDuration", &csound::Chord::setDuration)
        .function("setInstrument", &csound::Chord::setInstrument)
        .function("setLoudness", &csound::Chord::setLoudness)
        .function("setPan", &csound::Chord::setPan)
        .function("setPitch", &csound::Chord::setPitch)
        .function("T", &csound::Chord::T)
        .function("T_voiceleading", &csound::Chord::T_voiceleading)
        .function("Tform", &csound::Chord::Tform)
        .function("toString", &csound::Chord::toString)
        .function("v", &csound::Chord::v)
        .function("voiceleading", &csound::Chord::voiceleading)
        .function("voices", &csound::Chord::voices)
    ;
    // FINISHED
    emscripten::class_<csound::ChordLindenmayer, emscripten::base<csound::VoiceleadingNode> >("ChordLindenmayer")
        .constructor<>()
        .function("addRule", &csound::ChordLindenmayer::addRule)
        .function("clear", &csound::ChordLindenmayer::clear)
        .function("generateLocally", emscripten::select_overload<void()>(&csound::ChordLindenmayer::generateLocally))
        .function("getAngle", &csound::ChordLindenmayer::getAngle)
        .function("getAxiom", &csound::ChordLindenmayer::getAxiom)
        .function("getIterationCount", &csound::ChordLindenmayer::getIterationCount)
        .function("getReplacement", &csound::ChordLindenmayer::getReplacement)
        .function("getTurtleChord", &csound::ChordLindenmayer::getTurtleChord)
        .function("getTurtleModality", &csound::ChordLindenmayer::getTurtleModality)
        .function("getTurtleScale", &csound::ChordLindenmayer::getTurtleScale)
        .function("getTurtleScaleDegree", &csound::ChordLindenmayer::getTurtleScaleDegree)
        .function("initialize", &csound::ChordLindenmayer::initialize)
        .function("setAngle", &csound::ChordLindenmayer::setAngle)
        .function("setAxiom", &csound::ChordLindenmayer::setAxiom)
        .function("setIterationCount", &csound::ChordLindenmayer::setIterationCount)
        .function("setTurtleChord", &csound::ChordLindenmayer::setTurtleChord)
        .function("setTurtleModality", &csound::ChordLindenmayer::setTurtleModality)
        .function("setTurtleScale", &csound::ChordLindenmayer::setTurtleScale)
        .function("setTurtleScaleDegree", &csound::ChordLindenmayer::setTurtleScaleDegree)
    ;
    // FINISHED
    emscripten::class_<csound::PITV>("PITV")
        .constructor<>()
        .function("fromChord", &csound::PITV::fromChord)
        .function("initialize", &csound::PITV::initialize)
        .function("list", &csound::PITV::list)
        .function("preinitialize", &csound::PITV::preinitialize)
        .function("toChord", &csound::PITV::toChord)
        .property("countI", &csound::PITV::getCountI)
        .property("countP", &csound::PITV::getCountP)
        .property("countT", &csound::PITV::getCountT)
        .property("countV", &csound::PITV::getCountV)
        .property("g", &csound::PITV::getG)
        .property("N", &csound::PITV::getN)
        .property("range", &csound::PITV::getRange)
    ;
    // FINISHED
    emscripten::class_<csound::Composition>("Composition")
        .constructor<>()
        .function("clear", &csound::Composition::clear)
        .function("clearOutputSoundfileName", &csound::Composition::clearOutputSoundfileName)
        .function("generate", &csound::Composition::generate)
        .function("generateAllNames", &csound::Composition::generateAllNames)
        .class_function("generateFilename", &csound::Composition::generateFilename)
        .function("getAlbum", &csound::Composition::getAlbum)
        .function("getArtist", &csound::Composition::getArtist)
        .function("getAuthor", &csound::Composition::getAuthor)
        .function("getBasename", &csound::Composition::getBasename)
        .function("getCdSoundfileFilepath", &csound::Composition::getCdSoundfileFilepath)
        .function("getConformPitches", &csound::Composition::getConformPitches)
        .function("getCopyright", &csound::Composition::getCopyright)
        .function("getDuration", &csound::Composition::getDuration)
        .function("getFileFilepath", &csound::Composition::getFileFilepath)
        .function("getFilename", &csound::Composition::getFilename)
        .function("getFomusfileFilepath", &csound::Composition::getFomusfileFilepath)
        .function("getLicense", &csound::Composition::getLicense)
        .function("getLilypondfileFilepath", &csound::Composition::getLilypondfileFilepath)
        .function("getMidifileFilepath", &csound::Composition::getMidifileFilepath)
        .function("getMp3SoundfileFilepath", &csound::Composition::getMp3SoundfileFilepath)
        .function("getMusicXmlfileFilepath", &csound::Composition::getMusicXmlfileFilepath)
        .function("getNormalizedSoundfileFilepath", &csound::Composition::getNormalizedSoundfileFilepath)
        .function("getOutputDirectory", &csound::Composition::getOutputDirectory)
        .function("getOutputSoundfileFilepath", &csound::Composition::getOutputSoundfileFilepath)
        .function("getPerformanceRightsOrganization", &csound::Composition::getPerformanceRightsOrganization)
        .function("getScore", &csound::Composition::getScore)
        .function("getTieOverlappingNotes", &csound::Composition::getTieOverlappingNotes)
        .function("getTimestamp", &csound::Composition::getTimestamp)
        .function("getTitle", &csound::Composition::getTitle)
        .function("getTonesPerOctave", &csound::Composition::getTonesPerOctave)
        .function("getYear", &csound::Composition::getYear)
        .class_function("makeTimestamp", &csound::Composition::makeTimestamp)
        .function("normalizeOutputSoundfile", &csound::Composition::normalizeOutputSoundfile)
        .function("perform", &csound::Composition::perform)
        .function("performAll", &csound::Composition::performAll)
        .function("performMaster", &csound::Composition::performMaster)
        .function("performAll", &csound::Composition::performAll)
        .function("processArgs", &csound::Composition::processArgs)
        .function("processArgv", &csound::Composition::processArgv, emscripten::allow_raw_pointers())
        .function("render", &csound::Composition::render)
        .function("renderAll", &csound::Composition::renderAll)
        .function("setAlbum", &csound::Composition::setAlbum)
        .function("setArtist", &csound::Composition::setArtist)
        .function("setAuthor", &csound::Composition::setAuthor)
        .function("setConformPitches", &csound::Composition::setConformPitches)
        .function("setCopyright", &csound::Composition::setCopyright)
        .function("setDuration", &csound::Composition::setDuration)
        .function("setFilename", &csound::Composition::setFilename)
        .function("setLicense", &csound::Composition::setLicense)
        .function("setOutputDirectory", &csound::Composition::setOutputDirectory)
        .function("setOutputSoundfileName", &csound::Composition::setOutputSoundfileName)
        .function("setPerformanceRightsOrganization", &csound::Composition::setPerformanceRightsOrganization)
        .function("setScore", &csound::Composition::setScore)
        .function("setTieOverlappingNotes", &csound::Composition::setTieOverlappingNotes)
        .function("setTitle", &csound::Composition::setTitle)
        .function("setTonesPerOctave", &csound::Composition::setTonesPerOctave)
        .function("setYear", &csound::Composition::setYear)
        .function("tagFile", &csound::Composition::tagFile)
        .function("translateMaster", &csound::Composition::translateMaster)
        .function("translateToCdAudio", &csound::Composition::translateToCdAudio)
        .function("translateToMp3", &csound::Composition::translateToMp3)
        .function("translateToMp4", &csound::Composition::translateToMp4)
        .function("translateToNotation", &csound::Composition::translateToNotation)
        .function("write", &csound::Composition::write, emscripten::allow_raw_pointers())
    ;
    // FINISHED
    emscripten::class_<csound::Conversions>("Conversions")
        .constructor<>()
        .class_function("amplitudeToDecibels", &csound::Conversions::amplitudeToDecibels)
        .class_function("amplitudeToGain", &csound::Conversions::amplitudeToGain)
        .class_function("amplitudeToMidi", &csound::Conversions::amplitudeToMidi)
        .class_function("boolToString", &csound::Conversions::boolToString)
        .class_function("decibelsToAmplitude", &csound::Conversions::decibelsToAmplitude)
        .class_function("decibelsToMidi", &csound::Conversions::decibelsToMidi)
        .class_function("doubleToString", &csound::Conversions::doubleToString)
        .class_function("dupstr", &csound::Conversions::dupstr, emscripten::allow_raw_pointers())
        .class_function("findClosestPitchClass", &csound::Conversions::findClosestPitchClass)
        .class_function("gainToAmplitude", &csound::Conversions::gainToAmplitude)
        .class_function("gainToDb", &csound::Conversions::gainToDb)
        .class_function("get2PI", &csound::Conversions::get2PI)
        .class_function("getMaximumAmplitude", &csound::Conversions::getMaximumAmplitude)
        .class_function("getMaximumDynamicRange", &csound::Conversions::getMaximumDynamicRange)
        .class_function("getMiddleCHz", &csound::Conversions::getMiddleCHz)
        .class_function("getNORM_7", &csound::Conversions::getNORM_7)
        .class_function("getPI", &csound::Conversions::getPI)
        .class_function("getSampleSize", &csound::Conversions::getSampleSize)
        .class_function("hzToMidi", &csound::Conversions::hzToMidi)
        .class_function("hzToOctave", &csound::Conversions::hzToOctave)
        .class_function("hzToSamplingIncrement", &csound::Conversions::hzToSamplingIncrement)
        .class_function("initialize", &csound::Conversions::initialize)
        .class_function("intToString", &csound::Conversions::intToString)
        .class_function("leftPan", &csound::Conversions::leftPan)
        .class_function("midiToAmplitude", &csound::Conversions::midiToAmplitude)
        .class_function("midiToDecibels", &csound::Conversions::midiToDecibels)
        .class_function("midiToGain", &csound::Conversions::midiToGain)
        .class_function("midiToHz", &csound::Conversions::midiToHz)
        .class_function("midiToOctave", &csound::Conversions::midiToOctave)
        .class_function("midiToPitchClass", &csound::Conversions::midiToPitchClass)
        .class_function("midiToPitchClassSet", &csound::Conversions::midiToPitchClassSet)
        .class_function("midiToRoundedOctave", &csound::Conversions::midiToRoundedOctave)
        .class_function("midiToSamplingIncrement", &csound::Conversions::midiToSamplingIncrement)
        .class_function("modulus", &csound::Conversions::modulus)
        .class_function("mToName", &csound::Conversions::mToName)
        .class_function("nameToM", &csound::Conversions::nameToM)
        .class_function("nameToPitches", &csound::Conversions::nameToPitches)
        .class_function("octaveToHz", &csound::Conversions::octaveToHz)
        .class_function("octaveToMidi", &csound::Conversions::octaveToMidi)
        .class_function("octaveToSamplingIncrement", &csound::Conversions::octaveToSamplingIncrement)
        .class_function("phaseToTableLengths", &csound::Conversions::phaseToTableLengths)
        .class_function("pitchClassSetToMidi", &csound::Conversions::pitchClassSetToMidi)
        .class_function("pitchClassToMidi", &csound::Conversions::pitchClassToMidi)
        .class_function("rightPan", &csound::Conversions::rightPan)
        .class_function("round", &csound::Conversions::round)
        .class_function("stringToBool", &csound::Conversions::stringToBool)
        .class_function("stringToDouble", &csound::Conversions::stringToDouble)
        .class_function("stringToInt", &csound::Conversions::stringToInt)
        .class_function("stringToVector", &csound::Conversions::stringToVector)
        .class_function("swapInt", &csound::Conversions::swapInt)
        .class_function("swapShort", &csound::Conversions::swapShort)
        .class_function("temper", &csound::Conversions::temper)
        // NOT SUPPORTED
        //.class_function("trim", &csound::Conversions::trim)
        // NOT SUPPORTED
        //.class_function("trimQuotes", &csound::Conversions::trimQuotes)
    ;    
    // FINISHED
    emscripten::class_<Counterpoint>("Counterpoint")
        .constructor<>()
        .function("ABS", &Counterpoint::ABS)
        .function("AddInterval", &Counterpoint::AddInterval)
        .function("ADissonance", &Counterpoint::ADissonance)
        .function("AnOctave", &Counterpoint::AnOctave)
        .function("AnySpecies", &Counterpoint::AnySpecies, emscripten::allow_raw_pointers())
        .function("ARRBLT", &Counterpoint::ARRBLT, emscripten::allow_raw_pointers())
        .function("ASeventh", &Counterpoint::ASeventh)
        .function("ASkip", &Counterpoint::ASkip)
        .function("AStep", &Counterpoint::AStep)
        .function("ATenth", &Counterpoint::ATenth)
        .function("AThird", &Counterpoint::AThird)
        .function("BadMelody", &Counterpoint::BadMelody)
        .function("Bass", &Counterpoint::Bass)
        .function("Beat8", &Counterpoint::Beat8)
        .function("BestFitFirst", &Counterpoint::BestFitFirst)
        .function("Cantus", &Counterpoint::Cantus)
        .function("Check", &Counterpoint::Check)
        .function("CleanRhy", &Counterpoint::CleanRhy)
        .function("clear", &Counterpoint::clear)
        .function("ConsecutiveSkipsInSameDirection", &Counterpoint::ConsecutiveSkipsInSameDirection)
        .function("counterpoint", &Counterpoint::counterpoint, emscripten::allow_raw_pointers())
        .function("CurRhy", &Counterpoint::CurRhy)
        .function("DirectMotionToPerfectConsonance", &Counterpoint::DirectMotionToPerfectConsonance)
        .function("Doubled", &Counterpoint::Doubled)
        .function("DownBeat", &Counterpoint::DownBeat)
        .function("ExtremeRange", &Counterpoint::ExtremeRange)
        .function("fillCantus", &Counterpoint::fillCantus)
        .function("FillRhyPat", &Counterpoint::FillRhyPat)
        .function("FirstNote", &Counterpoint::FirstNote)
        .function("GoodRhy", &Counterpoint::GoodRhy)
        .function("initialize", &Counterpoint::initialize)
        .function("InMode", &Counterpoint::InMode)
        .function("LastNote", &Counterpoint::LastNote)
        .function("Look", &Counterpoint::Look, emscripten::allow_raw_pointers())
        .function("MAX", &Counterpoint::MAX)
        //.function("message", &Counterpoint::message)
        .function("MIN", &Counterpoint::MIN)
        .function("MotionType", &Counterpoint::MotionType)
        .function("NextToLastNote", &Counterpoint::NextToLastNote)
        .function("Other", &Counterpoint::Other)
        .function("OtherVoiceCheck", &Counterpoint::OtherVoiceCheck)
        .function("OutOfRange", &Counterpoint::OutOfRange)
        .function("PitchRepeats", &Counterpoint::PitchRepeats)
        .function("RANDOM", &Counterpoint::RANDOM)
        .function("SaveIndx", &Counterpoint::SaveIndx, emscripten::allow_raw_pointers())
        .function("SaveResults", &Counterpoint::SaveResults)
        .function("SetUs", &Counterpoint::SetUs)
        .function("Size", &Counterpoint::Size)
        .function("SpecialSpeciesCheck", &Counterpoint::SpecialSpeciesCheck)
        .function("toCsoundScore", &Counterpoint::toCsoundScore)
        .function("TooMuchOfInterval", &Counterpoint::TooMuchOfInterval)
        .function("TotalRange", &Counterpoint::TotalRange)
        .function("UpBeat", &Counterpoint::UpBeat)
        .function("Us", &Counterpoint::Us)
        .function("UsedRhy", &Counterpoint::UsedRhy)
        .function("VIndex", &Counterpoint::VIndex)
        .function("winners", &Counterpoint::winners, emscripten::allow_raw_pointers())
    ;
    // FINISHED but multiple inheritance is not supported in Embind.
    emscripten::class_<csound::CounterpointNode, /* emscripten::base<Counterpoint>, */ emscripten::base<csound::Node> >("CounterpointNode")
        .constructor<>()
        .property("generationMode", &csound::CounterpointNode::getGenerationMode, &csound::CounterpointNode::setGenerationMode)
        .property("musicMode", &csound::CounterpointNode::getMusicMode, &csound::CounterpointNode::setMusicMode)
        .property("species", &csound::CounterpointNode::getSpecies, &csound::CounterpointNode::setSpecies)
        .property("voices", &csound::CounterpointNode::getVoices, &csound::CounterpointNode::setVoices)
        .property("secondsPerPulse", &csound::CounterpointNode::getSecondsPerPulse, &csound::CounterpointNode::setSecondsPerPulse)
        // NOT SUPPORTED
        //.property("voiceBeginnings", &csound::CounterpointNode::getVoiceBeginnings, &csound::CounterpointNode::setVoiceBeginnings, emscripten::allow_raw_pointers())
    ;
    emscripten::function("event_lt", emscripten::select_overload<bool(const csound::Event&,const csound::Event&)>(&csound::operator<));
    // FINISHED
    emscripten::class_<csound::Event, emscripten::base<Eigen::VectorXd> >("Event")
        .constructor<>()
        //.constructor<const csound::Event&>()
        //.constructor<std::string>()
        //.constructor<const Eigen::VectorXd&>()
        .constructor<double, double, double, double, double, double, double, double, double, double, double>()
        .constructor<std::vector<double> >()
        .function("clearProperties", &csound::Event::clearProperties)
        .function("conformToPitchClassSet", &csound::Event::conformToPitchClassSet)
        .function("createNoteOffEvent", &csound::Event::createNoteOffEvent)
        .function("dump", &csound::Event::dump)
        .function("get", &csound::Event::get, emscripten::return_value_policy::reference())
        .function("getAmplitude", &csound::Event::getAmplitude)
        .function("getChannel", &csound::Event::getChannel)
        .function("getDepth", &csound::Event::getDepth)
        .function("getDuration", &csound::Event::getDuration)
        .function("getFrequency", &csound::Event::getFrequency)
        .function("getGain", &csound::Event::getGain)
        .function("getHeight", &csound::Event::getHeight)
        .function("getInstrument", &csound::Event::getInstrument)
        .function("getKey", &csound::Event::getKey)
        .function("getKey_tempered", &csound::Event::getKey_tempered)
        .function("getKeyNumber", &csound::Event::getKeyNumber)
        .function("getLeftGain", &csound::Event::getLeftGain)
        .function("getMidiStatus", &csound::Event::getMidiStatus)
        .function("getKeyNumber", &csound::Event::getKeyNumber)
        .function("getOffTime", &csound::Event::getOffTime)
        .function("getPan", &csound::Event::getPan)
        .function("getPhase", &csound::Event::getPhase)
        .function("getPitches", &csound::Event::getPitches)
        .function("getProperties", &csound::Event::getProperties)
        .function("getProperty", &csound::Event::getProperty)
        .function("getRightGain", &csound::Event::getRightGain)
        .function("getStatus", &csound::Event::getStatus)
        .function("getStatusNumber", &csound::Event::getStatusNumber)
        .function("getTime", &csound::Event::getTime)
        .function("getVelocity", &csound::Event::getVelocity)
        .function("getVelocityNumber", &csound::Event::getVelocityNumber)
        .function("initialize", &csound::Event::initialize)
        .function("isMatchingEvent", &csound::Event::isMatchingEvent)
        .function("isMatchingNoteOff", &csound::Event::isMatchingNoteOff)
        .function("isMidiEvent", &csound::Event::isMidiEvent)
        .function("isNote", &csound::Event::isNote)
        .function("isNoteOff", &csound::Event::isNoteOff)
        .function("isNoteOn", &csound::Event::isNoteOn)
        .function("operator=", emscripten::select_overload<csound::Event&(const csound::Event&)>(&csound::Event::operator=))
        ///.function("operator<", emscripten::select_overload<csound::Event&(const csound::Event&)>(&csound::Event::operator<))
        .function("removeProperty", &csound::Event::removeProperty)
        .function("set", &csound::Event::set)
        .function("setAmplitude", &csound::Event::setAmplitude)
        .function("setDepth", &csound::Event::setDepth)
        .function("setDuration", &csound::Event::setDuration)
        .function("setFrequency", &csound::Event::setFrequency)
        .function("setHeight", &csound::Event::setHeight)
        .function("setInstrument", &csound::Event::setInstrument)
        .function("setKey", &csound::Event::setKey)
        .function("setOffTime", &csound::Event::setOffTime)
        .function("setPan", &csound::Event::setPan)
        .function("setPhase", &csound::Event::setPhase)
        .function("setPitches", &csound::Event::setPitches)
        .function("setProperty", &csound::Event::setProperty)
        .function("setStatus", &csound::Event::setStatus)
        .function("setTime", &csound::Event::setTime)
        .function("setVelocity", &csound::Event::setVelocity)
        .function("temper", &csound::Event::temper)
        .function("toCsoundIStatement", &csound::Event::toCsoundIStatement)
        .function("toCsoundIStatementHeld", &csound::Event::toCsoundIStatementHeld)
        .function("toCsoundIStatementRelease", &csound::Event::toCsoundIStatementRelease)
        .function("toString", &csound::Event::toString)
    ;
    emscripten::function("getCorrectNegativeDurations", &csound::getCorrectNegativeDurations);
    emscripten::function("setCorrectNegativeDurations", &csound::setCorrectNegativeDurations);
    emscripten::function("hello", &hello);
    // FINISHED
    emscripten::class_<csound::Intercut, emscripten::base<csound::ScoreNode> >("Intercut")
        .constructor<>()
    ;
    // FINISHED
    emscripten::class_<csound::HarmonyPoint2, emscripten::base<Eigen::VectorXd> >("HarmonyPoint2")
        .constructor<>()
        .constructor<const csound::HarmonyPoint2 &>()
        .function("operator=", emscripten::select_overload<csound::HarmonyPoint2 &(const csound::HarmonyPoint2 &)>(&csound::HarmonyPoint2::operator=))
        .function("initialize", &csound::HarmonyPoint2::initialize)
        .property("t", &csound::HarmonyPoint2::get_t, &csound::HarmonyPoint2::set_t)
        .property("P", &csound::HarmonyPoint2::get_P, &csound::HarmonyPoint2::set_P)
        .property("I", &csound::HarmonyPoint2::get_I, &csound::HarmonyPoint2::set_I)
        .property("T", &csound::HarmonyPoint2::get_T, &csound::HarmonyPoint2::set_T)
        .property("V", &csound::HarmonyPoint2::get_V, &csound::HarmonyPoint2::set_V)
        .property("homogeneity", &csound::HarmonyPoint2::get_homogeneity, &csound::HarmonyPoint2::set_homogeneity)
        .function("toString", &csound::HarmonyPoint2::toString)
    ;
 
    // FINISHED
    emscripten::class_<csound::HarmonyInterpolationPoint2>("HarmonyInterpolationPoint2")
        .constructor<>()
        .constructor<const csound::HarmonyInterpolationPoint2 &>()
        .constructor<double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double>()
        //~ .function("operator=", emscripten::select_overload<csound::HarmonyInterpolationPoint &(const csound::HarmonyInterpolationPoint &)>(&csound::HarmonyInterpolationPoint::operator=))
        .function("toString", &csound::HarmonyInterpolationPoint2::toString)
    ;
    emscripten::class_<csound::HarmonyIFS2, emscripten::base<csound::ScoreNode> >("HarmonyIFS2")
        .constructor<>()
        .function("add_interpolation_point", &csound::HarmonyIFS2::add_interpolation_point)
        .function("add_interpolation_point_as_chord", &csound::HarmonyIFS2::add_interpolation_point_as_chord)
        .function("generate_score_attractor", &csound::HarmonyIFS2::generate_score_attractor)
        .function("add_transformation", &csound::HarmonyIFS2::add_transformation)
        .function("transformation_count", &csound::HarmonyIFS2::transformation_count)
        .function("initialize", &csound::HarmonyIFS2::initialize)
        .function("initialize_hutchinson_operator", &csound::HarmonyIFS2::initialize_hutchinson_operator)
        .function("iterate", &csound::HarmonyIFS2::iterate, emscripten::allow_raw_pointers())
        .function("remove_duplicate_notes", &csound::HarmonyIFS2::remove_duplicate_notes)
        .function("set_transformation", &csound::HarmonyIFS2::set_transformation)
        .function("post_process_score", &csound::HarmonyIFS2::post_process_score)
    ;
    // FINISHED
    emscripten::class_<csound::KMeansMCRM, emscripten::base<csound::MCRM> >("KMeansMCRM")
        .constructor<>()
        .function("deterministic_algorithm", &csound::KMeansMCRM::deterministic_algorithm)
        .function("iterate", &csound::KMeansMCRM::iterate, emscripten::allow_raw_pointers())
        .function("means_to_notes", &csound::KMeansMCRM::means_to_notes)
        .function("random_algorithm", &csound::KMeansMCRM::random_algorithm)
    ;
    // FINISHED
    emscripten::class_<csound::Koch, emscripten::base<csound::ScoreNode> >("Koch")
        .constructor<>()
        .function("setPitchOffsetForLayer", &csound::Koch::setPitchOffsetForLayer)
    ;
    // FINISHED
    emscripten::class_<csound::Lindenmayer, emscripten::base<csound::ScoreNode> >("Lindenmayer")
        .constructor<>()
        .function("addRule", &csound::Lindenmayer::addRule)
        .function("generateLocally", emscripten::select_overload<void()>(&csound::Lindenmayer::generateLocally))
        .function("getAngle", &csound::Lindenmayer::getAngle)
        .function("getAxiom", &csound::Lindenmayer::getAxiom)
        .function("getIterationCount", &csound::Lindenmayer::getIterationCount)
        .function("setAngle", &csound::Lindenmayer::setAngle)
        .function("setAxiom", &csound::Lindenmayer::setAxiom)
        .function("setIterationCount", &csound::Lindenmayer::setIterationCount)
    ;
    /* NOT SUPPORTED 
    // ECL doesn't yet build for WebAssembly as far as I know.
    // FINISHED
    emscripten::class_<csound::LispGenerator, emscripten::base<csound::LispNode> >("LispGenerator")
        .constructor<>()
    ;
    // FINISHED
    emscripten::class_<csound::LispNode, emscripten::base<csound::Node> >("LispNode")
        .constructor<>()
        .function("appendTopLevelForm", &csound::LispNode::appendTopLevelForm)
        .function("getStringFromForm", &csound::LispNode::getStringFromForm)
        .function("getTopLevelForms", &csound::LispNode::getTopLevelForms)
     ;
    // FINISHED
    emscripten::class_<csound::LispTransformer, emscripten::base<csound::LispNode> >("LispTransformer")
        .constructor<>()
    ;
    */
    // FINISHED
    emscripten::class_<csound::MCRM, emscripten::base<csound::ScoreNode> >("MCRM")
        .constructor<>()
        .function("generateLocally", emscripten::select_overload<void()>(&csound::MCRM::generateLocally))
        .function("resize", &csound::MCRM::resize)
        .function("setDepth", &csound::MCRM::setDepth)
        .function("setTransformationElement", &csound::MCRM::setTransformationElement)
        .function("setWeight", &csound::MCRM::setWeight)
    ;
    // FINISHED
    emscripten::class_<csound::Node>("Node")
        .constructor<>()
        .function("addChild", &csound::Node::addChild, emscripten::allow_raw_pointers())
        .function("childCount", &csound::Node::childCount)
        .function("clear", &csound::Node::clear)
        .function("createTransform", &csound::Node::createTransform)
        .function("element", &csound::Node::element)
        .function("generate", &csound::Node::generate)
        .function("getChild", &csound::Node::getChild, emscripten::allow_raw_pointers())
        .function("getLocalCoordinates", &csound::Node::getLocalCoordinates)
        .function("setElement", &csound::Node::setElement)
        .function("transform", &csound::Node::transform)
        .function("traverse", &csound::Node::traverse)
    ;
    // FINISHED
    emscripten::class_<csound::Random, emscripten::base<csound::Node> >("Random")
        .constructor<>()
        .function("createDistribution", &csound::Random::createDistribution)
        .function("getRandomCoordinates", &csound::Random::getRandomCoordinates)
        .function("sample", &csound::Random::sample)
        .class_function("seed", &csound::Random::seed)
    ;
    // FINISHED
    emscripten::class_<csound::RemoveDuplicates, emscripten::base<csound::Node> >("RemoveDuplicates")
        .constructor<>()
    ;
    // FINISHED
    emscripten::class_<csound::Rescale, emscripten::base<csound::Node> >("Rescale")
        .constructor<>()
        // NOT SUPPORTED
        // .function("getRescale", &csound::Rescale::getRescale, emscripten::allow_raw_pointers())
       .function("initialize", &csound::Rescale::initialize)
       .function("setRescale", &csound::Rescale::setRescale)
    ;
    // FINISHED
    emscripten::class_<csound::Scale, emscripten::base<csound::Chord>>("Scale")
        .constructor<>()
        .constructor<std::string>()
        .constructor<std::string, const csound::Chord&>()
        .function("conform_with_alteration", &csound::Scale::conform_with_alteration)
        .function("conform_with_interval", &csound::Scale::conform_with_interval)
        .function("conform", &csound::Scale::conform)
        .function("chord", &csound::Scale::chord)
        .function("degree", &csound::Scale::degree)
        .function("operator=", &csound::Scale::operator=)
        ///.function("operator<", &csound::Scale::operator<)
        .function("set_ratio", &csound::Scale::set_ratio)
        .function("from_scala", &csound::Scale::from_scala)
        .function("getTypeName", &csound::Scale::getTypeName)
        .function("modulations", &csound::Scale::modulations)
        .function("modulations_for_voices", &csound::Scale::modulations_for_voices)
        .function("modulations_for_scale_types", &csound::Scale::modulations_for_scale_types)
        .function("relative_tonicizations", &csound::Scale::relative_tonicizations)
        .function("relative_tonicizations_for_scale_types", &csound::Scale::relative_tonicizations_for_scale_types)
        .function("secondary", &csound::Scale::secondary)
        .function("semitones_for_degree", &csound::Scale::semitones_for_degree)
        .function("tonic", &csound::Scale::tonic)
        .function("tonicizations", &csound::Scale::tonicizations)
        .function("transpose", &csound::Scale::transpose)
        .function("transpose_degrees", &csound::Scale::transpose_degrees)
        .function("transpose_to_degree", &csound::Scale::transpose_to_degree)
    ;
    // FINISHED
    emscripten::class_<csound::Score, emscripten::base<std::vector<csound::Event>>>("Score")
        .constructor<>()
        .function("add", &csound::Score::add)
        .function("append", &csound::Score::append_note)
        .function("append_event", &csound::Score::append_event)
        .function("arrange_all", &csound::Score::arrange_all)
        .function("clear", &csound::Score::clear)
        .function("findScale", &csound::Score::findScale)
        .function("get", &csound::Score::get, emscripten::return_value_policy::reference())
        .function("getCsoundScore", &csound::Score::getCsoundScore)
        .function("getDuration", &csound::Score::getDuration)
        .function("getEventsDuration", &csound::Score::getEventsDuration)
        .function("getFirstOnTime", &csound::Score::getFirstOnTime)
        .function("getLastOffTime", &csound::Score::getLastOffTime)
        .function("getDurationFromZero", &csound::Score::getDurationFromZero)
        .function("getPitches", &csound::Score::getPitches)
        .function("getPT", &csound::Score::getPT)
        .function("getPTV", &csound::Score::getPTV)
        // NOT SUPPORTED
        // .function("getScale", &csound::Score::getScale, emscripten::allow_raw_pointers())
        .function("getPTV", &csound::Score::getPTV)
        .function("getRescaleMinima", &csound::Score::getRescaleMinima)
        .function("getRescaleRanges", &csound::Score::getRescaleRanges)
        .function("getScaleActualMinima", &csound::Score::getScaleActualMinima)
        .function("getScaleActualRanges", &csound::Score::getScaleActualRanges)
        .function("getScaleTargetMinima", &csound::Score::getScaleTargetMinima)
        .function("getScaleTargetRanges", &csound::Score::getScaleTargetRanges)
        .function("getVoicing", &csound::Score::getVoicing)
        .function("indexAfterTime", &csound::Score::indexAfterTime)
        .function("indexAtTime", &csound::Score::indexAtTime)
        .function("indexToTime", &csound::Score::indexToTime)
        .function("initialize", &csound::Score::initialize)
        .function("load", &csound::Score::load_filename)
        .function("process", &csound::Score::process)
        .function("remove", &csound::Score::remove)
        .function("removeArrangement", &csound::Score::removeArrangement)
        .function("rescale", emscripten::select_overload<void()>(&csound::Score::rescale))
        .function("save", &csound::Score::save_filename)
        .function("setDuration", &csound::Score::setDuration)
        .function("setDurationFromZero", &csound::Score::setDurationFromZero)
        .function("setK", &csound::Score::setK)
        .function("setKL", &csound::Score::setKL)
        .function("setKV", &csound::Score::setKV)
        .function("setPerformanceDuration", &csound::Score::setPerformanceDuration)
        .function("setPitchClassSet", &csound::Score::setPitchClassSet)
        .function("setPitches", &csound::Score::setPitches)
        .function("setPT", &csound::Score::setPT)
        .function("setPTV", &csound::Score::setPTV)
        .function("setQ", &csound::Score::setQ)
        .function("setQL", &csound::Score::setQL)
        .function("setQV", &csound::Score::setQV)
        .function("setScale", &csound::Score::setScale)
        .function("setVoicing", &csound::Score::setVoicing)
        .function("sort", &csound::Score::sort)
        .function("temper", &csound::Score::temper)
        .function("tieOverlappingNotes", &csound::Score::tieOverlappingNotes)
        .function("toString", &csound::Score::toString)
        .function("voicelead", &csound::Score::voicelead_segments)
        .function("voicelead_pitches", &csound::Score::voicelead_pitches)
        .function("setPTV", &csound::Score::setPTV)
    ;
    // FINISHED
    emscripten::class_<csound::ChordScore, emscripten::base<csound::Score> >("ChordScore")
        .constructor<>()
        .function("conformToChords", &csound::ChordScore::conformToChords)
        .function("getChord", &csound::ChordScore::getChord, emscripten::allow_raw_pointers())
        // NOT SUPPORTED.
        // .function("getScale", &csound::ChordScore::getScale, emscripten::allow_raw_pointers())
        .function("insertChord", &csound::ChordScore::insertChord)
        .function("setScale", &csound::ChordScore::setScale, emscripten::allow_raw_pointers())
    ;
    // FINISHED
    emscripten::class_<csound::ScoreModel, emscripten::base<csound::Composition> >("ScoreModel")
        .constructor<>()
        .function("addChild", &csound::ScoreModel::addChild, emscripten::allow_raw_pointers())
        .function("childCount", &csound::ScoreModel::childCount)
        .function("clear", &csound::ScoreModel::clear)
        .function("createTransform", &csound::ScoreModel::createTransform)
        .function("element", &csound::ScoreModel::element)
        .function("getChild", &csound::ScoreModel::getChild, emscripten::allow_raw_pointers())
        .function("getLocalCoordinates", &csound::ScoreModel::getLocalCoordinates)
        .function("getThisNode", &csound::ScoreModel::getThisNode, emscripten::allow_raw_pointers())
        .function("initialize", &csound::ScoreModel::initialize)
        .function("setElement", &csound::ScoreModel::setElement)
        .function("transform", &csound::ScoreModel::transform)
        .function("traverse", &csound::ScoreModel::traverse)
    ;
    // FINISHED
    emscripten::class_<csound::ScoreNode, emscripten::base<csound::Node> >("ScoreNode")
        .constructor<>()
        .function("getScore", &csound::ScoreNode::getScore, emscripten::allow_raw_pointers())
    ;
    // FINISHED
    emscripten::class_<csound::Sequence, emscripten::base<csound::Node> >("Sequence")
        .constructor<>()
    ;
    // FINISHED
    emscripten::class_<csound::Stack, emscripten::base<csound::ScoreNode> >("Stack")
        .constructor<>()
        .property("duration", &csound::Stack::getDuration, &csound::Stack::setDuration)
    ;
    // FINISHED
    emscripten::class_<csound::StrangeAttractor, emscripten::base<csound::ScoreNode> >("StrangeAttractor")
        .constructor<>()
        .function("calculateFractalDimension", &csound::StrangeAttractor::calculateFractalDimension)
        .function("calculateLyupanovExponent", &csound::StrangeAttractor::calculateLyupanovExponent)
        .function("codeRandomize", &csound::StrangeAttractor::codeRandomize)
        .function("evaluateAttractor", &csound::StrangeAttractor::evaluateAttractor)
        .function("generateLocally", emscripten::select_overload<void()>(&csound::StrangeAttractor::generateLocally))
        .function("getAttractorType", &csound::StrangeAttractor::getAttractorType)
        .function("getCode", &csound::StrangeAttractor::getCode)
        .function("getCoefficients", &csound::StrangeAttractor::getCoefficients)
        .function("getDimensionAndOrder", &csound::StrangeAttractor::getDimensionAndOrder)
        .function("getDimensionCount", &csound::StrangeAttractor::getDimensionCount)
        .function("getFractalDimension", &csound::StrangeAttractor::getFractalDimension)
        .function("getIteration", &csound::StrangeAttractor::getIteration)
        .function("getIterationCount", &csound::StrangeAttractor::getIterationCount)
        .function("getLyupanovExponent", &csound::StrangeAttractor::getLyupanovExponent)
        .function("getNormalizedW", &csound::StrangeAttractor::getNormalizedW)
        .function("getNormalizedX", &csound::StrangeAttractor::getNormalizedX)
        .function("getNormalizedY", &csound::StrangeAttractor::getNormalizedY)
        .function("getNormalizedZ", &csound::StrangeAttractor::getNormalizedZ)
        .function("getScoreType", &csound::StrangeAttractor::getScoreType)
        .function("getW", &csound::StrangeAttractor::getW)
        .function("getX", &csound::StrangeAttractor::getX)
        .function("getY", &csound::StrangeAttractor::getY)
        .function("getZ", &csound::StrangeAttractor::getZ)
        .function("initialize", &csound::StrangeAttractor::initialize)
        .function("iterate", &csound::StrangeAttractor::iterate)
        .function("iterate_without_rendering", &csound::StrangeAttractor::iterate_without_rendering)
        .function("reinitialize", &csound::StrangeAttractor::reinitialize)
        .function("render", &csound::StrangeAttractor::render)
        .function("reset", &csound::StrangeAttractor::reset)
        .function("searchForAttractor", &csound::StrangeAttractor::searchForAttractor)
        .function("setAttractorType", &csound::StrangeAttractor::setAttractorType)
        .function("setCode", &csound::StrangeAttractor::setCode)
        .function("setDimensionCount", &csound::StrangeAttractor::setDimensionCount)
        .function("setIteration", &csound::StrangeAttractor::setIteration)
        .function("setIterationCount", &csound::StrangeAttractor::setIterationCount)
        .function("setScoreType", &csound::StrangeAttractor::setScoreType)
        .function("setAttractorType", &csound::StrangeAttractor::setAttractorType)
        .function("setAttractorType", &csound::StrangeAttractor::setAttractorType)
        .function("setW", &csound::StrangeAttractor::setW)
        .function("setX", &csound::StrangeAttractor::setX)
        .function("setY", &csound::StrangeAttractor::setY)
        .function("setZ", &csound::StrangeAttractor::setZ)
        .function("shuffleRandomNumbers", &csound::StrangeAttractor::shuffleRandomNumbers)
        .function("specialFunctions", &csound::StrangeAttractor::specialFunctions)
    ;
    // FINISHED
    // Mostly not supported and not used, much else simplified.
    emscripten::class_<csound::System>("System")
        .constructor<>()
        .class_function("debug", &csound::System::debug_text)
        .class_function("error", &csound::System::error_text)
        .class_function("execute", &csound::System::execute, emscripten::allow_raw_pointers())
        .class_function("getDirectoryNames", &csound::System::getDirectoryNames)
        .class_function("getFilenames", &csound::System::getFilenames)
        .class_function("getMessageCallback", &csound::System::getMessageCallback)
        .class_function("getMessageLevel", &csound::System::getMessageLevel)
        .class_function("getUserdata", &csound::System::getUserdata, emscripten::allow_raw_pointers())
        .class_function("inform", &csound::System::inform_text)
        .class_function("message", &csound::System::message_text)
        // NOT SUPPORTED    
        // .class_function("parsePathname", &csound::System::parsePathname, emscripten::allow_raw_pointers())
        .class_function("setMessageCallback", &csound::System::setMessageCallback)
        .class_function("setMessageLevel", &csound::System::setMessageLevel)
        .class_function("setUserdata", &csound::System::setUserdata, emscripten::allow_raw_pointers())
        //.class_function("stopTiming", &csound::System::stopTiming)
        .class_function("warn", &csound::System::warn_text)
    ;
    emscripten::class_<csound::Voicelead>("Voicelead")
        .constructor<>()
        .function("addOctave", &csound::Voicelead::addOctave)
        .function("areParallel", &csound::Voicelead::areParallel)
        .function("chordToPTV", &csound::Voicelead::chordToPTV)
        .function("closer", &csound::Voicelead::closer)
        .function("closest", &csound::Voicelead::closest)
        .function("closestPitch", &csound::Voicelead::closestPitch)
        .function("conformToPitchClassSet", &csound::Voicelead::conformToPitchClassSet)
        .function("cToM", &csound::Voicelead::cToM)
        .function("cToP", &csound::Voicelead::cToP)
        .function("euclideanDistance", &csound::Voicelead::euclideanDistance)
        .function("I", &csound::Voicelead::I)
        .function("I_vector", &csound::Voicelead::I_vector)
        .function("Iform", &csound::Voicelead::Iform)
        .function("initializePrimeChordsForDivisionsPerOctave", &csound::Voicelead::initializePrimeChordsForDivisionsPerOctave)
        .function("inversions", &csound::Voicelead::inversions)
        .function("invert", &csound::Voicelead::invert)
        .function("K", &csound::Voicelead::K)
        .function("mToC", &csound::Voicelead::mToC)
        .function("mToPitchClassSet", &csound::Voicelead::mToPitchClassSet)
        .function("nameToC", &csound::Voicelead::nameToC)
        .function("nonBijectiveVoicelead", &csound::Voicelead::nonBijectiveVoicelead)
        .function("normalChord", &csound::Voicelead::normalChord)
        .function("orderedPcs", &csound::Voicelead::orderedPcs)
        .function("pAndTtoPitchClassSet", &csound::Voicelead::pAndTtoPitchClassSet)
        .function("pc", &csound::Voicelead::pc)
        .function("pcs", &csound::Voicelead::pcs)
        .function("pitchClassSetToM", &csound::Voicelead::pitchClassSetToM)
        .function("pitchClassSetToPandT", &csound::Voicelead::pitchClassSetToPandT)
        .function("primeChord", &csound::Voicelead::primeChord)
        .function("pToC", &csound::Voicelead::pToC)
        .function("pToPrimeChord", &csound::Voicelead::pToPrimeChord)
        .function("ptvToChord", &csound::Voicelead::ptvToChord)
        .function("Q", &csound::Voicelead::Q)
        .function("recursiveVoicelead", &csound::Voicelead::recursiveVoicelead)
        .function("rotate", &csound::Voicelead::rotate)
        .function("rotations", &csound::Voicelead::rotations)
        .function("simpler", &csound::Voicelead::simpler)
        .function("smoothness", &csound::Voicelead::smoothness)
        .function("sortByAscendingDistance", &csound::Voicelead::sortByAscendingDistance)
        .function("T", &csound::Voicelead::T)
        .function("T_vector", &csound::Voicelead::T_vector)
        .function("Tform", &csound::Voicelead::Tform)
        .function("toOrigin", &csound::Voicelead::toOrigin)
        .function("transpose", &csound::Voicelead::transpose)
        .function("uniquePcs", &csound::Voicelead::uniquePcs)
        .function("voicelead", &csound::Voicelead::voicelead)
        .function("voiceleading", &csound::Voicelead::voiceleading)
        .function("voicings", &csound::Voicelead::voicings)
        .function("wrap", &csound::Voicelead::wrap)
    ;     
    emscripten::class_<csound::VoiceleadingNode, emscripten::base<csound::Node>>("VoiceleadingNode")
        .constructor<>()
        .function("apply", &csound::VoiceleadingNode::apply)
        .function("C", &csound::VoiceleadingNode::C)
        .function("C_chord", &csound::VoiceleadingNode::C_name)
        .function("chordVoiceleading", &csound::VoiceleadingNode::chordVoiceleading)
        .function("CL", &csound::VoiceleadingNode::CL)
        .function("CL_name", &csound::VoiceleadingNode::CL_name)
        .function("CV", &csound::VoiceleadingNode::CV)
        .function("CV_name", &csound::VoiceleadingNode::CV_name)
        .function("getModality", &csound::VoiceleadingNode::getModality)
        .function("K", &csound::VoiceleadingNode::K)
        .function("KL", &csound::VoiceleadingNode::KL)
        .function("KV", &csound::VoiceleadingNode::KV)
        .function("L", &csound::VoiceleadingNode::L)
        .function("PT", &csound::VoiceleadingNode::PT)
        .function("PTL", &csound::VoiceleadingNode::PTL)
        .function("PTV", &csound::VoiceleadingNode::PTV)
        .function("Q", &csound::VoiceleadingNode::Q)
        .function("QL", &csound::VoiceleadingNode::QL)
        .function("QV", &csound::VoiceleadingNode::QV)
        .function("V", &csound::VoiceleadingNode::V)
    ;    
    emscripten::class_<Eigen::VectorXd>("VectorXd")
        .constructor<>()
    ;
 
}
