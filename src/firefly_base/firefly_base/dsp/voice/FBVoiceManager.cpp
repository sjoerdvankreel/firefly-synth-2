#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/proc/FBVoiceBlockParamState.hpp>

#include <limits>

FBVoiceManager::
FBVoiceManager(FBProcStateContainer* procState) :
_procState(procState) {}

void
FBVoiceManager::Return(int slot)
{
  FB_ASSERT(_voiceCount > 0);
  _voiceCount--;
  _voices[slot].state = FBVoiceState::Returned;
  _returnedVoices.push_back(_voices[slot].event.note);
  FB_ASSERT(_returnedVoices.size() < FBMaxVoices);
}

void
FBVoiceManager::ResetReturnedVoices()
{
  _returnedVoices.clear();
  for (int v = 0; v < FBMaxVoices; v++)
    if (IsReturned(v))
      _voices[v].state = FBVoiceState::Free;
}

void 
FBVoiceManager::InitFromExchange(
  std::array<FBVoiceInfo, FBMaxVoices> const& voices)
{
  _counter = 0;
  _voiceCount = 0;
  _returnedVoices.clear();
  for (int i = 0; i < FBMaxVoices; i++)
  {
    _num[i] = i;
    _voices[i].slot = i;
    _voices[i].event = voices[i].event;
    _voices[i].state = voices[i].state;
    _voices[i].initialOffset = voices[i].initialOffset;
    _counter++;
    if (_voices[i].state == FBVoiceState::Active)
      _voiceCount++;
  }
}

int
FBVoiceManager::Lease(FBNoteEvent const& event)
{
  FB_ASSERT(event.on);

  int slot = -1;
  for (int v = 0; v < _voices.size() && slot == -1; v++)
    if (IsFree(v))
    {
      slot = v;
      _voiceCount++;
      FB_ASSERT(_voiceCount <= FBMaxVoices);
    }

  std::uint64_t oldest = std::numeric_limits<std::uint64_t>::max();
  if (slot == -1)
    for (int v = 0; v < _voices.size(); v++)
      if (_num[v] < oldest)
      {
        slot = v;
        oldest = _num[v];
      }

  FB_ASSERT(0 <= slot && slot < _voices.size());
  if (IsActive(slot))
    _returnedVoices.push_back(_voices[slot].event.note);

  _num[slot] = ++_counter;
  _voices[slot].slot = slot;
  _voices[slot].event = event;
  _voices[slot].initialOffset = event.pos;
  _voices[slot].state = FBVoiceState::Active;

  for (int p = 0; p < _procState->Params().size(); p++)
    if (_procState->Params()[p].IsVoice())
      if (!_procState->Params()[p].IsAcc())
        _procState->Params()[p].VoiceBlock().BeginVoice(slot);
      else
        _procState->Params()[p].VoiceAcc().Voice()[slot].InitProcessing(_procState->Params()[p].Value());
  return slot;
}