#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

using namespace juce;

bool 
FBDiscreteParamNonRealTime::IsItems() const
{
  return false;
}

bool 
FBDiscreteParamNonRealTime::IsStepped() const 
{
  return true;
}

int
FBDiscreteParamNonRealTime::ValueCount() const 
{
  return valueCount;
}

FBEditType
FBDiscreteParamNonRealTime::GUIEditType() const
{
  return FBEditType::Stepped;
}

FBEditType
FBDiscreteParamNonRealTime::AutomationEditType() const
{
  return FBEditType::Stepped;
}

double 
FBDiscreteParamNonRealTime::PlainToNormalized(double plain) const 
{
  return std::clamp((plain - valueOffset) / (valueCount - 1.0), 0.0, 1.0);
}

double
FBDiscreteParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  int scaled = static_cast<int>(normalized * valueCount);
  return std::clamp(scaled, 0, valueCount - 1) + valueOffset;
}

std::string
FBDiscreteParamNonRealTime::PlainToText(FBTextDisplay display, double plain) const
{
  int result = static_cast<int>(std::round(plain));
  if (valueFormatter != nullptr)
    return valueFormatter(result);
  return std::to_string(result);
}

PopupMenu
FBDiscreteParamNonRealTime::MakePopupMenu() const
{
  PopupMenu result;
  for (int i = 0; i < ValueCount(); i++)
    result.addItem(i + 1, PlainToText(FBTextDisplay::Text, i));
  return result;
}

std::optional<double>
FBDiscreteParamNonRealTime::TextToPlain(FBTextDisplay display, std::string const& text) const
{
  if(valueFormatter != nullptr)
  { 
    for (int i = 0; i < ValueCount(); i++)
      if (PlainToText(FBTextDisplay::Text, i + valueOffset) == text)
        return { i + valueOffset };
    return { };
  }

  char* end;
  unsigned long plain = std::strtoul(text.c_str(), &end, 10);
  if (end != text.c_str() + text.size())
    return {};
  int iPlain = static_cast<int>(plain);
  if (iPlain < valueOffset || iPlain >= valueCount + valueOffset)
    return { };
  return { iPlain };
}