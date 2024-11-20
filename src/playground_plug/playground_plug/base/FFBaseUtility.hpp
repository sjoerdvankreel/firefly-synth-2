#pragma once

#define FF_STRINGIFY_(x) #x
#define FF_STRINGIFY(x) FF_STRINGIFY_(x)

#define FF_NOCOPY_NOMOVE_NODEFCTOR(x) \
  x(x&&) = delete; \
  x(x const&) = delete; \
  x& operator=(x&&) = delete; \
  x& operator=(x const&) = delete

#define FF_NOCOPY_NOMOVE_DEFCTOR(x) \
  FF_NOCOPY_NOMOVE_NODEFCTOR(x); \
  x() = default