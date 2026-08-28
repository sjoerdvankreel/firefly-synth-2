#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3Parameter.hpp>

#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

FBVST3Parameter::
FBVST3Parameter(FBHostGUIContext const* hostContext, FBRuntimeParam const* topo, ParameterInfo const& info) :
Parameter(info), _hostContext(hostContext), _topo(topo), _infoOverride(info) {}

void 
FBVST3Parameter::ClearNameOverride()
{
  FBVST3CopyToString128(_topo->longName, _infoOverride.title);
  FBVST3CopyToString128(_topo->shortName, _infoOverride.shortTitle);
}

void
FBVST3Parameter::SetNameOverride(std::string const& name)
{
  FBVST3CopyToString128(name, _infoOverride.title);
  FBVST3CopyToString128(name, _infoOverride.shortTitle);
}

bool 
FBVST3Parameter::GetNameOverride(std::string& name) const
{
  std::string temp;
  FBVST3CopyFromString128(info.title, temp);
  FBVST3CopyFromString128(_infoOverride.title, name);  
  return temp != name;
}

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