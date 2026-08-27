#pragma once

#include <public.sdk/source/vst/vstparameters.h>

using namespace Steinberg::Vst;

struct FBRuntimeParam;
class FBHostGUIContext;

class FBVST3Parameter final:
public Parameter
{
	FBHostGUIContext const* _hostContext;
	FBRuntimeParam const* const _topo;
	ParameterInfo _infoOverride;

public:
	virtual ~FBVST3Parameter() = default;
  FBVST3Parameter(FBHostGUIContext const* hostContext, FBRuntimeParam const* topo, ParameterInfo const& info);

	void OnNameChanged();
	ParameterInfo& getInfo() override { return _infoOverride; }
	ParameterInfo const& getInfo() const override { return _infoOverride; };
	void toString(ParamValue valueNormalized, String128 string) const override;
	bool fromString(const TChar* string, ParamValue& valueNormalized) const override;
};