#include "UI_Group_Cursor.h"
#include "GameInstance.h"
#include "UI_Effect_Back.h"
#include "UI_Back.h"

CUI_Group_Cursor::CUI_Group_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group_Cursor::CUI_Group_Cursor(const CUI_Group_Cursor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group_Cursor::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	if (FAILED(Add_Prototype()))
	{
		MSG_BOX("Failed CUI_Group_Cursor Add ProtoType");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Group_Cursor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Read_File(reinterpret_cast<const _tchar*>(pArg))))
		return E_FAIL;

	return S_OK;
}

void CUI_Group_Cursor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Set_CursorPos();
}

void CUI_Group_Cursor::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CUI_Group_Cursor::Set_CursorPos()
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	m_pUI_Effect_Back->Set_XY(_float2(_float(ptMouse.x), _float(ptMouse.y)));
}

HRESULT CUI_Group_Cursor::Add_Prototype()
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		CUI_Effect_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		CUI_Back::Create(m_pDevice, m_pContext), true)))
	{
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_Cursor::Add_Components()
{
	BEGININSTANCE;

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Effect_Back"),
		TEXT("Com_UI_Cursor"), reinterpret_cast<CComponent**>(&m_pUI_Effect_Back))))
	{
		MSG_BOX("CUI_Group_Cursor : Failed Clone Component (Com_UI_Cursor)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Back"),
		TEXT("Com_UI_Cursor_Inner"), reinterpret_cast<CComponent**>(&m_pUI_Back))))
	{
		MSG_BOX("CUI_Group_Cursor : Failed Clone Component (Com_UI_Cursor_Inner)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CUI_Group_Cursor::Read_File(const _tchar* pFilePath)
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

	m_pUI_Effect_Back->Load(Load_File(hFile));
	m_pUI_Effect_Back->Set_Effecttype(CUI_Effect_Back::EFFECTTYPE::CURSOR);
	m_pUI_Effect_Back->Set_Z(0.f);
	_uint iSize = { 0 };
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	m_pUI_Back->Load(Load_File(hFile));
	m_pUI_Back->Set_Parent(m_pUI_Effect_Back);
	m_pUI_Back->Set_RemoveBlack();
	m_pUI_Back->Set_Z(0.f);

	CloseHandle(hFile);

	return S_OK;
}

CUI::UIDESC CUI_Group_Cursor::Load_File(const HANDLE hFile)
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

CUI_Group_Cursor* CUI_Group_Cursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Group_Cursor* pInstance = new CUI_Group_Cursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Group_Cursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Group_Cursor::Clone(void* pArg)
{
	CUI_Group_Cursor* pInstance = new CUI_Group_Cursor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Group_Cursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Group_Cursor::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Effect_Back);
	Safe_Release(m_pUI_Back);
}

