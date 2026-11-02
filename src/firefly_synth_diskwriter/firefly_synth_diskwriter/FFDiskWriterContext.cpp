#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_base/dsp/shared/FBTuning.hpp>
#include <firefly_synth/dsp/plug/FFPlugProcessor.hpp>
#include <firefly_synth_diskwriter/FFDiskWriterContext.hpp>
#include <firefly_synth_diskwriter/FFDiskWriterOptions.hpp>

FFDiskWriterContext::
FFDiskWriterContext(FFDiskWriterOptions const& options):
FBHostContext(
  FFMakeTopo(FBPlugFormat::VST3, false), FBTuningGetMTSClient(),
  std::make_unique<FBExchangeStateQueue<FBQueuedExchangeState<FFExchangeState>>>()),
_options(options),
_audioParams(*Topo()) {}

float 
FFDiskWriterContext::SampleRate() const
{
  return (float)_options.sampleRate;
}

double 
FFDiskWriterContext::GetAudioParamNormalized(int index) const
{
  return *_audioParams.Params()[index];
}

std::unique_ptr<IFBPlugProcessor>
FFDiskWriterContext::MakePlugProcessor()
{
  return std::make_unique<FFPlugProcessor>(this);
}

void
FFDiskWriterContext::BulkUpdateAudioParamsToGUI(FBScalarStateContainer const& audioParams, bool patchOnly)
{
  _audioParams.CopyFrom(Topo(), audioParams, patchOnly);
}