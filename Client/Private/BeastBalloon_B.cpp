#include "..\Public\BeastBalloon_B.h"

#include "GameInstance.h"

CBeastBalloon_B::CBeastBalloon_B(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBeastBalloon_B::CBeastBalloon_B(const CBeastBalloon_B& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBeastBalloon_B::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	NULL_CHECK_RETURN(pArg, E_FAIL);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	BALLOONINITDESC* pInitDesc = static_cast<BALLOONINITDESC*>(pArg);

	m_iScore = pInitDesc->iScore;
	m_pTransform->Set_Position(pInitDesc->vPosition);
	m_pTransform->Set_Scale(_float3(3.f, 3.f, 3.f));

	return S_OK;
}

HRESULT CBeastBalloon_B::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CBeastBalloon_B::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (true == m_isDead)
	{
		m_eObjEvent = OBJ_DEAD;
		return;
	}

	m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
}

void CBeastBalloon_B::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CBeastBalloon_B::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	// 부딪 힌 대상의 정보를 가져와서 
	SCOREDESC ScoreDesc;
	ScoreDesc.Score = m_iScore;
	ScoreDesc.wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wcout << ScoreDesc.wstrObjectTag << endl;
	//매니저에게 전달한 후 

	// 내가 부딪혔다는 사실을 갱신한다.
	m_isDead = true;
}

HRESULT CBeastBalloon_B::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (FAILED(m_pEmissiveTexture->Bind_ShaderResource(m_pShader, "g_EmissiveTexture")))
		return E_FAIL;

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModelCom->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModelCom->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("Mesh_Emissive");

		if (FAILED(m_pModelCom->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBeastBalloon_B::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBeastBalloon_B::Add_Components()
{
	/* For.EmissiveTexture */
	m_pEmissiveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/NonAnims/Balloon/T_HM_Zonkos_Balloon_05_E.dds"));
	if (nullptr == m_pEmissiveTexture)
		throw TEXT("EmissiveTexture");

	/* Com_RigidBody */
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry pSphereGeomatry1 = PxSphereGeometry(1.5f);
	RigidBodyDesc.pGeometry = &pSphereGeomatry1;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(0.f, 0.f, 1.f, 1.f);
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_STATIC;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_ENEMY;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Body");
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("CLoadBeastBalloon_B Failed Clone Component : Com_RigidBody");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("CLoadBeastBalloon_B Failed Clone Component : Com_Renderer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBeastBalloon_B::Add_Components_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_Balloon"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		MSG_BOX("CLoadBeastBalloon_B Failed Clone Component : Com_Model");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBeastBalloon_B::SetUp_ShaderResources()
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

HRESULT CBeastBalloon_B::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

CBeastBalloon_B* CBeastBalloon_B::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBeastBalloon_B* pInstance = New CBeastBalloon_B(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBeastBalloon_B");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBeastBalloon_B::Clone(void* pArg)
{
	CBeastBalloon_B* pInstance = New CBeastBalloon_B(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBeastBalloon_B");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastBalloon_B::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShader);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pRigidBody);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pEmissiveTexture);
}