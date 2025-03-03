#include <playground_base/gui/glue/FBHostGUIContext.hpp>

void 
FBHostGUIContext::PerformImmediateAudioParamEdit(int index, float normalized)
{
  BeginAudioParamChange(index);
  PerformAudioParamEdit(index, normalized);
  EndAudioParamChange(index);
}