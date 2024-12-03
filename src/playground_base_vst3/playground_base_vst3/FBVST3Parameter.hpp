#pragma once

#include <playground_base/base/topo/FBStaticParam.hpp>
#include <public.sdk/source/vst/vstparameters.h>

using namespace Steinberg::Vst;

class FBVST3Parameter:
public Parameter
{
	FBStaticParam const _topo;

public:
	virtual ~FBVST3Parameter() = default;
  FBVST3Parameter(FBStaticParam const& topo, ParameterInfo const& info);

	void toString(ParamValue valueNormalized, String128 string) const override;
	bool fromString(const TChar* string, ParamValue& valueNormalized) const override;
};