#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <string>
#include <optional>

struct FBSteppedParamNonRealTime:
public IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBSteppedParamNonRealTime);
  bool IsStepped() const override final { return true; }
  virtual float PlainToNormalized(int plain) const = 0;
  virtual int NormalizedToPlain(float normalized) const = 0;
  virtual std::string PlainToText(FBValueTextDisplay display, int plain) const = 0;
  virtual std::optional<int> TextToPlain(FBValueTextDisplay display, std::string const& text) const = 0;
};