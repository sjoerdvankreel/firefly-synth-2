#pragma once

#define FB_CHANNELS_STEREO 2

#define FB_STRINGIFY_(x) #x
#define FB_STRINGIFY(x) FB_STRINGIFY_(x)

#define FB_EXPLICIT_COPY(x) \
  explicit x(x const&) = default; \
  x& operator=(x const&) = delete

#define FB_EXPLICIT_COPY_DEFAULT_CTOR(x) \
  x() = default; \
  FB_EXPLICIT_COPY(x)

#define FB_NOCOPY_NOMOVE(x) \
  x(x&&) = delete; \
  x(x const&) = delete; \
  x& operator=(x&&) = delete; \
  x& operator=(x const&) = delete

#define FB_NOCOPY_NOMOVE_DEFAULT_CTOR(x) \
  x() = default; \
  FB_NOCOPY_NOMOVE(x)