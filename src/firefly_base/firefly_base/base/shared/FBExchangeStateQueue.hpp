#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <readerwriterqueue.h>

class FBExchangeStateQueueBase
{
public:
  virtual bool TryDequeue(void* data) = 0;
  virtual bool TryEnqueue(void const* data) = 0;
  virtual ~FBExchangeStateQueueBase() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBExchangeStateQueueBase);
};

template <class TExchangeState>
class FBExchangeStateQueue:
public FBExchangeStateQueueBase
{
  // Prevent moody from calling the default ctor to init everything, we will overwrite anyway.
  struct alignas(alignof(TExchangeState)) RawExchangeState
  {
    std::byte data[sizeof(TExchangeState)];
  };

  moodycamel::ReaderWriterQueue<RawExchangeState, 2> _queue;
public:
  FBExchangeStateQueue() : _queue(2) {}
  bool TryDequeue(void* data) override
  { return _queue.try_dequeue(*static_cast<RawExchangeState*>(data)); }
  bool TryEnqueue(void const* data) override
  { return _queue.try_enqueue(*static_cast<RawExchangeState const*>(data)); }
};

// Just to make it work with the moodycamel queue without
// introducing accidental copy/move support into the plugin itself.
template <class TExchangeState>
struct alignas(alignof(TExchangeState)) FBQueuedExchangeState final
{
  TExchangeState state;
  FBQueuedExchangeState(FBQueuedExchangeState<TExchangeState> const& rhs);
  FBQueuedExchangeState& operator=(FBQueuedExchangeState<TExchangeState> const& rhs);
};

template <class TExchangeState>
FBQueuedExchangeState<TExchangeState>::
FBQueuedExchangeState(FBQueuedExchangeState<TExchangeState> const& rhs)
{
  FB_ASSERT(&rhs != this);
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdynamic-class-memaccess"
#endif
  memcpy(&state, &rhs.state, sizeof(TExchangeState));
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

template <class TExchangeState>
FBQueuedExchangeState<TExchangeState>&
FBQueuedExchangeState<TExchangeState>::operator=(FBQueuedExchangeState<TExchangeState> const& rhs)
{
  FB_ASSERT(&rhs != this);
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdynamic-class-memaccess"
#endif
  memcpy(&state, &rhs.state, sizeof(TExchangeState));
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  return *this;
}