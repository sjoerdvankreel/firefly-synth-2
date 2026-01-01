#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/components/FBFileBrowserComponent.hpp>

using namespace juce;

static int const FileBrowserWidth = 640;
static int const FileBrowserHeight = 480;

FBFileBrowserComponent::
~FBFileBrowserComponent() {}

FBFileBrowserComponent::
FBFileBrowserComponent(
  FBPlugGUI* plugGUI, bool isSave,
  std::string extension, std::string filterName,
  std::function<void(std::string const&)> onSelect):
_plugGUI(plugGUI),
_onSelect(onSelect)
{
  _okButton = std::make_unique<FBAutoSizeButton>("OK");
  _cancelButton = std::make_unique<FBAutoSizeButton>("Cancel");
  _filter = std::make_unique<WildcardFileFilter>("*." + extension, "", filterName);
  int browserFlags = FileBrowserComponent::canSelectFiles | FileBrowserComponent::useTreeView;
  if (isSave)
    browserFlags |= FileBrowserComponent::saveMode | FileBrowserComponent::warnAboutOverwriting;
  else
    browserFlags |= FileBrowserComponent::openMode | FileBrowserComponent::filenameBoxIsReadOnly;
  _browser = std::make_unique<FileBrowserComponent>(browserFlags, File(), _filter.get(), nullptr);
  _grid = std::make_unique<FBGridComponent>(true, std::vector<int> { 1, 0, }, std::vector<int> { 1, 0, 0, });
  _grid->Add(0, 0, 1, 3, _browser.get());
  _grid->Add(1, 1, 1, 1, _okButton.get());
  _grid->Add(1, 2, 1, 1, _cancelButton.get());
  _margin = std::make_unique<FBMarginComponent>(true, true, true, true, _grid.get(), true);
  addAndMakeVisible(_margin.get());
}

void
FBFileBrowserComponent::resized()
{
  _margin->setBounds(getBounds());
  _margin->resized();
}

void
FBFileBrowserComponent::Hide()
{
  _plugGUI->removeChildComponent(this);
  setVisible(false);
}

void 
FBFileBrowserComponent::Show()
{
  _plugGUI->addChildComponent(this, 1);
  setBounds(0, 0, 600, 600);
    //(_plugGUI->getBounds().getWidth() - FileBrowserWidth) / 2,
    //(_plugGUI->getBounds().getHeight() - FileBrowserHeight) / 3,
    //FileBrowserWidth, FileBrowserHeight);
  resized();
  setVisible(true);
}