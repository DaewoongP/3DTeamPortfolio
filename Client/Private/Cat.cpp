#include "..\Public\Cat.h"
#include "GameInstance.h"

CCat::CCat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCat::CCat(const CCat& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCat::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCat::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CCat Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<CATDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);

	return S_OK;
}

HRESULT CCat::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CCat Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	m_eCatAnimIndex = (CAT_ANIMATION)(m_ObjectDesc.iAnimIndex / 2);
	if (6 <= m_ObjectDesc.iAnimIndex)
		m_eCatAnimIndex = CAT_TURN;

	m_pModel->Change_Animation((_uint)m_eCatAnimIndex);

	if (CAT_TURN == m_eCatAnimIndex)
		m_pModel->Get_Animation(m_eCatAnimIndex)->Set_Loop(false);

	return S_OK;
}

void CCat::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Cat_Move(fTimeDelta);

	// 애니메이션이 변경되면 그때 애니메이션 삽입
	if (m_ePreCatAnimIndex != m_eCatAnimIndex)
	{
		m_ePreCatAnimIndex = m_eCatAnimIndex;
		m_pModel->Change_Animation(m_eCatAnimIndex);
	}

	if (nullptr != m_pModel)
		m_pModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CCat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CCat::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iMeshCount);

		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		// 몸통만
		if (0 == iMeshCount)
		{
			if(0 != m_iRandCatTexture)
				m_pCatTexture->Bind_ShaderResources(m_pShader, "g_DiffuseTexture_Cat");

			m_pShader->Begin("AnimMesh_Cat");
		}

		else
			m_pShader->Begin("AnimMesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCat::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShadowShader, "g_BoneMatrices", iMeshCount);

		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCat::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CCat Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}


	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CCat Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CCat Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	m_iRandCatTexture = rand() % 5;

	// 확률에 따라 랜덤한 색의 고양이 텍스처를 적용한다.
	if (1 == m_iRandCatTexture)
	{
		/* Com_Texture1 */
		if (FAILED(CComposite::Add_Component(LEVEL_SMITH, TEXT("Prototype_Component_Texture_Cat_Turk"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pCatTexture))))
		{
			MSG_BOX("Failed CCat Add_Component : (Com_Texture)");
			__debugbreak();
			return E_FAIL;
		}
	}

	else if (2 == m_iRandCatTexture)
	{
		/* Com_Texture */
		if (FAILED(CComposite::Add_Component(LEVEL_SMITH, TEXT("Prototype_Component_Texture_Cat_Spot"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pCatTexture))))
		{
			MSG_BOX("Failed CCat Add_Component : (Com_Texture)");
			__debugbreak();
			return E_FAIL;
		}
	}	

	else if (3 == m_iRandCatTexture)
	{
		/* Com_Texture */
		if (FAILED(CComposite::Add_Component(LEVEL_SMITH, TEXT("Prototype_Component_Texture_Cat_Calico"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pCatTexture))))
		{
			MSG_BOX("Failed CCat Add_Component : (Com_Texture)");
			__debugbreak();
			return E_FAIL;
		}
	}

	else if (4 == m_iRandCatTexture)
	{
		/* Com_Texture */
		if (FAILED(CComposite::Add_Component(LEVEL_SMITH, TEXT("Prototype_Component_Texture_Cat_Orange"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pCatTexture))))
		{
			MSG_BOX("Failed CCat Add_Component : (Com_Texture)");
			__debugbreak();
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CCat::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_iCatNum", &m_iRandCatTexture, sizeof(_int))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CCat::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

void CCat::Cat_Move(_float fTimeDelta)
{
	// 고양이가 같은 곳을 계속 돌아다니는 부분
	if (CAT_TURN == m_eCatAnimIndex)
	{
		if(m_pModel->Is_Finish_Animation())
			m_eCatAnimIndex = CAT_WALK;
	}

	else if (CAT_WALK == m_eCatAnimIndex)
	{
		m_fWalkTime += fTimeDelta;
		if (7.5f <= m_fWalkTime)
		{
			m_eCatAnimIndex = CAT_TURN;
			m_fWalkTime = 0.f;
		}			
	}		
}

CCat* CCat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCat* pInstance = New CCat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCat::Clone(void* pArg)
{
	CCat* pInstance = New CCat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCat::Free()
{
	__super::Free();

	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCatTexture);
	Safe_Release(m_pRenderer);
}
