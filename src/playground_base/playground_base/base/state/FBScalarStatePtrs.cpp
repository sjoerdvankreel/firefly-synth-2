#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

void
FBScalarStatePtrs::InitFrom(FBProcStatePtrs const& proc)
{
  for (int i = 0; i < all.size(); i++)
    if (proc.isAcc[i])
      *all[i] = proc.single.acc[i]->scalar;
    else
      *all[i] = *proc.single.block[i];
}

void 
FBScalarStatePtrs::InitFrom(FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < all.size(); i++)
    *all[i] = *scalar.all[i];
}