#pragma once

#define FB_CHANNEL_LEFT 0
#define FB_CHANNEL_RIGHT 1
#define FB_CHANNELS_STEREO 2

#define FB_STRINGIFY_(x) #x
#define FB_STRINGIFY(x) FB_STRINGIFY_(x)

#define FB_RAW_AUDIO_OUTPUT_BUFFER float* __restrict const* __restrict
#define FB_RAW_AUDIO_INPUT_BUFFER float const* __restrict const* __restrict