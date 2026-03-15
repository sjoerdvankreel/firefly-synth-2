#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBMSEGEditor.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static void
UpdateMSEGModel(FBPlugGUI* plugGUI, int moduleSlot, FBMSEGModel& model);

FFEnvParamListener::
~FFEnvParamListener()
{
  _plugGUI->RemoveParamListener(this);
}

FFEnvParamListener::
FFEnvParamListener(FBPlugGUI* plugGUI, std::vector<FBMSEGEditor*> const& msegEditors) :
  _plugGUI(plugGUI),
  _msegEditors(msegEditors)
{
  _plugGUI->AddParamListener(this);
}

void
FFEnvParamListener::AudioParamChanged(
  int index, double /*normalized*/, bool /*changedFromUI*/)
{
  auto const& indices = _plugGUI->HostContext()->Topo()->audio.params[index].topoIndices;
  if (indices.module.index != (int)FFModuleType::Env)
    return;

  // Dont do it right away, if MSEG is being dragged, 
  // we're in the same call stack and stuff gets overwritten.
  MessageManager::callAsync([this, indices]() {
    UpdateMSEGModel(_plugGUI, indices.module.slot, _msegEditors[indices.module.slot]->Model());
    _msegEditors[indices.module.slot]->UpdateModel();
    });
}

static std::string
GetMSEGTooltip(FBPlugGUI* plugGUI, int moduleSlot, FBMSEGNearestHitType hitType, int index)
{
  if (hitType == FBMSEGNearestHitType::None)
    return std::string("");
  if (hitType == FBMSEGNearestHitType::Start)
    return "Start: " + plugGUI->HostContext()->GetAudioParamText({ {(int)FFModuleType::Env, moduleSlot}, {(int)FFEnvParam::StartLevel, 0} });
  if (hitType == FBMSEGNearestHitType::Slope)
    return "Slope " + std::to_string(index + 1) + ": " +
    plugGUI->HostContext()->GetAudioParamText({ {(int)FFModuleType::Env, moduleSlot}, {(int)FFEnvParam::StageSlope, index} });
  if (hitType == FBMSEGNearestHitType::Point)
  {
    bool sync = plugGUI->HostContext()->GetAudioParamBool({ {(int)FFModuleType::Env, moduleSlot}, {(int)FFEnvParam::Sync, 0} });
    auto timeOrBarsParam = sync ? FFEnvParam::StageBars : FFEnvParam::StageTime;
    return "Stage " + std::to_string(index + 1) + ": " +
      plugGUI->HostContext()->GetAudioParamText({ {(int)FFModuleType::Env, moduleSlot}, {(int)timeOrBarsParam, index} }) + "/" +
      plugGUI->HostContext()->GetAudioParamText({ {(int)FFModuleType::Env, moduleSlot}, {(int)FFEnvParam::StageLevel, index} });
  }
  FB_ASSERT(false);
  return std::string("");
}

static void
UpdateMSEGModel(FBPlugGUI* plugGUI, int moduleSlot, FBMSEGModel& model)
{
  model.points.clear();
  auto context = plugGUI->HostContext();
  auto snapXCounts = FFEnvMakeMSEGSnapXCounts();
  auto snapYCounts = FFEnvMakeMSEGSnapYCounts();
  auto const& staticTopo = *plugGUI->HostContext()->Topo()->static_;
  bool sync = context->GetAudioParamBool({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Sync, 0 } });
  auto type = context->GetAudioParamList<FFEnvType>({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Type, 0 } });
  int snapXIndex = context->GetGUIParamList<int>({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvGUIParam::MSEGSnapXCount, 0 } });
  int snapYIndex = context->GetGUIParamList<int>({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvGUIParam::MSEGSnapYCount, 0 } });
  model.xMode = sync ? FBMSEGXMode::Bars : FBMSEGXMode::Time;
  model.yMode = type == FFEnvType::Exp ? FBMSEGYMode::Exponential : FBMSEGYMode::Linear;
  model.enabled = type != FFEnvType::Off;
  model.xEditMode = context->GetGUIParamList<FBMSEGXEditMode>({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvGUIParam::MSEGXEditMode, 0 } });
  model.yEditMode = context->GetGUIParamList<FBMSEGYEditMode>({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvGUIParam::MSEGYEditMode, 0 } });
  model.snapXCount = snapXCounts[std::clamp(snapXIndex, 0, (int)snapXCounts.size() - 1)];
  model.snapYCount = snapYCounts[std::clamp(snapYIndex, 0, (int)snapYCounts.size() - 1)];
  model.startY = context->GetAudioParamNormalized({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StartLevel, 0 } });
  model.looping = context->GetAudioParamDiscrete({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } }) != 0;
  model.loopStart = std::max(0, context->GetAudioParamDiscrete({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } }) - 1);
  model.loopLength = context->GetAudioParamDiscrete({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } });
  model.releasing = context->GetAudioParamDiscrete({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } }) != 0;
  model.releasePoint = std::max(0, context->GetAudioParamDiscrete({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } }) - 1);
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    FBMSEGPoint point = {};
    point.y = context->GetAudioParamNormalized({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageLevel, i } });
    point.slope = context->GetAudioParamNormalized({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageSlope, i } });
    point.lengthTime = context->GetAudioParamLinear({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } });
    double barsNorm = context->GetAudioParamNormalized({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } });
    auto bars = staticTopo.modules[(int)FFModuleType::Env].params[(int)FFEnvParam::StageBars].BarsNonRealTime().NormalizedToBars(barsNorm);
    point.lengthBars = bars;
    model.points.push_back(point);
  }
}

