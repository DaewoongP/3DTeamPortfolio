#include "Menu_Setting.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

CMenu_Setting::CMenu_Setting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMenu_Setting::CMenu_Setting(const CMenu_Setting& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMenu_Setting::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CMenu_Setting Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMenu_Setting::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_DefaultTexture();
	Ready_Button();


	return S_OK;
}

void CMenu_Setting::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CMenu_Setting::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMenu_Setting::Render()
{

	return S_OK;
}

void CMenu_Setting::Set_Open(_bool isOpen)
{
	m_isOpen = isOpen;
}


HRESULT CMenu_Setting::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMenu_Setting::Ready_DefaultTexture()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_Back* pBack = nullptr;
	CUI_Back* pShadow = nullptr;
	CUI_Back* pSSAO = nullptr;
	CUI_Back* pSound = nullptr;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"), TEXT("Com_UI_Back_Option_Icon"), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Menu_Setting : Failed Clone Component (Com_UI_Back_Option_Icon)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}
	m_pUIs.push_back(pBack);

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 250.f };
	UIDesc.fY = { 150.f };
	UIDesc.fZ = { 0.1f };
	UIDesc.fSizeX = { 256.f };
	UIDesc.fSizeY = { 99.f };
	_tchar szIconTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Setting/Option_Icon.png");
	lstrcpy(UIDesc.szTexturePath, szIconTexturePath);
	pBack->Load(UIDesc);



	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"), TEXT("Com_UI_Back_Shadow"), reinterpret_cast<CComponent**>(&pShadow))))
	{
		MSG_BOX("Com_Menu_Setting : Failed Clone Component (Com_UI_Back_Shadow)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}
	m_pUIs.push_back(pShadow);

	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 290.f };
	UIDesc.fZ = { 0.9f };
	UIDesc.fSizeX = { 1024.f };
	UIDesc.fSizeY = { 128.f };
	_tchar szShadowTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Setting/Setting_Shadow.png");
	lstrcpy(UIDesc.szTexturePath, szShadowTexturePath);
	pShadow->Load(UIDesc);


	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"), TEXT("Com_UI_Back_SSAO"), reinterpret_cast<CComponent**>(&pSSAO))))
	{
		MSG_BOX("Com_Menu_Setting : Failed Clone Component (Com_UI_Back_SSAO)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}
	m_pUIs.push_back(pSSAO);

	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 400.f };
	UIDesc.fZ = { 0.9f };
	UIDesc.fSizeX = { 1024.f };
	UIDesc.fSizeY = { 128.f };
	_tchar szSSAOTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Setting/Setting_SSAO.png");
	lstrcpy(UIDesc.szTexturePath, szSSAOTexturePath);
	pSSAO->Load(UIDesc);



	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"), TEXT("Com_UI_Back_Sound"), reinterpret_cast<CComponent**>(&pSound))))
	{
		MSG_BOX("Com_Menu_Setting : Failed Clone Component (Com_UI_Back_Sound)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}
	m_pUIs.push_back(pSound);

	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 510.f };
	UIDesc.fZ = { 0.9f };
	UIDesc.fSizeX = { 1024.f };
	UIDesc.fSizeY = { 128.f };
	_tchar szSoundTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Setting/Setting_Sound.png");
	lstrcpy(UIDesc.szTexturePath, szSoundTexturePath);
	pSound->Load(UIDesc);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMenu_Setting::Ready_Button()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Effect_Back_Shadow_Button"), reinterpret_cast<CComponent**>(&m_pShadowButton))))
	{
		MSG_BOX("Com_Menu_Setting : Failed Clone Component (Com_UI_Effect_Back_Shadow_Button)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 1080.f };
	UIDesc.fY = { 290.f };
	UIDesc.fZ = { 0.88f };
	UIDesc.fSizeX = { 128.f };
	UIDesc.fSizeY = { 64.f };

	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/Setting/Setting_Off.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);
	m_pShadowButton->Load(UIDesc);
	m_pShadowButton->Add_Texture(TEXT("../../Resources/UI/Game/Setting/Setting_On.png"));


	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Effect_Back_SSAO_Button"), reinterpret_cast<CComponent**>(&m_pSSAOButton))))
	{
		MSG_BOX("Com_Menu_Setting : Failed Clone Component (Com_UI_Effect_Back_SSAO_Button)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}

	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 1080.f };
	UIDesc.fY = { 400.f };
	UIDesc.fZ = { 0.88f };
	UIDesc.fSizeX = { 128.f };
	UIDesc.fSizeY = { 64.f };

	_tchar szTextureSSAOPath[MAX_PATH] = TEXT("../../Resources/UI/Game/Setting/Setting_Off.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);
	m_pSSAOButton->Load(UIDesc);
	m_pSSAOButton->Add_Texture(TEXT("../../Resources/UI/Game/Setting/Setting_On.png"));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMenu_Setting::Add_Components(wstring wszTag)
{
	
	return S_OK;
}


CMenu_Setting* CMenu_Setting::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMenu_Setting* pInstance = new CMenu_Setting(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMenu_Setting");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMenu_Setting::Clone(void* pArg)
{
	CMenu_Setting* pInstance = new CMenu_Setting(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMenu_Setting");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu_Setting::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& pText : m_pTexts)
	{
		Safe_Release(pText);
	}

	for (auto& pUI : m_pUIs)
	{
		Safe_Release(pUI);
	}

	Safe_Release(m_pShadowButton);
	Safe_Release(m_pSSAOButton);
	Safe_Release(m_pSoundButton);

	for (auto& pTexture : m_pTextures)
	{
		Safe_Release(pTexture);
	}
}
