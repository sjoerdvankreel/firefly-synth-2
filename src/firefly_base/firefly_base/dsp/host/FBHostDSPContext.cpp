#include <firefly_base/dsp/host/FBHostDSPContext.hpp>

FBHostDSPContext::
FBHostDSPContext(FBRuntimeTopo const* topo):
_dspAudioClips(),
_procState(std::make_unique<FBProcStateContainer>(*topo, &_dspAudioClips)),
_exchangeToGUIState(std::make_unique<FBExchangeStateContainer>(*topo)) { }