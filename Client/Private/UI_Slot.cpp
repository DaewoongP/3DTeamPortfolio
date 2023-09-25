#include "UI_Slot.h"
#include "GameInstance.h"

CUI_Slot::CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Slot::CUI_Slot(const CUI_Slot& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Slot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_Textures.resize(SLOT_END);
		m_AlphaTextures.resize(SLOT_END);


		UIDESC* pDesc = (UIDESC*)pArg;

		m_vCombinedXY = pDesc->vCombinedXY;
		m_fX = pDesc->fX;
		m_fY = pDesc->fY;
		m_fZ = pDesc->fZ;
		m_fSizeX = pDesc->fSizeX;
		m_fSizeY = pDesc->fSizeY;
		lstrcpy(m_wszTexturePath, pDesc->szTexturePath);
		//lstrcpy(m_wszAlphaTexturePath, pDesc->szAlphaTexturePath);
		m_Textures[BACK] = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/UI/UI_T_ButtonBack.png"));
		m_Textures[FRAME] = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/UI/FX/UI_T_Goldleaf_Large.png"));

		if (lstrcmp(TEXT(""), m_wszTexturePath))
		{
			m_Textures[ICON] = CTexture::Create(m_pDevice, m_pContext, m_wszTexturePath);
			if (nullptr == m_Textures[BACK] || nullptr == m_Textures[ICON] || nullptr == m_Textures[FRAME])
			{
				MSG_BOX("Failed Create UI Slot Texture");
				__debugbreak;
				return E_FAIL;
			}
		}
		else
		{
			m_Textures[ICON] = nullptr;
		}

		m_AlphaTextures[ICON] = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/UI/Icons/MenuAssets/UI_T_DropShadow.png"));
		m_AlphaTextures[FRAME] = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/UI/FX/Masks/UI_T_IconButtonOutlineMaskGold.png"));
		if (nullptr == m_AlphaTextures[ICON] || nullptr == m_AlphaTextures[FRAME])
		{
			MSG_BOX("Failed Create UI Slot Texture");
			__debugbreak;
			return E_FAIL;
		}
		
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	Make_Matrix(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CUI_Slot::Tick(_float fTimeDelta)
{
	if (nullptr == m_Textures[ICON])
		return;

	__super::Tick(fTimeDelta);
}

void CUI_Slot::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_Textures[ICON])
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_Slot::Render()
{
	for (size_t i = 0; i < SLOT_END; i++)
	{

		if (i == FRAME)
		{
			if (FAILED(SetUp_ShaderResources(i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin("UI")))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
		else
		{
			if (FAILED(SetUp_ShaderResources(i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin("UI")))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}

		// m_iTextureIndex++;
	}

	// m_iTextureIndex = 0;
	return S_OK;
}


HRESULT CUI_Slot::Add_Components()
{
	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		MSG_BOX("Failed CUI_Slot Add_Component : (Com_Shader)");
		__debugbreak;
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		MSG_BOX("Failed CUI_Slot Add_Component : (Com_Renderer)");
		__debugbreak;
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		MSG_BOX("Failed CUI_Slot Add_Component : (Com_Buffer)");
		__debugbreak;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Slot::SetUp_ShaderResources(_uint iIndex)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_Textures[iIndex]->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (nullptr != m_AlphaTextures[FRAME] && iIndex == _uint(FRAME))
	{
		if (FAILED(m_AlphaTextures[FRAME]->Bind_ShaderResources(m_pShaderCom, "g_AlphaTexture")))
			return E_FAIL;
	}

	if (nullptr != m_AlphaTextures[ICON] && iIndex == _uint(ICON))
	{
		if (FAILED(m_AlphaTextures[ICON]->Bind_ShaderResources(m_pShaderCom, "g_AlphaTexture")))
			return E_FAIL;
	}

	_bool isCollision = m_pButtonCom->Collision_Rect(g_hWnd, m_vCombinedXY, _float2(m_fSizeX, m_fSizeY));
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isOnCollision", &isCollision, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isClicked", &m_isClicked, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}


_bool CUI_Slot::Get_Clicked()
{
	return 	m_pButtonCom->Click(g_hWnd, m_vCombinedXY, _float2(m_fSizeX, m_fSizeY));
}

void CUI_Slot::Set_Clicked(_bool isClicked)
{
	m_isClicked = isClicked;
}

void CUI_Slot::Set_IconTexture(CTexture* pTexture)
{
	m_Textures[ICON] = pTexture;
}

CUI_Slot* CUI_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Slot* pInstance = New CUI_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUI_Progress");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Slot::Clone(void* pArg)
{
	CUI_Slot* pInstance = New CUI_Slot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUI_Progress");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pButtonCom);
}