#include "UI_Store.h"
#include "GameInstance.h"
#include "UI_Back.h"
#include "UI_Effect_Back.h"
#include "UI_Font.h"
#include "Item.h"
#include "UI_Dynamic_Back.h"
#include "UI_Group_Cursor.h"
#include "UI_Slot.h"
#include "Item.h"
#include "Player.h"
#include "Inventory.h"

CUI_Store::CUI_Store(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}
CUI_Store::CUI_Store(const CUI_Store& rhs)
	: CGameObject(rhs)
{
}
HRESULT CUI_Store::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
		, TEXT("../../Resources/UI/Game/Store/Store_Default.png"));
	pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
		, TEXT("../../Resources/UI/Game/Store/UI_T_Category_Economy.png"));
	pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
		, TEXT("../../Resources/UI/Game/Store/UI_T_Category_Inventory.png"));
	pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
		, TEXT("../../Resources/UI/Game/Store/UI_T_HeaderIconCircle_Base.png"));
	pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
		, TEXT("../../Resources/UI/Game/Store/UI_T_HeaderIconCircle_Shadow.png"));
	pGameInstance->Find_And_Add_Texture(m_pDevice, m_pContext, m_iLevel
		, TEXT("../../Resources/UI/Game/Store/UI_T_Knuts.png"));

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CUI_Store::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Compoents()))
		return E_FAIL;

	m_fOffset = _float2(130.f, 360.f);
	m_fWidth = 80.f;
	m_fHeight = 80.f;
	m_iHorizontal = 5;
	m_iVertical = 4;
	Ready_Offset();
	//Store_Sell_Read_File(TEXT("../../Resources/GameData/UIData/UI_Group_Store_Sell_Edit.uidata"));
	//Store_Buy_Read_File(TEXT("../../Resources/GameData/UIData/UI_Group_Store_Buy.uidata"));
	Ready_DefaultTexture();
	Set_Item();
	m_isOpen = false;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pInventory = static_cast<CInventory*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Inventory"), TEXT("GameObject_Inventory")));
	Safe_AddRef(m_pInventory);
	Safe_Release(pGameInstance);

	return S_OK;
}
void CUI_Store::Tick(_float fTimeDelta)
{
	if (false == m_isOpen) 
	{
		m_isSetGold = true;
		return;
	}

	__super::Tick(fTimeDelta);


	if (m_isSetGold)
	{
		m_pGoldFont->Set_Text(to_wstring(m_pInventory->Get_Gold()));
		m_isSetGold = false;
	}

	for (_uint i = 0; i < m_pSlots.size(); ++i)
	{
		if (m_pSlots[i]->Get_Clicked())
		{
			if (false == m_pItems[i]->Buy())
				cout << "아이템 구입 실패" << '\n';
			cout << "구입 성공" << '\n';

			m_pGoldFont->Set_Text(to_wstring(m_pInventory->Get_Gold()));
		}
	}
}
void CUI_Store::Late_Tick(_float fTimeDelta)
{
	if (false == m_isOpen)
		return;

	__super::Late_Tick(fTimeDelta);
}
void CUI_Store::Open()
{
	m_isOpen = true;
}
void CUI_Store::Close()
{
	m_isOpen = false;
}
HRESULT CUI_Store::Store_Sell_Read_File(const _tchar* pFilePath)
{
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
	_uint iSize = 0;
	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Effect_Back_SellFrame"), reinterpret_cast<CComponent**>(&m_pSellFrame))))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Effect_Back_SellFrame)");
		__debugbreak();
		return E_FAIL;
	}

	m_pSellFrame->Load(Load_File(hFile, false));
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Effect_Back_SellIcon"), reinterpret_cast<CComponent**>(&m_pSellIcon))))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Effect_Back_SellIcon)");
		__debugbreak();
		return E_FAIL;
	}

	m_pSellIcon->Load(Load_File(hFile, false));
	m_pSellIcon->Set_Parent(m_pSellFrame);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_SellBack"), reinterpret_cast<CComponent**>(&m_pSellBack))))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Back_SellBack)");
		__debugbreak();
		return E_FAIL;
	}

	m_pSellBack->Load(Load_File(hFile, false));
	m_pSellBack->Set_Parent(m_pSellFrame);

	CloseHandle(hFile);

	return S_OK;
}
HRESULT CUI_Store::Store_Buy_Read_File(const _tchar* pFilePath)
{
	/*if (nullptr == m_pIcons[iIndex] || nullptr == m_pFrames[iIndex] || nullptr == m_pCountBacks[iIndex]
		|| nullptr == m_pItemBacks[iIndex] || iIndex >= POTIONTAP::POTIONTAP_END)
	{
		MSG_BOX("Failed Load");
		return E_FAIL;
	}*/

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
	_uint iSize = { 0 };
	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);


	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Effect_Back_BuyFrame"), reinterpret_cast<CComponent**>(&m_pBuyFrame))))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Effect_Back_BuyFrame)");
		__debugbreak();
		return E_FAIL;
	}

	m_pBuyFrame->Load(Load_File(hFile, false));
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Effect_Back_BuyIcon"), reinterpret_cast<CComponent**>(&m_pBuyIcon))))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Effect_Back_BuyIcon)");
		__debugbreak();
		return E_FAIL;
	}

	m_pBuyIcon->Load(Load_File(hFile, false));
	m_pBuyIcon->Set_Parent(m_pBuyFrame);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Back_BuyBack"), reinterpret_cast<CComponent**>(&m_pBuyBack))))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Back_BuyBack)");
		__debugbreak();
		return E_FAIL;
	}

	m_pBuyBack->Load(Load_File(hFile, false));
	m_pBuyBack->Set_Parent(m_pBuyFrame);

	CloseHandle(hFile);

	return S_OK;
}
CUI::UIDESC CUI_Store::Load_File(const HANDLE hFile, _bool isDDS)
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
HRESULT CUI_Store::Set_Item()
{
	for (auto& pSlot : m_pSlots)
	{
		if (nullptr != pSlot)
		{
			pSlot->Set_IconTexture(nullptr);
		}
	}
	m_pItems.resize(m_pSlots.size());

	m_pItems[0] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_HAT_ARCANE, LEVEL_STATIC, nullptr);
	m_pItems[1] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_HAT_WIZARD, LEVEL_STATIC, nullptr);
	m_pItems[2] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_MASK_DEMIGUISE, LEVEL_STATIC, nullptr);
	m_pItems[3] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_MASK_GUARDIAN, LEVEL_STATIC, nullptr);
	m_pItems[4] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_MASK_NIFFLER, LEVEL_STATIC, nullptr);
	m_pItems[5] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_ROBE_ARCANE, LEVEL_STATIC, nullptr);
	m_pItems[6] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_ROBE_BEAST, LEVEL_STATIC, nullptr);
	m_pItems[7] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_ROBE_DARKARTS, LEVEL_STATIC, nullptr);
	m_pItems[8] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_ROBE_DARKARTS_DELUX, LEVEL_STATIC, nullptr);
	m_pItems[9] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_ROBE_QUDDITCH, LEVEL_STATIC, nullptr);
	m_pItems[10] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_OUTFIT_ARCANE, LEVEL_STATIC, nullptr);
	m_pItems[11] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_OUTFIT_CELTIC, LEVEL_STATIC, nullptr);
	m_pItems[12] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_OUTFIT_DARKARTS_DELUX, LEVEL_STATIC, nullptr);

	for (_uint i = 13; i < 20; ++i)
		m_pItems[i] = CItem::SimpleFactory(ITEM_ID::ITEM_ID_ROBE_ARCANE, LEVEL_STATIC, nullptr);

	_uint iSize = m_pItems.size();
	_uint iIndex = 0;
	for (auto& pItem : m_pItems)
	{
		if (nullptr == m_pSlots[iIndex])
		{
			wstring wszTag = TEXT("Com_UI_Slot");
			wszTag += std::to_wstring(iIndex);
			CUI::UIDESC Desc;
			ZEROMEM(&Desc);
			Desc.vCombinedXY = _float2(0.f, 0.f);
			Desc.fX = m_fPosition[iIndex].x;
			Desc.fY = m_fPosition[iIndex].y;
			Desc.fZ = 0.1f;
			Desc.fSizeX = 70.f;
			Desc.fSizeY = 70.f;

			lstrcpy(Desc.szTexturePath, TEXT(""));

			if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Slot"),
				wszTag.c_str(), reinterpret_cast<CComponent**>(&m_pSlots[iIndex]), &Desc)))
			{
				MSG_BOX("Com_Inventory : Failed Clone Component (Com_UI_Slot)");
				__debugbreak();
				return E_FAIL;
			}
			m_pSlots[iIndex]->Set_IconTexture(pItem->Get_UITexture());
		}
		else if (nullptr == m_pSlots[iIndex]->Get_IconTexture() && nullptr != m_pSlots[iIndex])
		{
			m_pSlots[iIndex]->Set_IconTexture(pItem->Get_UITexture());
		}
		++iIndex;
	}

	return S_OK;
}
HRESULT CUI_Store::Ready_Offset()
{
	_float2 fOffSet = m_fOffset;

	for (size_t y = 0; y < m_iVertical; y++)
	{
		for (size_t x = 0; x < m_iHorizontal; x++)
		{
			_float2 fPos = _float2(fOffSet.x + x * m_fWidth, fOffSet.y + y * m_fHeight);
			m_fPosition.push_back(fPos);
		}
	}
	m_pSlots.resize(m_iVertical * m_iHorizontal);

	return S_OK;
}

