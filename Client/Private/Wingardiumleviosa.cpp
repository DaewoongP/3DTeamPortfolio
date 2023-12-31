#include "Wingardiumleviosa.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CWingardiumleviosa::CWingardiumleviosa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CWingardiumleviosa::CWingardiumleviosa(const CWingardiumleviosa& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CWingardiumleviosa::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CWingardiumleviosa::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Player Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Player Add_Components");

		return E_FAIL;
	}
	//만약 타겟이 존재하지 않는다면? 윙 가르디움 레비오우사는 그러면 안나감.
	if (m_pTarget == nullptr)
	{
		Set_ObjEvent(OBJ_DEAD);
	}
	else 
	{
		m_pEffectTrans->Set_Position(m_pTransform->Get_Position());
	}
	return S_OK;
}

void CWingardiumleviosa::Tick(_float fTimeDelta)
{
	//파티클의 위치를 타겟의 위치로.
	m_pEffectTrans->Set_Position(m_CurrentTargetMatrix.Translation());
	__super::Tick(fTimeDelta);
}

void CWingardiumleviosa::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWingardiumleviosa::Add_Components()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Wingardium_Effect"),
		TEXT("Com_Effect"), reinterpret_cast<CComponent**>(&m_pEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Wingardium_Effect)");
		__debugbreak();
		return E_FAIL;
	}


	m_pEffectTrans = m_pEffect->Get_Transform();
	Safe_AddRef(m_pEffectTrans);
	return S_OK;
}

CWingardiumleviosa* CWingardiumleviosa::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CWingardiumleviosa* pInstance = New CWingardiumleviosa(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CWingardiumleviosa");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWingardiumleviosa::Clone(void* pArg)
{
	CWingardiumleviosa* pInstance = New CWingardiumleviosa(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWingardiumleviosa");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWingardiumleviosa::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pEffectTrans);
		Safe_Release(m_pEffect);

	}
	
}
