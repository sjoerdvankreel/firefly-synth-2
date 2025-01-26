#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FFModuleProcState;

class FFMasterProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterProcessor);
  void Process(
    FFModuleProcState const& state, 
    std::vector<float>& outputParamsNormalized);
};