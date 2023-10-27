#include "PotionTap.h"

#include "GameInstance.h"
#include "UI_Potion_Tap.h"
#include "UI_Effect_Back.h"
#include "Tool.h"
#include "Item.h"
#include "FocusPotion.h"
#include "UI_Font.h"

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
	Ready_Main_Tap();
	Ready_PotionTextures();

	return S_OK;
}

void CPotionTap::Tick(_float fTimeDelta)
{
	m_pUI_Main_Tap->Tick(fTimeDelta);
	if (m_eCurPotion != POTIONTAP_END && m_eCurPotion != -1)
	{
		m_pUI_Main_Count->Tick(fTimeDelta);
		m_pUI_Main_Count->Set_Text(to_wstring(m_pPotions[m_eCurPotion].size()).c_str());
	}
	else
		m_pUI_Main_Count->Set_Text(TEXT(""));


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_TAB, CInput_Device::KEY_PRESSING))
	{
		m_pUI_Potion_Tap->Tick(fTimeDelta);
	}
	Safe_Release(pGameInstance);
}

void CPotionTap::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);

	m_pUI_Main_Tap->Late_Tick(fTimeDelta);
	m_pUI_Main_Count-> Late_Tick(fTimeDelta);

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

_bool CPotionTap::Is_Valid(POTIONTAP ePotionTap)
{
	return ePotionTap >= 0 && ePotionTap < POTIONTAP_END;
}

CTool* CPotionTap::Get_CurTool()
{
	if (false == Is_Valid(m_eCurPotion))
		return nullptr;

	if (m_pPotions[m_eCurPotion].empty())
		return nullptr;

	return m_pPotions[m_eCurPotion].back();
}

HRESULT CPotionTap::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Potion_Tap"),
		TEXT("Com_UI_Group_Potion_Tap"), reinterpret_cast<CComponent**>(&m_pUI_Potion_Tap))))
	{
		MSG_BOX("Com_PotionTap : Failed Clone Component (Com_UI_Group_Potion_Tap )");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Main_Tap"), reinterpret_cast<CComponent**>(&m_pUI_Main_Tap))))
	{
		MSG_BOX("Com_Potion_Tap : Failed Clone Component (Com_UI_Main_Tap)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}
	
	CUI_Font::FONTDESC Desc;
	Desc.m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	Desc.m_fRotation = { 0.f };
	Desc.m_vOrigin = { 0.f, 0.f };
	Desc.m_vScale = { 0.4f, 0.4f };
	Desc.m_vPos = { 317.5f, 619.f };
	wstring wstrNCount = TEXT("");
	lstrcpy(Desc.m_pText, wstrNCount.c_str());

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
		TEXT("Com_UI_Count"), reinterpret_cast<CComponent**>(&m_pUI_Main_Count), &Desc)))
	{
		MSG_BOX("Com_Potion_Tap : Failed Clone Component (Com_UI_Count)");
		ENDINSTANCE;
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CPotionTap::Ready_Main_Tap()
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
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/PotionTap/Main_Tap.dds");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);

	m_pUI_Main_Tap->Load(UIDesc);

	CUI_Image::IMAGEDESC ImageDesc;
	ImageDesc.vCombinedXY = m_pUI_Main_Tap->Get_CombinedXY();
	ImageDesc.fX = m_pUI_Main_Tap->Get_XY().x;
	ImageDesc.fY = m_pUI_Main_Tap->Get_XY().y;
	ImageDesc.fZ = 0.f;
	ImageDesc.fSizeX = 64.f;
	ImageDesc.fSizeY = 64.f;

	m_pUI_Main_Tap->Set_ImageCom(ImageDesc);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPotionTap::Ready_PotionTextures()
{

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTap/UI_T_Edurus_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTap/UI_T_Maxima_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTap/UI_T_AMFillPotion_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTap/UI_T_AutoDamagePotion_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTap/UI_T_InvisibilityPotion_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTap/UI_T_Mandrake_Byproduct_Meter.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_WoundCleaning.png")));

	m_pPotionTextures.push_back(CTexture::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/UI/Game/PotionTap/UI_T_VenomousTentacula_Byproduct_Meter.png")));

	return S_OK;
}

CTool* CPotionTap::ToolFactory(POTIONTAP eType)
{
	CTool* pTool = { nullptr };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	switch (eType)
	{
	case Client::ENDURUS_POTION:
		pTool = static_cast<CTool*>(pGameInstance->Clone_Component(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_EdurusPotion_Item")));
		break;
	case Client::MAXIMA_POTION:
		pTool = static_cast<CTool*>(pGameInstance->Clone_Component(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_MaximaPotion_Item")));
		break;
	case Client::FOCUS_POTION:
		pTool = static_cast<CTool*>(pGameInstance->Clone_Component(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_FocusPotion_Item")));
		break;
	case Client::THUNDERBREW_POTION:
		pTool = static_cast<CTool*>(pGameInstance->Clone_Component(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_ThunderBrewPotion_Item")));
		break;
	case Client::INVISIBILITY_POTION:
		pTool = static_cast<CTool*>(pGameInstance->Clone_Component(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_InvisibilityPotion_Item")));
		break;
	case Client::MANDRAKE:
		break;
	case Client::HEALTH_POTION:
		pTool = static_cast<CTool*>(pGameInstance->Clone_Component(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_WiggenweldPotion_Item")));
		break;
	case Client::TENTACULAR:
		break;
	}

	Safe_Release(pGameInstance);

	return pTool;
}

void CPotionTap::Add_Potion(POTIONTAP eType)
{
	if (false == Is_Valid(eType))
		return;

	CTool* pTool = ToolFactory(eType);
	if (nullptr == pTool)
		return;

	m_pPotions[eType].push_back(pTool);
	m_pUI_Potion_Tap->Update_PotionCount(m_pPotions);
}

void CPotionTap::Delete_Potion(POTIONTAP eType, _uint iIndex)
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

void CPotionTap::Delete_Potion(POTIONTAP eType, CTool* pTool)
{
	for (auto iter = m_pPotions[eType].begin(); iter != m_pPotions[eType].end(); ++iter)
	{
		if (pTool == *iter)
		{
			Safe_Release(*iter);
			iter = m_pPotions[eType].erase(iter);
			break;
		}
	}
}

void CPotionTap::Set_CurPotion()
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

void CPotionTap::Use_Item(_float3 vPlayPos)
{
	if (false == Is_Valid(m_eCurPotion))
		return;

	if (m_pPotions[m_eCurPotion].empty())
		return;

	CTool* pTool = (m_pPotions[m_eCurPotion].back());
	pTool->Use(vPlayPos);

	Delete_Potion(m_eCurPotion, pTool);
	m_pUI_Potion_Tap->Update_PotionCount(m_pPotions);

	if (m_pPotions[m_eCurPotion].empty())
	{
		m_eCurPotion = POTIONTAP_END;
		m_pUI_Main_Tap->Set_Texture(nullptr);
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