static void
MSEGModelUpdated(FBPlugGUI* plugGUI, int moduleSlot, FBMSEGModel const& model)
{
  auto context = plugGUI->HostContext();
  auto topo = plugGUI->HostContext()->Topo();
  int snapXIndex = FFEnvIndexOfMSEGSnapXCount(model.snapXCount);
  int snapYIndex = FFEnvIndexOfMSEGSnapYCount(model.snapYCount);
  context->SetGUIParamList({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvGUIParam::MSEGXEditMode, 0 } }, (int)model.xEditMode);
  context->SetGUIParamList({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvGUIParam::MSEGYEditMode, 0 } }, (int)model.yEditMode);
  if (snapXIndex != -1)
    context->SetGUIParamList({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvGUIParam::MSEGSnapXCount, 0 } }, snapXIndex);
  if (snapYIndex != -1)
    context->SetGUIParamList({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvGUIParam::MSEGSnapYCount, 0 } }, snapYIndex);
  context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StartLevel, 0 } }, model.startY);
  if (model.releasing)
  {
    auto releaseTopo = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } });
    double releaseNorm = releaseTopo->static_.DiscreteNonRealTime().PlainToNormalized(model.releasePoint + 1);
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } }, releaseNorm);
  }
  else
  {
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } }, 0.0);
  }
  if (model.looping)
  {
    auto loopStartTopo = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } });
    auto loopLengthTopo = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } });
    double loopStartNorm = loopStartTopo->static_.DiscreteNonRealTime().PlainToNormalized(model.loopStart + 1);
    double loopLengthNorm = loopLengthTopo->static_.DiscreteNonRealTime().PlainToNormalized(model.loopLength);
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } }, loopStartNorm);
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } }, loopLengthNorm);
  }
  else
  {
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } }, 0.0);
  }
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageLevel, i } }, model.points[i].y);
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageSlope, i } }, model.points[i].slope);
    auto lengthTimeTopo = context->Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } });
    double lengthTimeNorm = lengthTimeTopo->static_.LinearNonRealTime().PlainToNormalized(model.points[i].lengthTime);
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } }, lengthTimeNorm);
    auto lengthBarsTopo = context->Topo()->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } });
    double lengthBarsPlain = lengthBarsTopo->static_.BarsNonRealTime().BarsToPlain(model.points[i].lengthBars);
    double lengthBarsNorm = lengthBarsTopo->static_.BarsNonRealTime().PlainToNormalized(lengthBarsPlain);
    context->PerformImmediateAudioParamEdit({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } }, lengthBarsNorm);
  }

  dynamic_cast<FFPlugGUI&>(*plugGUI).SwitchGraphsToModule((int)FFModuleType::Env, moduleSlot);
}

