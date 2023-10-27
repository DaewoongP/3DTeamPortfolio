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

void CBalloon_Coin::Reset(_float3 vPosition, COIN eCoinType)
{
	m_pTransform->Set_Position(vPosition + _float3(0.f, 1.f, 0.f));
	m_eCoinColor = eCoinType;
}

HRESULT CBalloon_Coin::Initialize(void* pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	COINDESC CoinDesc = *static_cast<COINDESC*>(pArg);

	m_eCoinColor = CoinDesc.eCoinColorType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Scale(_float3(CoinDesc.vScale.x, CoinDesc.vScale.y, 0.f));
	m_pTransform->Set_Position(CoinDesc.vPosition + _float3(0.f, 1.f, 0.f));
	m_fTime = 0.05f;

	return S_OK;
}

void CBalloon_Coin::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	__super::Tick(fTimeDelta);
}

void CBalloon_Coin::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBalloon_Coin::Render()
{
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	FAILED_CHECK_RETURN(m_pShader->Begin("Point"), E_FAIL);

	FAILED_CHECK_RETURN(m_pBuffer->Render(), E_FAIL);

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

	m_iCoinMaxIndex = 10;
	m_Textures.reserve(COIN_END);
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/BlackStarCoin_%d.png"), m_iCoinMaxIndex));
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/BronzeStarCoin_%d.png"), m_iCoinMaxIndex));
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/SilverStarCoin_%d.png"), m_iCoinMaxIndex));
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/GoldStarCoin_%d.png"), m_iCoinMaxIndex));
	m_Textures.push_back(CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Coin/RainbowStarCoin_%d.png"), m_iCoinMaxIndex));

	return S_OK;
}

HRESULT CBalloon_Coin::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()));
	FAILED_CHECK(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ)));

	FAILED_CHECK(m_pShader->Bind_RawValue("g_vCamPosition", pGameInstance->Get_CamPosition(), sizeof(_float4)));
	FAILED_CHECK(m_Textures[m_eCoinColor]->Bind_ShaderResource(m_pShader, "g_Texture", m_iCoinIndex));

	if (m_fTimeAcc > m_fTime)
	{
		m_fTimeAcc = 0.f;
		++m_iCoinIndex;
	}
		
	if (m_iCoinIndex >= m_iCoinMaxIndex)
		m_iCoinIndex = 0;

	Safe_Release(pGameInstance);

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