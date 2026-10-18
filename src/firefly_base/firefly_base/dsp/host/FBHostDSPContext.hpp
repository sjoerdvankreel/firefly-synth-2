#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <memory>

struct FBRuntimeTopo;
class MTSClient;
class IFBPlugProcessor;

class FBHostDSPContext
{
  std::map<std::string, FBAudioClip> _dspAudioClips;
  std::unique_ptr<FBProcStateContainer> _procState;
  std::unique_ptr<FBExchangeStateContainer> _exchangeToGUIState;

protected:
  FBHostDSPContext(FBRuntimeTopo const* topo);

public:
  virtual ~FBHostDSPContext() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostDSPContext);

  virtual float SampleRate() const = 0;
  virtual MTSClient* GetMTSClient() = 0;
  virtual void PullDSPAudioClipsFromGUI() = 0;
  virtual FBRuntimeTopo const* Topo() const = 0;
  virtual std::unique_ptr<IFBPlugProcessor> MakePlugProcessor() = 0;

  FBProcStateContainer* ProcState() { return _procState.get(); }
  FBExchangeStateContainer* ExchangeToGUIState() { return _exchangeToGUIState.get(); }
  std::map<std::string, FBAudioClip>& GetDSPAudioClips() { return _dspAudioClips; }
  std::map<std::string, FBAudioClip> const& GetDSPAudioClips() const { return _dspAudioClips; }
};