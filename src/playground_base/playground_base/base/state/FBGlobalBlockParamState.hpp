#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class FBGlobalBlockParamState final
{
  friend class FBProcParamState;
  float _value = {};
  void Value(float value) { _value = value; };

public:
  float Value() const { return _value; }
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGlobalBlockParamState);
};