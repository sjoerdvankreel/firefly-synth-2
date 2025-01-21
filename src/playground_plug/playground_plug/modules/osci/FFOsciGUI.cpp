#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component*
MakeSectionPW(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 1 });
  auto pw = plugGUI->Topo()->ParamAtTopo({(int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::PW, 0});
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(plugGUI, pw));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamSlider>(plugGUI, pw, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->AddComponent<FBSectionComponent>(plugGUI, grid);
}

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0 });
  auto on = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::On, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamToggleButton>(plugGUI, on));
  auto type = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamComboBox>(plugGUI, type));
  auto note = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  grid->Add(0, 4, plugGUI->AddComponent<FBParamLabel>(plugGUI, note));
  grid->Add(0, 5, plugGUI->AddComponent<FBParamComboBox>(plugGUI, note));
  auto cent = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  grid->Add(0, 6, plugGUI->AddComponent<FBParamLabel>(plugGUI, cent));
  grid->Add(0, 7, plugGUI->AddComponent<FBParamSlider>(plugGUI, cent, Slider::SliderStyle::Rotary));
  return plugGUI->AddComponent<FBSectionComponent>(plugGUI, grid);
}

static Component*
MakeSectionGain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto gain1 = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(plugGUI, gain1));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamSlider>(plugGUI, gain1, Slider::SliderStyle::Rotary));
  auto gain2 = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 1 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(plugGUI, gain2));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamSlider>(plugGUI, gain2, Slider::SliderStyle::Rotary));
  auto gLFOToGain = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  grid->Add(0, 4, plugGUI->AddComponent<FBParamLabel>(plugGUI, gLFOToGain));
  grid->Add(0, 5, plugGUI->AddComponent<FBParamSlider>(plugGUI, gLFOToGain, Slider::SliderStyle::Rotary));
  // TODO less ugly
  FBTopoIndices indices;
  indices.index = (int)FFModuleType::Osci;
  indices.slot = moduleSlot;
  FBParamsDependency dependency;
  dependency.When({ (int)FFOsciParam::On }, [](auto const& vs) { return vs[0] != 0; });
  return plugGUI->AddComponent<FBSectionComponent>(plugGUI, indices, dependency, grid);
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto result = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 1 });
  result->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  result->Add(0, 1, MakeSectionGain(plugGUI, moduleSlot));
  result->Add(0, 2, MakeSectionPW(plugGUI, moduleSlot));
  return result;
}

Component*
FFMakeOsciGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Osci, TabFactory);
}