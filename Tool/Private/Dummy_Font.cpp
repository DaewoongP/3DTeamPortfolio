#include "Dummy_Font.h"
#include "GameInstance.h"

CDummy_Font::CDummy_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy_Font::CDummy_Font(const CDummy_Font& rhs)
	: CGameObject(rhs)
	, m_vPos(rhs.m_vPos)
	, m_vScale(rhs.m_vScale)
{
}

HRESULT CDummy_Font::Initialize_Prototype(const _tchar* pFontFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pBatch = New SpriteBatch(m_pContext);

	m_pFont = New SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CDummy_Font::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float2* vSize = (_float2*)pArg;
		Set_vScale(*vSize);
	}

	return S_OK;
}

void CDummy_Font::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CDummy_Font::Late_Tick(_float fTimeDelta)
{
	Set_vPos(m_vPos);
	Set_vScale(m_vScale);

	return __super::Late_Tick(fTimeDelta);
}

HRESULT CDummy_Font::Render()
{
	if (nullptr == m_pFont ||
		nullptr == m_pBatch)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();
	
	m_pFont->DrawString(m_pBatch, m_pText, m_vPos, m_vColor, m_fRotation, m_vOrigin, m_vScale);

	m_pBatch->End();

	return S_OK;
}
/*
_bool CDummy_Font::Is_In_Rect()
{
	_bool		isIn = false;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	RECT		rcUI;

	SetRect(&rcUI, _int(m_fX - m_fSizeX * 0.5f), _int(m_fY - m_fSizeY * 0.5f), _int(m_fX + m_fSizeX * 0.5f), _int(m_fY + m_fSizeY * 0.5f));

	isIn = PtInRect(&rcUI, ptMouse);

	return isIn;
}
*/


CDummy_Font* CDummy_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CDummy_Font* pInstance = New CDummy_Font(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFontFilePath)))
	{
		MSG_BOX("Failed to Created CFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_Font::Clone(void* pArg)
{
	CDummy_Font* pInstance = New CDummy_Font(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_Font::Free()
{
	__super::Free();

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);
}
