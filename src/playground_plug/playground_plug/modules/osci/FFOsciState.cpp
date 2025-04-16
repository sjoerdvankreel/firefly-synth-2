#include <playground_plug/modules/osci/FFOsciState.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

FFOsciDSPState::~FFOsciDSPState() {}
FFOsciDSPState::FFOsciDSPState() :
processor(std::make_unique<FFOsciProcessor>()) {}