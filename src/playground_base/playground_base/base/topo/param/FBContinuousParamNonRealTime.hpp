#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <string>
#include <optional>

struct FBContinuousParamNonRealTime:
public IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBContinuousParamNonRealTime);
  int ValueCount() const override final { return 0; }
  bool IsItems() const override final { return false; }
  bool IsStepped() const override final { return false; }

  virtual float PlainToNormalized(float plain) const = 0;
  virtual float NormalizedToPlain(float normalized) const = 0;
  virtual std::string PlainToText(FBValueTextDisplay display, float plain) const = 0;
  virtual std::optional<float> TextToPlain(FBValueTextDisplay display, std::string const& text) const = 0;
};