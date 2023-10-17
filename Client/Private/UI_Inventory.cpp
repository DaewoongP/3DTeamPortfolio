#include "UI_Inventory.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"
#include "UI_Slot.h"
#include "Inventory.h"
#include "Item.h"
#include "Gear_Item.h"
CUI_Inventory::CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Inventory::CUI_Inventory(const CUI_Inventory& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Inventory::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Inventory Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Inventory::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		INVENDESC* pDesc = (INVENDESC*)pArg;

		CUI::UIDESC UIDesc;
		UIDesc.vCombinedXY = pDesc->UIDesc.vCombinedXY;
		UIDesc.fX = pDesc->UIDesc.fX;
		UIDesc.fY = pDesc->UIDesc.fY;
		UIDesc.fZ = pDesc->UIDesc.fZ;
		UIDesc.fSizeX = pDesc->UIDesc.fSizeX;
		UIDesc.fSizeY = pDesc->UIDesc.fSizeY;
		lstrcpy(UIDesc.szTexturePath, pDesc->UIDesc.szTexturePath);

		m_fOffset = pDesc->fOffset;
		m_fWidth = pDesc->fWidth;
		m_fHeight = pDesc->fHeight;
		m_iHorizontal = pDesc->iHorizontal;
		m_iVertical = pDesc->iVertical;
		m_eItemtype = pDesc->eItemtype;
		Ready_Offset();
		Ready_DefaultTexture(UIDesc.szTexturePath);
	}



	return S_OK;
}

void CUI_Inventory::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Swap_InventoryItem();
}

void CUI_Inventory::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Inventory::Render()
{
	return S_OK;
}

HRESULT CUI_Inventory::Add_Prototype()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

CUI::UIDESC CUI_Inventory::Load_File(const HANDLE hFile)
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

HRESULT CUI_Inventory::Ready_Offset()
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

HRESULT CUI_Inventory::Ready_DefaultTexture(const _tchar* pFilePath)
{
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Back_Default"), reinterpret_cast<CComponent**>(&m_pBack))))
	{
		MSG_BOX("Com_Menu_Quest : Failed Clone Component (Com_UI_Back_Quest_Frame)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 360.f };
	UIDesc.fZ = { 0.5f };
	UIDesc.fSizeX = { 1280.f };
	UIDesc.fSizeY = { 720.f };

	switch (m_eItemtype)
	{
	case Client::HAND:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Gear_Inventory_Hand_Frame.dds");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
	}
		break;
	case Client::FACE:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Gear_Inventory_Face_Frame.dds");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
	}
		break;
	case Client::HEAD:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Gear_Inventory_Head_Frame.dds");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
	}
		break;
	case Client::NECK:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Gear_Inventory_Neck_Frame.dds");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
	}
		break;
	case Client::BACK:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Gear_Inventory_Back_Frame.dds");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
	}
		break;
	case Client::OUTFIT:
	{
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Gear_Inventory_Outfit_Frame.dds");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
	}
		break;
	default:
		lstrcpy(UIDesc.szTexturePath, pFilePath);
	}

	m_pBack->Load(UIDesc);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Inventory::Add_ItemTexture()
{
	CGameInstance* pGameInstace = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstace);

	CTexture* pTexture = nullptr;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("../../Resources/"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&pTexture))))
	{
		MSG_BOX("Failed CUI_Image Add_Component : (Com_Texture)");
		__debugbreak();
		return E_FAIL;
	}
	m_ItemTextures.push_back(pTexture);

	Safe_Release(pGameInstace);
	return S_OK;
}

HRESULT CUI_Inventory::Set_InventoryItem(vector<CItem*>& pItems)
{
	for (auto& pSlot : m_pSlots)
	{
		if (nullptr != pSlot)
		{
			pSlot->Set_IconTexture(nullptr);
		}
	}
	
	_uint iSize = pItems.size();
	_uint iIndex = 0;
	for (auto& pItem : pItems)
	{
		if (nullptr == m_pSlots[iIndex])
		{
			wstring wszTag = TEXT("Com_UI_Slot");
			wszTag += std::to_wstring(iIndex);
			CUI::UIDESC Desc;
			ZEROMEM(&Desc);
			Desc.vCombinedXY = _float2(0.f,0.f);
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
		else if (nullptr == m_pSlots[iIndex]->Get_IconTexture() && nullptr!= m_pSlots[iIndex])
		{
				m_pSlots[iIndex]->Set_IconTexture(pItem->Get_UITexture());
		}
		++iIndex;
	}

	return S_OK;
}

HRESULT CUI_Inventory::Delete_InventoryItem(_uint iIndex)
{
	Safe_Release(m_pSlots[iIndex]);
	m_pSlots.erase(m_pSlots.begin() + iIndex);

	static_cast<CInventory*>(m_pOwner)->Delete_Item(m_eItemtype, iIndex);

	return S_OK;
}

HRESULT CUI_Inventory::Swap_InventoryItem()
{
	if (m_eItemtype > OUTFIT)
		return E_FAIL;

	_uint iIndex = 0;
	for (auto& pSlot : m_pSlots)
	{
		if (nullptr != pSlot && pSlot->Get_Clicked())
		{
			static_cast<CInventory*>(m_pOwner)->Swap_Item(iIndex, m_eItemtype);
		}
		++iIndex;
	}

	return S_OK;
}

CUI_Inventory* CUI_Inventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Inventory* pInstance = new CUI_Inventory(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Inventory");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Inventory::Clone(void* pArg)
{
	CUI_Inventory* pInstance = new CUI_Inventory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Inventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Inventory::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& pTexture : m_ItemTextures)
	{
		Safe_Release(pTexture);
	}

	for (auto& pUI : m_pSlots)
	{
		Safe_Release(pUI);
	}

	Safe_Release(m_pBack);
}
