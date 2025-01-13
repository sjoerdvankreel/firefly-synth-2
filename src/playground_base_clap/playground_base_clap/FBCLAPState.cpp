#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/glue/FBPlugGUIContext.hpp>

bool 
FBCLAPPlugin::implementsState() const noexcept 
{
  return true;
}

bool
FBCLAPPlugin::stateSave(const clap_ostream* stream) noexcept
{
  int64_t written = 0;
  int64_t numWritten = 0;
  std::string json = _topo->SaveState(_guiState);
  while ((numWritten = stream->write(stream, json.data() + written, json.size() - written)) != 0)
    if (numWritten == -1)
      return false;
    else
    {
      written += numWritten;
      numWritten = 0;
    }
  return true;
}

bool
FBCLAPPlugin::stateLoad(const clap_istream* stream) noexcept
{
  int64_t read = 0;
  char buffer[1024];
  std::string json = {};
  while ((read = stream->read(stream, buffer, sizeof(buffer))) != 0)
    if (read == -1)
      return false;
    else
      json.append(buffer, read);
  if (!_topo->LoadStateWithDryRun(json, _guiState))
    return false;
  for (int i = 0; i < _guiState.Params().size(); i++)
  {
    float normalized = *_guiState.Params()[i];
    PerformParamEdit(i, normalized);
    if (_gui)
      _gui->SetParamNormalized(i, normalized);
  }
}