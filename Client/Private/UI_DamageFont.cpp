#include "UI_DamageFont.h"
#include "GameInstance.h"

CUI_DamageFont::CUI_DamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_DamageFont::CUI_DamageFont(const CUI_DamageFont& rhs)
	: CGameObject(rhs)
	, m_pBatch(rhs.m_pBatch)
	, m_pFont(rhs.m_pFont)
{
}

HRESULT CUI_DamageFont::Initialize_Prototype(const _tchar* pFontFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pBatch = New SpriteBatch(m_pContext);

	m_pFont = New SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CUI_DamageFont::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		DAMAGEFONTDESC* pDesc = (DAMAGEFONTDESC*)pArg;

		lstrcpy(m_pText, pDesc->m_pText);
		m_vWorldPos = pDesc->m_vWorldPos;
		m_fWorldY = m_vWorldPos.y;

		m_vColor = pDesc->m_vColor;
		m_fRotation = pDesc->m_fRotation;
		m_vOrigin = pDesc->m_vOrigin;
		m_vMinScale = pDesc->m_vMinScale;
		m_vMaxScale = pDesc->m_vMaxScale;
		m_vScale = m_vMinScale;

		m_fLifeTime = pDesc->m_fLifeTime;
		m_fMoveSpeed = pDesc->m_fMoveSpeed;
		m_fAlphaSpeed = pDesc->m_fAlphaSpeed;
	}

	_float4 textSize = m_pFont->MeasureString(m_pText);
	_float textWidth = XMVectorGetX(textSize) * ((m_vMinScale.x + m_vMaxScale.x) / 2); // 텍스트의 너비
	_float textHeight = XMVectorGetY(textSize) * ((m_vMinScale.y + m_vMaxScale.y) / 2); // 텍스트의 높이

	return S_OK;
}

void CUI_DamageFont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_DamageFont::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Set_vPos(m_vPos);
	Set_vScale(m_vScale);

	Play_DamageFont(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CUI_DamageFont::Render()
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

void CUI_DamageFont::Left_Align()
{
	_float4 textSize = m_pFont->MeasureString(m_pText);
	_float textWidth = XMVectorGetX(textSize); // 텍스트의 너비
	_float textHeight = XMVectorGetY(textSize); // 텍스트의 높이

	m_vOrigin.x = textWidth / 2.f;
	m_vOrigin.y = textHeight / 2.f;
}

void CUI_DamageFont::Set_Text(wstring wText)
{
	lstrcpy(m_pText, wText.c_str());
}

HRESULT CUI_DamageFont::Add_Component()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CDummy_UI Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

void CUI_DamageFont::Play_DamageFont(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;
	if (m_fLifeTime  > m_fCurrentTime)
	{
		m_fWorldY += fTimeDelta * m_fMoveSpeed;
		m_vColor.w += fTimeDelta * m_fAlphaSpeed;
		m_vWorldPos.y = m_fWorldY;
		m_vPos = Caculate_Projection(m_vWorldPos);
	}
	else
	{
		m_isLife = false;
	}
}

_float2 CUI_DamageFont::Caculate_Projection(_float3 vPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float3 vOwnerPos = vPos;

	vOwnerPos.y = vOwnerPos.y + 0.5f;
	const _float4x4* viewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	const _float4x4* projMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	_float4x4 worldToProjMatrix = XMMatrixIdentity();
	worldToProjMatrix = (*viewMatrix) * (*projMatrix);

	_float3 ScreenPosition = XMVector3TransformCoord(vOwnerPos, worldToProjMatrix);

	if (ScreenPosition.x > 1.f ||
		ScreenPosition.x < -1.f ||
		ScreenPosition.y > 1.f ||
		ScreenPosition.y < -1.f ||
		ScreenPosition.z > 1.f ||
		ScreenPosition.z < 0.f)
	{
		Safe_Release(pGameInstance);
		return _float2(999.f, 999.f);
	}

	_float2 vViewPortSize = pGameInstance->Get_ViewPortSize(m_pContext);
	Safe_Release(pGameInstance);

	return _float2((ScreenPosition.x + 1.f) * (vViewPortSize.x / 2.f), 
		(-ScreenPosition.y + 1.f) * (vViewPortSize.y / 2.f));
}


CUI_DamageFont* CUI_DamageFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CUI_DamageFont* pInstance = New CUI_DamageFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFontFilePath)))
	{
		MSG_BOX("Failed to Created CFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_DamageFont::Clone(void* pArg)
{
	CUI_DamageFont* pInstance = New CUI_DamageFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DamageFont::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pFont);
	}

	Safe_Release(m_pRendererCom);
}
