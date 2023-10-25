#include "Racer.h"
#include "GameInstance.h"

CRacer::CRacer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRacer::CRacer(const CRacer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRacer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRacer::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CRacer Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRacer::Initialize_Level(_uint iCurrentLevelIndex)
{
	return S_OK;
}

void CRacer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRacer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CRacer::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//레이서는 공이랑만 충돌해서 태그 검사가 필요없어요.

}

HRESULT CRacer::Add_Components()
{
	/* Com_Model */
	//if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	//{
	//	MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
	//	__debugbreak();
	//	return E_FAIL;
	//}

	return S_OK;
}


CRacer* CRacer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRacer* pInstance = New CRacer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRacer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRacer::Clone(void* pArg)
{
	CRacer* pInstance = New CRacer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRacer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRacer::Free()
{
	__super::Free();
	if (m_isCloned)
	{
		Safe_Release(m_pRigidBody);
	}
}
