#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/host/FBHostBlock.hpp>

#include <array>
#include <vector>

class FBHostAudioBlock;

class FBBufferAudioBlock final
{
  std::array<std::vector<float>, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferAudioBlock);
  int Count() const { return static_cast<int>(_store[0].size()); }
  std::vector<float> const& operator[](int ch) const { return _store[ch]; }

  void Drop(int count);
  void AppendHostAudio(FBHostAudioBlock const& hostAudio);
  void AppendPlugAudio(FBSIMDArray2<float, FBFixedBlockSamples, 2> const& plugAudio);
};

struct FBBufferInputBlock final
{
  FBBufferAudioBlock audio = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferInputBlock);
};

struct FBFixedInputBlock final
{
  std::vector<FBNoteEvent> note = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};
  FBSIMDArray2<float, FBFixedBlockSamples, 2> audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedInputBlock);
};