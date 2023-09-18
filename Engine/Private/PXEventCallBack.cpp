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
		COLLEVENTDESC SourDesc, DestDesc;
		CGameObject* pSourObject = static_cast<CGameObject*>(pairHeader.actors[0]->userData);
		CGameObject* pDestObject = static_cast<CGameObject*>(pairHeader.actors[1]->userData);
		//pairHeader.flags == PxContactPairHeaderFlag::eREMOVED_ACTOR_0;

		if (nullptr == pSourObject ||
			nullptr == pDestObject)
			continue;

		SourDesc.pOtherObjectTag = pDestObject->Get_Tag();
		SourDesc.pOtherCollisionTag = static_cast<_tchar*>(pairs[i].shapes[1]->userData);
		SourDesc.pOtherOwner = pDestObject;
		SourDesc.pOtherTransform = pDestObject->Get_Transform();
		SourDesc.pArg = pDestObject->Get_CollisionData();

		DestDesc.pOtherObjectTag = pSourObject->Get_Tag();
		SourDesc.pOtherCollisionTag = static_cast<_tchar*>(pairs[i].shapes[0]->userData);
		DestDesc.pOtherOwner = pSourObject;
		DestDesc.pOtherTransform = pSourObject->Get_Transform();
		DestDesc.pArg = pSourObject->Get_CollisionData();

		if (pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND))
		{
			pSourObject->OnCollisionEnter(SourDesc);
			pDestObject->OnCollisionEnter(DestDesc);
		}

		if (pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_PERSISTS))
		{
			pSourObject->OnCollisionStay(SourDesc);
			pDestObject->OnCollisionStay(DestDesc);

			if (CComponent::OBJ_DEAD == pSourObject->Get_ObjEvent())
			{
				pSourObject->OnCollisionExit(SourDesc);
				pDestObject->OnCollisionExit(DestDesc);
				pairHeader.actors[0]->userData = nullptr;
			}
			else if (CComponent::OBJ_DEAD == pDestObject->Get_ObjEvent())
			{
				pSourObject->OnCollisionExit(SourDesc);
				pDestObject->OnCollisionExit(DestDesc);
				pairHeader.actors[1]->userData = nullptr;
			}
		}

		if (pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_LOST))
		{
			pSourObject->OnCollisionExit(SourDesc);
			pDestObject->OnCollisionExit(DestDesc);
		}
	}
}

void CPXEventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	std::lock_guard<std::mutex> lock(mtx);

	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;
		
		COLLEVENTDESC SourDesc, DestDesc;
		CGameObject* pSourObject = static_cast<CGameObject*>(pairs->triggerActor->userData);
		CGameObject* pDestObject = static_cast<CGameObject*>(pairs->otherActor->userData);
		
		if (nullptr == pSourObject ||
			nullptr == pDestObject)
			continue;

		SourDesc.pOtherObjectTag = pDestObject->Get_Tag();
		SourDesc.pOtherCollisionTag = static_cast<_tchar*>(pairs[i].otherShape->userData);
		SourDesc.pOtherOwner = pDestObject;
		SourDesc.pOtherTransform = pDestObject->Get_Transform();
		SourDesc.pArg = pDestObject->Get_CollisionData();

		DestDesc.pOtherObjectTag = pSourObject->Get_Tag();
		DestDesc.pOtherCollisionTag = static_cast<_tchar*>(pairs[i].triggerShape->userData);
		DestDesc.pOtherOwner = pSourObject;
		DestDesc.pOtherTransform = pSourObject->Get_Transform();
		DestDesc.pArg = pSourObject->Get_CollisionData();

		if (pairs->status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			pSourObject->OnCollisionEnter(SourDesc);
			pDestObject->OnCollisionEnter(DestDesc);
		}

		// trigger는 stay 존재 x 따로 제작해주거나 해야함.

		if (CComponent::OBJ_DEAD == pSourObject->Get_ObjEvent())
		{
			pSourObject->OnCollisionExit(SourDesc);
			pDestObject->OnCollisionExit(DestDesc);
			pairs->triggerActor->userData = nullptr;
			continue;
		}
		else if (CComponent::OBJ_DEAD == pDestObject->Get_ObjEvent())
		{
			pSourObject->OnCollisionExit(SourDesc);
			pDestObject->OnCollisionExit(DestDesc);
			pairs->otherActor->userData = nullptr;
			continue;
		}

		if (pairs->status == PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			pSourObject->OnCollisionExit(SourDesc);
			pDestObject->OnCollisionExit(DestDesc);
		}
	}
}

void CPXEventCallBack::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}

CPXEventCallBack* CPXEventCallBack::Create()
{
	CPXEventCallBack* pInstance = New CPXEventCallBack;

	return pInstance;
}

void CPXEventCallBack::Release()
{
	delete this;
}
