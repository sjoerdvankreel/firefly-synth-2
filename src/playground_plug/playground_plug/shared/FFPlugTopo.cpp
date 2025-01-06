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
  result->allocRawProcState = []() { return static_cast<void*>(new FFProcState); };
  result->allocRawScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result->freeRawProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result->freeRawScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };
  result->guiFactory = [](FBRuntimeTopo const* topo, IFBHostGUIContext* ctx) { 
    return std::make_unique<FFPlugGUI>(topo, ctx); };
  result->specialSelector = [](FBStaticTopo const& topo, void* state) {
    FBSpecialParams params = {};
    params.smoothing = MakeSpecialParam(topo, state, (int)FFModuleType::Master, (int)FFMasterParam::Smoothing);
    return params; };
  return result;
}
