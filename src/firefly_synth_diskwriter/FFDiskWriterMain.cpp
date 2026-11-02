#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_synth_diskwriter/FFDiskWriterContext.hpp>

int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  FFDiskWriterOptions options;
  options.bpm = 120;
  options.midiNote = 60;
  options.sampleRate = 48000;
  options.durationMilliseconds = 1000;
  options.outputRawPath = "c:\\temp\\testpatch123.raw";
  options.inputPatchPath = "c:\\temp\\testpatch123.ff2preset";

  auto context = std::make_unique<FFDiskWriterContext>(options);
  auto hostProcessor = std::make_unique<FBHostProcessor>(context.get());
  

  return 0;
}