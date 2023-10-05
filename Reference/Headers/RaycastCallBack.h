#pragma once
#include "Engine_Defines.h"

class CRaycastCallBack : public PxHitCallback<PxRaycastHit>
{
	virtual inline PxAgain processTouches(const PxRaycastHit* buffer, PxU32 nbHits) override
	{
		int a = 1;
	}
};

