#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/static/FBDiscreteParam.hpp>

using namespace juce;

bool FBDiscreteParamNonRealTime::IsItems() const { return false; }
bool FBDiscreteParamNonRealTime::IsStepped() const { return true; }
int FBDiscreteParamNonRealTime::ValueCount() const { return valueCount; }
FBEditType FBDiscreteParamNonRealTime::GUIEditType() const { return FBEditType::Stepped; }
FBEditType FBDiscreteParamNonRealTime::AutomationEditType() const { return FBEditType::Stepped; }

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
FBDiscreteParamNonRealTime::PlainToText(bool /*io*/, int /*moduleIndex*/, double plain) const
{
  int result = static_cast<int>(std::round(plain));
  if (valueFormatter != nullptr)
    return valueFormatter(result);
  return std::to_string(result);
}

PopupMenu
FBDiscreteParamNonRealTime::MakePopupMenu(int moduleIndex) const
{
  PopupMenu result;
  if (!subMenuFormatter)
  {
    for (int i = 0; i < ValueCount(); i++)
      result.addItem(i + 1, PlainToText(false, moduleIndex, i + valueOffset));
    return result;
  }

  int j = 0;
  PopupMenu subMenu;
  int subMenuIndex = 0;
  for (int i = 0; i < ValueCount(); i++, j++)
  {
    subMenu.addItem(i + 1, PlainToText(false, moduleIndex, i + valueOffset));
    if (j == ValueCount() - 1 || j == subMenuItemCount - 1)
    {
      result.addSubMenu(subMenuFormatter(subMenuIndex), subMenu);
      subMenu = {};
      j = -1;
      subMenuIndex++;
    }
  }
  return result;
}

std::optional<double>
FBDiscreteParamNonRealTime::TextToPlainInternal(bool io, int moduleIndex, std::string const& text) const
{
  if(valueFormatter != nullptr)
  { 
    for (int i = 0; i < ValueCount(); i++)
      if (PlainToText(io, moduleIndex, i + valueOffset) == text)
        return { i + valueOffset };
    return { };
  }

  char* end;
  unsigned long plain = std::strtoul(text.c_str(), &end, 10);
  if (end != text.c_str() + text.size())
  {
    FB_LOG_WARN(std::string("Parsing text remainder: '") + end + "'.");
    return {};
  }
  int iPlain = static_cast<int>(plain);
  if (iPlain < valueOffset || iPlain >= valueCount + valueOffset)
    return { };
  return { iPlain };
}