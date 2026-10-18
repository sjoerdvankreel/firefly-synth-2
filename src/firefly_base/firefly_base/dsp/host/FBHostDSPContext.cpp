#include <firefly_base/dsp/host/FBHostDSPContext.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

FBHostDSPContext::
FBHostDSPContext(FBRuntimeTopo const* topo):
_dspAudioClips(),
_procState(std::make_unique<FBProcStateContainer>(*topo, &_dspAudioClips)),
_exchangeToGUIState(std::make_unique<FBExchangeStateContainer>(*topo)) { }