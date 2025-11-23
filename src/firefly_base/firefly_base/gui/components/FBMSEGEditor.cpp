#include <firefly_base/gui/components/FBMSEGEditor.hpp>

using namespace juce;

FBMSEGEditor::
FBMSEGEditor(
  FBPlugGUI* plugGUI,
  std::string const& name,
  int maxPoints,
  int maxLengthRatioNum,
  int maxLengthRatioDen,
  double maxLengthReal):
Component(),
_plugGUI(plugGUI),
_canvas(std::make_unique<FBMSEGCanvas>(plugGUI, name, maxPoints, maxLengthRatioNum, maxLengthRatioDen, maxLengthReal))
{
  _canvas->getTooltipFor = [this](auto hitType, auto index) {
    if (getTooltipFor != nullptr)
      return getTooltipFor(hitType, index);
    return "";
  };
  _canvas->modelUpdated = [this](auto const& model) {
    if (modelUpdated != nullptr)
      modelUpdated(model);
  };
}

void
FBMSEGEditor::UpdateModel()
{
  _canvas->UpdateModel();
  repaint();
}

void
FBMSEGEditor::resized()
{
  _canvas->setBounds(getLocalBounds());
  _canvas->resized();
}
