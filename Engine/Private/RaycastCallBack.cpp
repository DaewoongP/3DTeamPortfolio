#include "..\Public\RaycastCallBack.h"

CRaycastCallBack::CRaycastCallBack(PxRaycastHit* aTouches, PxU32 aMaxNbTouches)
	: PxHitCallback<PxRaycastHit>(aTouches, aMaxNbTouches)
{
}

PxAgain CRaycastCallBack::processTouches(const PxRaycastHit* buffer, PxU32 nbHits)
{
	PxAgain isTrue = { false };
	for (_uint i = 0; i < nbHits - 1; ++i)
	{
		if (!strcmp(m_pCollisionTag, buffer[i].shape->getName()))
		{

			isTrue = true;
		}
	}

	return isTrue;
}

void CRaycastCallBack::finalizeQuery()
{
	int a = 1;
}

CRaycastCallBack* CRaycastCallBack::Create(PxRaycastHit* aTouches, PxU32 aMaxNbTouches)
{
	return New CRaycastCallBack(aTouches, aMaxNbTouches);
}

_ulong CRaycastCallBack::Release()
{
	delete this;
	return 0;
}
