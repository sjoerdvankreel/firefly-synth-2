#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>
#include <firefly_base/base/state/main/FBScalarStateContainer.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>

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
    auto data = Topo()->SaveStateToBytes(*this, *_editState, false);
    while ((numWritten = stream->write(stream, data.data() + written, data.size() - written)) != 0)
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
    std::vector<std::uint8_t> data;
    while ((read = stream->read(stream, buffer, sizeof(buffer))) != 0)
      if (read == -1)
        return false;
      else
        data.insert(data.end(), buffer, buffer + read);

    if (!Topo()->LoadCLAPFromBytesWithPreFormat2Check(data, *this, *_editState))
      return true; // bitwig
    PushProcStateToDSP();

    OnPatchNameChanged();
    OnInstanceNameChanged();
    OnGUIAudioClipsChanged();
    if(_gui)
      for(int i = 0; i < GUIState()->Params().size(); i++)
        _gui->SetGUIParamNormalizedFromHost(i, GetGUIParamNormalized(i));
    for (int i = 0; i < _editState->Params().size(); i++)
    {
      double normalized = *_editState->Params()[i];
      if (_gui)
        _gui->SetAudioParamNormalizedFromHost(i, normalized);
    }

    MarkPatchAsSessionState();
    NotifyHostOfParamNameChangesOnly();
    return true;
  });
}