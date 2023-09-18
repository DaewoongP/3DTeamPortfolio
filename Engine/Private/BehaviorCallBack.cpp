#include "../Public/BehaviorCallBack.h"

PxControllerBehaviorFlags CBehaviorCallBack::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
    return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CBehaviorCallBack::getBehaviorFlags(const PxController& controller)
{
    return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CBehaviorCallBack::getBehaviorFlags(const PxObstacle& obstacle)
{
    return PxControllerBehaviorFlags();
}

CBehaviorCallBack* CBehaviorCallBack::Create()
{
    CBehaviorCallBack* pInstance = New CBehaviorCallBack();

	return pInstance;
}

_ulong CBehaviorCallBack::Release()
{
	delete this;
	return 0;
}
