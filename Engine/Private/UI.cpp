#include "UI.h"
#include "GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}

void CUI::Set_Parent(CUI* pUIParent)
{
	m_pParent = pUIParent;
	Safe_AddRef(m_pParent);
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

	Make_CombinedXY();
}

void CUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CUI::Change_Position(_float fX, _float fY, _float fZ, _uint iWinSizeX, _uint iWinSizeY)
{
	m_pTransform->Set_Position(
		XMVectorSet(fX - iWinSizeX * 0.5f, -fY + iWinSizeY * 0.5f, fZ, 1.f));

	return S_OK;
}

HRESULT CUI::Change_Scale(_float fX, _float fY)
{
	m_pTransform->Set_Scale(_float3(fX, fY, 1.f));

	return S_OK;
}

HRESULT CUI::Load(UIDESC UIDesc)
{
	m_vCombinedXY = UIDesc.vCombinedXY;
	m_fX = UIDesc.fX;
	m_fY = UIDesc.fY;
	m_fZ = UIDesc.fZ;
	m_fSizeX = UIDesc.fSizeX;
	m_fSizeY = UIDesc.fSizeY;

	Change_Scale(m_fSizeX, m_fSizeY);
	Make_CombinedXY();
	
	if (lstrcmp(TEXT(""), UIDesc.szTexturePath))
		m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, UIDesc.szTexturePath));
	if (lstrcmp(TEXT(""), UIDesc.szAlphaTexturePath))
		m_AlphaTextures.push_back(CTexture::Create(m_pDevice, m_pContext, UIDesc.szAlphaTexturePath));
	
	return S_OK;
}

void CUI::Make_Matrix(_float fWinSizeX, _float fWinSizeY)
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_float(fWinSizeX), _float(fWinSizeY), 0.f, 100.f));
}

void CUI::Make_CombinedXY()
{
	if (m_pParent == nullptr)
	{
		m_vCombinedXY.x = m_fX;
		m_vCombinedXY.y = m_fY;
	}
	else
	{
		_float2 vParentPos = m_pParent->Get_XY();

		m_vCombinedXY.x = vParentPos.x + m_fX;
		m_vCombinedXY.y = vParentPos.y + m_fY;
	}

	Change_Position(m_vCombinedXY.x, m_vCombinedXY.y, m_fZ);
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pParent);

	for (auto& pTexture : m_Textures)
	{
		Safe_Release(pTexture);
	}
	m_Textures.clear();

	for (auto& pAlphaTexture : m_AlphaTextures)
	{
		Safe_Release(pAlphaTexture);
	}
	m_AlphaTextures.clear();
}