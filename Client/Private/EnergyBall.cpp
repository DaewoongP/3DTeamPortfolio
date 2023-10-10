#include "EnergyBall.h"

#include "GameInstance.h"

#include "MagicSlot.h"

CEnergyBall::CEnergyBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnergyBall::CEnergyBall(const CEnergyBall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnergyBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	BEGININSTANCE;
	pGameInstance->Find_And_Add_Model(m_pDevice, m_pContext, LEVEL_SANCTUM, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/SM_SpherePrimitiveRegularNormals_01.dat"));
	ENDINSTANCE;

	return S_OK;
}

HRESULT CEnergyBall::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CEnergyBall Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnergyBall::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CEnergyBall::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CEnergyBall::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}

	Safe_Release(pGameInstance);
}

HRESULT CEnergyBall::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("Mesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnergyBall::Render_Depth()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

void CEnergyBall::Set_Protego_Collision(CTransform* pTransform, CEnemy::ATTACKTYPE eType) const
{

}

HRESULT CEnergyBall::Make_Magics()
{
	//Basic Magic Protego
	{
		_uint iMagicType = rand() % 3 + 1; // 1 ~ 3(Yellow, Purple, Red)

		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_PROTEGO;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = (CMagic::MAGIC_TYPE)iMagicType;
		magicInitDesc.eMagicTag = PROTEGO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.fLifeTime = 60.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	return S_OK;
}

HRESULT CEnergyBall::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CEnergyBall Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CEnergyBall Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CEnergyBall Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	/* For.MagicSlot */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
		TEXT("Com_MagicSlot"), reinterpret_cast<CComponent**>(&m_pMagicSlot))))
	{
		MSG_BOX("Failed CEnergyBall Add_Component : (Com_MagicSlot)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnergyBall::Add_Components_Level(const _uint& iLevel)
{
	BEGININSTANCE;

	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iLevel, TEXT("Prototype_Component_Model_SM_SpherePrimitiveRegularNormals_01"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CEnergyBall Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CEnergyBall::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CEnergyBall::SetUp_ShadowShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_LightTransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_LightTransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

CEnergyBall* CEnergyBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnergyBall* pInstance = New CEnergyBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnergyBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnergyBall::Clone(void* pArg)
{
	CEnergyBall* pInstance = New CEnergyBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnergyBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnergyBall::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pMagicSlot);
		Safe_Release(m_pShader);
		Safe_Release(m_pShadowShader);
		Safe_Release(m_pModel);
		Safe_Release(m_pRenderer);
	}
}
