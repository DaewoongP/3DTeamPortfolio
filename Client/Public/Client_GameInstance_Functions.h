#pragma once

#include "GameInstance.h"

namespace Client
{
	template<typename T>
	HRESULT Create_Behavior(_Inout_ T& pBehavior) 
	{
		string strTypename = typeid(pBehavior).name();
		size_t iStartPosition = strTypename.find("::") + 3;
		size_t iSplitLength = strTypename.find(" *") - iStartPosition;
		wstring wstrBehaviorTag = strToWStr(strTypename.substr(iStartPosition, iSplitLength));
		wstring wstrPrototypeTag = TEXT("Prototype_Component_");
		wstrPrototypeTag += wstrBehaviorTag;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (nullptr == pGameInstance)
			return E_FAIL;

		Safe_AddRef(pGameInstance);
		pBehavior = static_cast<T>(pGameInstance->Clone_Component(LEVEL_STATIC, wstrPrototypeTag.c_str()));
		Safe_Release(pGameInstance);

		if (nullptr == pBehavior)
			return E_FAIL;

		return S_OK;
	}
}