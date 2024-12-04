#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

void 
FBProcStatePtrs::InitFrom(FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < scalar.all.size(); i++)
    if (isBlock[i])
      *block[i] = *scalar.all[i];
    else
      acc[i]->Reset(*scalar.all[i]);
}