#include "..\Public\Inventory.h"
#include "GameInstance.h"
#include "UI_Inventory.h"
#include "GameObject.h"

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

	m_pItems.reserve(ITEMTYPE_END);
	m_pPlayerCurItems.reserve(RESOURCE);

	return S_OK;
}

void CInventory::Late_Tick(_float fTimeDelta)
{
	if (m_isOpen)
	{
		//m_pUI_Inventory[m_eCurOpenItemtype]->Set_InventoryItem(m_pItems[m_eCurOpenItemtype]);
		m_pUI_Inventory[m_eCurOpenItemtype]->Late_Tick(fTimeDelta);
	}
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
			pDesc.fOffset = _float2(-360.f, 90.f);
			pDesc.fWidth = 80.f;
			pDesc.fHeight = 80.f;
			pDesc.iHorizontal = 5;
			pDesc.iVertical = 6;
			pDesc.eItemtype = ITEMTYPE(i);
		}
		else
		{
			wstring TexturePath = TEXT("../../Resources/UI/Game/UI_Edit/Inventory_Default_Edit.png");
			lstrcpy( pDesc.UIDesc.szTexturePath, TexturePath.c_str());
			pDesc.fOffset = _float2(-400.f, 100.f);
			pDesc.fWidth = 80.f;
			pDesc.fHeight = 80.f;
			pDesc.iHorizontal = 4;
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

void CInventory::Add_Item(CGameObject* pItem, ITEMTYPE eType)
{
	if (eType >= ITEMTYPE_END || eType < 0)
		return;

	if (eType < RESOURCE)
	{
		if (m_pItems[eType].size() >= iGearMax)
			return;

		m_pItems[eType].push_back(pItem);
		Safe_AddRef(pItem);

		m_pUI_Inventory[eType]->Set_InventoryItem(m_pItems[eType]);
	}
	else
	{
		if (m_pItems[eType].size() >= iResourceMax)
			return;

		m_pItems[eType].push_back(pItem);
		Safe_AddRef(pItem);

		m_pUI_Inventory[eType]->Set_InventoryItem(m_pItems[eType]);
	}
}

void CInventory::Delete_Item(ITEMTYPE eType, _uint iIndex)
{
	_uint Index = 0;
	auto iter = m_pItems[eType].begin();
	for (; iter != m_pItems[eType].end(); ++iter)
	{
		if (Index == iIndex)
		{
			Safe_Release(*iter);
			iter = m_pItems[eType].erase(iter);
			m_pUI_Inventory[eType]->Set_InventoryItem(m_pItems[eType]);
			break;
		}
		++Index;
	}
}

void CInventory::Swap_Item(_uint Index, ITEMTYPE eType)
{
	if (nullptr == m_pPlayerCurItems[eType])
	{
		m_pPlayerCurItems[eType] = m_pItems[eType][Index];
		m_pItems[eType][Index] = nullptr;
		m_pItems[eType].erase(m_pItems[eType].begin() + Index);
	}

	CGameObject* SourItem = m_pItems[eType][Index];
	m_pItems[eType][Index] = m_pPlayerCurItems[eType];
	m_pPlayerCurItems[eType] = SourItem;

	m_pUI_Inventory[eType]->Set_InventoryItem(m_pItems[eType]);
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
	for (size_t i = 0; i < ITEMTYPE_END; i++)
	{
		for (auto& pItem : m_pItems[i])
		{
			Safe_Release(pItem);
		}
	}

	for (auto& pUI_Inven : m_pUI_Inventory)
	{
		Safe_Release(pUI_Inven);
	}

	for (auto& pCurItem : m_pPlayerCurItems)
	{
		Safe_Release(pCurItem);
	}
}