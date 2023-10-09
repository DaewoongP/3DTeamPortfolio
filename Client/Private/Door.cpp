#include "..\Public\Door.h"
#include "GameInstance.h"

CDoor::CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CDoor::CDoor(const CDoor& rhs)
	: CMapObject(rhs)
{
}

HRESULT CDoor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoor::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CDoor Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CDoor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Open_Door(fTimeDelta);
}

void CDoor::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);

#ifdef _DEBUG
		for (auto& pRigidBody : m_RigidBodys)
		{
			m_pRenderer->Add_DebugGroup(pRigidBody);
		}
#endif // _DEBUG
	}
}

void CDoor::Open_Door(_float fTimeDelta)
{
}

CDoor* CDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoor* pInstance = New CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoor::Clone(void* pArg)
{
	CDoor* pInstance = New CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDoor::Free()
{
	__super::Free();

	for (auto& pRigidBody : m_RigidBodys)
		Safe_Release(pRigidBody);
	m_RigidBodys.clear();

	Safe_Release(m_pShader);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