static Component*
MakeEnvSectionMain(FBPlugGUI* plugGUI, int moduleSlot, FBMSEGEditor** msegEditor)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, 0, -1, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Type, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Type", FBLabelAlign::Left, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto loopStart = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, loopStart));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, loopStart));
  auto loopLength = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, loopLength));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, loopLength));
  auto release = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, release));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, release));
  auto startLevel = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StartLevel, 0 } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, startLevel));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, startLevel, Slider::SliderStyle::LinearHorizontal));
  auto sync = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Sync, 0 } });
  grid->Add(0, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(0, 11, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto smoothTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothTime, 0 } });
  grid->Add(0, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothTime));
  grid->Add(0, 13, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smoothTime, Slider::SliderStyle::LinearHorizontal));
  auto smoothBars = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothBars, 0 } });
  grid->Add(0, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothBars));
  grid->Add(0, 13, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, smoothBars));
  auto showMSEG = plugGUI->StoreComponent<FBParamValueLinkedButton>(plugGUI, type, "MSEG", [](int v) { return v != 0; });
  showMSEG->setTooltip("Show MSEG Editor");
  grid->Add(0, 14, showMSEG);

  auto const& staticTopo = topo->static_->modules[(int)FFModuleType::Env];
  std::string title = staticTopo.slotFormatter(*topo->static_, moduleSlot);
  *msegEditor = plugGUI->StoreComponent<FBMSEGEditor>(
    plugGUI, title, FFEnvStageCount, FFEnvMaxTime, FBBarsItem{ FFEnvMaxBarsNum, FFEnvMaxBarsDen },
    FFEnvMakeBarsItems(), FFEnvMakeMSEGSnapXCounts(), FFEnvMakeMSEGSnapYCounts());
  UpdateMSEGModel(plugGUI, moduleSlot, (*msegEditor)->Model());
  (*msegEditor)->UpdateModel();
  (*msegEditor)->modelUpdated = [plugGUI, moduleSlot](FBMSEGModel const& model) {
    MSEGModelUpdated(plugGUI, moduleSlot, model); };
  (*msegEditor)->getTooltipFor = [plugGUI, moduleSlot](FBMSEGNearestHitType hitType, int index) {
    return GetMSEGTooltip(plugGUI, moduleSlot, hitType, index); };

  showMSEG->onClick = [plugGUI, msegEditor_ = *msegEditor, title, moduleSlot]() {
    dynamic_cast<FFPlugGUI&>(*plugGUI).ShowOverlayComponent(title, (int)FFModuleType::Env, moduleSlot, msegEditor_, 800, 240, true, [plugGUI, title, moduleSlot]() {
      plugGUI->HostContext()->UndoState().Snapshot("Init " + title + " MSEG");
      plugGUI->HostContext()->DefaultAudioParam({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } });
      plugGUI->HostContext()->DefaultAudioParam({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StartLevel, 0 } });
      plugGUI->HostContext()->DefaultAudioParam({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } });
      plugGUI->HostContext()->DefaultAudioParam({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } });
      for (int i = 0; i < FFEnvStageCount; i++)
      {
        plugGUI->HostContext()->DefaultAudioParam({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } });
        plugGUI->HostContext()->DefaultAudioParam({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageLevel, i } });
        plugGUI->HostContext()->DefaultAudioParam({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageSlope, i } });
        plugGUI->HostContext()->DefaultAudioParam({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } });
      }
      });
    };

  grid->MarkSection({ { 0, 0 }, { 1, 1 } }, FBGridSectionMark::DefaultBackground);
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
MakeEnvTab(FBPlugGUI* plugGUI, int moduleSlot, FBMSEGEditor** msegEditor)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 1 });
  grid->Add(0, 0, MakeEnvSectionMain(plugGUI, moduleSlot, msegEditor));
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::Env, moduleSlot, grid);
}

