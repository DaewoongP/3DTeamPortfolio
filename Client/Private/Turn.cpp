#include "Turn.h"

#include "GameObject.h"
#include "BlackBoard.h"
#include "Transform.h"

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
	return S_OK;
}

HRESULT CTurn::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;
	
	_bool isLeft = { false };
	if (FAILED(m_pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
		return E_FAIL;

	_float fRadian = XMConvertToRadians(45.f);
	if (false == isLeft)
		fRadian *= -1.f;

	m_pOwnerTransform->Turn(_float3(0.f, 1.f, 0.f), fRadian, fTimeDelta);

	return BEHAVIOR_SUCCESS;
}

CTurn* CTurn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTurn* pInstance = new CTurn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTurn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTurn* CTurn::Clone(void* pArg)
{
	CTurn* pInstance = new CTurn(*this);

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
}
