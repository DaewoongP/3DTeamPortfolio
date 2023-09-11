#include "Pattern_Move.h"

#include "GameInstance.h"

#include "Action.h"
#include "TargetDegree.h"
#include "Selector_Degree.h"

CPattern_Move::CPattern_Move(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSequence(pDevice, pContext)
{
}

CPattern_Move::CPattern_Move(const CPattern_Move& rhs)
	: CSequence(rhs)
{
}

HRESULT CPattern_Move::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CPattern_Move::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	//m_pOwnerTransform->LookAt();

	return BEHAVIOR_RUNNING;
}

HRESULT CPattern_Move::Assemble_Childs(CTransform* pTransform)
{
	BEGININSTANCE;

	try
	{
		
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[Pattern_Move] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	return S_OK;
}

CPattern_Move* CPattern_Move::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPattern_Move* pInstance = new CPattern_Move(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPattern_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPattern_Move* CPattern_Move::Clone(void* pArg)
{
	CPattern_Move* pInstance = new CPattern_Move(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPattern_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPattern_Move::Free()
{
	__super::Free();
}