static Component*
MakeEnvDetail(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, -1, -1, std::vector<int>(6, 0), std::vector<int> { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Length"));
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Level"));
  grid->Add(2, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Slope"));
  grid->Add(3, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Length"));
  grid->Add(4, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Level"));
  grid->Add(5, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Slope"));
  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "1-8"));
  grid->Add(1, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "1-8"));
  grid->Add(2, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "1-8"));
  grid->Add(3, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "9-16"));
  grid->Add(4, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "9-16"));
  grid->Add(5, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "9-16"));
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    int r = i >= 8 ? 3 : 0;
    int c = i >= 8 ? i - 8 : i;
    auto time = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } });
    grid->Add(r + 0, c + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, time, Slider::SliderStyle::RotaryVerticalDrag));
    auto bars = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } });
    grid->Add(r + 0, c + 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bars));
    auto level = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot} , { (int)FFEnvParam::StageLevel, i } });
    grid->Add(r + 1, c + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, level, Slider::SliderStyle::RotaryVerticalDrag));
    auto slope = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageSlope, i } });
    grid->Add(r + 2, c + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, slope, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 1, 10 } }, FBGridSectionMark::AlternateBackground);
  grid->MarkSection({ { 2, 0 }, { 1, 10 } }, FBGridSectionMark::AlternateBackground);
  grid->MarkSection({ { 3, 0 }, { 1, 10 } }, FBGridSectionMark::AlternateBackground);
  grid->MarkSection({ { 5, 0 }, { 1, 10 } }, FBGridSectionMark::AlternateBackground);
  return grid;
}

Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI, std::vector<FBMSEGEditor*>& msegEditors)
{
  FB_LOG_ENTRY_EXIT();
  msegEditors.clear();
  FBMSEGEditor* msegEditor = nullptr;
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::EnvSelectedTab, 0 } });
  auto select = plugGUI->StoreComponent<FBSelectComponent>(plugGUI, moduleParam, std::vector<int> { 1 }, std::vector<int> { 1, 0, 0, 0, 0, 0, 0, 0 });
  select->AddLabel(0, 0, "ENV");
  select->AddSelector(0, 1, { (int)FFModuleType::Env, 0 }, "Amp", MakeEnvTab(plugGUI, FFAmpEnvSlot, &msegEditor));
  msegEditors.push_back(msegEditor);
  for (int i = FFEnvSlotOffset; i < FFEnvCount; i++)
  {
    msegEditor = nullptr;
    select->AddSelector(0, 1 + i, { (int)FFModuleType::Env, i }, std::to_string(i), MakeEnvTab(plugGUI, i, &msegEditor));
    msegEditors.push_back(msegEditor);
  }
  select->ActivateStoredSelection();

  PopupMenu insertMenu;
  PopupMenu removeMenu;
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    insertMenu.addItem(1000 + i, std::to_string(i + 1));
    removeMenu.addItem(2000 + i, std::to_string(i + 1));
  }
  select->extendedMenu.addSubMenu("Remove Stage At", removeMenu);
  select->extendedMenu.addSubMenu("Insert Stage Before", insertMenu);
  select->extendedMenuHandler = [](FBPlugGUI* plugGUI, FBTopoIndices const& indices, int id) {

    std::vector<FFEnvParam> stageParams = {
      FFEnvParam::StageTime, FFEnvParam::StageBars,
      FFEnvParam::StageLevel, FFEnvParam::StageSlope };

    if (1000 <= id && id < 1000 + FFEnvStageCount)
    {
      int stage = id - 1000;
      plugGUI->HostContext()->UndoState().Snapshot("Insert Stage Before " + std::to_string(stage + 1));
      for (int i = FFEnvStageCount - 1; i > stage; i--)
        for (int j = 0; j < stageParams.size(); j++)
          plugGUI->HostContext()->CopyAudioParam(
            { indices, { (int)stageParams[j], i - 1 } }, { indices, { (int)stageParams[j], i } });
    }

    else if (2000 <= id && id < 2000 + FFEnvStageCount)
    {
      int stage = id - 2000;
      plugGUI->HostContext()->UndoState().Snapshot("Remove Stage At " + std::to_string(stage + 1));
      for (int i = stage; i < FFEnvStageCount - 1; i++)
        for (int j = 0; j < stageParams.size(); j++)
          plugGUI->HostContext()->CopyAudioParam(
            { indices, { (int)stageParams[j], i + 1 } }, { indices, { (int)stageParams[j], i } });
    }
  };

  return plugGUI->StoreComponent<FBThemedComponent>(plugGUI, (int)FFThemedComponentId::EnvSelector, select);
}

Component*
FFMakeEnvDetailGUI(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  int index = topo->moduleTopoToRuntime.at({ (int)FFModuleType::Env, moduleSlot });
  auto name = topo->modules[index].name;
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Type, 0 } });
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 1 }, std::vector<int> { 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, FBAsciiToUpper(name), FBLabelAlign::Right, FBLabelColors::PrimaryForeground));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamDisplayLabel>(plugGUI, type));
  grid->Add(1, 0, 1, 2, MakeEnvDetail(plugGUI, moduleSlot));
  grid->MarkSection({ { 0, 0 }, { 1, 2 } }, FBGridSectionMark::DefaultBackground);
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, true, true, false, true, card);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::Env, moduleSlot, margin);
}