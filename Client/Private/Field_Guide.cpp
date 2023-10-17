#include "Field_Guide.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"
#include "Main_Menu.h"
#include "Inventory.h"

CField_Guide::CField_Guide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CField_Guide::CField_Guide(const CField_Guide& rhs)
	:CGameObject(rhs)
{
}

HRESULT CField_Guide::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CField_Guide Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CField_Guide::Initialize(void* pArg)
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
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Inventory_Alpha.uidata"), InventoryTag, INVENTORY);
	//Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Talent.uidata"), TalentTag, TALENT);
	//Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Collection.uidata"), CollectionTag, COLLECTION);
	//Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Action.uidata"), ActionTag, ACTION);
	//Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_OwlMail.uidata"), InventoryTag, OWLMAIL);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Quest_Alpha.uidata"), QuestTag, QUEST);
	//Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Map.uidata"), MapTag, MAP);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Setting_Alpha.uidata"), SettingTag, SETTING);

	Create_BackGround();

	Add_Cursor();

	return S_OK;
}

void CField_Guide::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pMenu == nullptr)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		m_pMenu = dynamic_cast<CMain_Menu*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Menu_UI"), TEXT("GameObject_UI_Main_Menu")));
		Safe_AddRef(m_pMenu);

		Safe_Release(pGameInstance);
		if (nullptr == m_pPlayerInventory)
		{
			CGameInstance* pGameInstacne = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstacne);

			m_pPlayerInventory = static_cast<CInventory*>(pGameInstacne->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Inventory"), TEXT("GameObject_Inventory")));
			Safe_AddRef(m_pPlayerInventory);

			Safe_Release(pGameInstacne);
		}
	}
	else
	{
		Set_SelectedText();
	}	
} 

void CField_Guide::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CField_Guide::Render()
{
	return S_OK;
}

HRESULT CField_Guide::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CField_Guide::Add_Components(wstring wszTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Effect_Back* pFrame = nullptr;
	CUI_Effect_Back* pMain = nullptr;
	CUI_Back* pBack = nullptr;
	CUI_Back* pLine = nullptr;

	wstring frame = TEXT("Com_UI_Effect_Back_Frame_");
	frame += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		frame.c_str(), reinterpret_cast<CComponent**>(&pFrame))))
	{
		MSG_BOX("Com_FieldGuide : Failed Clone Component (Com_UI_Effect_Back_Frame)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pFrames.push_back(pFrame);


	wstring main = TEXT("Com_UI_Back_Main_");
	main += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		main.c_str(), reinterpret_cast<CComponent**>(&pMain))))
	{
		MSG_BOX("Com_FieldGuide : Failed Clone Component (Com_UI_Back_Frame)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pMains.push_back(pMain);


	wstring back = TEXT("Com_UI_Back_Back_");
	back += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		back.c_str(), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_FieldGuide : Failed Clone Component (Com_UI_Back_Number)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pBacks.push_back(pBack);

	wstring line = TEXT("Com_UI_Back_Line_");
	line += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		line.c_str(), reinterpret_cast<CComponent**>(&pLine))))
	{
		MSG_BOX("Com_FieldGuide : Failed Clone Component (Com_UI_Back_Number)");
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pLines.push_back(pLine);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CField_Guide::Add_Cursor()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_tchar szFilePath[MAX_PATH] = TEXT("");
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Cursor.uidata"));
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Cursor"),
		TEXT("Com_UI_Cursor"), reinterpret_cast<CComponent**>(&m_pCursor), szFilePath)))
	{
		MSG_BOX("Com_Field_Guide : Failed Clone Component (Com_UI_Group_Cursor)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CField_Guide::Read_File(const _tchar* pFilePath, MENU iIndex)
{
	if (nullptr == m_pFrames[iIndex] || nullptr == m_pMains[iIndex] || nullptr == m_pBacks[iIndex] || nullptr == m_pLines[iIndex] ||
		iIndex >= MENU_END)
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
	m_pMains[iIndex]->Set_Effecttype(CUI_Effect_Back::ALPHA);
	

	m_pBacks[iIndex]->Load(Load_File(hFile));
	m_pBacks[iIndex]->Set_Parent(m_pFrames[iIndex]);


	m_pLines[iIndex]->Load(Load_File(hFile));
	m_pLines[iIndex]->Set_Parent(m_pFrames[iIndex]);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CField_Guide::Load_File(const HANDLE hFile)
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

HRESULT CField_Guide::Create_First(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_FieldGuide_UI"), TEXT("Com_UI_Effect_Back_FieldGuide_Back"))))
	{
		MSG_BOX("Com_FieldGuide : Failed Clone Component (Com_UI_Effect_Back_FieldGuide_Back)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"), TEXT("Layer_FieldGuide_UI"), TEXT("Com_UI_Effect_Back_FieldGuide_House"))))
	{
		MSG_BOX("Com_FieldGuide : Failed Clone Component (Com_UI_Effect_Back_FieldGuide_House)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"), TEXT("Layer_FieldGuide_UI"), TEXT("Com_UI_Effect_Back_FieldGuide_Rayburst"))))
	{
		MSG_BOX("Com_FieldGuide : Failed Clone Component (Com_UI_Effect_Back_FieldGuide_Rayburst");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	//_tchar FirstTag[MAX_PATH] = TEXT("First");
	wstring wstrTag = TEXT("Gear");
	if (FAILED(Add_Components(wstrTag)))
		return E_FAIL;

	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg), GEAR)))
		return E_FAIL;

	return S_OK;
}

