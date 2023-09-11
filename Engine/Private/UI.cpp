#include "UI.h"
#include "GameInstance.h"
#include "Component_Manager.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
	, m_fX(rhs.m_fX)
	, m_fY(rhs.m_fY)
	, m_fSizeX(rhs.m_fSizeX)
	, m_fSizeY(rhs.m_fSizeY)
	, m_iWinSizeX(rhs.m_iWinSizeX)
	, m_iWinSizeY(rhs.m_iWinSizeY)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
	//lstrcpy(m_wszTextureName, rhs.m_wszTextureName);
}

HRESULT CUI::Initialize_Prototype(_uint iWinSizeX, _uint iWinSizeY)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_iWinSizeX, m_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		DWORD dwByte = 0;
		Load(pArg, dwByte);
	}

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pParent == nullptr)
	{
		m_vCombinedXY.x = m_fX;
		m_vCombinedXY.y = m_fY;
	}
	else
	{
		_float2 vParentPos = m_pParent->Get_fXY();

		m_vCombinedXY.x = vParentPos.x + m_fX;
		m_vCombinedXY.y = vParentPos.y + m_fY;
	}
}

void CUI::Late_Tick(_float fTimeDelta)
{
	Change_Scale(m_fSizeX, m_fSizeY);
	Change_Position(m_fX, m_fY);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI::Render()
{
	__super::Render();

	return S_OK;
}

_bool CUI::Is_In_Rect(HWND hWnd)
{
	_bool		isIn = false;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(hWnd, &ptMouse);

	RECT		rcUI;

	SetRect(&rcUI, _int(m_vCombinedXY.x - m_fSizeX * 0.5f), _int(m_vCombinedXY.y - m_fSizeY * 0.5f), 
		_int(m_vCombinedXY.x + m_fSizeX * 0.5f), _int(m_vCombinedXY.y + m_fSizeY * 0.5f));

	isIn = PtInRect(&rcUI, ptMouse);

	return isIn;
}

_float2 CUI::UIPos_To_WorldPos(_float fX, _float fY)
{
	_float2 fXY = _float2(fX - m_iWinSizeX * 0.5f, -fY + m_iWinSizeY * 0.5f);

	return fXY;
}

_float2 CUI::WorldPos_To_UIPos(_float fX, _float fY)
{
	_float2 fXY = _float2(fX + m_iWinSizeX * 0.5f, -(fY - m_iWinSizeY * 0.5f));

	return fXY;
}

HRESULT CUI::Change_Position(_float fX, _float fY)
{
	m_pTransform->Set_Position(
		XMVectorSet(m_vCombinedXY.x - m_iWinSizeX * 0.5f, -m_vCombinedXY.y + m_iWinSizeY * 0.5f, m_fZ, 1.f));

	return S_OK;
}

HRESULT CUI::Change_Scale(_float fX, _float fY)
{
	//	m_fSizeX = fX;
	//	m_fSizeY = fY;

	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));

	return S_OK;
}

HRESULT CUI::Save(HANDLE hFile, _ulong& dwByte)
{
	wstring strPath = L"3DTeamPortfolio";
	wstring strFullPath = m_wszTexturePath;

	strFullPath.find(strPath);

	_uint found = strFullPath.find(strPath);

	if (found != std::wstring::npos)
	{
		_tchar wszPath[MAX_PATH] = TEXT("");
		std::wstring result = TEXT("..\\..") + strFullPath.substr(found + strPath.length());
		lstrcpy(m_wszTexturePath, result.c_str());
	}
	else
	{
		MSG_BOX("Failed UI Save");
	}

	WriteFile(hFile, &m_vCombinedXY, sizeof m_vCombinedXY, &dwByte, nullptr);
	WriteFile(hFile, &m_fX, sizeof m_fX, &dwByte, nullptr);
	WriteFile(hFile, &m_fY, sizeof m_fY, &dwByte, nullptr);
	WriteFile(hFile, &m_fZ, sizeof m_fZ, &dwByte, nullptr);
	WriteFile(hFile, &m_fSizeX, sizeof m_fSizeX, &dwByte, nullptr);
	WriteFile(hFile, &m_fSizeY, sizeof m_fSizeY, &dwByte, nullptr);
	WriteFile(hFile, m_wszTextureName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, m_wszTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_isParent, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_isAlpha, sizeof m_isAlpha, &dwByte, nullptr);
	WriteFile(hFile, &m_vColor, sizeof m_vColor, &dwByte, nullptr);
	WriteFile(hFile, m_wszAlphaTexturePrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, m_wszAlphaTextureFilePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_eUIType, sizeof(UI_ID), &dwByte, nullptr);
	
	m_isSave = true;
	WriteFile(hFile, &m_isSave, sizeof(_bool), &dwByte, nullptr);

	return S_OK;
}

HRESULT CUI::Load(HANDLE hFile, _ulong& dwByte)
{
	ReadFile(hFile, &m_vCombinedXY, sizeof(_float2), &dwByte, nullptr);
	ReadFile(hFile, &m_fX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fZ, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fSizeX, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fSizeY, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, m_wszTextureName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, m_wszTexturePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_isParent, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_isAlpha, sizeof m_isAlpha, &dwByte, nullptr);
	ReadFile(hFile, &m_vColor, sizeof m_vColor, &dwByte, nullptr);
	ReadFile(hFile, m_wszAlphaTexturePrototypeTag, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, m_wszAlphaTextureFilePath, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_eUIType, sizeof(UI_ID), &dwByte, nullptr);
	ReadFile(hFile, &m_isSave, sizeof(_bool), &dwByte, nullptr);

	return S_OK;
}


void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pParent);
}