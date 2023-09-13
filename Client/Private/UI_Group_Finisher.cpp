#include "UI_Group_Finisher.h"
#include "GameInstance.h"


#include "UI_Back.h"
#include "UI_Finisher.h"

CUI_Group_Finisher::CUI_Group_Finisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Group(pDevice, pContext)
{
}

CUI_Group_Finisher::CUI_Group_Finisher(const CUI_Group_Finisher& rhs)
	: CUI_Group(rhs)
	, m_ProtoTypeTags(rhs.m_ProtoTypeTags)
	, m_pBacks(rhs.m_pBacks)
	, m_pFinishers(rhs.m_pFinishers)
{
}

HRESULT CUI_Group_Finisher::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_ProtoType()))
	{
		MSG_BOX("Failed CUI_Group_Finisher Add ProtoType");
		return E_FAIL;
	}

	m_pFinishers.clear();

	return S_OK;
}

HRESULT CUI_Group_Finisher::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	BEGININSTANCE
#pragma region Front Load
	_tchar wszGroupName[MAX_PATH] = TEXT("");
	DWORD dwStrByte;
	DWORD dwByte = 0;
	ReadFile(pArg, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(pArg, wszGroupName, dwStrByte, &dwByte, nullptr);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"),
		TEXT("GameObject_UI_Finisher_Frame_Front"), pArg);

	CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Finisher_Frame_Front")));
	m_pBacks.push_back(dynamic_cast<CUI_Back*>(pUI));
	Safe_AddRef(pUI);
	pUI = nullptr;

	_uint iSize = 0;
	ReadFile(pArg, &iSize, sizeof(iSize), &dwByte, nullptr);
	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[1], TEXT("Layer_UI"), TEXT("GameObject_UI_Finisher_Gauge_Front"), pArg);

	pUI = dynamic_cast<CUI*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Finisher_Gauge_Front")));
	pUI->Set_Parent(m_pBacks[0]);
	Safe_AddRef(m_pBacks[0]);

	m_pFinishers.push_back(dynamic_cast<CUI_Finisher*>(pUI));
	Safe_AddRef(pUI);
	pUI = nullptr;

	CloseHandle(pArg);
#pragma endregion

#pragma region Back Load
	_tchar pFilePath[MAX_PATH] = TEXT("../../Resources/GameData/UIData/UI_Group_Finisher_Back.uidata");
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	lstrcpy(wszGroupName, TEXT(""));
	dwStrByte = 0;
	dwByte = 0;
	ReadFile(pArg, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(pArg, wszGroupName, dwStrByte, &dwByte, nullptr);

	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[0], TEXT("Layer_UI"),
		TEXT("GameObject_UI_Finisher_Frame_Back"), pArg);

	pUI = dynamic_cast<CUI*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Finisher_Frame_Back")));
	m_pBacks.push_back(dynamic_cast<CUI_Back*>(pUI));
	Safe_AddRef(pUI);
	pUI = nullptr;

	iSize = 0;
	ReadFile(pArg, &iSize, sizeof(iSize), &dwByte, nullptr);
	pGameInstance->Add_Component(LEVEL_MAINGAME, m_ProtoTypeTags[1], TEXT("Layer_UI"), TEXT("GameObject_UI_Finisher_Gauge_Back"), pArg);

	pUI = dynamic_cast<CUI*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_UI"), TEXT("GameObject_UI_Finisher_Gauge_Back")));
	pUI->Set_Parent(m_pBacks[1]);
	Safe_AddRef(m_pBacks[1]);

	m_pFinishers.push_back(dynamic_cast<CUI_Finisher*>(pUI));
	Safe_AddRef(pUI);

#pragma endregion

	ENDINSTANCE;

	return S_OK;
}

void CUI_Group_Finisher::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_Finisher::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_bool CUI_Group_Finisher::Set_Gauge(_float fMin, _float fMax, _float fCurrent, CUI_Progress::GAUGE eType)
{
	if (nullptr == m_pFinishers[BACK] || nullptr == m_pFinishers[FRONT]
		|| m_pBacks.size() < 2 || m_pFinishers.size() < 2)
		return false;

	_float fPercent = fCurrent / fMax;
	if (fPercent <= 0.5f)
	{
		m_pFinishers[FRONT]->Set_Gauge(0.f, eType);
		m_pFinishers[BACK]->Set_Gauge(fCurrent * 2.f, eType);
	}
	else if (fPercent > 0.5f)
	{
		m_pFinishers[FRONT]->Set_Gauge((fCurrent - 50.f) * 2.f, eType);
		m_pFinishers[BACK]->Set_Gauge(fMax, eType);
	}

	return true;
}

HRESULT CUI_Group_Finisher::Add_ProtoType()
{
	BEGININSTANCE

		_tchar pName[MAX_PATH] = TEXT("");
	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Back"));


	CComponent* pComponent = pGameInstance->Find_Prototype(LEVEL_MAINGAME, pName);

	if (nullptr == pComponent)
	{
		pGameInstance->Add_Prototype(LEVEL_MAINGAME, pName, CUI_Back::Create(m_pDevice, m_pContext));
	}

	m_ProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));



	lstrcpy(pName, TEXT("Prototype_GameObject_UI_Finisher"));

	pComponent = pGameInstance->Find_Prototype(LEVEL_MAINGAME, pName);

	if (nullptr == pComponent)
	{
		pGameInstance->Add_Prototype(LEVEL_MAINGAME, pName, CUI_Finisher::Create(m_pDevice, m_pContext));
	}
	

	m_ProtoTypeTags.push_back(pGameInstance->Make_WChar(pName));

	ENDINSTANCE

	return S_OK;
}

CUI_Group_Finisher* CUI_Group_Finisher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Finisher* pInstance = new CUI_Group_Finisher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Finisher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Finisher::Clone(void* pArg)
{
	CUI_Group_Finisher* pInstance = new CUI_Group_Finisher(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Finisher");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_Finisher::Free()
{
	__super::Free();


	for (auto& iter : m_pBacks)
	{
		Safe_Release(iter);
	}
	m_pBacks.clear();

	for (auto& iter : m_pFinishers)
	{
		Safe_Release(iter);
	}
	m_pFinishers.clear();


}

