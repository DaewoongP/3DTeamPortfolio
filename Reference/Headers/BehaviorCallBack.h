#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CBehaviorCallBack final : public PxControllerBehaviorCallback
{
private:
	explicit CBehaviorCallBack() = default;
	virtual ~CBehaviorCallBack() = default;

public:
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) override;

public:
	static CBehaviorCallBack* Create();
	_ulong Release();
};

END