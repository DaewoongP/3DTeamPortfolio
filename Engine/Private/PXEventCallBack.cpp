#include "..\Public\PXEventCallBack.h"
#include "GameObject.h"

void CPXEventCallBack::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void CPXEventCallBack::onWake(PxActor** actors, PxU32 count)
{
}

void CPXEventCallBack::onSleep(PxActor** actors, PxU32 count)
{
}

void CPXEventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; ++i)
	{
		COLLISIONDESC Desc;
		CComposite* pSourData = reinterpret_cast<CComposite*>(pairHeader.actors[0]->userData);
		CComposite* pDestData = reinterpret_cast<CComposite*>(pairHeader.actors[1]->userData);
		if (nullptr == pSourData ||
			nullptr == pDestData)
			continue;
		
		CGameObject* pSourObject = static_cast<CGameObject*>(pSourData->Get_Owner());
		CGameObject* pDestObject = static_cast<CGameObject*>(pDestData->Get_Owner());

		if (nullptr == pSourObject ||
			nullptr == pDestObject)
			continue;

		if (pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND))
		{
			pSourObject->OnCollisionEnter(Desc);
			pDestObject->OnCollisionEnter(Desc);
		}

		if (pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_PERSISTS))
		{
			pSourObject->OnCollisionStay(Desc);
			pDestObject->OnCollisionStay(Desc);
		}

		if (pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_LOST))
		{
			pSourObject->OnCollisionExit(Desc);
			pDestObject->OnCollisionExit(Desc);
		}
	}
}

void CPXEventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	cout << "Trigger Collision" << endl;
	for (PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;
		
	}
}

void CPXEventCallBack::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}

CPXEventCallBack* CPXEventCallBack::Create()
{
	CPXEventCallBack* pInstance = new CPXEventCallBack;

	return pInstance;
}

void CPXEventCallBack::Release()
{
	delete this;
}
