#include <playground_base/pipeline/FBPlugToHostProcessor.hpp>
#include <playground_base/shared/FBHostBlock.hpp>
#include <cassert>

void 
FBPlugToHostProcessor::FromPlug(FBPlugAudioBlock const& plug)
{
  for (int s = 0; s < FB_PLUG_BLOCK_SIZE; s++)
    _pipeline->Append(plug[0][s], plug[1][s]);
}

void 
FBPlugToHostProcessor::ToHost(FBHostAudioBlock& host)
{
  if (_pipeline->Count() >= FB_PLUG_BLOCK_SIZE)
    _hitPlugBlockSize = true;
  
  if (!_hitPlugBlockSize)
  {
    for (int s = 0; s < host.Count(); s++)
      host.Set(s, 0.0f, 0.0f);
    return;
  }

  int padded = std::max(0, host.Count() - _pipeline->Count());
  assert(!_paddedOnce || padded == 0);
  _paddedOnce |= padded > 0;
  for (int s = 0; s < padded; s++)
    host.Set(s, 0.0f, 0.0f);

  int used = std::min(host.Count(), _pipeline->Count());
  for (int s = 0; s < used; s++)
    host.Set(s + padded, _pipeline->At(0, s), _pipeline->At(1, s));
  _pipeline->Drop(used);
}