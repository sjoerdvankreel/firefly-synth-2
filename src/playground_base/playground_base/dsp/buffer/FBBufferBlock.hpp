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
  void Drop(int count);
  void AppendHost(FBHostAudioBlock const& rhs);
  void AppendFixed(FBSIMDArray2<float, FBFixedBlockSamples, 2> const& rhs);

  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferAudioBlock);
  int Count() const { return static_cast<int>(_store[0].size()); }
  std::vector<float> const& operator[](int ch) const { return _store[ch]; }
};

struct FBBufferInputBlock final
{
  FBBufferAudioBlock audio = {};
  std::vector<FBNoteEvent> note = {};
  std::vector<FBAccAutoEvent> accAutoByParamThenSample = {};
  std::vector<FBAccModEvent> accModByParamThenNoteThenSample = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferInputBlock);
};