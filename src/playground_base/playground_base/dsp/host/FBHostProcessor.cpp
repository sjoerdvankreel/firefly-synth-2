#include <playground_base/dsp/plug/FBPlugProcessor.hpp>
#include <playground_base/dsp/host/FBHostProcessor.hpp>
#include <playground_base/dsp/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/fixed/FBRampProcessor.hpp>
#include <playground_base/dsp/buffer/FBHostBufferProcessor.hpp>
#include <playground_base/dsp/buffer/FBFixedBufferProcessor.hpp>

#include <utility>

FBHostProcessor::
FBHostProcessor(std::unique_ptr<IFBPlugProcessor>&& plug):
_plug(std::move(plug)),
_ramp(std::make_unique<FBRampProcessor>()),
_hostBuffer(std::make_unique<FBHostBufferProcessor>()),
_fixedBuffer(std::make_unique<FBFixedBufferProcessor>())
{
  _fixedOut.state = _plug->StateAddrs();
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
    _plug->ProcessPlug(fixedIn->audio, _fixedOut.audio);
    _fixedBuffer->BufferFromFixed(_fixedOut.audio);
  }
  _fixedBuffer->ProcessToHost(output);
}