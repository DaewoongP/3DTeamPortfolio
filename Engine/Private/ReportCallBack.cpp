#include "../public/ReportCallBack.h"

void CReportCallBack::onShapeHit(const PxControllerShapeHit& hit)
{
	// �ٸ� ������ٵ�� �浹�� ���⼭ �����մϴ�.

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