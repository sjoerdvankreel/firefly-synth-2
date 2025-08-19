#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <map>
#include <memory>
#include <cstdint>

// Single-threaded, grow only.
class FBMemoryPool final
{
  struct Entry
  {
    bool inUse = {};
    std::unique_ptr<std::byte[]> memory = {};
  };

  std::map<void*, std::size_t> _ptrToSize = {};
  std::map<std::size_t, std::vector<Entry>> _storage = {};

public:
  FB_NOCOPY_MOVE_DEFCTOR(FBMemoryPool);

  void Return(void* ptr);
  void* Lease(std::size_t size);
};