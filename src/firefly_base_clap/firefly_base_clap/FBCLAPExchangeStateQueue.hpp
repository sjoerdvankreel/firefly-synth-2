#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <readerwriterqueue.h>
#include <cstddef>

class FBCLAPExchangeStateQueueBase
{
public:
  virtual bool TryDequeue(void* data) = 0;
  virtual bool TryEnqueue(void const* data) = 0;
  virtual ~FBCLAPExchangeStateQueueBase() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBCLAPExchangeStateQueueBase);
};

template <class TExchangeState>
class FBCLAPExchangeStateQueue:
public FBCLAPExchangeStateQueueBase
{
  // Prevent moody from calling the default ctor to init everything, we will overwrite anyway.
  struct alignas(alignof(TExchangeState)) RawExchangeState
  {
    std::byte data[sizeof(TExchangeState)];
  };

  moodycamel::ReaderWriterQueue<RawExchangeState, 2> _queue;
public:
  FBCLAPExchangeStateQueue() : _queue(2) {}
  bool TryDequeue(void* data) override
  { return _queue.try_dequeue(*static_cast<RawExchangeState*>(data)); }
  bool TryEnqueue(void const* data) override
  { return _queue.try_enqueue(*static_cast<RawExchangeState const*>(data)); }
};