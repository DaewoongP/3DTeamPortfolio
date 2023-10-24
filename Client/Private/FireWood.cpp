#include "..\Public\FireWood.h"
#include "GameInstance.h"

CFireWood::CFireWood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CFireWood::CFireWood(const CFireWood& rhs)
	: CMapObject(rhs)
{
}

HRESULT CFireWood::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireWood::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CLightStand Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_LightStand_Fire"),
		TEXT("Com_Torch"), reinterpret_cast<CComponent**>(&m_pEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_LightStand_Fire)");
		__debugbreak();
		return E_FAIL;
	}

	m_pEffect->Play(m_pTransform->Get_Position() + _float3(0, 1.25f, 0));

	return S_OK;
}

HRESULT CFireWood::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// 자체 발광 추가
	CLight::LIGHTDESC		LightDescHork;
	ZeroMemory(&LightDescHork, sizeof LightDescHork);

	LightDescHork.eType = CLight::TYPE_POINT;
	LightDescHork.vPos = m_pTransform->Get_Position().TransCoord() + _float3(0, 1.25f, 0);
	LightDescHork.fRange = 2.5f;

	LightDescHork.vDiffuse = _float4(0.5f, 0.5f, 0.f, 1.f);
	LightDescHork.vAmbient = LightDescHork.vDiffuse;
	LightDescHork.vSpecular = LightDescHork.vDiffuse;

	BEGININSTANCE;
	pGameInstance->Add_Light(LightDescHork, nullptr);
	ENDINSTANCE;

	return S_OK;
}

void CFireWood::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

CFireWood* CFireWood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireWood* pInstance = New CFireWood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFireWood");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFireWood::Clone(void* pArg)
{
	CFireWood* pInstance = New CFireWood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFireWood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireWood::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}