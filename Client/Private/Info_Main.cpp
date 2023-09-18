#include "..\Public\Info_Main.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

CInfo_Main::CInfo_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInfo_Main::CInfo_Main(const CInfo_Main& rhs)
	:CGameObject(rhs)
{
}

HRESULT CInfo_Main::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CInfo_Main Add ProtoType");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CInfo_Main::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	wstring GearTag = TEXT("Gear");
	wstring InventoryTag = TEXT("Inventory");
	wstring TalentTag = TEXT("Talent");
	wstring CollectionTag = TEXT("Collection");
	wstring ActionTag = TEXT("Action");
	wstring SettingTag = TEXT("Setting");
	wstring OwlMailTag = TEXT("OwlMail");
	wstring QuestTag = TEXT("Quest");
	wstring MapTag = TEXT("Map");

	Create_First(pArg); // GEAR
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Inventory5.uidata"), InventoryTag, INVENTORY);

	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Collection5.uidata"), CollectionTag, COLLECTION);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Action.uidata"), ActionTag, ACTION);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Setting5.uidata"), SettingTag, SETTING);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_OwlMail5.uidata"), InventoryTag, OWLMAIL);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Quest5.uidata"), QuestTag, QUEST);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Map5.uidata"), MapTag, MAP);

	return S_OK;
}

void CInfo_Main::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CInfo_Main::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CInfo_Main::Render()
{
	return S_OK;
}

HRESULT CInfo_Main::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CInfo_Main::Add_Components(wstring wszTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Effect_Back* pFrame = nullptr;
	CUI_Effect_Back* pMain = nullptr;
	CUI_Back* pBack = nullptr;
	CUI_Back* pLine = nullptr;

	wstring frame = TEXT("Com_UI_Effect_Back_Frame_");
	frame += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		frame.c_str(), reinterpret_cast<CComponent**>(&pFrame))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Frame)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pFrames.push_back(pFrame);


	wstring main = TEXT("Com_UI_Back_Main_");
	main += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		main.c_str(), reinterpret_cast<CComponent**>(&pMain))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Back_Frame)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pMains.push_back(pMain);


	wstring back = TEXT("Com_UI_Back_Back_");
	back += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		back.c_str(), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Back_Number)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pBacks.push_back(pBack);

	wstring line = TEXT("Com_UI_Back_Line_");
	line += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		line.c_str(), reinterpret_cast<CComponent**>(&pLine))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Back_Number)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pLines.push_back(pLine);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CInfo_Main::Read_File(const _tchar* pFilePath, INFOLIST iIndex)
{
	if (nullptr == m_pFrames[iIndex] || nullptr == m_pMains[iIndex] || nullptr == m_pBacks[iIndex] || nullptr == m_pLines[iIndex] ||
		iIndex >= INFOLIST_END)
	{
		MSG_BOX("Failed Load");
		return E_FAIL;
	}

	_ulong dwByte = 0;
	DWORD dwStrByte = 0;

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	_tchar szGroupName[MAX_PATH] = TEXT("");

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	m_pFrames[iIndex]->Load(Load_File(hFile));


	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pMains[iIndex]->Load(Load_File(hFile));
	m_pMains[iIndex]->Set_Parent(m_pFrames[iIndex]);

	m_pBacks[iIndex]->Load(Load_File(hFile));
	m_pBacks[iIndex]->Set_Parent(m_pFrames[iIndex]);

	m_pLines[iIndex]->Load(Load_File(hFile));
	m_pLines[iIndex]->Set_Parent(m_pFrames[iIndex]);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CInfo_Main::Load_File(const HANDLE hFile)
{
	CUI::UIDESC UIDesc;
	ZEROMEM(&UIDesc);

	_ulong dwByte = 0;
	DWORD dwStrByte = 0;
	_tchar szTextureName[MAX_PATH] = TEXT("");
	_tchar szAlphaPrototypeTag[MAX_PATH] = TEXT("");
	_bool isParent, isAlpha, isSave;
	_int eID;

	ReadFile(hFile, &UIDesc.vCombinedXY, sizeof(_float2), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fZ, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fSizeX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.fSizeY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, szTextureName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, UIDesc.szTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &isParent, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &isAlpha, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &UIDesc.vColor, sizeof(_float4), &dwByte, nullptr);
	ReadFile(hFile, szAlphaPrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, UIDesc.szAlphaTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &eID, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &isSave, sizeof(_bool), &dwByte, nullptr);

	return UIDesc;
}

HRESULT CInfo_Main::Create_First(void* pArg)
{
	//_tchar FirstTag[MAX_PATH] = TEXT("First");
	wstring wstrTag = TEXT("Gear");
	if (FAILED(Add_Components(wstrTag)))
		return E_FAIL;

	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg), GEAR)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInfo_Main::Create_Component(const _tchar* pFIlePath, wstring wszTag, INFOLIST eType)
{

	if (FAILED(Add_Components(wszTag)))
		return E_FAIL;

	if (FAILED(Read_File(pFIlePath, eType)))
		return E_FAIL;

	return S_OK;
}

CInfo_Main* CInfo_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInfo_Main* pInstance = new CInfo_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CInfo_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInfo_Main::Clone(void* pArg)
{
	CInfo_Main* pInstance = new CInfo_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CInfo_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInfo_Main::Free()
{
	__super::Free();

	for (auto& Frame : m_pFrames)
	{
		Safe_Release(Frame);
	}
	m_pFrames.clear();

	for (auto& main : m_pMains)
	{
		Safe_Release(main);
	}
	m_pMains.clear();

	for (auto& back : m_pBacks)
	{
		Safe_Release(back);
	}
	m_pBacks.clear();

	for (auto& line : m_pLines)
	{
		Safe_Release(line);
	}
	m_pLines.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTexture);
}
