#include "UI_Font.h"
#include "GameInstance.h"

CUI_Font::CUI_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Font::CUI_Font(const CUI_Font& rhs)
	: CGameObject(rhs)
	, m_pBatch(rhs.m_pBatch)
	, m_pFont(rhs.m_pFont)
	, m_vPos(rhs.m_vPos)
	, m_vScale(rhs.m_vScale)
{
}

HRESULT CUI_Font::Initialize_Prototype(const _tchar* pFontFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pBatch = New SpriteBatch(m_pContext);

	m_pFont = New SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CUI_Font::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		FONTDESC* pDesc = (FONTDESC*)pArg;

		lstrcpy(m_pText, pDesc->m_pText);
		m_vPos = pDesc->m_vPos;
		m_vColor = pDesc->m_vColor;
		m_fRotation = pDesc->m_fRotation;
		m_vOrigin = pDesc->m_vOrigin;
		m_vScale = pDesc->m_vScale;
		
		if (nullptr != pDesc->m_pParent)
		{
			m_pParent = pDesc->m_pParent;
			_float2 vParentPos = m_pParent->Get_XY();
			//m_vPrePos = m_vPos - vParentPos;
			//m_vPos = m_vPrePos + vParentPos;
			m_vCombinedPos = m_vPos + vParentPos;

		}
	}

	_float4 textSize = m_pFont->MeasureString(m_pText);
	_float textWidth = XMVectorGetX(textSize) * m_vScale.x; // 텍스트의 너비
	_float textHeight = XMVectorGetY(textSize) * m_vScale.y; // 텍스트의 높이


	if (nullptr != m_pParent)
		m_vCombinedPos.x -= (textWidth / 2.0f); // 중앙 위치에서 왼쪽으로 텍스트의 반 너비만큼 이동
	else
		m_vPos.x -= (textWidth / 2.0f); // 중앙 위치에서 왼쪽으로 텍스트의 반 너비만큼 이동

//	m_vPos.y -= (textHeight / 2.0f); // 중앙 위치에서 위로 텍스트의 반 높이만큼 이동

	return S_OK;
}

void CUI_Font::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Font::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	BEGININSTANCE;

	if (pGameInstance->Is_Playing_CutScene())
	{
		ENDINSTANCE;
		return;
	}

	ENDINSTANCE;

	if (true)
	{

	}


	if (nullptr != m_pParent)
	{
		_float2 vParentPos = m_pParent->Get_XY();
		//m_vPos = m_vPrePos + vParentPos;
		m_vCombinedPos = m_vPos + vParentPos;
	}
	else
		Set_vPos(m_vPos);

	Set_vScale(m_vScale);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Font::Render()
{
	if (nullptr == m_pFont ||
		nullptr == m_pBatch)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();
	
	if (nullptr != m_pParent)
		m_pFont->DrawString(m_pBatch, m_pText, m_vCombinedPos, m_vColor, m_fRotation, m_vOrigin, m_vScale);
	else
		m_pFont->DrawString(m_pBatch, m_pText, m_vPos, m_vColor, m_fRotation, m_vOrigin, m_vScale);

	m_pBatch->End();

	return S_OK;
}

void CUI_Font::Left_Align()
{
	_float4 textSize = m_pFont->MeasureString(m_pText);
	_float textWidth = XMVectorGetX(textSize); // 텍스트의 너비
	_float textHeight = XMVectorGetY(textSize); // 텍스트의 높이

	m_vOrigin.x = textWidth / 2.f;
	m_vOrigin.y = textHeight / 2.f;
}

void CUI_Font::Set_Text(wstring wText)
{
	lstrcpy(m_pText, wText.c_str());
}

HRESULT CUI_Font::Add_Component()
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

_bool CUI_Font::Is_In_Rect()
{
	_bool		isIn = false;

	POINT		ptMouse;
	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	RECT		rcUI = RECT();

	//SetRect(&rcUI, _int(m_vCombinedXY.x - m_fSizeX * 0.5f), _int(m_vCombinedXY.y - m_fSizeY * 0.5f), _int(m_vCombinedXY.x + m_fSizeX * 0.5f), _int(m_vCombinedXY.y + m_fSizeY * 0.5f));

	isIn = PtInRect(&rcUI, ptMouse);

	return isIn;
}

CUI_Font* CUI_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CUI_Font* pInstance = New CUI_Font(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFontFilePath)))
	{
		MSG_BOX("Failed to Created CFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Font::Clone(void* pArg)
{
	CUI_Font* pInstance = New CUI_Font(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Font::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pFont);
	}

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pParent);
}