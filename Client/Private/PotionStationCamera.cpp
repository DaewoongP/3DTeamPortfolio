#include "PotionStationCamera.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Transform.h"

CPotionStationCamera::CPotionStationCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

HRESULT CPotionStationCamera::Initialize(void* pArg)
{
	POTIONSTATION_CAMERA_DESC* pDesc = static_cast<POTIONSTATION_CAMERA_DESC*>(pArg);

	if(FAILED(__super::Initialize(pDesc->pSuperDesc)))
		return E_FAIL;

	m_vAt = pDesc->vAt;
	m_vEye = _float3(97.234, 8.100, 78.389);
	
	return S_OK;
}

void CPotionStationCamera::Tick(const _float& fTimeDelta)
{
	BEGININSTANCE;

	pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixLookAtLH(m_vEye, m_vAt, _float3(0.0f, 1.0f, 0.0f)));
#ifdef _DEBUG
	//ADD_IMGUI([&] { this->Tick_Imgui(fTimeDelta); });
#endif // _DEBUG
	
	ENDINSTANCE;
}
#ifdef _DEBUG
void CPotionStationCamera::Tick_Imgui(_float fTimeDelta)
{
	ImGui::Begin("PotionStationCamera");
	ImGui::DragFloat3("Eye", reinterpret_cast<_float*>(&m_vEye), 0.01f);
	ImGui::DragFloat3("At", reinterpret_cast<_float*>(&m_vAt), 0.01f);
	ImGui::End();
}
#endif // _DEBUG
CPotionStationCamera* CPotionStationCamera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
	CPotionStationCamera* pInstance = New CPotionStationCamera(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create CPotionStationCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPotionStationCamera::Free()
{
	__super::Free();
}