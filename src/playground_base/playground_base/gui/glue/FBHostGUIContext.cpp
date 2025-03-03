#include <playground_base/gui/glue/FBHostGUIContext.hpp>

void 
FBHostGUIContext::PerformImmediateAudioParamEdit(int index, double normalized)
{
  BeginAudioParamChange(index);
  PerformAudioParamEdit(index, normalized);
  EndAudioParamChange(index);
}