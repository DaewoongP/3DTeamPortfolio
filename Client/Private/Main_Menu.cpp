#include "Main_Menu.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

#include "Menu_Gear.h"
#include "Menu_Invectory.h"
#include "Menu_Quest.h"
#include "Menu_Setting.h"

CMain_Menu::CMain_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMain_Menu::CMain_Menu(const CMain_Menu& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMain_Menu::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CMain_Menu Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain_Menu::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg))))
		return E_FAIL;


	Ready_Menus();

	return S_OK;
}

void CMain_Menu::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	switch (m_eCurMenu)
	{
	case GEAR:
		m_pGear->Tick(fTimeDelta);
		break;
	case INVENTORY:
		break;
	case QUEST:
		break;
	case SETTING:
		break;
	case MENU_END:
		break;
	default:
		break;
	}

	Set_SelectedText();
}

void CMain_Menu::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	switch (m_eCurMenu)
	{
	case GEAR:
		m_pGear->Late_Tick(fTimeDelta);
		break;
	case INVENTORY:
		break;
	case QUEST:
		break;
	case SETTING:
		break;
	case MENU_END:
		break;
	default:
		break;
	}
}

HRESULT CMain_Menu::Render()
{
	return S_OK;
}

HRESULT CMain_Menu::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMain_Menu::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Frame"))))
	{
		MSG_BOX("Com_Menu : Failed Clone Component (Com_UI_Effect_Back_Menu_Frame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Q"))))
	{
		MSG_BOX("Com_Menu : Failed Clone Component (Com_UI_Effect_Back_Menu_Keys)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_E"))))
	{
		MSG_BOX("Com_Menu : Failed Clone Component (Com_UI_Effect_Back_Menu_Keys)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	wstring text = TEXT("Gear");
	Add_TextComponent(text);
	text = TEXT("Inventory");
	Add_TextComponent(text);
	//text = TEXT("Talent");
	//Add_TextComponent(text);
	//text = TEXT("Collection");
	//Add_TextComponent(text);
	//text = TEXT("Action");
	//Add_TextComponent(text);
	//text = TEXT("Map");
	//Add_TextComponent(text);
	text = TEXT("Quest");
	Add_TextComponent(text);
	//text = TEXT("OwlMail");
	//Add_TextComponent(text);
	text = TEXT("Setting");
	Add_TextComponent(text);

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Back"))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Menu_Back)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMain_Menu::Add_TextComponent(wstring wszTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Effect_Back* pText = nullptr;

	wstring text = TEXT("Com_UI_Effect_Back_Text_");
	text += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		text.c_str(), reinterpret_cast<CComponent**>(&pText))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Text)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	m_pTexts.push_back(pText);
	
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMain_Menu::Read_File(const _tchar* pFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	
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


	CUI_Back* pFrame = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Frame")));
	pFrame->Load(Load_File(hFile));

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	CUI_Back* pQ = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Q")));
	pQ->Load(Load_File(hFile));
	pQ->Set_Parent(pFrame);

	CUI_Back* pE= dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_E")));
	pE->Load(Load_File(hFile));
	pE->Set_Parent(pFrame);

	m_pTexts[GEAR]->Load(Load_File(hFile));
	m_pTexts[GEAR]->Set_Parent(pFrame);
	m_pTexts[GEAR]->Set_Effecttype(CUI_Effect_Back::EFFECTTYPE::TEXT);

	m_pTexts[INVENTORY]->Load(Load_File(hFile));
	m_pTexts[INVENTORY]->Set_Parent(pFrame);
	m_pTexts[INVENTORY]->Set_Effecttype(CUI_Effect_Back::EFFECTTYPE::TEXT);

	//m_pTexts[TALENT]->Load(Load_File(hFile));
	//m_pTexts[TALENT]->Set_Parent(pFrame);

	//m_pTexts[COLLECTION]->Load(Load_File(hFile));
	//m_pTexts[COLLECTION]->Set_Parent(pFrame);

	//m_pTexts[ACTION]->Load(Load_File(hFile));
	//m_pTexts[ACTION]->Set_Parent(pFrame);

	//m_pTexts[MAP]->Load(Load_File(hFile));
	//m_pTexts[MAP]->Set_Parent(pFrame);

	m_pTexts[QUEST]->Load(Load_File(hFile));
	m_pTexts[QUEST]->Set_Parent(pFrame);
	m_pTexts[QUEST]->Set_Effecttype(CUI_Effect_Back::EFFECTTYPE::TEXT);

	//m_pTexts[OWLMAIL]->Load(Load_File(hFile));
	//m_pTexts[OWLMAIL]->Set_Parent(pFrame);

	m_pTexts[SETTING]->Load(Load_File(hFile));
	m_pTexts[SETTING]->Set_Parent(pFrame);
	m_pTexts[SETTING]->Set_Effecttype(CUI_Effect_Back::EFFECTTYPE::TEXT);

	CloseHandle(hFile);


	HANDLE hFile2 = CreateFile(TEXT("../../Resources/GameData/UIData/UI_Group_Menu_Back.uidata"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);


	CUI_Back* pBack = dynamic_cast<CUI_Back*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Menu_UI"), TEXT("Com_UI_Effect_Back_Menu_Back")));
	pBack->Load(Load_File(hFile2));
	CloseHandle(hFile2);


	Safe_Release(pGameInstance);
	return S_OK;
}

CUI::UIDESC CMain_Menu::Load_File(const HANDLE hFile)
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

void CMain_Menu::Set_Menu(_uint iIndex)
{
	if (m_iSelectedText < 0)
	{
		m_iSelectedText = iIndex;
		m_pTexts[m_iSelectedText]->Set_Clicked(true);
		return;
	}

	m_pTexts[m_iSelectedText]->Set_Clicked(false);
	m_iSelectedText = iIndex;
	m_pTexts[m_iSelectedText]->Set_Clicked(true);
}

void CMain_Menu::Set_SelectedText()
{
	_int iIndex = 0;
	for (auto& pText : m_pTexts)
	{
		if (pText->Get_Clicked())
		{
			if (m_iSelectedText == -1)
			{
				m_iSelectedText = iIndex;
				m_eCurMenu = (MENU)m_iSelectedText;
				m_pTexts[m_iSelectedText]->Set_Clicked(true);
				return;
			}

			m_pTexts[m_iSelectedText]->Set_Clicked(false);
			m_iSelectedText = iIndex;
			m_eCurMenu = (MENU)m_iSelectedText;
			m_pTexts[m_iSelectedText]->Set_Clicked(true);

#ifdef _DEBUG
			cout << m_iSelectedText << endl;
#endif // _DEBUG
		}
		iIndex++;
	}
}

HRESULT CMain_Menu::Ready_Menus()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	m_pGear = dynamic_cast<CMenu_Gear*>(pGameInstance->Clone_Component(LEVEL_MAINGAME,
		TEXT("Prototype_GameObject_Menu_Gear")));
	//Safe_AddRef(m_pGear);

	Safe_Release(pGameInstance);

	return S_OK;
}

CMain_Menu* CMain_Menu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMain_Menu* pInstance = new CMain_Menu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMain_Menu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMain_Menu::Clone(void* pArg)
{
	CMain_Menu* pInstance = new CMain_Menu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMain_Menu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain_Menu::Free()
{
	__super::Free();

	for (auto& Frame : m_pTexts)
	{
		Safe_Release(Frame);
	}
	m_pTexts.clear();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTexture);
	Safe_Release(m_pGear);	
}