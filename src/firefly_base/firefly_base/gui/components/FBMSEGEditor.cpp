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
  _canvas->modelUpdated = [this](auto const&) {
    ModelUpdated();
  };

  PopupMenu xEditMenu;
  xEditMenu.addItem(1, "Free");
  xEditMenu.addItem(2, "Snap");
  xEditMenu.addItem(3, "Stretch");
  PopupMenu yEditMenu;
  yEditMenu.addItem(1, "Free");
  yEditMenu.addItem(2, "Snap");

  PopupMenu snapXCountMenu;
  for (int i = 0; i < _snapXCounts.size(); i++)
    snapXCountMenu.addItem(i + 1, std::to_string(_snapXCounts[i]));
  PopupMenu snapYCountMenu;
  for (int i = 0; i < _snapYCounts.size(); i++)
    snapYCountMenu.addItem(i + 1, std::to_string(_snapYCounts[i]));

  _editXLabel = std::make_unique<FBAutoSizeLabel>("X Mode");
  _xEditModeCombo = std::make_unique<FBAutoSizeComboBox>(xEditMenu);
  _xEditModeCombo->onChange = [this] { ModelUpdated(); };
  _snapXCountCombo = std::make_unique<FBAutoSizeComboBox>(snapXCountMenu);
  _snapXCountCombo->onChange = [this] { ModelUpdated(); };
  _editYLabel = std::make_unique<FBAutoSizeLabel>("Y Mode");
  _yEditModeCombo = std::make_unique<FBAutoSizeComboBox>(yEditMenu);
  _yEditModeCombo->onChange = [this] { ModelUpdated(); };
  _snapYCountCombo = std::make_unique<FBAutoSizeComboBox>(snapYCountMenu);
  _snapYCountCombo->onChange = [this] { ModelUpdated(); };
  _controlFiller = std::make_unique<FBFillerComponent>(1, 1);
  _controlGrid = std::make_unique<FBGridComponent>(true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 0, 0, 0, 0, 0, 0, 1 });
  _controlGrid->Add(0, 0, _editXLabel.get());
  _controlGrid->Add(0, 1, _xEditModeCombo.get());
  _controlGrid->Add(0, 2, _snapXCountCombo.get());
  _controlGrid->Add(0, 3, _editYLabel.get());
  _controlGrid->Add(0, 4, _yEditModeCombo.get());
  _controlGrid->Add(0, 5, _snapYCountCombo.get());
  _controlGrid->Add(0, 6, _controlFiller.get());
  _controlGrid->MarkSection({ { 0, 0 }, { 1, 7 } });
  
  _grid = std::make_unique<FBGridComponent>(true, -1, -1, std::vector<int> { 1, 0 }, std::vector<int> { 1 });
  _grid->Add(0, 0, _canvas.get());
  _grid->Add(1, 0, _controlGrid.get());
  addAndMakeVisible(_grid.get());
}

void
FBMSEGEditor::resized()
{
  _grid->setBounds(getLocalBounds());
  _grid->resized();
}

void
FBMSEGEditor::ModelUpdated()
{
  auto& model = Model();
  int xIndex = std::clamp(_snapXCountCombo->getSelectedItemIndex(), 0, (int)_snapXCounts.size() - 1);
  int yIndex = std::clamp(_snapXCountCombo->getSelectedItemIndex(), 0, (int)_snapYCounts.size() - 1);
  model.snapXCount = _snapXCounts[xIndex];
  model.snapYCount = _snapYCounts[yIndex];
  model.xEditMode = (FBMSEGXEditMode)std::clamp(_xEditModeCombo->getSelectedItemIndex(), 0, (int)FBMSEGXEditMode::Count - 1);
  model.yEditMode = (FBMSEGYEditMode)std::clamp(_yEditModeCombo->getSelectedItemIndex(), 0, (int)FBMSEGYEditMode::Count - 1);
  if (modelUpdated != nullptr)
    modelUpdated(Model());
}

void
FBMSEGEditor::UpdateModel()
{
  auto const& model = Model();
  FB_ASSERT(model.snapXCount > 0);
  FB_ASSERT(model.snapYCount > 0);
  _snapXCountCombo->setEnabled(model.xEditMode == FBMSEGXEditMode::Snap);
  _snapYCountCombo->setEnabled(model.yEditMode == FBMSEGYEditMode::Snap);
  _xEditModeCombo->setSelectedItemIndex((int)model.xEditMode, dontSendNotification);
  _yEditModeCombo->setSelectedItemIndex((int)model.yEditMode, dontSendNotification);
  auto xIter = std::find(_snapXCounts.begin(), _snapXCounts.end(), model.snapXCount);
  auto yIter = std::find(_snapYCounts.begin(), _snapYCounts.end(), model.snapYCount);
  if (xIter != _snapXCounts.end())
    _snapXCountCombo->setSelectedItemIndex((int)(xIter - _snapXCounts.begin()), dontSendNotification);
  if (yIter != _snapYCounts.end())
    _snapYCountCombo->setSelectedItemIndex((int)(yIter - _snapYCounts.begin()), dontSendNotification);
  _canvas->UpdateModel();
  repaint();
}
