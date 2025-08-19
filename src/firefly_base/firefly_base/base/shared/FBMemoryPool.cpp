#include <firefly_base/base/shared/FBMemoryPool.hpp>

void 
FBMemoryPool::Return(void* ptr)
{
  auto iter1 = _ptrToSize.find(ptr);
  FB_ASSERT(iter1 != _ptrToSize.end());
  auto iter2 = _storage.find(iter1->second);
  FB_ASSERT(iter2 != _storage.end());
  for (int i = 0; i < iter2->second.size(); i++)
    if (iter2->second[i].memory.get() == ptr)
    {
      FB_ASSERT(iter2->second[i].inUse);
      iter2->second[i].inUse = false;
      return;
    }
  FB_ASSERT(false);
}

void* 
FBMemoryPool::Lease(std::size_t size)
{
  auto iter = _storage.find(size);
  if (iter != _storage.end())
    for (int i = 0; i < iter->second.size(); i++)
      if (!iter->second[i].inUse)
      {
        iter->second[i].inUse = true;
        return iter->second[i].memory.get();
      }
  _storage[size].push_back({ true, std::make_unique<std::byte[]>(size) });
  void* result = _storage[size][_storage[size].size() - 1].memory.get();
  _ptrToSize[result] = size;
  return result;
}