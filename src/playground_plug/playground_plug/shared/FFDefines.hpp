#pragma once

#define FF_STRINGIFY_(x) #x
#define FF_STRINGIFY(x) FF_STRINGIFY_(x)

#define FF_RAW_OUTPUT_BUFFER float* __restrict const* __restrict
#define FF_RAW_INPUT_BUFFER float const* __restrict const* __restrict