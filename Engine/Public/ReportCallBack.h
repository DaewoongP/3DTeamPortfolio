#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CReportCallBack final : public PxUserControllerHitReport
{
private:
	explicit CReportCallBack() = default;
	virtual ~CReportCallBack() = default;

public:
	virtual void onShapeHit(const PxControllerShapeHit & hit) override;
	virtual void onControllerHit(const PxControllersHit & hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit & hit) override;

public:
	static CReportCallBack* Create();
	_ulong Release();
};

END