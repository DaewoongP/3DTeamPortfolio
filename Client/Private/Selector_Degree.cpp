#include "Selector_Degree.h"

#include "GameInstance.h"
#include "BlackBoard.h"
#include "Action.h"

CSelector_Degree::CSelector_Degree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSelector(pDevice, pContext)
{
}

CSelector_Degree::CSelector_Degree(const CSelector_Degree& rhs)
	: CSelector(rhs)
{
}

HRESULT CSelector_Degree::Initialize(void* pArg)
{
	/* ÄðÅ¸ÀÓ */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			BEGININSTANCE;
			_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			ENDINSTANCE;

			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CSelector_Degree::Tick(const _float& fTimeDelta)
{
	HRESULT hr = __super::Tick(fTimeDelta);

	BEGININSTANCE;
	_float fdata = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
	
	ENDINSTANCE;
	if (BEHAVIOR_SUCCESS == hr)
	{
		BEGININSTANCE;
		m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
		ENDINSTANCE;
	}

	return hr;
}

HRESULT CSelector_Degree::Assemble_Behavior(DEGREES eType, CBehavior* pBehavior)
{
	BEGININSTANCE;

	try
	{
		wstring wstrBehaviorTag = { TEXT("") };
		switch (eType)
		{
		case Client::CSelector_Degree::LEFT_FRONT:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (false == isLeft || fDegree >= 22.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Left_Front");
			break;

		case Client::CSelector_Degree::LEFT_BACK:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (false == isLeft || fDegree < 157.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Left_Back");
			break;

		case Client::CSelector_Degree::LEFT_45:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (false == isLeft ||
						fDegree >= 67.5f || fDegree < 22.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Left45");
			break;

		case Client::CSelector_Degree::LEFT_90:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (false == isLeft ||
						fDegree >= 112.5f || fDegree < 67.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Left90");
			break;

		case Client::CSelector_Degree::LEFT_135:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (false == isLeft ||
						fDegree >= 157.5f || fDegree < 112.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Left135");
			break;

		case Client::CSelector_Degree::RIGHT_FRONT:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (true == isLeft || fDegree >= 22.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Right_Front");
			break;

		case Client::CSelector_Degree::RIGHT_BACK:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (true == isLeft || fDegree < 157.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Right_Back");
			break;

		case Client::CSelector_Degree::RIGHT_45:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (true == isLeft ||
						fDegree >= 67.5f || fDegree < 22.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Right45");
			break;

		case Client::CSelector_Degree::RIGHT_90:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (true == isLeft ||
						fDegree >= 112.5f || fDegree < 67.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Right90");
			break;

		case Client::CSelector_Degree::RIGHT_135:
			pBehavior->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (true == isLeft ||
						fDegree >= 157.5f || fDegree < 112.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Right135");
			break;

		default:
			throw TEXT("Invalid Degree Type");
		}

		if (FAILED(__super::Assemble_Behavior(wstrBehaviorTag, pBehavior)))
			throw wstrBehaviorTag;
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[Selector_Degree] Failed Assemble_Behavior : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

CSelector_Degree* CSelector_Degree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSelector_Degree* pInstance = New CSelector_Degree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSelector_Degree* CSelector_Degree::Clone(void* pArg)
{
	CSelector_Degree* pInstance = New CSelector_Degree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector_Degree::Free()
{
	__super::Free();
}
