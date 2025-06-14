#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <readerwriterqueue.h>

class FBCLAPExchangeStateQueueBase
{
public:
  virtual bool TryDequeue(void* data) = 0;
  virtual bool Enqueue(void const* data) = 0;
  virtual ~FBCLAPExchangeStateQueueBase() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBCLAPExchangeStateQueueBase);
};

template <class TExchangeState>
class FBCLAPExchangeStateQueue:
public FBCLAPExchangeStateQueueBase
{
  moodycamel::ReaderWriterQueue<TExchangeState, 2> _queue;
public:
  FBCLAPExchangeStateQueue() : _queue(2) {}
  bool TryDequeue(void* data) override
  { return _queue.try_dequeue(*static_cast<TExchangeState*>(data)); }
  bool Enqueue(void const* data) override
  { return _queue.enqueue(*static_cast<TExchangeState const*>(data)); }
};