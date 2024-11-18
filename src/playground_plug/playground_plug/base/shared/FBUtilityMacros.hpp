#pragma once

#define FB_CHANNEL_L 0
#define FB_CHANNEL_R 1
#define FB_CHANNELS_STEREO 2

#define FB_STRINGIFY_(x) #x
#define FB_STRINGIFY(x) FB_STRINGIFY_(x)

#define FB_NOCOPY_NOMOVE(x) \
  x(x&&) = delete; \
  x(x const&) = delete; \
  x& operator=(x&&) = delete; \
  x& operator=(x const&) = delete

#define FB_NOCOPY_NOMOVE_DEFAULT_CTOR(x) \
  x() = default; \
  FB_NOCOPY_NOMOVE(x)