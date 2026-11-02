#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_synth_diskwriter/FFDiskWriterContext.hpp>

int 
main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  // get options from commandline
  FFDiskWriterOptions options;
  options.bpm = 120;
  options.midiNote = 60;
  options.sampleRate = 48000;
  options.durationMilliseconds = 1000;
  options.outputRawPath = "c:\\temp\\testpatch123.raw";
  options.inputPatchPath = "c:\\temp\\testpatch123.ff2preset";

  // we render a single note hit
  FBNoteEvent noteOn;
  noteOn.pos = 0;
  noteOn.on = true;
  noteOn.velo = 1.0f;
  noteOn.note.id = 0;
  noteOn.note.channel = 0;
  noteOn.note.key = options.midiNote;

  // set up the input data -- no events other than a single note
  int maxFrameCount = options.sampleRate * options.durationMilliseconds / 1000;
  FBHostInputBlock inputBlock = {};
  inputBlock.projectTimeSamples = 0;
  inputBlock.bpm = (float)options.bpm;
  inputBlock.sampleCount = maxFrameCount;
  inputBlock.noteEvents.push_back(noteOn);

  // set up the input data buffers -- we won't use them, but the plug needs them
  // we are acting as the host right now
  std::array<std::vector<float>, 2> zeroInData = {};
  for (int ch = 0; ch < 2; ch++)
    zeroInData[ch] = std::vector<float>(maxFrameCount, 0.0f);
  float* zeroIn[2] = { zeroInData[0].data(), zeroInData[1].data() };
  inputBlock.mainAudio = FBHostAudioBlock(zeroIn, maxFrameCount);
  inputBlock.sidechainAudio = FBHostAudioBlock(zeroIn, maxFrameCount);

  // set up the output block, we will render everything in 1 pass
  FBHostOutputBlock outputBlock = {};
  std::array<std::vector<float>, 2> outData = {};
  for (int ch = 0; ch < 2; ch++)
    outData[ch] = std::vector<float>(maxFrameCount, 0.0f);
  float* out[2] = { outData[0].data(), outData[1].data() };
  outputBlock.audio = FBHostAudioBlock(out, maxFrameCount);

  // create the plugin context, load the patchfile, and set up all audio parameters on both gui and dsp side
  juce::MemoryBlock block;
  juce::File inFile(options.inputPatchPath);
  auto context = std::make_unique<FFDiskWriterContext>(options);
  if (!inFile.loadFileAsData(block))
    return 1;
  std::vector<std::uint8_t> data((uint8_t*)block.getData(), (uint8_t*)block.getData() + block.getSize());
  FBScalarStateContainer audioParams(*context->Topo());
  if (!context->Topo()->LoadPatchFromBytesWithPreFormat2Check(data, *context, audioParams))
    return 1;
  context->BulkUpdateAudioParamsToGUI(audioParams, true);
  context->PushProcStateToDSP();

  // make some noise
  auto hostProcessor = std::make_unique<FBHostProcessor>(context.get());
  hostProcessor->ProcessHost(inputBlock, outputBlock);

  // interleave the channels and dump to raw audio file
  std::vector<float> rawAudioInterleaved(2 * maxFrameCount, 0.0f);
  for (int f = 0; f < maxFrameCount; f++)
  {
    rawAudioInterleaved[f * 2] = out[0][f];
    rawAudioInterleaved[f * 2 + 1] = out[1][f];
  }
  juce::File f(options.outputRawPath);
  f.replaceWithData(rawAudioInterleaved.data(), rawAudioInterleaved.size() * sizeof(float));

  return 0;
}