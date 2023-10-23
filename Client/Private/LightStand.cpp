#include "..\Public\LightStand.h"
#include "GameInstance.h"

CLightStand::CLightStand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CLightStand::CLightStand(const CLightStand& rhs)
	: CMapObject(rhs)
{
}

HRESULT CLightStand::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	BEGININSTANCE;

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_LightStand_Fire")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_LightStand_Fire")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/LightStand_Fire"), LEVEL_STATIC))))
		{
			ENDINSTANCE;
			MSG_BOX("Failed to Create Prototype_GameObject_LightStand_Fire");
			return E_FAIL;
		}
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLightStand::Initialize(void* pArg)
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

	m_pEffect->Play(m_pTransform->Get_Position() + _float3(0, 2.5f, 0));
	m_pEffect->Get_MainModuleRef().fSimulationSpeed = Random_Generator(0.5f, 1.0f);

	return S_OK;
}

HRESULT CLightStand::Initialize_Level(_uint iCurrentLevelIndex)
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
	LightDescHork.vPos = m_pTransform->Get_Position().TransCoord();
	LightDescHork.fRange = 10.f;

	LightDescHork.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDescHork.vAmbient = WHITEDEFAULT;
	LightDescHork.vSpecular = LightDescHork.vDiffuse;

	BEGININSTANCE;
	pGameInstance->Add_Light(LightDescHork, nullptr);
	ENDINSTANCE;

	return S_OK;
}

void CLightStand::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLightStand::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

CLightStand* CLightStand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightStand* pInstance = New CLightStand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLightStand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightStand::Clone(void* pArg)
{
	CLightStand* pInstance = New CLightStand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLightStand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLightStand::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
