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
	return S_OK;
}

HRESULT CSelector_Degree::Tick(const _float& fTimeDelta)
{
	return __super::Tick(fTimeDelta);
}

HRESULT CSelector_Degree::Assemble_Childs(DEGREES eType, CBehavior* pBehavior)
{
	BEGININSTANCE;

	try
	{
		wstring wstrBehaviorTag = { TEXT("") };
		switch (eType)
		{
		case Client::CSelector_Degree::FRONT:
			pBehavior->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (fDegree >= 22.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Front");
			break;

		case Client::CSelector_Degree::BACK:
			pBehavior->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (fDegree < 157.5f)
						return false;
					return true;
				});
			wstrBehaviorTag = TEXT("Behavior_Back");
			break;

		case Client::CSelector_Degree::LEFT_45:
			pBehavior->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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
			pBehavior->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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
			pBehavior->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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

		case Client::CSelector_Degree::RIGHT_45:
			pBehavior->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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
			pBehavior->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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
			pBehavior->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
				{
					_bool isLeft = { false };
					if (FAILED(pBlackBoard->Get_Type("isTargetToLeft", isLeft)))
						return false;
					_float fDegree = { 0.f };
					if (FAILED(pBlackBoard->Get_Type("fTargetToDegree", fDegree)))
						return false;

					if (true == isLeft)
						cout << "Left" << endl;
					else
						cout << "Right" << endl;
					cout << fDegree << endl;

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

		if (FAILED(Assemble_Behavior(wstrBehaviorTag, pBehavior)))
			throw wstrBehaviorTag;
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

CSelector_Degree* CSelector_Degree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSelector_Degree* pInstance = new CSelector_Degree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSelector_Degree* CSelector_Degree::Clone(void* pArg)
{
	CSelector_Degree* pInstance = new CSelector_Degree(*this);

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
