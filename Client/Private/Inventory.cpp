#include "..\Public\Inventory.h"
#include "GameInstance.h"
#include "UI_Inventory.h"
#include "Item.h"
#include "Ingredient.h"
#include "UI_Farming.h"
#include "Gear_Item.h"
CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInventory::CInventory(const CInventory& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInventory::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pItems.resize(ITEMTYPE_END);
	m_pPlayerCurItems.resize(RESOURCE);
	m_ResourcesCount.resize(INGREDIENT_END, 0);
	Add_Components();

	Setting_Item(ITEM_ID::ITEM_ID_ROBE_QUDDITCH);
	Setting_Item(ITEM_ID::ITEM_ID_MASK_GUARDIAN);
	Setting_Item(ITEM_ID::ITEM_ID_OUTFIT_ARCANE);

	//BEGININSTANCE;

	//CWiggenweldPotion::INIT_DESC initDesc;
	//initDesc.pHealthCom = nullptr;
	//CWiggenweldPotion* pWiggenweldPotion = static_cast<CWiggenweldPotion*>(
	//	pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_WiggenweldPotion"), &initDesc));
	//
	//Add_Item(pWiggenweldPotion, ITEMTYPE::GEAR);
	//
	//Safe_Release(pWiggenweldPotion);
	//ENDINSTANCE

	return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
	if (m_isSetting)
	{
		m_isSetting = false;
	}

	if (m_isOpen)
	{
		m_pUI_Inventory[m_eCurOpenItemtype]->Tick(fTimeDelta);
	}
}

void CInventory::Late_Tick(_float fTimeDelta)
{
	if (m_isOpen)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		if (pGameInstance->Get_DIKeyState(DIK_ESCAPE, CInput_Device::KEY_DOWN))
		{
			m_isOpen = false;
		}
		Safe_Release(pGameInstance);
		//m_pUI_Inventory[m_eCurOpenItemtype]->Set_InventoryItem(m_pItems[m_eCurOpenItemtype]);
		m_pUI_Inventory[m_eCurOpenItemtype]->Late_Tick(fTimeDelta);
	}
}

_bool CInventory::Can_Purchase(_uint iCost)
{
	return (m_iGold >= iCost) ? true : false;
}

void CInventory::Pay_Money(_uint iCost)
{
	m_iGold -= iCost;
}

void CInventory::Earn_Money(_uint iCost)
{
	m_iGold += iCost;
}

