#include "../public/ReportCallBack.h"

void CReportCallBack::onShapeHit(const PxControllerShapeHit& hit)
{
	// 다른 리지드바디와 충돌은 여기서 관리합니다.

}

void CReportCallBack::onControllerHit(const PxControllersHit& hit)
{
}

void CReportCallBack::onObstacleHit(const PxControllerObstacleHit& hit)
{
}

CReportCallBack* CReportCallBack::Create()
{
	CReportCallBack* pInstance = New CReportCallBack();

	return pInstance;
}

_ulong CReportCallBack::Release()
{
	delete this;
	return 0;
}