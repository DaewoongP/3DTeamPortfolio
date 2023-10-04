#include "UI_Potion_Tap.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"
#include "UI_Font.h"
#include "Item.h"

CUI_Potion_Tap::CUI_Potion_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Potion_Tap::CUI_Potion_Tap(const CUI_Potion_Tap& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Potion_Tap::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Potion_Tap Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Potion_Tap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_DefaultTexture();

	wstring FirstTag = TEXT("First");
	wstring SecondTag = TEXT("Second");
	wstring ThirdTag = TEXT("Third");
	wstring FourthTag = TEXT("Fourth");
	wstring FifthTag = TEXT("Fifth");

	wstring SixthTag = TEXT("Sixth");
	wstring SeventhTag = TEXT("Seventh");
	wstring EighthTag = TEXT("Eighth");

	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Potion_Tap_1_Edit.uidata"), FirstTag, POTIONTAP::DEFENSIVE_POWER_UP);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Potion_Tap_2_Edit.uidata"), SecondTag, POTIONTAP::ATTACK_POWER_UP);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Potion_Tap_3_Edit.uidata"), ThirdTag, POTIONTAP::SHOW_TIME);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Potion_Tap_4_Edit.uidata"), FourthTag, POTIONTAP::THUNDER_CLOUD);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Potion_Tap_5_Edit.uidata"), FifthTag, POTIONTAP::INVISIBILITY_PILL);
	
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Potion_Tap_6_Edit.uidata"), SixthTag, POTIONTAP::MANDRAKE);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Potion_Tap_7_Edit.uidata"), SeventhTag, POTIONTAP::BITE_CABBAGE);
	Create_Component(TEXT("../../Resources/GameData/UIData/UI_Group_Potion_Tap_8_Edit.uidata"), EighthTag, POTIONTAP::TENTACULAR);

	return S_OK;
}

void CUI_Potion_Tap::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//Swap_InventoryItem();
}

void CUI_Potion_Tap::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}
	
HRESULT CUI_Potion_Tap::Render()
{
	return S_OK;
}

HRESULT CUI_Potion_Tap::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		CUI_Font::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fonts/NexonGothic.spritefont")), true)))
	{
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CUI::UIDESC CUI_Potion_Tap::Load_File(const HANDLE hFile, _bool isDDS)
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

	if (isDDS)
	{
		fs::path fsPath = UIDesc.szTexturePath;

		std::wstring wszExtension = fsPath.extension();
		if (wszExtension == TEXT(".png"))
		{
			fsPath.replace_extension(L".dds");
			lstrcpy(UIDesc.szTexturePath, fsPath.c_str());
		}
	}

	return UIDesc;
}

