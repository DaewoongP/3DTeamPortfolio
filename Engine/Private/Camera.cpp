#include "../Public/Camera.h"
#include "PipeLine.h"
#include "Transform.h"

HRESULT CCamera::Initialize(const CAMERADESC& CameraDesc)
{
	m_pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(m_pPipeLine);

	m_CameraDesc = CameraDesc;

	return S_OK;
}

void CCamera::Tick(const _float& fTimeDelta)
{
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.m_fFovY, m_CameraDesc.m_fAspect, m_CameraDesc.m_fNear, m_CameraDesc.m_fFar));
	m_pPipeLine->Set_CameraFar(m_CameraDesc.m_fFar);
}

CCamera* CCamera::Create(const CAMERADESC& CameraDesc)
{
	CCamera* pInstance = new CCamera();

	if (FAILED(pInstance->Initialize(CameraDesc)))
	{
		MSG_BOX("Failed to Created CCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera::Free()
{
	Safe_Release(m_pPipeLine);
}
