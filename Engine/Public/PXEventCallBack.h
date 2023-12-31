#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//
/* =============================================== */

#include "Engine_Defines.h"

BEGIN(Engine)

class CPXEventCallBack : public PxSimulationEventCallback
{
	enum EVENTSHAPE { SHAPE_SOUR, SHAPE_DEST, SHAPE_END };
private:
	CPXEventCallBack() = default;
	~CPXEventCallBack() = default;

public:
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	virtual void onWake(PxActor** actors, PxU32 count) override;
	virtual void onSleep(PxActor** actors, PxU32 count) override;
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;

public:
	static CPXEventCallBack* Create();
	void Release();
};

END