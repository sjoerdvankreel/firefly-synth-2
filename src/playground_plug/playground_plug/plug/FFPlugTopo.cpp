#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>

FBStaticModule FFMakeGLFOTopo();
FBStaticModule FFMakeOsciTopo();
FBStaticModule FFMakeMasterTopo();

std::unique_ptr<FBStaticTopo>
FFMakeTopo()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->modules.resize(FFModuleCount);
  result->version.major = FF_PLUG_VERSION_MAJOR;
  result->version.minor = FF_PLUG_VERSION_MINOR;
  result->version.patch = FF_PLUG_VERSION_PATCH;
  result->modules[FFModuleGLFO] = FFMakeGLFOTopo();
  result->modules[FFModuleOsci] = FFMakeOsciTopo();
  result->modules[FFModuleMaster] = FFMakeMasterTopo();
  result->modules[FFModuleGFilter] = std::move(*FFMakeGFilterTopo());
  result->allocRawProcState = []() { return static_cast<void*>(new FFProcState); };
  result->allocRawScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result->freeRawProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result->freeRawScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };
  result->specialSelector = [](FBStaticTopo const& topo, void* state) {
    FBSpecialParams params = {};
    params.smooth = topo.modules[FFModuleMaster].params[FFMasterBlockSmooth].globalBlockAddr(0, 0, state);
    return params; };
  return result;
}
