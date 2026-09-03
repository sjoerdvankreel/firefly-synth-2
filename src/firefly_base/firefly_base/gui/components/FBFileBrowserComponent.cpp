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
  FBPlugGUI* plugGUI, bool isSave, bool hasPreview,
  std::string const& title, std::string const& extension, std::string const& filterName, juce::File initialPath, 
  std::function<void(File const&)> onSelect, std::function<void(File const&)> onPreview, std::function<void()> onCancel):
_plugGUI(plugGUI),
_isSave(isSave),
_hasPreview(hasPreview),
_extension(extension),
_onSelect(onSelect),
_onPreview(onPreview),
_onCancel(onCancel)
{
  _title = std::make_unique<FBAutoSizeLabel>(plugGUI, title, FBLabelAlign::Center);
  _okButton = std::make_unique<FBAutoSizeButton>(plugGUI, "OK");
  _okButton->onClick = [this]() { SelectFile(_browser->getSelectedFile(0)); };
  _cancelButton = std::make_unique<FBAutoSizeButton>(plugGUI, "Cancel");
  _cancelButton->onClick = [this]() { _onCancel(); Hide(); };
  _filter = std::make_unique<WildcardFileFilter>("*." + extension, "", filterName);
  if (hasPreview)
  {
    _previewLabel = std::make_unique<FBAutoSizeLabel>(plugGUI, "Preview");
    _previewToggle = std::make_unique<FBAutoSizeToggleButton>();
  }
  
  int browserFlags = FileBrowserComponent::canSelectFiles | FileBrowserComponent::useTreeView;
  if (isSave)
    browserFlags |= FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting;
  else
    browserFlags |= FileBrowserComponent::openMode | FileBrowserComponent::filenameBoxIsReadOnly;
  _browser = std::make_unique<FileBrowserComponent>(browserFlags, initialPath, _filter.get(), nullptr);
  _browser->addListener(this);
  _grid = std::make_unique<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 1, 0, }, std::vector<int> { 1, 0, 0, 0, 0 });
  _grid->Add(0, 0, 1, 5, _title.get());
  _grid->Add(1, 0, 1, 5, _browser.get());
  if (hasPreview)
  {
    _grid->Add(2, 1, 1, 1, _previewLabel.get());
    _grid->Add(2, 2, 1, 1, _previewToggle.get());
  }
  _grid->Add(2, 3, 1, 1, _okButton.get());
  _grid->Add(2, 4, 1, 1, _cancelButton.get());
  _grid->MarkSection({ { 0, 0 }, { 3, 5 } }, FBGridSectionMark::DefaultBackgroundDefaultBorder);
  _margin = std::make_unique<FBMarginComponent>(plugGUI, true, true, true, true, _grid.get(), true);
  addAndMakeVisible(_margin.get());
}

void
FBFileBrowserComponent::fileDoubleClicked(const File& file)
{
  SelectFile(file);
}

bool
FBFileBrowserComponent::IsPreviewEnabled() const
{
  return _hasPreview && _previewToggle->getToggleState();
}

void
FBFileBrowserComponent::SetPreviewEnabled(bool enabled)
{
  if (_hasPreview)
    _previewToggle->setToggleState(enabled, dontSendNotification);
}

void 
FBFileBrowserComponent::fileClicked(const File& file, const MouseEvent& /*event*/)
{
  if (_hasPreview && _previewToggle->getToggleState())
    _onPreview(file);
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
FBFileBrowserComponent::SelectFile(File const& file0)
{
  File newFile = file0;
  if (newFile.getFullPathName().length() == 0)
    return;
  if (!_isSave && !newFile.existsAsFile())
    return;
  if (!_isSave && newFile.getFileExtension() != String(".") + _extension)
    return;
  if (_isSave && newFile.getFileExtension() != String(".") + _extension)
    newFile = File(newFile.getFullPathName() + "." + _extension);
  if (!_isSave || !newFile.existsAsFile())
  {
    _onSelect(newFile);
    Hide();
    return;
  }
  AlertWindow::showAsync(MessageBoxOptions()
    .withIconType(MessageBoxIconType::NoIcon)
    .withTitle("Warning")
    .withMessage("Overwite existing file?")
    .withButton("OK")
    .withButton("Cancel")
    .withParentComponent(_plugGUI),
    [this, newFile](int result) {
      if (result != 1)
        return;
      _onSelect(newFile);
      Hide();
    });
}

void 
FBFileBrowserComponent::Show()
{
  _browser->refresh();
  _plugGUI->addChildComponent(this, 1);
  setBounds(
    (_plugGUI->getBounds().getWidth() - FileBrowserWidth) / 2,
    (_plugGUI->getBounds().getHeight() - FileBrowserHeight) / 3,
    FileBrowserWidth, FileBrowserHeight);
  resized();
  setVisible(true);
}