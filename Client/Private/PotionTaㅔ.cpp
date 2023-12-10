#include "PotionTab.h"

#include "GameInstance.h"
#include "UI_Potion_Tap.h"
#include "UI_Effect_Back.h"
#include "Tool.h"
#include "Item.h"

CPotionTab::CPotionTab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPotionTab::CPotionTab(const CPotionTab& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPotionTab::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CPotionTab::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pPotions.resize(POTIONTAP_END);

	Add_Components();
	Ready_Main_Tap();
	Ready_PotionTextures();

	return S_OK;
}

void CPotionTab::Tick(_float fTimeDelta)
{
	m_pUI_Main_Tap->Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_TAB, CInput_Device::KEY_PRESSING))
	{
		m_pUI_Potion_Tap->Tick(fTimeDelta);
	}
	Safe_Release(pGameInstance);
}

void CPotionTab::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);

	m_pUI_Main_Tap->Late_Tick(fTimeDelta);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_TAB, CInput_Device::KEY_PRESSING))
	{
		m_pUI_Potion_Tap->Late_Tick(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_TAB, CInput_Device::KEY_UP))
	{
		Set_CurPotion();
	}
	Safe_Release(pGameInstance);

}

HRESULT CPotionTab::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Potion_Tap"),
		TEXT("Com_UI_Group_Potion_Tap"), reinterpret_cast<CComponent**>(&m_pUI_Potion_Tap))))
	{
		MSG_BOX("Com_PotionTab : Failed Clone Component (Com_UI_Group_Potion_Tap )");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Main_Tap"), reinterpret_cast<CComponent**>(&m_pUI_Main_Tap))))
	{
		MSG_BOX("Com__Potion_Tap : Failed Clone Component (Com_UI_Main_Tap)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CPotionTab::Ready_Main_Tap()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 300.f };
	UIDesc.fY = { 600.f };
	UIDesc.fZ = { 0.8f };
	UIDesc.fSizeX = { 112.f };
	UIDesc.fSizeY = { 112.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/PotionTab/Main_Tap.dds");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);

	m_pUI_Main_Tap->Load(UIDesc);

	CUI_Image::IMAGEDESC ImageDesc;
	ImageDesc.vCombinedXY = m_pUI_Main_Tap->Get_CombinedXY();
	ImageDesc.fX = m_pUI_Main_Tap->Get_XY().x;
	ImageDesc.fY = m_pUI_Main_Tap->Get_XY().y;
	ImageDesc.fZ = 0.f;
	ImageDesc.fSizeX = m_pUI_Main_Tap->Get_SizeXY().x;
	ImageDesc.fSizeY = m_pUI_Main_Tap->Get_SizeXY().y;

	m_pUI_Main_Tap->Set_ImageCom(ImageDesc);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPotionTab::Ready_PotionTextures()
{

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTab/UI_T_Edurus_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTab/UI_T_Maxima_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTab/UI_T_AMFillPotion_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTab/UI_T_AutoDamagePotion_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTab/UI_T_InvisibilityPotion_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTab/UI_T_Mandrake_Byproduct_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTab/UI_T_ChompingCabbage_Byproduct_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTab/UI_T_VenomousTentacula_Byproduct_Meter.png")));

	return S_OK;
}

void CPotionTab::Add_Potion(CItem* pItem, POTIONTAP eType)
{
	if (eType >= POTIONTAP_END || eType < 0)
		return;

	m_pPotions[eType].push_back(pItem);
}

void CPotionTab::Delete_Potion(POTIONTAP eType, _uint iIndex)
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

void CPotionTab::Set_CurPotion()
{
	if (nullptr != m_pUI_Potion_Tap)
		m_eCurPotion = (POTIONTAP)m_pUI_Potion_Tap->Set_CurTapItem();
	
	if (-1 == m_eCurPotion)
		return;
	
	if (nullptr != m_pPotionTextures[m_eCurPotion])
	{
		m_pUI_Main_Tap->Set_Texture(m_pPotionTextures[m_eCurPotion]);
	}
}

void CPotionTab::Use_Item(_float3 vPlayPos)
{
	if (m_pPotions[m_eCurPotion].size() <= 0)
		return;

	dynamic_cast<CTool*>(m_pPotions[m_eCurPotion][0])->Use(vPlayPos);
	Delete_Potion(m_eCurPotion, m_pPotions[m_eCurPotion].size() - 1);
}


CPotionTab* CPotionTab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPotionTab* pInstance = new CPotionTab(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPotionTab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPotionTab::Clone(void* pArg)
{
	CPotionTab* pInstance = new CPotionTab(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPotionTab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPotionTab::Free()
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
				}
			}
			m_pPotions.clear();
		}
		Safe_Release(m_pUI_Potion_Tap);
	}

	Safe_Release(m_pUI_Main_Tap);
	Safe_Release(m_pUI_Main_Count);

	for (auto& pTexture : m_pPotionTextures)
	{
		Safe_Release(pTexture);
	}
	m_pPotionTextures.clear();

}