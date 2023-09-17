#include "UI_Group_HP.h"
#include "GameInstance.h"
#include "UI_Back.h"
#include "UI_HP.h"

CUI_Group_HP::CUI_Group_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_HP::CUI_Group_HP(const CUI_Group_HP& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_HP::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_HP Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_HP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg))))
		return E_FAIL;

	return S_OK;
}

void CUI_Group_HP::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Group_HP::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Group_HP::Add_Prototype()
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_HP"),
		CUI_HP::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_HP::Add_Components()
{
	BEGININSTANCE;

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_HP_Frame"), reinterpret_cast<CComponent**>(&m_pUI_Back))))
	{
		MSG_BOX("CUI_Group_HP : Failed Clone Component (Com_UI_HP_Frame)");
		ENDINSTANCE;
		return E_FAIL;
	}

	CUI_HP::HPTYPE eType = CUI_HP::HPTYPE::PLAYER;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_HP"),
		TEXT("Com_UI_HP"), reinterpret_cast<CComponent**>(&m_pUI_HP), &eType)))
	{
		MSG_BOX("CUI_Group_HP : Failed Clone Component (Com_UI_HP)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_HP::Read_File(const _tchar* pFilePath)
{
	_ulong dwByte = 0;
	DWORD dwStrByte = 0;

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	_tchar szGroupName[MAX_PATH] = TEXT("");

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, szGroupName, dwStrByte, &dwByte, nullptr);

	m_pUI_Back->Load(Load_File(hFile));

	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pUI_HP->Load(Load_File(hFile));
	m_pUI_HP->Set_Parent(m_pUI_Back);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CUI_Group_HP::Load_File(const HANDLE hFile)
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

	fs::path fsPath = UIDesc.szTexturePath;

	std::wstring wszExtension = fsPath.extension();
	if (wszExtension == TEXT(".png"))
	{
		fsPath.replace_extension(L".dds");
		lstrcpy(UIDesc.szTexturePath, fsPath.c_str());
	}

	return UIDesc;
}

CUI_Group_HP* CUI_Group_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_HP* pInstance = new CUI_Group_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_HP::Clone(void* pArg)
{
	CUI_Group_HP* pInstance = new CUI_Group_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_HP::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Back);
	Safe_Release(m_pUI_HP);

}

