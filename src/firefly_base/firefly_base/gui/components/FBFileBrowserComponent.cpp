#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/components/FBFileBrowserComponent.hpp>

using namespace juce;

static int const FileBrowserWidth = 640;
static int const FileBrowserHeight = 480;

FBFileBrowserComponent::
~FBFileBrowserComponent() 
{
  _browser->removeListener(this);
}

FBFileBrowserComponent::
FBFileBrowserComponent(
  FBPlugGUI* plugGUI, bool isSave,
  std::string extension, std::string filterName,
  std::function<void(File const&)> onSelect):
_plugGUI(plugGUI),
_isSave(isSave),
_extension(extension),
_onSelect(onSelect)
{
  _okButton = std::make_unique<FBAutoSizeButton>("OK");
  _okButton->onClick = [this]() { SelectFile(_browser->getSelectedFile(0)); };
  _cancelButton = std::make_unique<FBAutoSizeButton>("Cancel");
  _cancelButton->onClick = [this]() { Hide(); };
  _filter = std::make_unique<WildcardFileFilter>("*." + extension, "", filterName);
  int browserFlags = FileBrowserComponent::canSelectFiles | FileBrowserComponent::useTreeView;
  if (isSave)
    browserFlags |= FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting;
  else
    browserFlags |= FileBrowserComponent::openMode | FileBrowserComponent::filenameBoxIsReadOnly;
  _browser = std::make_unique<FileBrowserComponent>(browserFlags, File(), _filter.get(), nullptr);
  _browser->addListener(this);
  _grid = std::make_unique<FBGridComponent>(true, std::vector<int> { 1, 0, }, std::vector<int> { 1, 0, 0, });
  _grid->Add(0, 0, 1, 3, _browser.get());
  _grid->Add(1, 1, 1, 1, _okButton.get());
  _grid->Add(1, 2, 1, 1, _cancelButton.get());
  _grid->MarkSection({ { 0, 0 }, { 2, 3 } }, FBGridSectionMark::Border);
  _margin = std::make_unique<FBMarginComponent>(true, true, true, true, _grid.get(), true);
  addAndMakeVisible(_margin.get());
}

void
FBFileBrowserComponent::fileDoubleClicked(const File& file)
{
  SelectFile(file);
}

void
FBFileBrowserComponent::resized()
{
  _margin->setBounds(getLocalBounds());
  _margin->resized();
}

void
FBFileBrowserComponent::Hide()
{
  _plugGUI->removeChildComponent(this);
  setVisible(false);
}

void 
FBFileBrowserComponent::SelectFile(juce::File const& file)
{
  if (file.getFullPathName().length() == 0)
    return;
  if (!_isSave && !file.existsAsFile())
    return;
  if (!_isSave && file.getFileExtension() != String(".") + _extension)
    return;
  if (_isSave && file.getFileExtension() != String(".") + _extension)
    _onSelect(File(file.getFullPathName() + "." + _extension));
  else
    _onSelect(file);
  Hide();
}

void 
FBFileBrowserComponent::Show()
{
  _plugGUI->addChildComponent(this, 1);
  setBounds(
    (_plugGUI->getBounds().getWidth() - FileBrowserWidth) / 2,
    (_plugGUI->getBounds().getHeight() - FileBrowserHeight) / 3,
    FileBrowserWidth, FileBrowserHeight);
  resized();
  setVisible(true);
}