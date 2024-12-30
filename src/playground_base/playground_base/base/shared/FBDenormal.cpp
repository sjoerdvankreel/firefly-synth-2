#include <playground_base/base/shared/FBDenormal.hpp>

#if (defined __APPLE__) && defined(__aarch64__)
#include <sse2neon.h>
#else
#include <immintrin.h>
#endif

void
FBRestoreDenormal(FBDenormalState state)
{
  static_assert(sizeof(void*) == 8);
  _MM_SET_FLUSH_ZERO_MODE(state.first);
  _MM_SET_DENORMALS_ZERO_MODE(state.second);
}

FBDenormalState
FBDisableDenormal()
{
  std::uint32_t ftz = _MM_GET_FLUSH_ZERO_MODE();
  std::uint32_t daz = _MM_GET_DENORMALS_ZERO_MODE();
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  return std::make_pair(ftz, daz);
}