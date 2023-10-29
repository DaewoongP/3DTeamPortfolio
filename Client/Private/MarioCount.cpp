#include "..\Public\MarioCount.h"
#include "GameInstance.h"
#include "Level.h"

CMarioCount::CMarioCount(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMarioCount::CMarioCount(const CMarioCount& rhs)
	: CGameObject(rhs)
{
}

void CMarioCount::Start()
{
	m_isStart = true;
}

_bool CMarioCount::Is_Finished()
{
	return m_isFinished;
}

void CMarioCount::Set_RankTexture(_uint iRank)
{
	m_iRank = iRank - 1;
	if (m_iRank > 7)
		m_iRank = 7;
	m_isRank = true;
	m_pTransform->Set_Scale(_float3(128.f, 128.f, 0.f));

	m_fRankTimeAcc = 0.f;
}

HRESULT CMarioCount::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	m_pTransform->Set_Scale(_float3(512.f, 512.f, 0.f));
	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));
	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 100.f);

	m_fTime = 1.f;
	m_fRankTime = 3.f;

	return S_OK;
}

void CMarioCount::Tick(_float fTimeDelta)
{
	if (true == m_isRank)
	{
		m_fRankTimeAcc += fTimeDelta;
		if (m_fRankTimeAcc > m_fRankTime)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Get_CurrentLevel()->Set_NextLevel(LEVEL_SMITH);
			Safe_Release(pGameInstance);

			m_isRank = false;
		}
	}
	

	if (true == m_isStart)
	{
		m_fTimeAcc += fTimeDelta;
	}
	
	__super::Tick(fTimeDelta);
}

void CMarioCount::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_isStart &&
		m_fTimeAcc > m_fTime)
	{
		m_fTimeAcc = 0.f;
		++m_iCountIndex;
	}

	if (6 == m_iCountIndex)
	{
		m_isFinished = true;
		m_isStart = false;
	}

	if (true == m_isStart &&
		6 > m_iCountIndex)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);

	if (true == m_isRank)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMarioCount::Render()
{
	FAILED_CHECK_RETURN(Setup_ShaderResources(), E_FAIL);

	FAILED_CHECK_RETURN(m_pShader->Begin("UI"), E_FAIL);

	FAILED_CHECK_RETURN(m_pBuffer->Render(), E_FAIL);

	return S_OK;
}

HRESULT CMarioCount::Add_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)), E_FAIL);

	/* For.Com_Shader */
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)), E_FAIL);

	/* For.Com_Buffer */
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer)), E_FAIL);

	// 0~4 -> 5,4,3,2,1 tex / 5 -> go tex
	m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Texture/MarioCount/MarioCount_%d.png"), 6);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);

	// 8개밖에 없음 ㅠ
	m_pFinishRankTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Texture/MarioRank/Rank_%d.png"), 8);
	NULL_CHECK_RETURN(m_pFinishRankTexture, E_FAIL);

	return S_OK;
}

HRESULT CMarioCount::Setup_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr()), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
	FAILED_CHECK_RETURN(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);

	if (true == m_isRank)
	{
		FAILED_CHECK_RETURN(m_pFinishRankTexture->Bind_ShaderResource(m_pShader, "g_Texture", m_iRank), E_FAIL);
	}
	else
	{
		FAILED_CHECK_RETURN(m_pTexture->Bind_ShaderResource(m_pShader, "g_Texture", m_iCountIndex), E_FAIL);
	}
	
	Safe_Release(pGameInstance);

	return S_OK;
}

CMarioCount* CMarioCount::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMarioCount* pInstance = New CMarioCount(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMarioCount");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMarioCount::Clone(void* pArg)
{
	CMarioCount* pInstance = New CMarioCount(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMarioCount");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMarioCount::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pFinishRankTexture);
}
