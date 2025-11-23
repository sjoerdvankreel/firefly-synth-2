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

  PopupMenu snapXMenu;
  for (int i = 0; i < _snapXCounts.size(); i++)
    snapXMenu.addItem(i + 1, std::to_string(_snapXCounts[i]));
  PopupMenu snapYMenu;
  for (int i = 0; i < _snapYCounts.size(); i++)
    snapYMenu.addItem(i + 1, std::to_string(_snapYCounts[i]));

  _snapXLabel = std::make_unique<FBAutoSizeLabel>("Snap X");
  _snapXToggle = std::make_unique<FBAutoSizeToggleButton>();
  _snapXToggle->onClick = [this] { ModelUpdated(); };
  _snapXCombo = std::make_unique<FBAutoSizeComboBox>(snapXMenu);
  _snapXCombo->onChange = [this] { ModelUpdated(); };
  _snapYLabel = std::make_unique<FBAutoSizeLabel>("Snap Y");
  _snapYToggle = std::make_unique<FBAutoSizeToggleButton>();
  _snapYToggle->onClick = [this] { ModelUpdated(); };
  _snapYCombo = std::make_unique<FBAutoSizeComboBox>(snapYMenu);
  _snapYCombo->onChange = [this] { ModelUpdated(); };
  _controlFiller = std::make_unique<FBFillerComponent>(1, 1);
  _controlGrid = std::make_unique<FBGridComponent>(true, -1, -1, std::vector<int> { { 1 } }, std::vector<int> { 0, 0, 0, 0, 0, 0, 1 });
  _controlGrid->Add(0, 0, _snapXLabel.get());
  _controlGrid->Add(0, 1, _snapXToggle.get());
  _controlGrid->Add(0, 2, _snapXCombo.get());
  _controlGrid->Add(0, 3, _snapYLabel.get());
  _controlGrid->Add(0, 4, _snapYToggle.get());
  _controlGrid->Add(0, 5, _snapYCombo.get());
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
  int xIndex = std::clamp(_snapXCombo->getSelectedItemIndex(), 0, (int)_snapXCounts.size() - 1);
  int yIndex = std::clamp(_snapYCombo->getSelectedItemIndex(), 0, (int)_snapYCounts.size() - 1);
  Model().snapX = _snapXToggle->getToggleState();
  Model().snapY = _snapYToggle->getToggleState();
  Model().snapXCount = _snapXCounts[xIndex];
  Model().snapYCount = _snapYCounts[yIndex];
  if (modelUpdated != nullptr)
    modelUpdated(Model());
}

void
FBMSEGEditor::UpdateModel()
{
  _canvas->UpdateModel();
  _snapXCombo->setEnabled(Model().snapX);
  _snapYCombo->setEnabled(Model().snapY);
  _snapXToggle->setToggleState(Model().snapX, dontSendNotification);
  _snapYToggle->setToggleState(Model().snapY, dontSendNotification);
  auto xIter = std::find(_snapXCounts.begin(), _snapXCounts.end(), Model().snapXCount);
  auto yIter = std::find(_snapYCounts.begin(), _snapYCounts.end(), Model().snapYCount);
  if (xIter != _snapXCounts.end())
    _snapXCombo->setSelectedItemIndex((int)(xIter - _snapXCounts.begin()), dontSendNotification);
  if (yIter != _snapYCounts.end())
    _snapYCombo->setSelectedItemIndex((int)(yIter - _snapYCounts.begin()), dontSendNotification);
  repaint();
}
