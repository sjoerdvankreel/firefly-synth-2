#pragma once

#include <firefly_base/base/topo/static/FBParamNonRealTime.hpp>

#include <map>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <functional>

class FBHostGUIContext;

typedef std::function<std::string(
int moduleIndex, int itemSlot)>
FBItemSlotFormatter;

typedef std::function<bool(
FBHostGUIContext const*, int runtimeValue)>
FBItemEnabledSelector;

typedef std::function<bool(
int runtimeSourceValue, int runtimeTargetValue)>
FBItemLinkedTargetEnabledSelector;

struct FBListItem final
{
  std::string id = {};
  std::string name = {};
};

struct FBListParam
{
  int linkedSource = -1;
  int linkedTarget = -1;
  std::vector<FBListItem> items = {};
  std::map<int, std::string> submenuStart = {};
  FBItemSlotFormatter slotFormatter = {};
  FBItemEnabledSelector enabledSelector = {};
  FBItemLinkedTargetEnabledSelector linkedTargetEnabledSelector = {};

  int NormalizedToPlainFast(float normalized) const;
  std::string GetName(int moduleIndex, int itemSlot) const;
};

struct FBListParamNonRealTime final :
public FBListParam,
public FBItemsParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  FBEditType GUIEditType() const override;
  FBEditType AutomationEditType() const override;
  juce::PopupMenu MakePopupMenu(int moduleIndex) const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(bool io, int moduleIndex, double plain) const override;
  std::optional<double> TextToPlainInternal(bool io, int moduleIndex, std::string const& text) const override;
};

inline int
FBListParam::NormalizedToPlainFast(float normalized) const
{
  int count = static_cast<int>(items.size());
  return std::clamp(static_cast<int>(normalized * count), 0, count - 1);
}