HRESULT CUI_Store::Ready_DefaultTexture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Default"), reinterpret_cast<CComponent**>(&m_pStoreDefault))))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Back_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 360.f };
	UIDesc.fZ = { 0.9f };
	UIDesc.fSizeX = { 1280.f };
	UIDesc.fSizeY = { 720.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Store/Store_Default.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);

	m_pStoreDefault->Load(UIDesc);


	_tchar szFilePath[MAX_PATH] = TEXT("");
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Cursor.uidata"));
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Cursor"),
		TEXT("Com_UI_PotionTap_Cursor"), reinterpret_cast<CComponent**>(&m_pCursor), szFilePath)))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Back_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI_Font::FONTDESC Desc;
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_vPos = { 440.f, 260.f };
	wstring wstrNCount = TEXT("");

	lstrcpy(Desc.m_pText, wstrNCount.c_str());
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_GoldFont"), reinterpret_cast<CComponent**>(&m_pGoldFont), &Desc)))
	{
		MSG_BOX("Com_CUI_Store : Failed Clone Component (Com_UI_Back_Default)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Store::Add_Compoents()
{
	//const _tchar* pComTag[20] = { TEXT("Com_Slot0"), TEXT("Com_Slot1"), TEXT("Com_Slot2"),
	//	TEXT("Com_Slot3"), TEXT("Com_Slot4"), TEXT("Com_Slot5"), TEXT("Com_Slot6"), TEXT("Com_Slot7"), TEXT("Com_Slot8"),
	//TEXT("Com_Slot9"), TEXT("Com_Slot10"),  TEXT("Com_Slot11"),  TEXT("Com_Slot12"),  TEXT("Com_Slot13"), TEXT("Com_Slot14"),
	//TEXT("Com_Slot15"), TEXT("Com_Slot16"), TEXT("Com_Slot17"), TEXT("Com_Slot18"), TEXT("Com_Slot19") };
	//
	//// Prototype_GameObject_UI_Slot;

	//
	//for (_uint i = 0; i < 20; ++i)
	//{
	//	CUI::UIDESC UIDesc;
	//	UIDesc.fX = m_fPosition[i].x;
	//	UIDesc.fY = m_fPosition[i].y;
	//	UIDesc.fSizeX = 30.f;
	//	UIDesc.fSizeY = 30.f;
	//	UIDesc.fZ = 0.3f;
	//	UIDesc.szTexturePath = TEXT("../../Resources/UI/Game/UI/Icons/MenuAssets/UI_T_ButtonBack.png");
	//	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Slot")
	//		, pComTag[i], reinterpret_cast<CComponent**>(&m_pSlots[i]))))
	//		return E_FAIL;
	//}

	return S_OK;
}

CUI_Store* CUI_Store::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_Store* pInstance = New CUI_Store(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Create CUI_Store");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Store::Clone(void* pArg)
{
	CUI_Store* pInstance = New CUI_Store(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone CUI_Store");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Store::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pStoreDefault);
		Safe_Release(m_pCursor);
		Safe_Release(m_pSellFrame);
		Safe_Release(m_pSellIcon);
		Safe_Release(m_pSellBack);
		Safe_Release(m_pBuyFrame);
		Safe_Release(m_pBuyIcon);
		Safe_Release(m_pBuyBack);
		Safe_Release(m_pGoldFont);

		for (auto& pSlot : m_pSlots)
		{
			Safe_Release(pSlot);
		}
		m_pSlots.clear();

		for (auto& pItem : m_pItems)
		{
			Safe_Release(pItem);
		}
		m_pItems.clear();
		Safe_Release(m_pInventory);
	}
}
