#include "Menu_Gear.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

CMenu_Gear::CMenu_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMenu_Gear::CMenu_Gear(const CMenu_Gear& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMenu_Gear::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CMenu_Gear Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMenu_Gear::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Gear_Frame()))
		return E_FAIL;

	if (FAILED(Ready_Gear_Slot(TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Hand.uidata"), TEXT("Hand"), HAND)))
		return E_FAIL;

	if (FAILED(Ready_Gear_Slot(TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Face.uidata"), TEXT("Face"), FACE)))
		return E_FAIL;

	if (FAILED(Ready_Gear_Slot(TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Headwear.uidata"), TEXT("Head"), HEAD)))
		return E_FAIL;

	if (FAILED(Ready_Gear_Slot(TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Neck.uidata"), TEXT("Neck"), NECK)))
		return E_FAIL;

	if (FAILED(Ready_Gear_Slot(TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Back.uidata"), TEXT("Back"), BACK)))
		return E_FAIL;

	if (FAILED(Ready_Gear_Slot(TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Outfit.uidata"), TEXT("Outfit"), OUTFIT)))
		return E_FAIL;
	
	if (FAILED(Ready_Gear_Status()))
		return E_FAIL;

	return S_OK;
}

void CMenu_Gear::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMenu_Gear::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMenu_Gear::Render()
{
	return S_OK;
}

HRESULT CMenu_Gear::Add_Prototype()
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


HRESULT CMenu_Gear::Ready_Gear_Frame()
{
	Add_Components_Frame();
	Read_File_Frame(TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Frame.uidata"));

	return S_OK;
}

HRESULT CMenu_Gear::Add_Components_Frame()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Back* pBack = nullptr;
	wstring tag = TEXT("Com_UI_Back_Gear_Frame") + Generate_HashtagW(true);
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Gear_Frame1"), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pComponents.push_back(pBack);


	pBack = nullptr;
	tag = TEXT("Com_UI_Back_Gear_Frame") + Generate_HashtagW(true);
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Gear_Frame1"), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pComponents.push_back(pBack);


	pBack = nullptr;
	tag = TEXT("Com_UI_Back_Gear_Frame") + Generate_HashtagW(true);
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Gear_Frame1"), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pComponents.push_back(pBack);


	pBack = nullptr;
	tag = TEXT("Com_UI_Back_Gear_Frame") + Generate_HashtagW(true);
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Gear_Frame1"), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pComponents.push_back(pBack);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMenu_Gear::Read_File_Frame(const _tchar* pFilePath)
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

	pComponents[0]->Load(Load_File(hFile));

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	
	pComponents[1]->Load(Load_File(hFile));
	pComponents[1]->Set_Parent(pComponents[0]);

	pComponents[2]->Load(Load_File(hFile));
	pComponents[2]->Set_Parent(pComponents[0]);

	pComponents[3]->Load(Load_File(hFile));
	pComponents[3]->Set_Parent(pComponents[0]);

	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMenu_Gear::Ready_Gear_Slot(const _tchar* pFilePath, wstring wszTag, GEARSLOT eType)
{
	if (FAILED(Add_Components_Slot(wszTag)))
		return E_FAIL;
	
	if (FAILED(Read_File_Slot(pFilePath, wszTag, eType)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMenu_Gear::Add_Components_Slot(wstring wszTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Effect_Back* pSlot = nullptr;

	wstring Frame = TEXT("Com_UI_Effect_Back_Gear_SlotFrame_");
	Frame += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		Frame.c_str(), reinterpret_cast<CComponent**>(&pSlot))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pSlotFrames.push_back(pSlot);


	CUI_Effect_Back* pIcon = nullptr;
	wstring Icon = TEXT("Com_UI_Back_Gear_SlotIcon_");
	Icon += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		Icon.c_str(), reinterpret_cast<CComponent**>(&pIcon))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pSlotIcons.push_back(pIcon);
	
	CUI_Back* pBack = nullptr;
	wstring Back = TEXT("Com_UI_Back_Gear_SlotBack_");
	Back += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		Back.c_str(), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pComponents.push_back(pBack);

	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CMenu_Gear::Read_File_Slot(const _tchar* pFilePath, wstring wszTag, GEARSLOT eType)
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

	pSlotFrames[eType]->Load(Load_File(hFile));
	pSlotFrames[eType]->Set_Effecttype(CUI_Effect_Back::ALPHA);

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	wstring Icon = TEXT("Com_UI_Back_Gear_SlotIcon_");
	Icon += wszTag;
	CUI_Effect_Back* pIcon = dynamic_cast<CUI_Effect_Back*>(Find_Component(Icon.c_str()));
	pIcon->Load(Load_File(hFile));
	pIcon->Set_Parent(pSlotFrames[eType]);

	wstring Back = TEXT("Com_UI_Back_Gear_SlotBack_");
	Back += wszTag;
	CUI_Back* pBack = dynamic_cast<CUI_Back*>(Find_Component(Back.c_str()));
	pBack->Load(Load_File(hFile));
	pBack->Set_Parent(pSlotFrames[eType]);

	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMenu_Gear::Ready_Gear_Status()
{
	if (FAILED(Add_Components_Status()))
		return E_FAIL;

	if (FAILED(Read_File_Statust(TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Status.uidata"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMenu_Gear::Add_Components_Status()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Back* pUI = nullptr;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Gear_Status_Text"), reinterpret_cast<CComponent**>(&pUI))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pComponents.push_back(pUI);

	CUI_Back* pHP = nullptr;
	CUI_Back* pDEF = nullptr;
	CUI_Back* pSTAT = nullptr;

	wstring HP = TEXT("Com_UI_Effect_Back_Gear_Status_HP");
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		HP.c_str(), reinterpret_cast<CComponent**>(&pHP))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pStatuses.push_back(pHP);

	wstring DEF = TEXT("Com_UI_Effect_Back_Gear_SlotFrame_");
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		DEF.c_str(), reinterpret_cast<CComponent**>(&pDEF))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pStatuses.push_back(pDEF);

	wstring ATT = TEXT("Com_UI_Effect_Back_Gear_SlotFrame_");
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		ATT.c_str(), reinterpret_cast<CComponent**>(&pSTAT))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	pStatuses.push_back(pSTAT);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMenu_Gear::Read_File_Statust(const _tchar* pFilePath)
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
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	_tchar szGroupName[MAX_PATH] = TEXT("");

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	CUI_Back* pStatText = dynamic_cast<CUI_Back*>(Find_Component(TEXT("Com_UI_Back_Gear_Status_Text")));
	pStatText->Load(Load_File(hFile));

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	pStatuses[0]->Load(Load_File(hFile));
	pStatuses[0]->Set_Parent(pStatText);

	pStatuses[1]->Load(Load_File(hFile));
	pStatuses[1]->Set_Parent(pStatText);

	pStatuses[2]->Load(Load_File(hFile));
	pStatuses[2]->Set_Parent(pStatText);

	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}

CUI::UIDESC CMenu_Gear::Load_File(const HANDLE hFile)
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

CMenu_Gear* CMenu_Gear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMenu_Gear* pInstance = new CMenu_Gear(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMenu_Gear");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMenu_Gear::Clone(void* pArg)
{
	CMenu_Gear* pInstance = new CMenu_Gear(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMenu_Gear");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu_Gear::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	for(auto& pSlotFrame : pSlotFrames)
	{
		Safe_Release(pSlotFrame);
	}

	for (auto& pIcon : pSlotIcons)
	{
		Safe_Release(pIcon);
	}

	for(auto& pStatus : pStatuses)
	{
		Safe_Release(pStatus);
	}

	for(auto& pCom : pComponents)
	{
		Safe_Release(pCom);
	}
}
