#include "Check_Degree.h"

#include "GameObject.h"
#include "BlackBoard.h"
#include "Transform.h"

CCheck_Degree::CCheck_Degree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CCheck_Degree::CCheck_Degree(const CCheck_Degree& rhs)
	: CBehavior(rhs)
{
}

HRESULT CCheck_Degree::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCheck_Degree::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorations())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

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

	_float3 vTargetPosition = (*ppTarget)->Get_Transform()->Get_Position();
	_float3 vPosition = m_pOwnerTransform->Get_Position();

	_float3 vLook = m_pOwnerTransform->Get_Look();
	_float3 vDirection = vTargetPosition - vPosition;
	vLook.y = 0.f;
	vDirection.y = 0.f;
	vLook.Normalize();
	vDirection.Normalize();

	_float fCrossY = vLook.Cross(vDirection).y;
	_bool isLeft = { false };
	isLeft = (0.f > fCrossY);

	_float fDegree = XMConvertToDegrees(acosf(vLook.Dot(vDirection)));

	if (0 != isnan(fDegree))
	{
		fDegree = 0.f;
	}

	if (FAILED(m_pBlackBoard->Set_Type("isTargetToLeft", isLeft)))
	{
		MSG_BOX("[CCheck_Degree] Failed Set_Type fTargetToLR");
		return E_FAIL;
	}

	if (FAILED(m_pBlackBoard->Set_Type("fTargetToDegree", fDegree)))
	{
		MSG_BOX("[CCheck_Degree] Failed Set_Type fTargetToLR");
		return E_FAIL;
	}

	/*if (true == isLeft)
		cout << "Left" << endl;
	else
		cout << "RIght" << endl;

	cout << fDegree << endl;*/

	return BEHAVIOR_SUCCESS;
}

CCheck_Degree* CCheck_Degree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCheck_Degree* pInstance = new CCheck_Degree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCheck_Degree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCheck_Degree* CCheck_Degree::Clone(void* pArg)
{
	CCheck_Degree* pInstance = new CCheck_Degree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCheck_Degree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCheck_Degree::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pOwnerTransform);
	}
}
