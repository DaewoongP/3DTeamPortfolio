#include "Death.h"

#include "GameObject.h"
#include "BlackBoard.h"
#include "Transform.h"

CDeath::CDeath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CDeath::CDeath(const CDeath& rhs)
	: CBehavior(rhs)
{
}

HRESULT CDeath::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CDeath::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorators())
		return BEHAVIOR_FAIL;

	// 죽을때 까지 돌아라...
	m_DeathFunction(fTimeDelta);

	return BEHAVIOR_RUNNING;
}

CDeath* CDeath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeath* pInstance = new CDeath(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDeath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDeath* CDeath::Clone(void* pArg)
{
	CDeath* pInstance = new CDeath(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDeath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeath::Free()
{
	__super::Free();
}
