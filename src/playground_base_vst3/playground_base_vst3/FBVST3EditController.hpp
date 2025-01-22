#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBStringify.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBGUIState;
struct FBStaticTopo;
struct FBRuntimeTopo;

class FBVST3GUIEditor;

class FBVST3EditController final:
public EditControllerEx1,
public FBHostGUIContext
{
  FBVST3GUIEditor* _guiEditor = {};
  std::unique_ptr<FBRuntimeTopo> _topo;
  std::unique_ptr<FBGUIState> _guiState;

public:
  ~FBVST3EditController();
  FBVST3EditController(FBStaticTopo const& topo);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3EditController);

  void ResetView();
  void EndParamChange(int index) override;
  void BeginParamChange(int index) override;
  float GetParamNormalized(int index) const override;
  void PerformParamEdit(int index, float normalized) override;
  void ParamContextMenuClicked(int paramIndex, int itemTag) override;
  std::unique_ptr<juce::PopupMenu> MakeParamContextMenu(int index) override;

  IPlugView* PLUGIN_API createView(FIDString name) override;
  tresult PLUGIN_API setState(IBStream* state) override;
  tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setComponentState(IBStream* state) override;
  tresult PLUGIN_API setParamNormalized(ParamID tag, ParamValue value) override;
};