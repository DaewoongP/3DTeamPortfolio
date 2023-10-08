#include "PotionStationCamera.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Transform.h"
PotionStationCamera::PotionStationCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

HRESULT PotionStationCamera::Initialize(void* pArg)
{
	POTIONSTATION_CAMERA_DESC* pDesc = static_cast<POTIONSTATION_CAMERA_DESC*>(pArg);

	if(FAILED(__super::Initialize(pDesc->pSuperDesc)))
		return E_FAIL;

	m_vAt = pDesc->vAt;
	m_vEye = _float3(m_vAt.x - 1.f, m_vAt.y + 1.f, m_vAt.z - 1.f);
	
	return S_OK;
}

void PotionStationCamera::Tick(const _float& fTimeDelta)
{
	BEGININSTANCE;

	pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixLookAtLH(m_vEye, m_vAt, _float3(0.0f, 1.0f, 0.0f)));

	ENDINSTANCE;
}

PotionStationCamera* PotionStationCamera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
	PotionStationCamera* pInstance = New PotionStationCamera(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create PotionStationCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void PotionStationCamera::Free()
{
}
