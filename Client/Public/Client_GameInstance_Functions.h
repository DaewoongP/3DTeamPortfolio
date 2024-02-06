#pragma once

#include "GameInstance.h"

namespace Client
{
	template<typename T>
	HRESULT Create_Behavior(_Inout_ T& pBehavior) 
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (nullptr == pGameInstance)
			return E_FAIL;

		string strTypename = typeid(pBehavior).name();
		size_t iStartPosition = strTypename.find("::") + 3;
		size_t iSplitLength = strTypename.find(" *") - iStartPosition;
		wstring wstrBehaviorTag = strToWStr(strTypename.substr(iStartPosition, iSplitLength));
		wstring wstrPrototypeTag = TEXT("Prototype_Component_");
		wstrPrototypeTag += wstrBehaviorTag;

		Safe_AddRef(pGameInstance);
		pBehavior = static_cast<T>(pGameInstance->Clone_Component(LEVEL_STATIC, wstrPrototypeTag.c_str()));
		Safe_Release(pGameInstance);

		if (nullptr == pBehavior)
			return E_FAIL;

		return S_OK;
	}

	wstring Make_Prototype_Tag(const CComponent* pComponent, OBJ_TYPE eType, const _tchar* strDetail = nullptr)
	{
		string strTypename = typeid(*pComponent).name();
		size_t iStartPosition = strTypename.find("::") + 3;
		size_t iSplitLength = strTypename.find(" const") - iStartPosition;
		wstring wstrObjectTag = strToWStr(strTypename.substr(iStartPosition, iSplitLength));
		if (nullptr != strDetail)
			wstrObjectTag = wstrObjectTag + TEXT("_") + strDetail;
		wstring wstrPrototypeTag = COMPONENT == eType ? TEXT("Prototype_Component_") : TEXT("Prototype_GameObject_");
		return wstrPrototypeTag += wstrObjectTag;
	}

	void Create_Prototype(LEVELID eLevelID, CComponent* pComponent, OBJ_TYPE eType, const _tchar* strDetail = nullptr)
	{
		wstring wstrPtototypeTag = Make_Prototype_Tag(pComponent, eType, strDetail);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (nullptr == pGameInstance)
			throw wstrPtototypeTag + TEXT(" | Failed CGameInstance::GetInstance()");

		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Add_Prototype(eLevelID, wstrPtototypeTag.c_str(),
			pComponent)))
		{
			Safe_Release(pGameInstance);
			throw wstrPtototypeTag;
		}

		Safe_Release(pGameInstance);
	}
}