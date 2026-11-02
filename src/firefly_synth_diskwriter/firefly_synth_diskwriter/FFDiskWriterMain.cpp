#include <firefly_base/dsp/shared/FBTuning.hpp>
#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth_diskwriter/FFDiskWriterContext.hpp>

#include <juce_audio_formats/juce_audio_formats.h>
#include <iostream>

static bool
RenderPatchToFile(FFDiskWriterOptions const& options)
{
  // we render a single note hit
  FBNoteEvent noteOn;
  noteOn.pos = 0;
  noteOn.on = true;
  noteOn.velo = 1.0f;
  noteOn.note.id = 0;
  noteOn.note.channel = 0;
  noteOn.note.key = options.midiNote;

  FBNoteEvent noteOff;
  noteOff.pos = options.sampleRate * options.holdDurationMilliseconds / 1000;
  noteOff.on = false;
  noteOff.velo = 1.0f;
  noteOff.note.id = 0;
  noteOff.note.channel = 0;
  noteOff.note.key = options.midiNote;

  // set up the input data -- no events other than a single note on/off
  int maxFrameCount = options.sampleRate * options.totalDurationMilliseconds / 1000;
  maxFrameCount += FBFixedBlockSamples; // because of the delay, we drop it later
  FBHostInputBlock inputBlock = {};
  inputBlock.projectTimeSamples = 0;
  inputBlock.bpm = (float)options.bpm;
  inputBlock.sampleCount = maxFrameCount;
  inputBlock.noteEvents.push_back(noteOn);
  inputBlock.noteEvents.push_back(noteOff);

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
  {
    std::cerr << "Load file failed." << std::endl;
    return false;
  }
  std::vector<std::uint8_t> data((uint8_t*)block.getData(), (uint8_t*)block.getData() + block.getSize());
  FBScalarStateContainer audioParams(*context->Topo());
  if (!context->Topo()->LoadPatchFromBytesWithPreFormat2Check(data, *context, audioParams))
  {
    std::cerr << "Load patch failed." << std::endl;
    return false;
  }
  context->BulkUpdateAudioParamsToGUI(audioParams, true);
  context->PushProcStateToDSP();

  // make some noise
  auto hostProcessor = std::make_unique<FBHostProcessor>(context.get());
  hostProcessor->ProcessHost(inputBlock, outputBlock);

  // dump to wav file
  juce::AudioFormatWriterOptions juceOptions;
  juceOptions = juceOptions.withNumChannels(2);
  juceOptions = juceOptions.withBitsPerSample(32);
  juceOptions = juceOptions.withSampleRate(options.sampleRate);
  juceOptions = juceOptions.withSampleFormat(juce::AudioFormatWriterOptions::SampleFormat::floatingPoint);

  juce::WavAudioFormat format;
  out[0] = out[0] + FBFixedBlockSamples;
  out[1] = out[1] + FBFixedBlockSamples;
  juce::File outFile(options.outputWavPath);
  outFile.deleteFile();
  std::unique_ptr<juce::OutputStream> outStream = std::make_unique<juce::FileOutputStream>(outFile);
  auto writer = format.createWriterFor(outStream, juceOptions);
  if (!writer)
  {
    std::cerr << "Create writer failed." << std::endl;
    return false;
  }
  if (!writer->writeFromFloatArrays(out, 2, maxFrameCount - FBFixedBlockSamples))
  {
    std::cerr << "Write audio failed." << std::endl;
    return false;
  }

  return true;
}

static bool
ParseOptions(int argc, char** argv, FFDiskWriterOptions& options)
{
  if (argc != 8)
  {
    std::cerr << "Usage: FireflySynth2DiskWriter inputPatchPath outputWavPath sampleRate midiNote bpm totalDurationMilliseconds holdDurationMilliseconds" << std::endl;
    return false;
  }

  options.inputPatchPath = argv[1];
  if (!juce::File(options.inputPatchPath).exists())
  {
    std::cerr << "Input file not found." << std::endl;
    return false;
  }

  options.outputWavPath = argv[2];

  options.sampleRate = std::stoi(argv[3]);
  if (options.sampleRate < 1 || options.sampleRate > 192000)
  {
    std::cerr << "Invalid sample rate." << std::endl;
    return false;
  }
  
  options.midiNote = std::stoi(argv[4]);
  if (options.midiNote < 0 || options.midiNote > 127)
  {
    std::cerr << "Invalid midi note." << std::endl;
    return false;
  }

  options.bpm = std::stoi(argv[5]);
  if (options.bpm < 1 || options.bpm > 1000)
  {
    std::cerr << "Invalid bpm." << std::endl;
    return false;
  }

  options.totalDurationMilliseconds = std::stoi(argv[6]);
  if (options.totalDurationMilliseconds < 1 || options.totalDurationMilliseconds > 10000)
  {
    std::cerr << "Invalid total duration." << std::endl;
    return false;
  }

  options.holdDurationMilliseconds = std::stoi(argv[7]);
  if (options.holdDurationMilliseconds < 1 || options.holdDurationMilliseconds > 10000)
  {
    std::cerr << "Invalid hold duration." << std::endl;
    return false;
  }

  return true;
}

int 
main(int argc, char** argv)
{
  auto meta = FFPlugMeta(FBPlugFormat::VST3, false);
  FBLogInit(meta);
  FBUtilityInit(meta);
  FBTuningInit();
  
  FFDiskWriterOptions options;
  bool result = ParseOptions(argc, argv, options);
  if(result)
    result &= RenderPatchToFile(options);

  FBTuningTerminate();
  FBUtilityTerminate();
  FBLogTerminate();
  return result ? 0 : 1;
}