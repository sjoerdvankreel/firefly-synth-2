#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBRampProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBHostBufferProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <utility>

FBHostProcessor::
~FBHostProcessor() {}

FBHostProcessor::
FBHostProcessor(
  FBRuntimeTopo const& topo,
  std::unique_ptr<IFBPlugProcessor>&& plug, 
  float sampleRate):
_plug(std::move(plug)),
_ramp(std::make_unique<FBRampProcessor>()),
_smooth(std::make_unique<FBSmoothProcessor>()),
_hostBuffer(std::make_unique<FBHostBufferProcessor>()),
_fixedBuffer(std::make_unique<FBFixedBufferProcessor>())
{
  _fixedOut.state = topo.MakeProcStatePtrs(_plug->ProcState());
  for (int p = 0; p < _fixedOut.state.acc.size(); p++)
    _fixedOut.state.acc[p]->smooth =
    FBOnePoleFilter(sampleRate, FB_PARAM_SMOOTH_SEC);
}

void 
FBHostProcessor::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.block)
    *_fixedOut.state.block[be.index] = be.normalized;

  FBFixedInputBlock const* fixedIn;
  _hostBuffer->BufferFromHost(input);
  while ((fixedIn = _hostBuffer->ProcessToFixed()) != nullptr)
  {
    _ramp->ProcessRamping(*fixedIn, _fixedOut);
    _smooth->ProcessSmoothing(_fixedOut);
    _plug->ProcessPlug(fixedIn->audio, _fixedOut.audio);
    _fixedBuffer->BufferFromFixed(_fixedOut.audio);
  }
  _fixedBuffer->ProcessToHost(output);
}