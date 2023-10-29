#include "..\Public\SmithToCliff_Gate.h"
#include "GameInstance.h"

#include "Player.h"

CSmithToCliff_Gate::CSmithToCliff_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CSmithToCliff_Gate::CSmithToCliff_Gate(const CSmithToCliff_Gate& rhs)
	: CMapObject(rhs)
{
}

HRESULT CSmithToCliff_Gate::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSmithToCliff_Gate::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CSmithToCliff_Gate Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_SmithToCliff_Gate_Portal"),
		TEXT("Com_Portal"), reinterpret_cast<CComponent**>(&m_pEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_SmithToCliff_Gate_Portal)");
		__debugbreak();
		return E_FAIL;
	}

	m_pEffect->Play(m_pTransform->Get_Position() + _float3(0.f, 2.5f, 0.f));
	m_pEffect->Get_MainModuleRef().fSimulationSpeed = Random_Generator(0.5f, 1.0f);

	return S_OK;
}

HRESULT CSmithToCliff_Gate::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CSmithToCliff_Gate::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	BEGININSTANCE;
	if (false == pGameInstance->Is_SoundPlaying(m_iSound0))
	{
	//	m_iSound0 = pGameInstance->Play_Sound(TEXT("Portal_Idle.wav"), 1.f);
	}

	ENDINSTANCE;
}

void CSmithToCliff_Gate::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

CSmithToCliff_Gate* CSmithToCliff_Gate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmithToCliff_Gate* pInstance = New CSmithToCliff_Gate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSmithToCliff_Gate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSmithToCliff_Gate::Clone(void* pArg)
{
	CSmithToCliff_Gate* pInstance = New CSmithToCliff_Gate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSmithToCliff_Gate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSmithToCliff_Gate::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
