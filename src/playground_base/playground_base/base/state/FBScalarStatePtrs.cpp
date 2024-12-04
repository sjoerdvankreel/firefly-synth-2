#include <playground_base/base/state/FBScalarStatePtrs.hpp>

void 
FBScalarStatePtrs::InitFrom(FBScalarStatePtrs const& scalar)
{
  for (int i = 0; i < all.size(); i++)
    *all[i] = *scalar.all[i];
}