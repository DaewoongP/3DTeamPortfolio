#include "..\Public\UI_Dynamic_Back.h"
#include "GameInstance.h"
#include "CoolTime.h"

CUI_Dynamic_Back::CUI_Dynamic_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Back(pDevice, pContext)
{

}
CUI_Dynamic_Back::CUI_Dynamic_Back(const CUI_Dynamic_Back& rhs)
	: CUI_Back(rhs)
{
}

HRESULT CUI_Dynamic_Back::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Dynamic_Back::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pDuration = CCoolTime::Create(m_pDevice, m_pContext);
	m_pDuration->Set_Enable(false);
	m_pDuration->Set_MaxCoolTime(3.f);

	return S_OK;
}

void CUI_Dynamic_Back::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	switch (m_eState)
	{
	case Client::CUI_Dynamic_Back::FADE_IN:
		OnFadeInTick(fTimeDelta);
		break;
	case Client::CUI_Dynamic_Back::FADE_OUT:
		OnFadeOutTick(fTimeDelta);
		break;
	case Client::CUI_Dynamic_Back::LOOP:
		OnLoopTick(fTimeDelta);
		break;
	default:
		break;
	}
}

void CUI_Dynamic_Back::Late_Tick(_float fTimeDelta)
{
	if (FADE_STATE_END == m_eState)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}

}

HRESULT CUI_Dynamic_Back::Render()
{
	m_iTextureIndex = 0;
	for (size_t i = 0; i < m_Textures.size(); ++i)
	{
		if (nullptr == m_Textures[i])
		{
			m_iTextureIndex++;
			continue;
		}
		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin("Dynamic_Back")))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
		m_iTextureIndex++;
	}
	return S_OK;
}

void CUI_Dynamic_Back::Set_Duration(_float fDuration)
{
	m_pDuration->Set_MaxCoolTime(fDuration);
}

HRESULT CUI_Dynamic_Back::Set_Texture(_uint iIndex, CTexture* pTexture)
{
	if (iIndex >= m_Textures.size())
		return E_FAIL;

	Safe_Release(m_Textures[iIndex]);

	m_Textures[iIndex] = pTexture;
	Safe_AddRef(m_Textures[iIndex]);

	return S_OK;
}

void CUI_Dynamic_Back::Play_FadeIn()
{
	m_eState = FADE_IN;
	m_fAlphaRatio = 0.f;
	m_pDuration->Play_CoolTime();
}

void CUI_Dynamic_Back::PushBackNull()
{
	m_Textures.push_back(nullptr);
}

void CUI_Dynamic_Back::OnFadeInTick(_float fTimeDelta)
{
	m_fAlphaRatio += fTimeDelta;

	if (m_fAlphaRatio >= 1.f)
		m_eState = LOOP;
}

void CUI_Dynamic_Back::OnLoopTick(_float fTimeDelta)
{
	m_pDuration->Tick(fTimeDelta);

	if (false == m_pDuration->IsEnable())
		m_eState = FADE_OUT;
}

void CUI_Dynamic_Back::OnFadeOutTick(_float fTimeDelta)
{
	m_fAlphaRatio -= fTimeDelta;

	if (m_fAlphaRatio < 0.f)
		m_eState = FADE_STATE_END;
}

HRESULT CUI_Dynamic_Back::SetUp_ShaderResources()
{
	// For. Matrices
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));

	// For. Textures
	if (m_Textures.size() > 0)
	{
		FAILED_CHECK(m_Textures[m_iTextureIndex]->Bind_ShaderResources(m_pShaderCom, "g_Texture"));
	}

	if (m_AlphaTextures.size() > 0)
	{
		FAILED_CHECK(m_AlphaTextures[m_iTextureIndex]->Bind_ShaderResources(m_pShaderCom, "g_AlphaTexture"));
	}
	
	// For. RawValues
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fAlphaRatio", &m_fAlphaRatio, sizeof(m_fAlphaRatio)));

	return S_OK;
}

CUI_Dynamic_Back* CUI_Dynamic_Back::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Dynamic_Back* pInstance = New CUI_Dynamic_Back(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Dynamic_Back");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Dynamic_Back::Clone(void* pArg)
{
	CUI_Dynamic_Back* pInstance = New CUI_Dynamic_Back(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Dynamic_Back");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Dynamic_Back::Free(void)
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pDuration);
	}
}