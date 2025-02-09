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
  if (!FBParamTypeIsStepped(typed.type) && IsVoiceBlock())
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
  if (!FBParamTypeIsStepped(typed.type) && IsVoiceBlock())
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
  switch (typed.type)
  {
  case FBParamType::List:
    return typed.List().PlainToNormalized(plain);
  case FBParamType::Note:
    return typed.Note().PlainToNormalized(plain);
  case FBParamType::Boolean:
    return typed.Boolean().PlainToNormalized(plain);
  case FBParamType::Discrete:
    return typed.Discrete().PlainToNormalized(plain);
  default:
    assert(false);
    return {};
  }
}

int 
FBStaticParam::NormalizedToAnyDiscreteSlow(float normalized) const
{
  switch (typed.type)
  {
  case FBParamType::List:
    return typed.List().NormalizedToPlain(normalized);
  case FBParamType::Note:
    return typed.Note().NormalizedToPlain(normalized);
  case FBParamType::Boolean:
    return typed.Boolean().NormalizedToPlain(normalized);
  case FBParamType::Discrete:
    return typed.Discrete().NormalizedToPlain(normalized);
  default:
    assert(false);
    return {};
  }
}

std::string 
FBStaticParam::NormalizedToText(FBValueTextDisplay display, float normalized) const
{
  switch (typed.type)
  {
  case FBParamType::Note:
    return typed.Note().PlainToText(typed.Note().NormalizedToPlain(normalized));
  case FBParamType::Boolean:
    return typed.Boolean().PlainToText(typed.Boolean().NormalizedToPlain(normalized));
  case FBParamType::Discrete:
    return typed.Discrete().PlainToText(typed.Discrete().NormalizedToPlain(normalized));
  case FBParamType::List:
    return typed.List().PlainToText(display, typed.List().NormalizedToPlain(normalized));
  case FBParamType::Linear:
    return typed.Linear().PlainToText(display, typed.Linear().NormalizedToPlain(normalized));
  case FBParamType::FreqOct:
    return typed.FreqOct().PlainToText(display, typed.FreqOct().NormalizedToPlain(normalized));
  default:
    assert(false);
    return {};
  }
}

std::optional<float> 
FBStaticParam::TextToNormalized(bool io, std::string const& text) const
{
  switch (typed.type)
  {
    case FBParamType::Note:
    {
      auto plain = typed.Note().TextToPlain(text);
      if (!plain) return {};
      return typed.Note().PlainToNormalized(plain.value());
    }
    case FBParamType::List:
    {
      auto plain = typed.List().TextToPlain(io, text);
      if (!plain) return {};
      return typed.List().PlainToNormalized(plain.value());
    }
    case FBParamType::Boolean:
    {
      auto plain = typed.Boolean().TextToPlain(text);
      if (!plain) return {};
      return typed.Boolean().PlainToNormalized(plain.value());
    }
    case FBParamType::Linear:
    {
      auto plain = typed.Linear().TextToPlain(text);
      if (!plain) return {};
      return typed.Linear().PlainToNormalized(plain.value());
    }
    case FBParamType::FreqOct:
    {
      auto plain = typed.FreqOct().TextToPlain(text);
      if (!plain) return {};
      return typed.FreqOct().PlainToNormalized(plain.value());
    }
    case FBParamType::Discrete:
    {
      auto plain = typed.Discrete().TextToPlain(text);
      if (!plain) return {};
      return typed.Discrete().PlainToNormalized(plain.value());
    }
    default:
    {
      assert(false);
      return {};
    }
  }
}