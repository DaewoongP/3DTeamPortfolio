#include "UI_Potion_Tap.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"
#include "UI_Slot.h"
#include "Inventory.h"
#include "UI_Font.h"

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

	if (nullptr != pArg)
	{

		//Ready_Offset();
	}



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

CUI::UIDESC CUI_Potion_Tap::Load_File(const HANDLE hFile)
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
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Potion_Tap_Default.dds");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);
	
	m_pBack->Load(UIDesc);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CUI_Potion_Tap::Set_ItemCount(vector<CItem*>& pItems)
{

	return S_OK;
}

HRESULT CUI_Potion_Tap::Set_CurTapItem()
{
	
	return S_OK;
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

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& pIcon : m_pIcons)
	{
		Safe_Release(pIcon);
	}

	for (auto& pFrames : m_pFrames)
	{
		Safe_Release(pFrames);
	}

	for (auto& pCountBack : m_pCountBack)
	{
		Safe_Release(pCountBack);
	}

	for (auto& pItemBack : m_pItemBack)
	{
		Safe_Release(pItemBack);
	}
	
	for (auto& pFonts : m_pFonts)
	{
		Safe_Release(pFonts);
	}

	Safe_Release(m_pBack);
}
