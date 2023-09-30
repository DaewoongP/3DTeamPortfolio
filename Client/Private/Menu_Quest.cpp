#include "Menu_Quest.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

CMenu_Quest::CMenu_Quest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMenu_Quest::CMenu_Quest(const CMenu_Quest& rhs)
	: CGameObject(rhs)
{
}

void CMenu_Quest::Set_Open(_bool isOpen)
{
	m_isOpen = isOpen;
}

HRESULT CMenu_Quest::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CMenu_Quest Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMenu_Quest::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Offset();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	wstring tag;
	CUI_Back* pBack = nullptr;

	tag = TEXT("Com_UI_Back_Quest_Frame") + Generate_HashtagW(true);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Back"), TEXT("Com_UI_Back_Quest_Frame"), reinterpret_cast<CComponent**>(&pBack))))
	{
		MSG_BOX("Com_Menu_Quest : Failed Clone Component (Com_UI_Back_Quest_Frame)");
		Safe_Release(pGameInstance);
		__debugbreak();
		return E_FAIL;
	}
	m_pUIs.push_back(pBack);

	CUI::UIDESC UIDesc;
	UIDesc.vCombinedXY = { 0.f, 0.f };
	UIDesc.fX = { 640.f };
	UIDesc.fY = { 300.f };
	UIDesc.fZ = { 0.8f };
	UIDesc.fSizeX = { 1280.f };
	UIDesc.fSizeY = { 720.f };
	_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Quest_Frame_Capture.png");
	lstrcpy(UIDesc.szTexturePath, szTexturePath);
	pBack->Load(UIDesc);

	Safe_Release(pGameInstance);
	return S_OK;
}

void CMenu_Quest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMenu_Quest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMenu_Quest::Render()
{

	return S_OK;
}

HRESULT CMenu_Quest::Add_Prototype()
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

CUI::UIDESC CMenu_Quest::Load_File(const HANDLE hFile)
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

HRESULT CMenu_Quest::Ready_Offset()
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

CMenu_Quest* CMenu_Quest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMenu_Quest* pInstance = new CMenu_Quest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMenu_Quest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMenu_Quest::Clone(void* pArg)
{
	CMenu_Quest* pInstance = new CMenu_Quest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMenu_Quest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu_Quest::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& pUI : m_pUIs)
	{
		Safe_Release(pUI);
	}
}
