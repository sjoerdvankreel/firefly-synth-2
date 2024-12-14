#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

FBStaticModule FFMakeOsciTopo();

std::unique_ptr<FBStaticTopo>
FFMakeTopo()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->modules.resize(FFModuleCount);
  result->version.major = FF_PLUG_VERSION_MAJOR;
  result->version.minor = FF_PLUG_VERSION_MINOR;
  result->version.patch = FF_PLUG_VERSION_PATCH;
  result->allocProcState = []() { return static_cast<void*>(new FFProcState); };
  result->allocScalarState = []() { return static_cast<void*>(new FFScalarState); };
  result->freeProcState = [](void* state) { delete static_cast<FFProcState*>(state); };
  result->freeScalarState = [](void* state) { delete static_cast<FFScalarState*>(state); };
  result->modules[FFModuleOsci] = FFMakeOsciTopo();
  result->modules[FFModuleGLFO] = std::move(*FFMakeGLFOTopo());
  result->modules[FFModuleMaster] = std::move(*FFMakeMasterTopo()); 
  return result;
}
