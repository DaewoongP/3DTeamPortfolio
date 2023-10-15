#include "..\Public\PXEventCallBack.h"
#include "Level_Manager.h"
#include "Level.h"
#include "RigidBody.h"
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
	CLevel_Manager* pLevel_Manager = CLevel_Manager::GetInstance();
	Safe_AddRef(pLevel_Manager);
	if (true == pLevel_Manager->Get_CurrentLevel()->Is_Loading())
	{
		Safe_Release(pLevel_Manager);
		return;
	}	
	Safe_Release(pLevel_Manager);

	for (PxU32 i = 0; i < nbPairs; ++i)
	{
		{
			std::lock_guard<std::mutex> lock(mtx);

			COLLEVENTDESC SourDesc, DestDesc;

			if (nullptr == pairs[i].shapes[SHAPE_SOUR]->userData ||
				nullptr == pairs[i].shapes[SHAPE_DEST]->userData)
				continue;

			CRigidBody::COLLISIONDATADESC SourCollisionDesc = *static_cast<CRigidBody::COLLISIONDATADESC*>(pairs[i].shapes[SHAPE_SOUR]->userData);
			CRigidBody::COLLISIONDATADESC DestCollisionDesc = *static_cast<CRigidBody::COLLISIONDATADESC*>(pairs[i].shapes[SHAPE_DEST]->userData);

			CGameObject* pSourObject = SourCollisionDesc.pOwnerObject;
			CGameObject* pDestObject = DestCollisionDesc.pOwnerObject;

			if (nullptr == pSourObject ||
				nullptr == pDestObject)
				continue;

			SourDesc.pOtherObjectTag = pDestObject->Get_Tag();
			MultiByteToWideChar(CP_ACP, 0, pairs[i].shapes[SHAPE_DEST]->getName(), strlen(pairs[i].shapes[SHAPE_DEST]->getName()) + 1, SourDesc.pOtherCollisionTag, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, pairs[i].shapes[SHAPE_SOUR]->getName(), strlen(pairs[i].shapes[SHAPE_SOUR]->getName()) + 1, SourDesc.pThisCollisionTag, MAX_PATH);
			SourDesc.pOtherOwner = pDestObject;
			SourDesc.pOtherTransform = pDestObject->Get_Transform();
			SourDesc.pArg = DestCollisionDesc.pCollisionData;

			DestDesc.pOtherObjectTag = pSourObject->Get_Tag();
			MultiByteToWideChar(CP_ACP, 0, pairs[i].shapes[SHAPE_SOUR]->getName(), strlen(pairs[i].shapes[SHAPE_SOUR]->getName()) + 1, DestDesc.pOtherCollisionTag, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, pairs[i].shapes[SHAPE_DEST]->getName(), strlen(pairs[i].shapes[SHAPE_DEST]->getName()) + 1, DestDesc.pThisCollisionTag, MAX_PATH);
			DestDesc.pOtherOwner = pSourObject;
			DestDesc.pOtherTransform = pSourObject->Get_Transform();
			DestDesc.pArg = SourCollisionDesc.pCollisionData;

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
					static_cast<CRigidBody::COLLISIONDATADESC*>(pairs[i].shapes[SHAPE_SOUR]->userData)->pOwnerObject = nullptr;
				}
				else if (CComponent::OBJ_DEAD == pDestObject->Get_ObjEvent())
				{
					pSourObject->OnCollisionExit(SourDesc);
					pDestObject->OnCollisionExit(DestDesc);
					static_cast<CRigidBody::COLLISIONDATADESC*>(pairs[i].shapes[SHAPE_DEST]->userData)->pOwnerObject = nullptr;
				}
			}

			if (pairs[i].events.isSet(PxPairFlag::eNOTIFY_TOUCH_LOST))
			{
				pSourObject->OnCollisionExit(SourDesc);
				pDestObject->OnCollisionExit(DestDesc);
			}
		}
	}
}

void CPXEventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	CLevel_Manager* pLevel_Manager = CLevel_Manager::GetInstance();
	Safe_AddRef(pLevel_Manager);
	if (true == pLevel_Manager->Get_CurrentLevel()->Is_Loading())
	{
		Safe_Release(pLevel_Manager);
		return;
	}
	Safe_Release(pLevel_Manager);

	for (PxU32 i = 0; i < count; i++)
	{
		{
			std::lock_guard<std::mutex> lock(mtx);

			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

			if (nullptr == pairs->triggerShape->userData ||
				nullptr == pairs->otherShape->userData)
				continue;

			CRigidBody::COLLISIONDATADESC SourCollisionDesc = *static_cast<CRigidBody::COLLISIONDATADESC*>(pairs->triggerShape->userData);
			CRigidBody::COLLISIONDATADESC DestCollisionDesc = *static_cast<CRigidBody::COLLISIONDATADESC*>(pairs->otherShape->userData);

			COLLEVENTDESC SourDesc, DestDesc;
			CGameObject* pSourObject = SourCollisionDesc.pOwnerObject;
			CGameObject* pDestObject = DestCollisionDesc.pOwnerObject;

			if (nullptr == pSourObject ||
				nullptr == pDestObject)
				continue;

			SourDesc.pOtherObjectTag = pDestObject->Get_Tag();
			MultiByteToWideChar(CP_ACP, 0, pairs[i].otherShape->getName(), strlen(pairs[i].otherShape->getName()) + 1, SourDesc.pOtherCollisionTag, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, pairs[i].triggerShape->getName(), strlen(pairs[i].triggerShape->getName()) + 1, SourDesc.pThisCollisionTag, MAX_PATH);
			SourDesc.pOtherOwner = pDestObject;
			SourDesc.pOtherTransform = pDestObject->Get_Transform();
			SourDesc.pArg = DestCollisionDesc.pCollisionData;

			DestDesc.pOtherObjectTag = pSourObject->Get_Tag();
			MultiByteToWideChar(CP_ACP, 0, pairs[i].triggerShape->getName(), strlen(pairs[i].triggerShape->getName()) + 1, DestDesc.pOtherCollisionTag, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, pairs[i].otherShape->getName(), strlen(pairs[i].otherShape->getName()) + 1, DestDesc.pThisCollisionTag, MAX_PATH);
			DestDesc.pOtherOwner = pSourObject;
			DestDesc.pOtherTransform = pSourObject->Get_Transform();
			DestDesc.pArg = SourCollisionDesc.pCollisionData;

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
				static_cast<CRigidBody::COLLISIONDATADESC*>(pairs->triggerShape->userData)->pOwnerObject = nullptr;
				continue;
			}
			else if (CComponent::OBJ_DEAD == pDestObject->Get_ObjEvent())
			{
				pSourObject->OnCollisionExit(SourDesc);
				pDestObject->OnCollisionExit(DestDesc);
				static_cast<CRigidBody::COLLISIONDATADESC*>(pairs->otherShape->userData)->pOwnerObject = nullptr;
				continue;
			}

			if (pairs->status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				pSourObject->OnCollisionExit(SourDesc);
				pDestObject->OnCollisionExit(DestDesc);
			}
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
