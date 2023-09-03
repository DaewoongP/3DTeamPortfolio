#include "..\Public\PXEventCallBack.h"

void CPXEventCallBack::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
	cout << "1" << endl;
}

void CPXEventCallBack::onWake(PxActor** actors, PxU32 count)
{
	cout << "1" << endl;
}

void CPXEventCallBack::onSleep(PxActor** actors, PxU32 count)
{
	cout << "1" << endl;
}

void CPXEventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	cout << "1" << endl;
}

void CPXEventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	cout << "1" << endl;
	for (PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		// Detect for example that a player entered a checkpoint zone
		if (pairs[i].otherShape->getActor() &&
			pairs[i].triggerShape->getActor())
		{
		}
	}
}

void CPXEventCallBack::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
	cout << "1" << endl;
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
