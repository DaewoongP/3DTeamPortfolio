#include "CInventory_Quest.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

CInventory_Quest::CInventory_Quest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInventory_Quest::CInventory_Quest(const CInventory_Quest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInventory_Quest::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CInventory_Quest Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInventory_Quest::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Offset();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	wstring tag;
	CUI_Back* pBack = nullptr;

	tag = TEXT("Com_UI_Back_Gear_Frame") + Generate_HashtagW(true);
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"), TEXT("Com_UI_Back_Inventory_Frame"), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Info_Main : Failed Clone Component (Com_UI_Effect_Back_Gear_SlotFrame)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	m_pUIs.push_back(pBack);

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 360.f };
	UIDesc.fZ = { 0.8f };
	UIDesc.fSizeX = { 1280.f };
	UIDesc.fSizeY = { 720.f };
	_tchar szAlphaTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Inventory_Default_Edit.png");
	lstrcpy(UIDesc.szTexturePath, szAlphaTexturePath);
	pBack->Load(UIDesc);

	Safe_Release(pGameInstance);
	return S_OK;
}

void CInventory_Quest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CInventory_Quest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CInventory_Quest::Render()
{

	return S_OK;
}

HRESULT CInventory_Quest::Add_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CUI::UIDESC CInventory_Quest::Load_File(const HANDLE hFile)
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

HRESULT CInventory_Quest::Ready_Offset()
{
	_float2 fOffSet = _float2(-350.f, 90.f);

	for (size_t y = 0; y < 5; y++)
	{
		for (size_t x = 0; x < 6; x++)
		{
			_float2 fPos = _float2(fOffSet.x + x * 80, fOffSet.y - y * 80);
			m_fOffset.push_back(fPos);
		}
	}

	return S_OK;
}

HRESULT CInventory_Quest::Add_ItemTexture()
{
	CGameInstance* pGameInstace = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstace);

	CTexture* pTexture = nullptr;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("../../Resources/"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&pTexture))))
	{
		MSG_BOX("Failed CUI_Image Add_Component : (Com_Texture)");
		return E_FAIL;
	}
	m_ItemTextures.push_back(pTexture);

	Safe_Release(pGameInstace);
	return S_OK;
}

CInventory_Quest* CInventory_Quest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventory_Quest* pInstance = new CInventory_Quest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CInventory_Quest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInventory_Quest::Clone(void* pArg)
{
	CInventory_Quest* pInstance = new CInventory_Quest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CInventory_Quest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory_Quest::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& pTexture : m_ItemTextures)
	{
		Safe_Release(pTexture);
	}

	for (auto& pUI : m_pUIs)
	{
		Safe_Release(pUI);
	}
}
