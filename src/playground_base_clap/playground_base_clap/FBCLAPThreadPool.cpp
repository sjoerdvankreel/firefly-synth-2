#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/base/shared/FBDenormal.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessor.hpp>

bool 
FBCLAPPlugin::implementsThreadPool() const noexcept
{
  return true;
}

void 
FBCLAPPlugin::threadPoolExec(uint32_t taskIndex) noexcept
{
  auto denormalState = FBDisableDenormal();
  _hostProcessor->ProcessVoice(taskIndex);
  FBRestoreDenormal(denormalState);
}