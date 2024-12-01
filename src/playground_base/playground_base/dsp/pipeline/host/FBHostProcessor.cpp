#include <playground_base/base/plug/FBPlugConfig.hpp>
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
  std::unique_ptr<IFBPlugProcessor>&& plug, float sampleRate):
_plug(std::move(plug)),
_ramp(std::make_unique<FBRampProcessor>()),
_smooth(std::make_unique<FBSmoothProcessor>()),
_hostBuffer(std::make_unique<FBHostBufferProcessor>()),
_fixedBuffer(std::make_unique<FBFixedBufferProcessor>())
{
  _fixedOut.state = _plug->StateAddrs();
  for (int p = 0; p < _fixedOut.state.proc->param.size(); p++)
    _fixedOut.state.proc->param[p]->smooth = 
      FBOnePoleFilter(sampleRate, FB_PLUG_PARAM_SMOOTH_SEC);
}

void 
FBHostProcessor::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.block)
    *_fixedOut.state.scalar->block[be.index] = be.normalized;

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