#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

void
FBProcStatePtrs::CopyFrom(FBScalarStatePtrs const& scalar)
{
  for (int p = 0; p < Params().size(); p++)
    Params()[p].Value(*scalar.Params()[p]);
}