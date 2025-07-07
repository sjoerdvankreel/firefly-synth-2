#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBListParam.hpp>

using namespace juce;

bool FBListParamNonRealTime::IsItems() const { return true; }
bool FBListParamNonRealTime::IsStepped() const { return true; }
int FBListParamNonRealTime::ValueCount() const { return static_cast<int>(items.size()); }
FBEditType FBListParamNonRealTime::GUIEditType() const { return FBEditType::Stepped; }
FBEditType FBListParamNonRealTime::AutomationEditType() const { return FBEditType::Stepped; }

std::string 
FBListItem::GetName(int moduleIndex, int itemSlot) const
{
  FB_ASSERT(name2.empty() != (slotFormatter == nullptr));
  if (slotFormatter != nullptr)
    return slotFormatter(moduleIndex, itemSlot);
  return name2;
}

double 
FBListParamNonRealTime::PlainToNormalized(double plain) const 
{
  return std::clamp(plain / (ValueCount() - 1.0), 0.0, 1.0);
}

double
FBListParamNonRealTime::NormalizedToPlain(double normalized) const 
{
  return std::clamp(static_cast<int>(normalized * ValueCount()), 0, ValueCount() - 1);
}

PopupMenu
FBListParamNonRealTime::MakePopupMenu(int moduleIndex) const
{
  PopupMenu result;
  if (submenuStart.empty())
  {
    for (int i = 0; i < ValueCount(); i++)
      result.addItem(i + 1, PlainToText(false, moduleIndex, i));
    return result;
  }
  PopupMenu submenu;
  std::string submenuHeader = submenuStart.at(0);
  for (int i = 0; i < ValueCount(); i++)
  {
    if (i != 0 && submenuStart.contains(i))
    {
      result.addSubMenu(submenuHeader, submenu);
      submenu = {};
      submenuHeader = submenuStart.at(i);
    }
    submenu.addItem(i + 1, PlainToText(false, moduleIndex, i));
    if (i == ValueCount() - 1)
    {
      result.addSubMenu(submenuHeader, submenu);
    }
  }
  return result;
}

std::optional<double>
FBListParamNonRealTime::TextToPlainInternal(bool io, int moduleIndex, std::string const& text) const
{
  for (int i = 0; i < items.size(); i++)
    if (text == (io ? items[i].id : items[i].GetName(moduleIndex, i)))
      return { i };
  return {};
}

std::string
FBListParamNonRealTime::PlainToText(bool io, int moduleIndex, double plain) const
{
  int discrete = static_cast<int>(std::round(plain));
  return io ? items[discrete].id : items[discrete].GetName(moduleIndex, discrete);
}