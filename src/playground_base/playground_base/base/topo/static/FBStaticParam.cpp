#include <playground_base/base/topo/static/FBStaticParam.hpp>

bool 
FBStaticParam::IsVoiceAcc() const
{
  return addrSelectors.voiceAccProc != nullptr;
}

bool 
FBStaticParam::IsGlobalAcc() const
{
  return addrSelectors.globalAccProc != nullptr;
}

bool 
FBStaticParam::IsVoiceBlock() const
{
  return addrSelectors.voiceBlockProc != nullptr;
}

bool 
FBStaticParam::IsGlobalBlock() const
{
  return addrSelectors.globalBlockProc != nullptr;
}

bool 
FBStaticParam::IsAcc() const
{
  return IsVoiceAcc() || IsGlobalAcc();
}

bool 
FBStaticParam::IsVoice() const
{
  return IsVoiceAcc() || IsVoiceBlock();
}

FBAutomationType 
FBStaticParam::AutomationType() const
{
  if (acc)
    return FBAutomationType::Modulate;
  if (!FBParamTypeIsStepped(type) && IsVoiceBlock())
    return FBAutomationType::Automate;
  return FBAutomationType::None;
}

std::string
FBStaticParam::AutomationTooltip() const
{
  if (acc)
    if (IsVoice())
      return "Sample Accurate Per Voice";
    else
      return "Sample Accurate";
  if (!FBParamTypeIsStepped(type) && IsVoiceBlock())
    return "At Voice Start";
  return "None";
}

float
FBStaticParam::DefaultNormalizedByText() const
{
  if (defaultText.size() == 0)
    return 0.0f;
  return TextToNormalized(false, defaultText).value();
}

std::string
FBStaticParam::NormalizedToText(FBParamTextDisplay display, float normalized) const
{
  std::string result = NormalizedToText((FBValueTextDisplay)display, normalized);
  if (display == FBParamTextDisplay::TooltipWithUnit && !unit.empty())
    result += " " + unit;
  return result;
}

float
FBStaticParam::AnyDiscreteToNormalizedSlow(int plain) const
{
  switch (type)
  {
  case FBParamType::List:
    return list.PlainToNormalized(plain);
  case FBParamType::Note:
    return note.PlainToNormalized(plain);
  case FBParamType::Boolean:
    return boolean.PlainToNormalized(plain);
  case FBParamType::Discrete:
    return discrete.PlainToNormalized(plain);
  default:
    assert(false);
    return {};
  }
}

int 
FBStaticParam::NormalizedToAnyDiscreteSlow(float normalized) const
{
  switch (type)
  {
  case FBParamType::List:
    return list.NormalizedToPlain(normalized);
  case FBParamType::Note:
    return note.NormalizedToPlain(normalized);
  case FBParamType::Boolean:
    return boolean.NormalizedToPlain(normalized);
  case FBParamType::Discrete:
    return discrete.NormalizedToPlain(normalized);
  default:
    assert(false);
    return {};
  }
}

std::string 
FBStaticParam::NormalizedToText(FBValueTextDisplay display, float normalized) const
{
  switch (type)
  {
  case FBParamType::Note:
    return note.PlainToText(note.NormalizedToPlain(normalized));
  case FBParamType::Boolean:
    return boolean.PlainToText(boolean.NormalizedToPlain(normalized));
  case FBParamType::Discrete:
    return discrete.PlainToText(discrete.NormalizedToPlain(normalized));
  case FBParamType::List:
    return list.PlainToText(display, list.NormalizedToPlain(normalized));
  case FBParamType::Linear:
    return linear.PlainToText(display, linear.NormalizedToPlain(normalized));
  case FBParamType::FreqOct:
    return freqOct.PlainToText(display, freqOct.NormalizedToPlain(normalized));
  default:
    assert(false);
    return {};
  }
}

std::optional<float> 
FBStaticParam::TextToNormalized(bool io, std::string const& text) const
{
  switch (type)
  {
    case FBParamType::Note:
    {
      auto plain = note.TextToPlain(io, text); // todo no io
      if (!plain) return {};
      return note.PlainToNormalized(plain.value());
    }
    case FBParamType::List:
    {
      auto plain = list.TextToPlain(io, text);
      if (!plain) return {};
      return list.PlainToNormalized(plain.value());
    }
    case FBParamType::Boolean:
    {
      auto plain = boolean.TextToPlain(text);
      if (!plain) return {};
      return boolean.PlainToNormalized(plain.value());
    }
    case FBParamType::Linear:
    {
      auto plain = linear.TextToPlain(text);
      if (!plain) return {};
      return linear.PlainToNormalized(plain.value());
    }
    case FBParamType::FreqOct:
    {
      auto plain = freqOct.TextToPlain(text);
      if (!plain) return {};
      return freqOct.PlainToNormalized(plain.value());
    }
    case FBParamType::Discrete:
    {
      auto plain = discrete.TextToPlain(text);
      if (!plain) return {};
      return discrete.PlainToNormalized(plain.value());
    }
    default:
    {
      assert(false);
      return {};
    }
  }
}