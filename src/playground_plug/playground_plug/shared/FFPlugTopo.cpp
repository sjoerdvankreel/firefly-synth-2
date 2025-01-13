#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

static FBSpecialParam
MakeSpecialParam(
  FBStaticTopo const& topo, void* state, 
  int moduleIndex, int paramIndex)
{
  FBSpecialParam result;
  result.paramIndex = paramIndex;
  result.moduleIndex = moduleIndex;
  result.state = topo.modules[moduleIndex].params[paramIndex].globalBlockAddr(0, 0, state);
  return result;
}

std::unique_ptr<FBStaticTopo>
FFMakeTopo()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->modules.resize((int)FFModuleType::Count);
  result->version.major = FF_PLUG_VERSION_MAJOR;
  result->version.minor = FF_PLUG_VERSION_MINOR;
  result->version.patch = FF_PLUG_VERSION_PATCH;
  result->modules[(int)FFModuleType::GLFO] = std::move(*FFMakeGLFOTopo());
  result->modules[(int)FFModuleType::Osci] = std::move(*FFMakeOsciTopo());
  result->modules[(int)FFModuleType::Master] = std::move(*FFMakeMasterTopo());
  result->modules[(int)FFModuleType::GFilter] = std::move(*FFMakeGFilterTopo());
  result->gui.plugWidth = 800;
  result->gui.minUserScale = 0.5f;
  result->gui.maxUserScale = 1.5f; // TODO
  result->gui.aspectRatioWidth = 4;
  result->gui.aspectRatioHeight = 3;
  result->gui.factory = [](FBRuntimeTopo const* topo, IFBHostGUIContext* ctx) {
    return std::make_unique<FFPlugGUI>(topo, ctx); };
  result->state.allocRawProcState = []() { return static_cast<void*>(new FFProcState); };
  result->state.allocRawScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result->state.freeRawProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result->state.freeRawScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };
  result->state.specialSelector = [](FBStaticTopo const& topo, void* state) {
    FBSpecialParams params = {};
    params.smoothing = MakeSpecialParam(topo, state, (int)FFModuleType::Master, (int)FFMasterParam::Smoothing);
    return params; };
  return result;
}