HRESULT CField_Guide::Create_Component(const _tchar* pFIlePath, wstring wszTag, MENU eType)
{
	if (FAILED(Add_Components(wszTag)))
		return E_FAIL;

	if (FAILED(Read_File(pFIlePath, eType)))
		return E_FAIL;

	return S_OK;
}

HRESULT CField_Guide::Create_BackGround()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_ulong dwByte = 0;
	DWORD dwStrByte = 0;

	HANDLE hFile = CreateFile(TEXT("../../Resources/GameData/UIData/UI_Group_FieldGuide_Back.uidata"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}
	
	_tchar szGroupName[MAX_PATH] = TEXT("");

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	CUI_Back* pBack = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_FieldGuide_UI"), TEXT("Com_UI_Effect_Back_FieldGuide_Back")));
	pBack->Load(Load_File(hFile));

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	CUI_Effect_Back* pHouse = dynamic_cast<CUI_Effect_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_FieldGuide_UI"), TEXT("Com_UI_Effect_Back_FieldGuide_House")));
	pHouse->Load(Load_File(hFile));
	pHouse->Set_Parent(pBack);
	pHouse->Set_Effecttype(CUI_Effect_Back::ALPHA);

	CUI_Effect_Back* pRayburst = dynamic_cast<CUI_Effect_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_FieldGuide_UI"), TEXT("Com_UI_Effect_Back_FieldGuide_Rayburst")));
	pRayburst->Load(Load_File(hFile));
	pRayburst->Set_Parent(pBack);
	pRayburst->Set_Effecttype(CUI_Effect_Back::ALPHA);

	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}


void CField_Guide::Set_SelectedText()
{
	_int iIndex = 0;
	for (auto& pFrame : m_pFrames)
	{
		if (pFrame->Get_Clicked())
		{
			FieldGuide_To_Menu(iIndex);
			if (iIndex == 1)
			{
				m_pPlayerInventory->Set_Open(true);
				m_pPlayerInventory->Set_CurItemtype(ITEMTYPE::RESOURCE);
			}
		}
		iIndex++;
	}
}

void CField_Guide::FieldGuide_To_Menu(_uint iIndex)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Set_CurrentScene(TEXT("Scene_Menu"), false);
	m_pMenu->Set_Menu(iIndex);

	if (iIndex == 1)
	{
		m_pPlayerInventory->Set_Open(true);
		m_pPlayerInventory->Set_CurItemtype(ITEMTYPE::RESOURCE);
	}
	
	Safe_Release(pGameInstance);
}

CField_Guide* CField_Guide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CField_Guide* pInstance = new CField_Guide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CField_Guide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CField_Guide::Clone(void* pArg)
{
	CField_Guide* pInstance = new CField_Guide(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CField_Guide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CField_Guide::Free()
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
	Safe_Release(m_pMenu);
	Safe_Release(m_pCursor);
	if (m_isCloned)
	{
		Safe_Release(m_pPlayerInventory);
	}

}
