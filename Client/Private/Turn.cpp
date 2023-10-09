#include "Turn.h"

#include "GameInstance.h"
#include "Check_Degree.h"

CTurn::CTurn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CTurn::CTurn(const CTurn& rhs)
	: CBehavior(rhs)
{
}

HRESULT CTurn::Initialize(void* pArg)
{
	BEGININSTANCE;
	m_pCheckDegree = dynamic_cast<CCheck_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Degree")));
	ENDINSTANCE;
	if (nullptr == m_pCheckDegree)
	{
		MSG_BOX("[CTurn] Failed Initialize : m_pCheckDegree is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTurn::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorators())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	_bool isLeft = { false };
	if (FAILED(m_pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
		return E_FAIL;

	_float fDegree = m_fTickPerDegree;

	if (0.f == fDegree)
	{
		if (FAILED(m_pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
			return E_FAIL;
	}

	_float fRadian = XMConvertToRadians(fDegree);
	if (true == isLeft)
		fRadian *= -1.f;

	m_pOwnerTransform->Turn(_float3(0.f, 1.f, 0.f), fRadian * 2.f, fTimeDelta);

	// 목표 각도 설정시 목표 각도가 달성될때까지 running한다.
	if (0.f < m_fTargetDegree)
	{
		m_pCheckDegree->Tick(fTimeDelta);

		if (FAILED(m_pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
			return E_FAIL;
		
		if (fDegree > m_fTargetDegree)
			return BEHAVIOR_RUNNING;
	}

	return BEHAVIOR_SUCCESS;
}

HRESULT CTurn::Assemble_Childs()
{
	m_pCheckDegree->Set_Option(m_pOwnerTransform);

	if (FAILED(Assemble_Behavior(TEXT("Tsk_Check_Degree"), m_pCheckDegree)))
	{
		MSG_BOX("[CTurn] Failed Assemble_Childs : \nFailed Assemble_Behavior Tsk_Check_Degree");
		return E_FAIL;
	}
	Safe_AddRef(m_pCheckDegree);

	return S_OK;
}

CTurn* CTurn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTurn* pInstance = New CTurn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTurn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTurn* CTurn::Clone(void* pArg)
{
	CTurn* pInstance = New CTurn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTurn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTurn::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pCheckDegree);
		Safe_Release(m_pOwnerTransform);
	}
}
