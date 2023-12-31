#include "Level_Tool.h"
#include "GameInstance.h"
#include"Light.h"
CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_For_Layer_Tool(TEXT("Layer_Tool"))))
		return E_FAIL;
	if (FAILED(Ready_For_Layer_Light(TEXT("Layer_Light"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_Tool::Render()
{
	if (FAILED(__super::Render()))
	{
		MSG_BOX("Failed CLevel_Tool Rendering");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tool::Ready_For_Layer_Tool(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
		TEXT("Prototype_GameObject_Terrain"), pLayerTag, TEXT("GameObject_Terrain"))))
	{
		MSG_BOX("Failed Add GameObject CTerrain");
		return E_FAIL;
	}

	/* For.GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
		TEXT("Prototype_GameObject_Camera_Free"), pLayerTag, TEXT("GameObject_Camera_Free"))))
	{
		MSG_BOX("Failed Add GameObject GameObject_Camera_Free");
		return E_FAIL;
	}

	/* For.GameObject_Sky */
	if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
		TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"))))
	{
		MSG_BOX("Failed Add GameObject GameObject_Sky");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_For_Layer_Light(const _tchar* pLayerTag)
{
	BEGININSTANCE
	CLight::LIGHTDESC LightDesc;
	ZEROMEM(&LightDesc);
	LightDesc.vPos = _float4(0.f, 30.f, 0.f, 1.f);
	LightDesc.vDir = _float4(0.f, -0.98f, 0.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	pGameInstance->Add_Light(LightDesc);
	ENDINSTANCE
	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = New CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
}
