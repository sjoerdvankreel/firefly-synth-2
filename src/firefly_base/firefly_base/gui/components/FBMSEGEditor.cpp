#include <firefly_base/gui/components/FBMSEGEditor.hpp>

using namespace juce;

FBMSEGEditor::
FBMSEGEditor(
  FBPlugGUI* plugGUI,
  std::string const& name,
  int maxPoints,
  int maxLengthRatioNum,
  int maxLengthRatioDen,
  double maxLengthReal,
  std::vector<int> const& snapXCounts,
  std::vector<int> const& snapYCounts) :
Component(),
_plugGUI(plugGUI),
_snapXCounts(snapXCounts),
_snapYCounts(snapYCounts)
{
  _canvas = std::make_unique<FBMSEGCanvas>(plugGUI, name, maxPoints, maxLengthRatioNum, maxLengthRatioDen, maxLengthReal);
  _canvas->getTooltipFor = [this](auto hitType, auto index) {
    if (getTooltipFor != nullptr)
      return getTooltipFor(hitType, index);
    return std::string("");
  };
  _canvas->modelUpdated = [this](auto const& model) {
    if (modelUpdated != nullptr)
      modelUpdated(model);
  };

  PopupMenu snapXMenu;
  for (int i = 0; i < _snapXCounts.size(); i++)
    snapXMenu.addItem(i + 1, std::to_string(_snapXCounts[i]));
  PopupMenu snapYMenu;
  for (int i = 0; i < _snapYCounts.size(); i++)
    snapYMenu.addItem(i + 1, std::to_string(_snapYCounts[i]));

  _snapXLabel = std::make_unique<FBAutoSizeLabel>("Snap X");
  _snapXToggle = std::make_unique<FBAutoSizeToggleButton>();
  _snapXCombo = std::make_unique<FBAutoSizeComboBox>(snapXMenu);
  _snapYLabel = std::make_unique<FBAutoSizeLabel>("Snap Y");
  _snapYToggle = std::make_unique<FBAutoSizeToggleButton>();
  _snapYCombo = std::make_unique<FBAutoSizeComboBox>(snapYMenu);
  _topGrid = std::make_unique<FBGridComponent>(true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 0, 0, 0, 0, 0, 0, 1 });
  _topGrid->Add(0, 0, _snapXLabel.get());
  _topGrid->Add(0, 1, _snapXToggle.get());
  _topGrid->Add(0, 2, _snapXCombo.get());
  _topGrid->Add(0, 3, _snapYLabel.get());
  _topGrid->Add(0, 4, _snapYToggle.get());
  _topGrid->Add(0, 5, _snapYCombo.get());
  _topGrid->MarkSection({ { 0, 0 }, { 1, 7 } });
  
  _grid = std::make_unique<FBGridComponent>(true, -1, -1, std::vector<int> { 0, 1 }, std::vector<int> { 0 });
  _grid->Add(0, 0, _topGrid.get());
  _grid->Add(1, 0, _canvas.get());
  addAndMakeVisible(_grid.get());
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
  _grid->setBounds(getLocalBounds());
  _grid->resized();
}
