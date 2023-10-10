#include "..\Public\UI_ScaleButton.h"
#include "GameInstance.h"
CUI_ScaleButton::CUI_ScaleButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Button(pDevice, pContext)
{
}

CUI_ScaleButton::CUI_ScaleButton(const CUI_Button& rhs)
	: CUI_Button(rhs)
{
}

HRESULT CUI_ScaleButton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUI_ScaleButton::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_ScaleButton::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (true == m_isCollsion) // ¸¶¿ì½º ´ë¸é Á¡Á¡ Ä¿Áü
	{
		if (m_fRatio > 1.f)
			return;

		m_fRatio += m_fLerpSpeed * fTimeDelta;
		m_vLerpScale = _float2::Lerp(m_vOriginScale, m_vTargetScale, m_fRatio);
	}
	else  // ¸¶¿ì½º ¶¼¸é Á¡Á¡ ÀÛ¾ÆÁü.
	{
		if (m_fRatio < 0.f)
			return;

		m_fRatio -= m_fLerpSpeed * fTimeDelta;
		m_vLerpScale = _float2::Lerp(m_vOriginScale, m_vTargetScale, m_fRatio);
	}
}

void CUI_ScaleButton::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

_bool CUI_ScaleButton::Collision_Rect(HWND hWnd, _float2 vPos, _float2 vSize)
{
	m_vSize = vSize;
	m_vTargetScale = m_vOriginScale * 2.f;
	return m_isCollsion = __super::Collision_Rect(hWnd, vPos, vSize);
}

CUI_ScaleButton* CUI_ScaleButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ScaleButton* pInstance = New CUI_ScaleButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_ScaleButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CUI_ScaleButton::Clone(void* pArg)
{
	CUI_ScaleButton* pInstance = New CUI_ScaleButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_ScaleButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ScaleButton::Free()
{
	__super::Free();
}
