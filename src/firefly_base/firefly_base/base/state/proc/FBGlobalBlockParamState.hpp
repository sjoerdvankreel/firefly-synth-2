#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

class FBGlobalBlockParamState final
{
  friend class FBProcParamState;
  
  float _value = {};  
  void Value(float value) { _value = value; };

public:
  float Value() const { return _value; }
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGlobalBlockParamState);
};