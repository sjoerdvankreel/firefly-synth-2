#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

#include <limits>
#include <cassert>

void 
FBVoiceManager::Return(int leased)
{
  assert(_voices[leased].active);
  _voices[leased].active = false;
}

int 
FBVoiceManager::Lease(FBNoteEvent const& event)
{
  int result = -1;
  for (int i = 0; i < _voices.size() && result == -1; i++)
    if (!_voices[i].active)
      result = i;

  std::uint64_t oldest = std::numeric_limits<std::uint64_t>::max();
  if(result == -1)
    for(int i = 0; i < _voices.size(); i++)
      if (_num[i] < oldest)
      {
        result = i;
        oldest = _num[i];
      }

  assert(0 <= result && result < _voices.size());
  _num[result] = _counter++;
  _voices[result].event = event;
  _voices[result].active = true;
  return result;
}