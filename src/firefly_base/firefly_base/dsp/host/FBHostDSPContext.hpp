#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <memory>

class MTSClient;
struct FBRuntimeTopo;

class IFBPlugProcessor;
class FBProcStateContainer;
class FBExchangeStateContainer;

class IFBHostDSPContext
{
public:
  virtual ~IFBHostDSPContext() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBHostDSPContext);

  virtual float SampleRate() const = 0;
  virtual MTSClient* GetMTSClient() = 0;
  virtual FBRuntimeTopo const* Topo() const = 0;
  virtual FBProcStateContainer* ProcState() = 0;
  virtual FBExchangeStateContainer* ExchangeToGUIState() = 0;
  virtual std::unique_ptr<IFBPlugProcessor> MakePlugProcessor() = 0;
};