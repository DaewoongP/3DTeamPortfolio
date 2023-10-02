#include "PotionTap.h"
#include "GameInstance.h"
#include "UI_Inventory.h"
#include "GameObject.h"
#include "Item.h"
#include "UI_Potion_Tap.h"

CPotionTap::CPotionTap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPotionTap::CPotionTap(const CPotionTap& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPotionTap::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CPotionTap::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pPotions.resize(POTIONTAP_END);

	Add_Components();

	return S_OK;
}

void CPotionTap::Late_Tick(_float fTimeDelta)
{
	if (m_isOpen)
	{
		//m_pUI_Inventory[m_eCurOpenItemtype]->Set_InventoryItem(m_pItems[m_eCurOpenItemtype]);
		m_pUI_Potion_Tap->Late_Tick(fTimeDelta);
	}
}

HRESULT CPotionTap::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Inventory"),
		TEXT("Com_UI_Group_Potion_Tap"), reinterpret_cast<CComponent**>(&m_pUI_Potion_Tap))))
	{
		MSG_BOX("Com_Inventory : Failed Clone Component (Com_UI_Inventory )");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

void CPotionTap::Add_Potion(CItem* pItem, ITEMTYPE eType)
{
	if (eType >= POTIONTAP_END || eType < 0)
		return;

	m_pPotions[eType].push_back(pItem);
}

void CPotionTap::Delete_Potion(ITEMTYPE eType, _uint iIndex)
{
	_uint Index = 0;
	auto iter = m_pPotions[eType].begin();
	for (; iter != m_pPotions[eType].end(); ++iter)
	{
		if (Index == iIndex)
		{
			Safe_Release(*iter);
			iter = m_pPotions[eType].erase(iter);
			break;
		}
		++Index;
	}
}

CPotionTap* CPotionTap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPotionTap* pInstance = new CPotionTap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPotionTap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPotionTap::Clone(void* pArg)
{
	CPotionTap* pInstance = new CPotionTap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPotionTap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPotionTap::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		if (m_pPotions.size() > 0)
		{
			for (size_t i = 0; i < POTIONTAP_END; i++)
			{
				for (auto& pPotion : m_pPotions[i])
				{
					Safe_Release(pPotion);
					pPotion == nullptr;
				}
			}
			m_pPotions.clear();
		}

		Safe_Release(m_pUI_Potion_Tap);
	}
}