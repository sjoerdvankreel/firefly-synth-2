#include <firefly_base/dsp/host/FBHostBlock.hpp>
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

  FBNoteEvent noteOn;
  noteOn.pos = 0;
  noteOn.on = true;
  noteOn.velo = 1.0f;
  noteOn.note.id = 0;
  noteOn.note.channel = 0;
  noteOn.note.key = options.midiNote;

  int maxFrameCount = options.sampleRate * options.durationMilliseconds / 1000;
  FBHostInputBlock inputBlock = {};
  inputBlock.projectTimeSamples = 0;
  inputBlock.bpm = (float)options.bpm;
  inputBlock.sampleCount = maxFrameCount;
  inputBlock.noteEvents.push_back(noteOn);

  std::array<std::vector<float>, 2> zeroInData = {};
  for (int ch = 0; ch < 2; ch++)
    zeroInData[ch] = std::vector<float>(maxFrameCount, 0.0f);
  float* zeroIn[2] = { zeroInData[0].data(), zeroInData[1].data() };
  inputBlock.mainAudio = FBHostAudioBlock(zeroIn, maxFrameCount);
  inputBlock.sidechainAudio = FBHostAudioBlock(zeroIn, maxFrameCount);

  FBHostOutputBlock outputBlock = {};
  std::array<std::vector<float>, 2> zeroOutData = {};
  for (int ch = 0; ch < 2; ch++)
    zeroOutData[ch] = std::vector<float>(maxFrameCount, 0.0f);
  float* zeroOut[2] = { zeroOutData[0].data(), zeroOutData[1].data() };
  outputBlock.audio = FBHostAudioBlock(zeroOut, maxFrameCount);

  auto context = std::make_unique<FFDiskWriterContext>(options);
  auto hostProcessor = std::make_unique<FBHostProcessor>(context.get());
  hostProcessor->ProcessHost(inputBlock, outputBlock);

  return 0;
}