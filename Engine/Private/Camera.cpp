#include "../Public/Camera.h"
#include "PipeLine.h"
#include "Transform.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	m_pDevice(pDevice),
	m_pContext(pContext)
	
{
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize(const CAMERADESC& CameraDesc)
{
	m_pPipeLine = CPipeLine::GetInstance();

	Safe_AddRef(m_pDevice);

	m_CameraDesc = CameraDesc;

	m_pTransform = CTransform::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CCamera::Tick(const _float& fTimeDelta)
{
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.m_fFovY, m_CameraDesc.m_fAspect, m_CameraDesc.m_fNear, m_CameraDesc.m_fFar));
	m_pPipeLine->Set_CameraFar(m_CameraDesc.m_fFar);
}

CCamera* CCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CAMERADESC& CameraDesc)
{
	CCamera* pInstance = new CCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize(CameraDesc)))
	{
		MSG_BOX("Failed to Created CCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
