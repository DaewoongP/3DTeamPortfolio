#include "UI_Inventory.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

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

		UIDesc.vCombinedXY = pDesc->UIDesc.vCombinedXY;
		UIDesc.fX = pDesc->UIDesc.fX;
		UIDesc.fY = pDesc->UIDesc.fY;
		UIDesc.fZ = pDesc->UIDesc.fZ;
		UIDesc.fSizeX = pDesc->UIDesc.fSizeX;
		UIDesc.fSizeY = pDesc->UIDesc.fSizeY;
		lstrcpy(UIDesc.szTexturePath, pDesc->UIDesc.szTexturePath);
		pBack->Load(UIDesc);

		m_fOffset = pDesc->fOffset;
		m_fWidth = pDesc->fWidth;
		m_fHeight = pDesc->fHeight;
		m_iHorizontal = pDesc->iHorizontal;
		m_iVertical = pDesc->iVertical;

		Safe_Release(pGameInstance);
	}

	Ready_Offset();
	return S_OK;
}

void CUI_Inventory::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
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
			_float2 fPos = _float2(fOffSet.x + x * m_fWidth, fOffSet.y - y * m_fHeight);
			m_fPosition.push_back(fPos);
		}
	}

	return S_OK;
}

HRESULT CUI_Inventory::Add_ItemTexture()
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

	for (auto& pUI : m_pUIs)
	{
		Safe_Release(pUI);
	}
}
