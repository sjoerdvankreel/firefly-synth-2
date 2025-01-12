#include <playground_base/gui/glue/FBPlugGUIBase.hpp>
#include <cmath>

using namespace juce;

FBPlugGUIBase::
FBPlugGUIBase():
IFBPlugGUI()
{
  setOpaque(true);
  setVisible(true);
}

void 
FBPlugGUIBase::paint(Graphics& g)
{
  g.fillAll(Colours::black);
}

void
FBPlugGUIBase::RemoveFromDesktop()
{
  removeFromDesktop();
}

void
FBPlugGUIBase::AddToDesktop(void* parent)
{
  addToDesktop(0, parent);
}

void
FBPlugGUIBase::SetVisible(bool visible)
{
  setVisible(visible);
}

int 
FBPlugGUIBase::GetScaledWidth() const
{
  return (int)std::round(_scale * getWidth());
}

int 
FBPlugGUIBase::GetScaledHeight() const
{
  return (int)std::round(_scale * getHeight());
}

int 
FBPlugGUIBase::GetMinScaledWidth() const
{
  return (int)std::round(_scale * GetMinUnscaledWidth());
}

int 
FBPlugGUIBase::GetMaxScaledWidth() const
{
  return (int)std::round(_scale * GetMaxUnscaledWidth());
}

void
FBPlugGUIBase::SetParamNormalized(int index, float normalized)
{
  GetParamControlForIndex(index)->SetValueNormalized(normalized);
}

void
FBPlugGUIBase::SetScaledSize(int w, int h)
{
  setSize((int)std::round(w / _scale), (int)(std::round(h / _scale)));
}

int 
FBPlugGUIBase::GetDefaultUnscaledHeight() const
{
  int w = GetDefaultUnscaledWidth();
  int arW = GetAspectRatioWidth();
  int arH = GetAspectRatioHeight();
  return w * arH / arW;
}

void
FBPlugGUIBase::SetContentScaleFactor(float scale)
{
  _scale = scale;
  setTransform(AffineTransform::scale(scale));
}