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
		COLLISIONDESC SourDesc, DestDesc;
		CGameObject* pSourObject = static_cast<CGameObject*>(pairHeader.actors[0]->userData);
		CGameObject* pDestObject = static_cast<CGameObject*>(pairHeader.actors[1]->userData);

		SourDesc.pOtherObjectTag = pDestObject->Get_Tag();
		SourDesc.pOtherOwner = pDestObject;
		SourDesc.pOtherTransform = pDestObject->Get_Transform();

		DestDesc.pOtherObjectTag = pSourObject->Get_Tag();
		DestDesc.pOtherOwner = pSourObject;
		DestDesc.pOtherTransform = pSourObject->Get_Transform();

		if (nullptr == pSourObject ||
			nullptr == pDestObject)
			continue;

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
	for (PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;
		
		COLLISIONDESC SourDesc, DestDesc;

		
		CGameObject* pSourObject = static_cast<CGameObject*>(pairs->triggerActor->userData);
		CGameObject* pDestObject = static_cast<CGameObject*>(pairs->otherActor->userData);

		SourDesc.pOtherObjectTag = pDestObject->Get_Tag();
		SourDesc.pOtherOwner = pDestObject;
		SourDesc.pOtherTransform = pDestObject->Get_Transform();

		DestDesc.pOtherObjectTag = pSourObject->Get_Tag();
		DestDesc.pOtherOwner = pSourObject;
		DestDesc.pOtherTransform = pSourObject->Get_Transform();

		if (nullptr == pSourObject ||
			nullptr == pDestObject)
			continue;
		
		if (pairs->status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			pSourObject->OnCollisionEnter(SourDesc);
			pDestObject->OnCollisionEnter(DestDesc);
		}

		// trigger는 stay 존재 x 따로 제작해주거나 해야함.

		if (pairs->status == PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			pSourObject->OnCollisionExit(SourDesc);
			pDestObject->OnCollisionExit(DestDesc);
		}

		if (CComponent::OBJ_DEAD == pSourObject->Get_ObjEvent())
		{
			pairs->triggerActor->userData = nullptr;
		}
		else if (CComponent::OBJ_DEAD == pDestObject->Get_ObjEvent())
		{
			pairs->otherActor->userData = nullptr;
		}
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