HRESULT CInventory::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (size_t i = 0; i < ITEMTYPE_END; i++)
	{
		CUI_Inventory::INVENDESC pDesc;
		ZEROMEM(&pDesc);

		pDesc.UIDesc.vCombinedXY = _float2(0.f, 0.f);
		pDesc.UIDesc.fX = 0.f;
		pDesc.UIDesc.fY = 0.f;
		pDesc.UIDesc.fZ = 0.8f;
		pDesc.UIDesc.fSizeX = 1280.f;;
		pDesc.UIDesc.fSizeY = 720.f;

		if (i >= 0 && i <= 5)
		{
			wstring TexturePath = TEXT("../../Resources/UI/Game/UI_Edit/Gear_Item_Frame.png");
			lstrcpy(pDesc.UIDesc.szTexturePath, TexturePath.c_str());
			pDesc.fOffset = _float2(240.f, 260.f);
			pDesc.fWidth = 80.f;
			pDesc.fHeight = 80.f;
			pDesc.iHorizontal = 4;
			pDesc.iVertical = 5;
			pDesc.eItemtype = ITEMTYPE(i);
		}
		else
		{
			wstring TexturePath = TEXT("../../Resources/UI/Game/UI_Edit/Inventory_Default_Edit.png");
			lstrcpy(pDesc.UIDesc.szTexturePath, TexturePath.c_str());
			pDesc.fOffset = _float2(280.f, 270.f);
			pDesc.fWidth = 80.f;
			pDesc.fHeight = 80.f;
			pDesc.iHorizontal = 6;
			pDesc.iVertical = 5;
			pDesc.eItemtype = ITEMTYPE(i);
		}

		wstring wszTag = TEXT("Com_UI_Inventory_");
		wszTag += std::to_wstring(i);
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Inventory"),
			wszTag.c_str(), reinterpret_cast<CComponent**>(&m_pUI_Inventory[i]), &pDesc)))
		{
			MSG_BOX("Com_Inventory : Failed Clone Component (Com_UI_Inventory )");
			Safe_Release(pGameInstance);
			__debugbreak();
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

_bool CInventory::Add_Item(CItem* pItem, ITEMTYPE eType)
{
	if (eType >= ITEMTYPE_END || eType < 0)
	{
		// 아이템 타입 검사하기.
		__debugbreak();
		return false;
	}

	// 파밍UI 가져오기.
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	CUI_Farming* pFarming = static_cast<CUI_Farming*>(pGameInstance->Find_Component_In_Layer(
		LEVEL_STATIC
		, TEXT("Layer_UI")
		, TEXT("GameObject_UI_Farming")));
	if (nullptr == pFarming)
	{
		Safe_Release(pGameInstance);
		return false;
	}

	Safe_Release(pGameInstance);

	CIngredient* pIngredient = dynamic_cast<CIngredient*>(pItem);
	if (nullptr != pIngredient)
	{
		m_ResourcesCount[pIngredient->Get_Ingredient()]++;
	}

	if (eType < RESOURCE)
	{
		if (m_pItems[eType].size() >= iGearMax)
			return false;
		pFarming->Play(pItem);
		m_pItems[eType].push_back(pItem);
		m_pUI_Inventory[eType]->Set_GearInventoryItem(m_pItems[eType]);
	}
	else if (RESOURCE == eType)
	{
		/*if (m_pItems[eType].size() >= iResourceMax)
			return false;*/
		pFarming->Play(pItem);
		m_pItems[eType].push_back(pItem);
		m_pUI_Inventory[eType]->Set_ResourceInventoryItem(m_pItems[eType], &m_ResourcesCount);
	}

	return true;
}

_bool CInventory::Add_Item(const _tchar* pPrototypeTag, _uint iLevel, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	CItem* pItem = static_cast<CItem*>(pGameInstance->Clone_Component(iLevel, pPrototypeTag, pArg));
	Safe_Release(pGameInstance);
	
	// 잘못된 아이템을 넣은 경우 디버그 브레이크
	if (nullptr == pItem)
	{
		__debugbreak();
		return false;
	}

	if (false == Add_Item(pItem, pItem->Get_Type()))
	{
		Safe_Release(pItem);
		return false;
	}

	return true;
}

_bool CInventory::Add_Item(ITEM_ID eItemID, _uint iLevel, void* pArg)
{

	CItem* pItem = CItem::SimpleFactory(eItemID, iLevel, pArg);
	if (nullptr == pItem)
		return false;

	ITEMTYPE eCurType = m_pUI_Inventory[pItem->Get_Type()]->Get_CurType();

	if (eCurType > OUTFIT)
	{
		if (m_pItems[eCurType].size() / iResourceMax >= 1.f)
		{
			/*Safe_Release(pItem);
			return false;*/
		}
	}
	else
	{
		if (m_pItems[eCurType].size() / iGearMax >= 1.f)
		{
			Safe_Release(pItem);
			return false;
		}
	}

	if (false == Add_Item(pItem, pItem->Get_Type()))
	{
		Safe_Release(pItem);
		return false;
	}
	return true;
}

_bool CInventory::Setting_Item(ITEM_ID eItemID, _uint iLevel, void* pArg)
{
	CItem* pItem = CItem::SimpleFactory(eItemID, iLevel, pArg);
	if (nullptr == pItem)
		return false;

	ITEMTYPE eCurType = m_pUI_Inventory[pItem->Get_Type()]->Get_CurType();

	m_pPlayerCurItems[eCurType] = pItem;

	return true;
}

void CInventory::Delete_Item(ITEM_ID eTargetItemID)
{
	for (_uint i = 0; i < ITEMTYPE_END; ++i)
	{
		for (auto iter = m_pItems[i].begin(); iter != m_pItems[i].end();)
		{
			if ((*iter)->Get_ItemID() == eTargetItemID)
			{
				ITEMTYPE eType = (*iter)->Get_Type();
				if (eType == ITEMTYPE::RESOURCE)
				{
					CIngredient* pIngredient = static_cast<CIngredient*>(*iter);
					m_ResourcesCount[pIngredient->Get_Ingredient()]--;
					Safe_Release(*iter);
					m_pItems[eType].erase(iter);
					m_pUI_Inventory[eType]->Set_ResourceInventoryItem(m_pItems[eType], &m_ResourcesCount);
				}
				else
				{
					Safe_Release(*iter);
					m_pItems[eType].erase(iter);
					m_pUI_Inventory[eType]->Set_GearInventoryItem(m_pItems[eType]);
				}
				return;
			}
			else
				++iter;
		}
	}
}

_bool CInventory::Delete_Item(ITEMTYPE eType, CItem* pItem)
{
	return _bool();
}

_bool CInventory::Delete_Item(ITEMTYPE eType, _uint iIndex)
{
	_uint Index = 0;
	auto iter = m_pItems[eType].begin();
	for (; iter != m_pItems[eType].end(); ++iter)
	{
		if (Index == iIndex)
		{
			Safe_Release(*iter);
			iter = m_pItems[eType].erase(iter);
			if (eType == RESOURCE)
			{
				m_pUI_Inventory[eType]->Set_ResourceInventoryItem(m_pItems[eType], &m_ResourcesCount);
			}
			else
			{
				m_pUI_Inventory[eType]->Set_GearInventoryItem(m_pItems[eType]);
			}
			return true;
		}
		++Index;
	}

	return false;
}

void CInventory::Swap_Item(_uint Index, ITEMTYPE eType)
{
	if (nullptr == m_pPlayerCurItems[eType])
	{

		m_pPlayerCurItems[eType] = m_pItems[eType][Index];
		m_pItems[eType][Index] = nullptr;
		m_pItems[eType].erase(m_pItems[eType].begin() + Index);
	}
	else
	{
		CItem* SourItem = m_pItems[eType][Index];
		m_pItems[eType][Index] = m_pPlayerCurItems[eType];
		m_pPlayerCurItems[eType] = SourItem;
	}
	
	m_pUI_Inventory[eType]->Set_GearInventoryItem(m_pItems[eType]);
	
	BEGININSTANCE;
	pGameInstance->Play_Sound(TEXT("Item_Equip.wav"), 1.0f);
	ENDINSTANCE

	CGear_Item* pGearItem = static_cast<CGear_Item*>(m_pPlayerCurItems[eType]);
	if (nullptr != pGearItem)
		pGearItem->Equipment();

}

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventory* pInstance = new CInventory(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CInventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
	CInventory* pInstance = new CInventory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CInventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		if (m_pItems.size() > 0)
		{
			for (size_t i = 0; i < ITEMTYPE_END; i++)
			{
				for (auto& pItem : m_pItems[i])
				{
					Safe_Release(pItem);
				}
			}
			m_pItems.clear();
		}

		for (auto& pUI_Inven : m_pUI_Inventory)
		{
			Safe_Release(pUI_Inven);
		}

		for (auto& pCurItem : m_pPlayerCurItems)
		{
			Safe_Release(pCurItem);
		}
		m_pPlayerCurItems.clear();
	}
}