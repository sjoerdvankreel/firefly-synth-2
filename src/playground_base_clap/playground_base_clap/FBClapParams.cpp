#include <playground_base_clap/FBCLAPPlugin.hpp>

uint32_t 
FBCLAPPlugin::paramsCount() const noexcept
{
}

bool 
FBCLAPPlugin::paramsValue(clap_id paramId, double* value) noexcept
{
}

bool 
FBCLAPPlugin::paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept
{
}

bool 
FBCLAPPlugin::paramsTextToValue(clap_id paramId, const char* display, double* value) noexcept
{
}

void 
FBCLAPPlugin::paramsFlush(const clap_input_events* in, const clap_output_events* out) noexcept
{
}

bool 
FBCLAPPlugin::paramsValueToText(clap_id paramId, double value, char* display, uint32_t size) noexcept
{
}