#include "../Public/Camera.h"
#include "PipeLine.h"
#include "Transform.h"

HRESULT CCamera::Initialize(TYPE eType, _float fFovY, _float fAspect, _float fNear, _float fFar)
{
	m_pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(m_pPipeLine);

	m_eType = eType;
	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));
	m_pPipeLine->Set_CameraFar(m_fFar);
}

CCamera* CCamera::Create(TYPE eType, _float fFovY, _float fAspect, _float fNear, _float fFar)
{
	CCamera* pInstance = new CCamera();

	if (FAILED(pInstance->Initialize(eType, fFovY, fAspect, fNear, fFar)))
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
