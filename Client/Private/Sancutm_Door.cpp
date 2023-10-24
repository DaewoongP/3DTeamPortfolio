#include "..\Public\Sancutm_Door.h"
#include "GameInstance.h"

#include "LightStand.h"

CSancutm_Door::CSancutm_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CSancutm_Door::CSancutm_Door(const CSancutm_Door& rhs)
	: CMapObject(rhs)
{
}

HRESULT CSancutm_Door::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSancutm_Door::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CSancutm_Door Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSancutm_Door::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CSancutm_Door Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	BEGININSTANCE;
	auto pBackGroundLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SANCTUM, TEXT("Layer_BackGround"));
	ENDINSTANCE;

	for (auto Pair : *pBackGroundLayer)
	{
		wstring wsObjTag = Pair.first;

		if (wstring::npos != wsObjTag.find(TEXT("LightStand")))
		{
			m_pLightStands.push_back(static_cast<CLightStand*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	return S_OK;
}

void CSancutm_Door::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Door_Action(fTimeDelta);
}

void CSancutm_Door::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

void CSancutm_Door::Door_Action(_float fTimeDelta)
{

}

CSancutm_Door* CSancutm_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSancutm_Door* pInstance = New CSancutm_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSancutm_Door");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSancutm_Door::Clone(void* pArg)
{
	CSancutm_Door* pInstance = New CSancutm_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSancutm_Door");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSancutm_Door::Free()
{
	__super::Free();

	for (auto& iter : m_pLightStands)
		Safe_Release(iter);
}
