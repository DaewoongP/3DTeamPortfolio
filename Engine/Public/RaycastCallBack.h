#pragma once
#include "Engine_Defines.h"

class CRaycastCallBack : public PxHitCallback<PxRaycastHit>
{
public:
	void Set_Tag(const _char* pTag) { strcpy_s(m_pCollisionTag, sizeof(_char) * MAX_PATH, pTag); }

public:
	explicit CRaycastCallBack(PxRaycastHit* aTouches, PxU32 aMaxNbTouches);
	virtual ~CRaycastCallBack() = default;

public:
	virtual PxAgain processTouches(const PxRaycastHit* buffer, PxU32 nbHits) override;
	virtual void finalizeQuery() override;

private:
	_char			m_pCollisionTag[MAX_PATH] = "";
	
public:
	static CRaycastCallBack* Create(PxRaycastHit* aTouches, PxU32 aMaxNbTouches);
	_ulong Release();
};

