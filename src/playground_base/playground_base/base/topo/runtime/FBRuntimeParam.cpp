#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

FBRuntimeParam::
FBRuntimeParam(
  FBStaticModule const& staticModule,
  FBStaticParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
FBRuntimeParamBase(staticModule, staticParam, topoIndices, runtimeModuleIndex, runtimeParamIndex),
static_(staticParam)
{
#ifndef NDEBUG
  std::set<std::string> itemNames = {};
  if(staticParam.type == FBParamType::List)
    for (int i = 0; i < staticParam.List().items.size(); i++)
      assert(itemNames.insert(staticParam.List().items[i].name).second);
#endif
}