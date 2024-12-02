#pragma once

#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <public.sdk/source/vst/vstparameters.h>

using namespace Steinberg::Vst;

struct FBStaticParam;

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