#include "Quest_Potion.h"
#include "GameInstance.h"
#include "Inventory.h"

#include "UI_Effect_Back.h"
#include "UI_Back.h"
#include "UI_Font.h"

CQuest_Potion::CQuest_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CQuest(pDevice, pContext)
{
}

_bool CQuest_Potion::Is_Finished()
{
	if (false == m_isClear)
		return false;

	return true;
}

HRESULT CQuest_Potion::Initialize()
{
	return S_OK;
}

void CQuest_Potion::Tick(_float fTimeDelta)
{
	if (!m_isClear && m_eCurState == QUESTSTATE_UNLOCK)
	{
		if (nullptr == m_pInventory)
		{
			CGameInstance* pGameInstacne = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstacne);

			m_pInventory = static_cast<CInventory*>(pGameInstacne->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Inventory"), TEXT("GameObject_Inventory")));
			Safe_AddRef(m_pInventory);

			Safe_Release(pGameInstacne);
		}

		m_iCurResourceSize = 0;
		for (size_t i = 0; i < INGREDIENT_END; i++)
		{
			m_iCurResourceSize += m_pInventory->Get_Resource(INGREDIENT(i));
			_int iSize = m_iCurResourceSize - m_iPreResourceSize;
			if (iSize > m_iRequestSize && m_isCheckResource == false)
			{
				m_isClear = true;
				Set_State(QUESTSTATE::QUESTSTATE_CLEAR);
			}
		}

		if (m_isCheckResource)
		{
			m_iPreResourceSize = m_iCurResourceSize;
			m_isCheckResource = false;
		} 
	}
}

void CQuest_Potion::Late_Tick(_float fTimeDelta)
{
	m_pExplainFont->Late_Tick(fTimeDelta);
	m_pCountFont->Late_Tick(fTimeDelta);
}

HRESULT CQuest_Potion::Render()
{
	return S_OK;
}

HRESULT CQuest_Potion::Add_Component()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Font::FONTDESC Desc;
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_vPos = { 1250.f, 380.f };
	lstrcpy(Desc.m_pText, TEXT("재료 현황"));

	m_pExplainFont = static_cast<CUI_Font*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"), &Desc));

	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_vPos = { 1250.f, 390.f };
	lstrcpy(Desc.m_pText, TEXT("0 / 10"));

	m_pCountFont = static_cast<CUI_Font*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"), &Desc));

	Safe_Release(pGameInstance);


	return S_OK;
}

void CQuest_Potion::Clear_Quest()
{
	m_isClear = true;
}

CQuest_Potion* CQuest_Potion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Potion* pInstance = New CQuest_Potion(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CQuest_Potion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest_Potion::Free()
{
	__super::Free();

	Safe_Release(m_pInventory);

	Safe_Release(m_pExplainFont);
	Safe_Release(m_pCountFont);

}
