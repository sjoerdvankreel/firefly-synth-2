#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>

bool 
FBCLAPPlugin::implementsState() const noexcept 
{
  return true;
}

bool
FBCLAPPlugin::stateSave(const clap_ostream* stream) noexcept
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, stream]()
  {
    int64_t written = 0;
    int64_t numWritten = 0;
    std::string json = _topo->SaveEditAndGUIStateToString(*_editState, *_guiState);
    while ((numWritten = stream->write(stream, json.data() + written, json.size() - written)) != 0)
      if (numWritten == -1)
        return false;
      else
      {
        written += numWritten;
        numWritten = 0;
      }
    return true;
  });
}

bool
FBCLAPPlugin::stateLoad(const clap_istream* stream) noexcept
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, stream]()
  {
    int64_t read = 0;
    char buffer[1024];
    std::string json = {};
    while ((read = stream->read(stream, buffer, sizeof(buffer))) != 0)
      if (read == -1)
        return false;
      else
        json.append(buffer, read);

    UndoState().BeginAction("Load State");
    _topo->LoadEditAndGUIStateFromStringWithDryRun(json, *_editState, *_guiState);
    for (int i = 0; i < _editState->Params().size(); i++)
    {
      double normalized = *_editState->Params()[i];
      _procState->InitProcessing(i, static_cast<float>(normalized));
      if (_gui)
        _gui->SetAudioParamNormalizedFromHost(i, normalized);
    }
    UndoState().EndAction();
    UndoState().Clear();
    return true;
  });
}