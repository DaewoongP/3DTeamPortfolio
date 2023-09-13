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
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	_bool isLeft = { false };
	if (FAILED(m_pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
		return E_FAIL;

	_float fDegree = { 0.f };
	if (FAILED(m_pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
		return E_FAIL;

	_float fRadian = XMConvertToRadians(fDegree);
	if (true == isLeft)
		fRadian *= -1.f;

	// 미구현 사용금지 
	//m_pOwnerTransform->LookAt(_float3(0.f, 1.f, 0.f), fRadian * 2.f, fTimeDelta);

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
