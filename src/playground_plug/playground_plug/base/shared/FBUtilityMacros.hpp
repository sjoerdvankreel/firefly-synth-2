#pragma once

#define FB_STRINGIFY_(x) #x
#define FB_STRINGIFY(x) FB_STRINGIFY_(x)

#define FB_RAW_OUTPUT_BUFFER float* __restrict const* __restrict
#define FB_RAW_INPUT_BUFFER float const* __restrict const* __restrict