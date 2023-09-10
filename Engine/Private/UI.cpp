#include "UI.h"
#include "GameInstance.h"

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
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
	//lstrcpy(m_wszTextureName, rhs.m_wszTextureName);
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

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

	SetRect(&rcUI, _int(m_vCombinedXY.x - m_fSizeX * 0.5f), _int(m_vCombinedXY.y - m_fSizeY * 0.5f), _int(m_vCombinedXY.x + m_fSizeX * 0.5f), _int(m_vCombinedXY.y + m_fSizeY * 0.5f));

	isIn = PtInRect(&rcUI, ptMouse);

	return isIn;
}

_float2 CUI::UIPos_To_WorldPos(_float fX, _float fY)
{
	_float2 fXY = _float2(fX - m_fWinSizeX * 0.5f, -fY + m_fWinSizeY * 0.5f);

	return fXY;
}

_float2 CUI::WorldPos_To_UIPos(_float fX, _float fY)
{
	_float2 fXY = _float2(fX + m_fWinSizeX * 0.5f, -(fY - m_fWinSizeY * 0.5f));

	return fXY;
}

HRESULT CUI::Change_Position(_float fX, _float fY)
{

	m_pTransform->Set_Position(
		XMVectorSet(m_vCombinedXY.x - m_fWinSizeX * 0.5f, -m_vCombinedXY.y + m_fWinSizeY * 0.5f, m_fZ, 1.f));

	return S_OK;
}

HRESULT CUI::Change_Scale(_float fX, _float fY)
{
	//	m_fSizeX = fX;
	//	m_fSizeY = fY;

	m_pTransform->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));

	return S_OK;
}

void CUI::Free()
{
	__super::Free();

}