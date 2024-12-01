#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

#include <vector>

class FBFixedCVBlock;
class FBOnePoleFilter;

struct FBProcStateAddrs;
struct FBScalarStateAddrs;

struct FBStateAddrs
{
  FBProcStateAddrs* proc = {};
  FBScalarStateAddrs* scalar = {};
};

struct FBProcParamState
{
  friend class FBRampProcessor;

private:
  int pos = 0;
  FBOnePoleFilter smooth = {};
  FBFixedCVBlock rampedCV = {};

public:
  FBFixedCVBlock smoothedCV = {};
};

struct FBScalarStateAddrs
{
  std::vector<float*> acc = {};
  std::vector<float*> block = {};

  virtual ~FBScalarStateAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBScalarStateAddrs);
};

struct FBProcStateAddrs
{
  virtual ~FBProcStateAddrs() {}
  std::vector<FBProcParamState*> param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcStateAddrs);
};