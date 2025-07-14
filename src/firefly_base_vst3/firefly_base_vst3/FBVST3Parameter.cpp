#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3Parameter.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

FBVST3Parameter::
FBVST3Parameter(FBRuntimeParam const* topo, ParameterInfo const& info) :
Parameter(info),
_topo(topo) {}

void 
FBVST3Parameter::toString(ParamValue valueNormalized_, String128 string) const
{
  FBWithLogException([this, valueNormalized_, &string]()
  {
    auto text = _topo->NormalizedToText(false, valueNormalized_);
    FBVST3CopyToString128(text, string);
  });
}

bool 
FBVST3Parameter::fromString(const TChar* string, ParamValue& valueNormalized_) const
{
  return FBWithLogException([this, string, &valueNormalized_]()
  {
    std::string str;
    FBVST3CopyFromString128(string, str);
    auto parsed = _topo->TextToNormalized(false, str);
    if (!parsed.has_value())
      return false;
    valueNormalized_ = parsed.value();
    return true;
  });
}