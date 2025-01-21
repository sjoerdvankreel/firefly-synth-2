#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

class FBPlugGUI;

// TODO also visible
class FBParamsDependent
{
protected:
  FBPlugGUI* const _plugGUI;

private:
  std::vector<int> _evaluateValues;
  std::vector<int> const _evaluateWhen;

public:
  bool Evaluate();
  virtual bool Evaluate(std::vector<int> const& vals) const = 0;
  std::vector<int> const& EvaluateWhen() const { return _evaluateWhen; }

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamsDependent);
  FBParamsDependent(FBPlugGUI* plugGUI, std::vector<int> const& evaluateWhen);
};