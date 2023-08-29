#pragma once
#include "Engine_Defines.h"

class CPXAllocator : public PxAllocatorCallback
{
public:
	virtual ~CPXAllocator() = default;

public:
	void* allocate(size_t iSize, const _char* pTypeName, const _char* pFilename, _int iLine)
	{
		void* ptr = platformAlignedAlloc(iSize);
		PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15) == 0);
		return ptr;
	}

	void deallocate(void* ptr)
	{
		platformAlignedFree(ptr);
	}

private:
	PX_FORCE_INLINE void* platformAlignedAlloc(size_t size)
	{
		return _aligned_malloc(size, 16);
	}

	PX_FORCE_INLINE void platformAlignedFree(void* ptr)
	{
		_aligned_free(ptr);
	}
};



