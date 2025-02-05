#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostProcessor.hpp>

bool 
FBCLAPPlugin::implementsNotePorts() const noexcept
{
  return true;
}

uint32_t 
FBCLAPPlugin::notePortsCount(bool isInput) const noexcept
{
  return isInput ? 1 : 0;
}

bool
FBCLAPPlugin::notePortsInfo(uint32_t index, bool isInput, clap_note_port_info* info) const noexcept 
{
  if (!isInput)
    return false;
  if (index != 0)
    return false;

  info->id = 0;
  info->name[0] = 0;
  info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
  // TODO actually handle ALL midi not just what vst3 can do?
  info->supported_dialects = CLAP_NOTE_DIALECT_CLAP/* | CLAP_NOTE_DIALECT_MIDI*/;
  return true;
}