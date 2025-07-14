#pragma once

#include <public.sdk/source/vst/vstparameters.h>

using namespace Steinberg::Vst;

struct FBRuntimeParam;

class FBVST3Parameter final:
public Parameter
{
	FBRuntimeParam const* const _topo;

public:
	virtual ~FBVST3Parameter() = default;
  FBVST3Parameter(FBRuntimeParam const* topo, ParameterInfo const& info);

	void toString(ParamValue valueNormalized, String128 string) const override;
	bool fromString(const TChar* string, ParamValue& valueNormalized) const override;
};