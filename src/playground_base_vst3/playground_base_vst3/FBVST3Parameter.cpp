#include <playground_base_vst3/FBVST3Utility.hpp>
#include <playground_base_vst3/FBVST3Parameter.hpp>

FBVST3Parameter::
FBVST3Parameter(FBStaticParam const& topo, ParameterInfo const& info) :
Parameter(info),
_topo(topo) {}

void 
FBVST3Parameter::toString(ParamValue valueNormalized, String128 string) const
{
  auto text = _topo.NormalizedToText(false, valueNormalized);
  FBVST3CopyToString128(text, string);
}

bool 
FBVST3Parameter::fromString(const TChar* string, ParamValue& valueNormalized) const
{
  // TODO
  return false;
}