#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <public.sdk/source/vst/utility/dataexchange.h>

#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBStaticTopo;
struct FBRuntimeTopo;

class FBVST3GUIEditor;
class FBGUIStateContainer;
class FBExchangeStateContainer;

class FBVST3EditController final:
public EditControllerEx1,
public IDataExchangeReceiver,
public FBHostGUIContext
{
  FBVST3GUIEditor* _guiEditor = {};
  std::unique_ptr<FBRuntimeTopo> _topo;
  std::unique_ptr<FBGUIStateContainer> _guiState;
  std::unique_ptr<FBExchangeStateContainer> _exchangeState;
  DataExchangeReceiverHandler _exchangeHandler;

public:
  DEFINE_INTERFACES
    DEF_INTERFACE(IDataExchangeReceiver)
  END_DEFINE_INTERFACES(EditController)
  DELEGATE_REFCOUNT(EditController)

  ~FBVST3EditController();
  FBVST3EditController(FBStaticTopo const& topo);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3EditController);

  void ResetView();

  void EndAudioParamChange(int index) override;
  void BeginAudioParamChange(int index) override;
  void PerformAudioParamEdit(int index, double normalized) override;

  double GetAudioParamNormalized(int index) const override;
  double GetGUIParamNormalized(int index) const override;
  void SetGUIParamNormalized(int index, double normalized) override;

  void AudioParamContextMenuClicked(int paramIndex, int juceTag) override;
  std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int index) override;

  FBRuntimeTopo const* Topo() const override { return _topo.get(); }
  FBGUIStateContainer* GUIState() override { return _guiState.get(); }
  FBExchangeStateContainer const* ExchangeState() const override { return _exchangeState.get(); }

  tresult PLUGIN_API notify(IMessage* message) override;
  tresult PLUGIN_API setState(IBStream* state) override;
  tresult PLUGIN_API getState(IBStream* state) override;
  IPlugView* PLUGIN_API createView(FIDString name) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setComponentState(IBStream* state) override;
  void PLUGIN_API queueClosed(DataExchangeUserContextID /*id*/) override {}
  tresult PLUGIN_API setParamNormalized(ParamID tag, ParamValue value) override;
  void PLUGIN_API queueOpened(DataExchangeUserContextID /*id*/, uint32 /*blockSize*/, TBool& /*bg*/) override {}
  void PLUGIN_API onDataExchangeBlocksReceived(DataExchangeUserContextID id, uint32 numBlocks, DataExchangeBlock* blocks, TBool bg) override;
};