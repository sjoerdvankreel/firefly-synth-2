#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3Parameter.hpp>
#include <firefly_base/base/topo/static/FBStaticParam.hpp>

FBVST3Parameter::
FBVST3Parameter(FBStaticParam const* topo, ParameterInfo const& info) :
Parameter(info),
_topo(topo) {}

void 
FBVST3Parameter::toString(ParamValue valueNormalized, String128 string) const
{
  auto text = _topo->NonRealTime().NormalizedToText(false, valueNormalized);
  FBVST3CopyToString128(text, string);
}

bool 
FBVST3Parameter::fromString(const TChar* string, ParamValue& valueNormalized) const
{
  std::string str;
  FBVST3CopyFromString128(string, str);
  auto parsed = _topo->NonRealTime().TextToNormalized(false, str);
  if (!parsed.has_value())
    return false;
  valueNormalized = parsed.value();
  return true;
}