HRESULT CUI_Potion_Tap::Ready_DefaultTexture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Default"), reinterpret_cast<CComponent**>(&m_pBack))))
	{
		MSG_BOX("Com_UI_Potion_Tap : Failed Clone Component (Com_UI_Back_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 360.f };
	UIDesc.fZ = { 0.8f };
	UIDesc.fSizeX = { 1280.f };
	UIDesc.fSizeY = { 720.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Potion_Tap_Default_Edit.dds");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);
	
	m_pBack->Load(UIDesc);


	_tchar szFilePath[MAX_PATH] = TEXT("");
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Cursor.uidata"));
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Cursor"),
		TEXT("Com_UI_PotionTap_Cursor"), reinterpret_cast<CComponent**>(&m_pCursor), szFilePath)))
	{
		MSG_BOX("Com_UI_Potion_Tap : Failed Clone Component (Com_UI_Back_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}




	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Potion_Tap::Create_Component(const _tchar* pFIlePath, wstring wszTag, POTIONTAP eType)
{
	if (FAILED(Add_Components(wszTag, eType)))
		return E_FAIL;

	if (FAILED(Read_File(pFIlePath, eType)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Potion_Tap::Add_Components(wstring wszTag, POTIONTAP eType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Back* pIcon = nullptr;
	CUI_Effect_Back* pFrame = nullptr;
	CUI_Back* pCountBack = nullptr;
	CUI_Effect_Back* pItemBack = nullptr;
	CUI_Font* pFont = nullptr;

	wstring icon = TEXT("Com_UI_Back_Icon_");
	icon += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		icon.c_str(), reinterpret_cast<CComponent**>(&pIcon))))
	{
		MSG_BOX("Com_UI_PotionTap : Failed Clone Component (Com_UI_Back_Icon_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pIcons.push_back(pIcon);

	wstring frame = TEXT("Com_UI_Effect_Back_Frame_");
	frame += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		frame.c_str(), reinterpret_cast<CComponent**>(&pFrame))))
	{
		MSG_BOX("Com_UI_PotionTap : Failed Clone Component (Com_UI_Effect_Back_Frame_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pFrames.push_back(pFrame);

	wstring countback = TEXT("Com_UI_Back_CountBack_");
	countback += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		countback.c_str(), reinterpret_cast<CComponent**>(&pCountBack))))
	{
		MSG_BOX("Com_UI_PotionTap : Failed Clone Component (Com_UI_Back_CountBack_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pCountBacks.push_back(pCountBack);


	wstring itemback = TEXT("Com_UI_Back_ItemBack_");
	itemback += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		itemback.c_str(), reinterpret_cast<CComponent**>(&pItemBack))))
	{
		MSG_BOX("Com_UI_PotionTap : Failed Clone Component (Com_UI_Back_ItemBack_)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pItemBacks.push_back(pItemBack);

	CUI_Font::FONTDESC Desc;
	Desc.m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };

	switch (eType)
	{
	case Client::DEFENSIVE_POWER_UP:
	{
		Desc.m_vPos = { 662.f, 144.f };
		wstring wstrNCount = to_wstring(0);
		lstrcpy(Desc.m_pText, wstrNCount.c_str());
	}
		break;
	case Client::ATTACK_POWER_UP:
	{
		Desc.m_vPos = { 794.f, 194.f };
		wstring wstrNCount = to_wstring(0);
		lstrcpy(Desc.m_pText, wstrNCount.c_str());
	}
		break;
	case Client::SHOW_TIME:
	{
		Desc.m_vPos = { 854.f, 333.f };
		wstring wstrNCount = to_wstring(0);
		lstrcpy(Desc.m_pText, wstrNCount.c_str());
	}
		break;
	case Client::THUNDER_CLOUD:
	{
		Desc.m_vPos = { 803.f, 463.f };
		wstring wstrNCount = to_wstring(0);
		lstrcpy(Desc.m_pText, wstrNCount.c_str());
	}
		break;
	case Client::INVISIBILITY_PILL:
	{
		Desc.m_vPos = { 663.f, 525.f };
		wstring wstrNCount = to_wstring(0);
		lstrcpy(Desc.m_pText, wstrNCount.c_str());
	}
		break;
	case Client::MANDRAKE:
	{
		Desc.m_vPos = { 532.f, 463.f };
		wstring wstrNCount = to_wstring(0);
		lstrcpy(Desc.m_pText, wstrNCount.c_str());
	}
		break;
	case Client::BITE_CABBAGE:
	{
		Desc.m_vPos = { 473.f, 333.f };
		wstring wstrNCount = to_wstring(0);
		lstrcpy(Desc.m_pText, wstrNCount.c_str());
	}
		break;
	case Client::TENTACULAR:
	{
		Desc.m_vPos = { 534.f, 199.f };
		wstring wstrNCount = to_wstring(0);
		lstrcpy(Desc.m_pText, wstrNCount.c_str());
	}
		break;
	case Client::POTIONTAP_END:
		break;
	default:
		break;
	}


	wstring number = TEXT("Com_UI_Font_Number_");
	number += wszTag;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		number.c_str(), reinterpret_cast<CComponent**>(&pFont), &Desc)))
	{
		MSG_BOX("Com_UI_PotionTap : Failed Clone Component (Com_UI_Font_Level)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}
	m_pFonts.push_back(pFont);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CUI_Potion_Tap::Read_File(const _tchar* pFilePath, POTIONTAP iIndex)
{
	if (nullptr == m_pIcons[iIndex] || nullptr == m_pFrames[iIndex] || nullptr == m_pCountBacks[iIndex] 
		|| nullptr == m_pItemBacks[iIndex] || iIndex >= POTIONTAP::POTIONTAP_END)
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

	m_pIcons[iIndex]->Load(Load_File(hFile));
	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pFrames[iIndex]->Load(Load_File(hFile, false));
	m_pFrames[iIndex]->Set_Parent(m_pIcons[iIndex]);

	m_pCountBacks[iIndex]->Load(Load_File(hFile));
	m_pCountBacks[iIndex]->Set_Parent(m_pIcons[iIndex]);

	m_pItemBacks[iIndex]->Load(Load_File(hFile));
	m_pItemBacks[iIndex]->Set_Parent(m_pIcons[iIndex]);
	
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Potion_Tap::Set_ItemCount(vector<vector<CItem*>>& pItems)
{
	_uint iCount = pItems.size();
	for (size_t i = 0; i < iCount; i++)
	{
		if (nullptr == m_pFonts[i])
			continue;

		m_pFonts[i]->Set_Text(to_wstring(pItems[i].size()));
	}

	return S_OK;
}

_uint CUI_Potion_Tap::Set_CurTapItem()
{
	_uint iIndex = 0;
	for (auto& pFrame : m_pFrames)
	{
		if (pFrame->Get_Collision())
			return iIndex;
		
		++iIndex;
	}

	return -1;
}

CUI_Potion_Tap* CUI_Potion_Tap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Potion_Tap* pInstance = new CUI_Potion_Tap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Potion_Tap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Potion_Tap::Clone(void* pArg)
{
	CUI_Potion_Tap* pInstance = new CUI_Potion_Tap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Potion_Tap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Potion_Tap::Free()
{
	__super::Free();


	for (auto& pIcon : m_pIcons)
	{
		Safe_Release(pIcon);
	}

	for (auto& pFrames : m_pFrames)
	{
		Safe_Release(pFrames);
	}

	for (auto& pCountBack : m_pCountBacks)
	{
		Safe_Release(pCountBack);
	}

	for (auto& pItemBack : m_pItemBacks)
	{
		Safe_Release(pItemBack);
	}
	
	for (auto& pFonts : m_pFonts)
	{
		Safe_Release(pFonts);
	}

	Safe_Release(m_pBack);
	Safe_Release(m_pCursor);
}