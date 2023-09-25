#include "LookAt.h"

#include "GameObject.h"
#include "BlackBoard.h"
#include "Transform.h"

CLookAt::CLookAt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CLookAt::CLookAt(const CLookAt& rhs)
	: CBehavior(rhs)
{
}

HRESULT CLookAt::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CLookAt::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorators())
		return BEHAVIOR_FAIL;

	const CGameObject** ppTarget = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("cppTarget", ppTarget)))
	{
		MSG_BOX("[CCheck_Distance] Failed Get_Type cppTarget");
		return E_FAIL;
	}
	if (nullptr == *ppTarget)
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	m_pOwnerTransform->LookAt_Lerp((*ppTarget)->Get_Transform()->Get_Position(), fTimeDelta, true);

	return BEHAVIOR_SUCCESS;
}

CLookAt* CLookAt::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLookAt* pInstance = new CLookAt(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLookAt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLookAt* CLookAt::Clone(void* pArg)
{
	CLookAt* pInstance = new CLookAt(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLookAt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLookAt::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pOwnerTransform);
	}
}
