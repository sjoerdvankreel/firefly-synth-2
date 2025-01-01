#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>

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
  result->specialSelector = [](FBStaticTopo const& topo, void* state) {
    FBSpecialParams params = {};
    params.smooth = topo.modules[(int)FFModuleType::Master].params[(int)FFMasterParam::Smooth].globalBlockAddr(0, 0, state);
    return params; };
  return result;
}
