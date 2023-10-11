#include "..\Public\Cliff_Gate.h"
#include "GameInstance.h"

CCliff_Gate::CCliff_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCliff_Gate::CCliff_Gate(const CCliff_Gate& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCliff_Gate::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCliff_Gate::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CCliff_Gate Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCliff_Gate::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CCliff_Gate Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	m_pModel->Set_CurrentAnimIndex(0);
	m_pModel->Get_Animation(0)->Set_Loop(false);

	// 리지드 바디 초기화
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.fStaticFriction = 1.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry MyGeometry = PxSphereGeometry(8.f);
	RigidBodyDesc.pGeometry = &MyGeometry;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.vDebugColor = _float4(0.f, 1.f, 0.f, 1.f);
	RigidBodyDesc.eThisCollsion = COL_STATIC;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Cliff_Gate");

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CCliff_Gate Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CCliff_Gate::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	BEGININSTANCE;

	// 충돌중인 상태에서 특정 키(레파로)를 누르면 애니메이션 동작
	if (pGameInstance->Get_DIKeyState(DIK_R, CInput_Device::KEY_DOWN) && true == m_isCol_with_Player)
	{
		m_isCheckOnce = false;
	}

	ENDINSTANCE;

	if(false == m_isCheckOnce)
		m_pModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
	else
		m_pModel->Play_Animation(0, CModel::UPPERBODY, m_pTransform);
}

void CCliff_Gate::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	BEGININSTANCE;

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}

	ENDINSTANCE;
}

void CCliff_Gate::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// 플레이어와 충돌했다면 활성화
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = true;
}

void CCliff_Gate::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// 플레이어와 충돌이 끝났다면 비활성화
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = false;
}

HRESULT CCliff_Gate::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iMeshCount);

		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("AnimMesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCliff_Gate::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

HRESULT CCliff_Gate::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CCliff_Gate Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CCliff_Gate Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CCliff_Gate Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCliff_Gate::SetUp_ShaderResources()
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

	ENDINSTANCE;

	return S_OK;
}

HRESULT CCliff_Gate::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

void CCliff_Gate::Check_MinMaxPoint(_float3 vPoint)
{
	if (m_vMinPoint.x > vPoint.x)
		m_vMinPoint.x = vPoint.x;
	if (m_vMinPoint.y > vPoint.y)
		m_vMinPoint.y = vPoint.y;
	if (m_vMinPoint.z > vPoint.z)
		m_vMinPoint.z = vPoint.z;

	if (m_vMaxPoint.x < vPoint.x)
		m_vMaxPoint.x = vPoint.x;
	if (m_vMaxPoint.y < vPoint.y)
		m_vMaxPoint.y = vPoint.y;
	if (m_vMaxPoint.z < vPoint.z)
		m_vMaxPoint.z = vPoint.z;
}

CCliff_Gate* CCliff_Gate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCliff_Gate* pInstance = New CCliff_Gate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCliff_Gate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCliff_Gate::Clone(void* pArg)
{
	CCliff_Gate* pInstance = New CCliff_Gate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCliff_Gate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCliff_Gate::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
