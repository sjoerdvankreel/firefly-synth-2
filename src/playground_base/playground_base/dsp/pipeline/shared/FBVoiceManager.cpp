#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBVoiceBlockParamState.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

#include <limits>
#include <cassert>

FBVoiceManager::
FBVoiceManager(FBProcStateContainer* state) :
_state(state) {}

void
FBVoiceManager::ResetReturnedVoices()
{
  _returnedVoices.clear();
  for (int v = 0; v < FBMaxVoices; v++)
    if (IsReturned(v))
      _voices[v].state = FBVoiceState::Free;
}

void
FBVoiceManager::ReturnOldest(FBNoteEvent const& event)
{
  assert(!event.on);

  int slot = -1;
  std::uint64_t oldest = std::numeric_limits<std::uint64_t>::max();
  for (int v = 0; v < _voices.size(); v++)
    if (event.note.Matches(_voices[v].event.note))
      if (IsActive(v) && _num[v] < oldest)
      {
        slot = v;
        oldest = _num[v];
      }

  if (slot != -1)
  {
    _voices[slot].state = FBVoiceState::Returned;
    _returnedVoices.push_back(_voices[slot].event.note);
    assert(_returnedVoices.size() < FBMaxVoices);
  }
}

int
FBVoiceManager::Lease(FBNoteEvent const& event)
{
  assert(event.on);

  int slot = -1;
  for (int v = 0; v < _voices.size() && slot == -1; v++)
    if (IsFree(v))
      slot = v;

  std::uint64_t oldest = std::numeric_limits<std::uint64_t>::max();
  if (slot == -1)
    for (int v = 0; v < _voices.size(); v++)
      if (_num[v] < oldest)
      {
        slot = v;
        oldest = _num[v];
      }

  assert(0 <= slot && slot < _voices.size());
  if (IsActive(slot))
    _returnedVoices.push_back(_voices[slot].event.note);

  _num[slot] = ++_counter;
  _voices[slot].slot = slot;
  _voices[slot].event = event;
  _voices[slot].initialOffset = event.pos;
  _voices[slot].state = FBVoiceState::Active;

  for (int p = 0; p < _state->Params().size(); p++)
    if (_state->Params()[p].IsVoice())
      if (!_state->Params()[p].IsAcc())
        _state->Params()[p].VoiceBlock().BeginVoice(slot);
      else
        _state->Params()[p].VoiceAcc().Voice()[slot].InitProcessing(_state->Params()[p].Value());

  return slot;
}