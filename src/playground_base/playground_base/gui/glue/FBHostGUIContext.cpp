#include <playground_base/gui/glue/FBHostGUIContext.hpp>

void 
FBHostGUIContext::PerformImmediateParamEdit(int index, float normalized)
{
  BeginParamChange(index);
  PerformParamEdit(index, normalized);
  EndParamChange(index);
}