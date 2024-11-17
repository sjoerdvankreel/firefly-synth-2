#pragma once

#define FB_CHANNEL_LEFT 0
#define FB_CHANNEL_RIGHT 1
#define FB_CHANNELS_STEREO 2

#define FB_STRINGIFY_(x) #x
#define FB_STRINGIFY(x) FB_STRINGIFY_(x)

#define FB_NOCOPY_NOMOVE_DEFAULT_CTOR(x) \
  x() = default; \
  x(x&&) = delete; \
  x(x const&) = delete; \
  x& operator=(x&&) = delete; \
  x& operator=(x const&) = delete