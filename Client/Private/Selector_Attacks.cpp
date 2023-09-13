#include "Selector_Attacks.h"

#include "GameInstance.h"
#include "BlackBoard.h"
#include "Action.h"

CSelector_Attacks::CSelector_Attacks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CSelector_Attacks::CSelector_Attacks(const CSelector_Attacks& rhs)
	: CSelector(rhs)
{
}

HRESULT CSelector_Attacks::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CSelector_Attacks::Tick(const _float& fTimeDelta)
{
	return __super::Tick(fTimeDelta);
}

HRESULT CSelector_Attacks::Assemble_Childs(CBehavior* pBehavior)
{
	BEGININSTANCE;

	try
	{

	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[Selector_Degree] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

CSelector_Attacks* CSelector_Attacks::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSelector_Attacks* pInstance = new CSelector_Attacks(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSelector_Attacks* CSelector_Attacks::Clone(void* pArg)
{
	CSelector_Attacks* pInstance = new CSelector_Attacks(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector_Attacks::Free()
{
	__super::Free();
}
