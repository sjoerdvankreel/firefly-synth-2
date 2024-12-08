#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

void
FBScalarStatePtrs::CopyFrom(FBProcStatePtrs const& proc)
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = proc.Params()[p].Value();
}

void 
FBScalarStatePtrs::CopyFrom(FBScalarStatePtrs const& scalar)
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = *scalar.Params()[p];
}