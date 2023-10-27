#include "..\Public\Balloon_Coin.h"
#include "GameInstance.h"

CBalloon_Coin::CBalloon_Coin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBalloon_Coin::CBalloon_Coin(const CBalloon_Coin& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalloon_Coin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBalloon_Coin::Tick(_float fTimeDelta)
{
}

void CBalloon_Coin::Late_Tick(_float fTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBalloon_Coin::Render()
{
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

HRESULT CBalloon_Coin::Add_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)));

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)));

	_uint iNum = 1;
	/* For.Com_Buffer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer), &iNum));

	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/BlackStarCoin_%d.png"), 10));
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/BronzeStarCoin_%d.png"), 10));
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/SilverStarCoin_%d.png"), 10));
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/GoldStarCoin_%d.png"), 10));
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/RainbowStarCoin_%d.png"), 10));

	return S_OK;
}

HRESULT CBalloon_Coin::SetUp_ShaderResources()
{
	return S_OK;
}

CBalloon_Coin* CBalloon_Coin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBalloon_Coin* pInstance = New CBalloon_Coin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBalloon_Coin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBalloon_Coin::Clone(void* pArg)
{
	CBalloon_Coin* pInstance = New CBalloon_Coin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBalloon_Coin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBalloon_Coin::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pRenderer);

	for (auto& pTexture : m_Textures)
	{
		Safe_Release(pTexture);
	}

	m_Textures.clear();
}