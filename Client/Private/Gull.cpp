#include "..\Public\Gull.h"
#include "GameInstance.h"

#include "Player.h"

CGull::CGull(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CGull::CGull(const CGull& rhs)
	: CGameObject(rhs)
{
}


HRESULT CGull::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGull::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CGull Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);
	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(-7.5f);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGull::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CGatherer Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	m_GullAnimIndex = GULL_IDLE2;
	m_pModel->Change_Animation((_uint)m_GullAnimIndex);

	// ������ �ٵ� �ʱ�ȭ
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.fStaticFriction = 1.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry MyGeometry = PxSphereGeometry(3.f);
	RigidBodyDesc.pGeometry = &MyGeometry;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.vDebugColor = _float4(0.f, 1.f, 0.f, 1.f);
	RigidBodyDesc.eThisCollsion = COL_STATIC;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Gull");

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CDoor Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	// �÷��̾� ã��
	BEGININSTANCE;
	m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(iCurrentLevelIndex, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	Safe_AddRef(m_pPlayer);
	ENDINSTANCE;

	return S_OK;
}

void CGull::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// �÷��̾�� �Ÿ� ��
	_float3 vPlayerPos = m_pPlayer->Get_PlayerPos();
	_float3 vGullPos = m_pTransform->Get_Position();

	m_fDist_From_Player = sqrtf((vPlayerPos.x - vGullPos.x) * (vPlayerPos.x - vGullPos.x) +
		(vPlayerPos.y - vGullPos.y) * (vPlayerPos.y - vGullPos.y) +
		(vPlayerPos.z - vGullPos.z) * (vPlayerPos.z - vGullPos.z));

	Check_Dist_From_Player(fTimeDelta); // �÷��̾� �Ÿ��� ���� �ִϸ��̼� ����

	// �ִϸ��̼��� ����Ǹ� �׶� �ִϸ��̼� ����
	if (m_PreGullAnimIndex != m_GullAnimIndex)
	{
		m_PreGullAnimIndex = m_GullAnimIndex;
		m_pModel->Change_Animation(m_GullAnimIndex);
	}

	if (nullptr != m_pModel)
		m_pModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CGull::Late_Tick(_float fTimeDelta)
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

void CGull::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// �÷��̾�� �浹�ߴٸ� ä�� Ȱ��ȭ
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = true;
}

void CGull::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// �÷��̾�� �浹�� �����ٸ� ä�� ��Ȱ��ȭ
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = false;
}

HRESULT CGull::Render()
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

HRESULT CGull::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

HRESULT CGull::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CGull Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CGull Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CGull Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGull::SetUp_ShaderResources()
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

HRESULT CGull::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

void CGull::Check_Dist_From_Player(_float fTimeDelta)
{
	if (nullptr == m_pModel)
		return;

	// ���� ���۰� ���õ� ������ ���
	if (GULL_START_FLY == m_GullAnimIndex)
	{
		// �ش� �ִϸ��̼��� ������ �ٲ����
		if (true == m_pModel->Is_Finish_Animation())
		{
			m_GullAnimIndex = GULL_FLY;
		}
		return;
	}

	// ���� ���� ���
	if (GULL_FLY == m_GullAnimIndex)
	{
		m_pTransform->Go_Straight(fTimeDelta);
		m_fDeadTime += fTimeDelta;

		// ���� �ð��� ������ ���ó��
		if (10.f <= m_fDeadTime)
		{
			Set_ObjEvent(OBJ_DEAD);
		}
		return;
	}

	// �Ÿ��� ���� �ִϸ��̼� ó��
	// ���� �Ÿ��� �ٰ����� ���
	if (5.f >= m_fDist_From_Player && false == m_isCol_with_Player)
	{
		m_GullAnimIndex = GULL_ALERT;
	}

	// ��迡�� �� �ٰ����� ���ư�
	else if (5.f >= m_fDist_From_Player && true == m_isCol_with_Player)
	{
		m_GullAnimIndex = GULL_START_FLY;
	}

	// ����
	else
	{
		m_GullAnimIndex = GULL_IDLE2;
	}
}

CGull* CGull::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGull* pInstance = New CGull(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CGull");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGull::Clone(void* pArg)
{
	CGull* pInstance = New CGull(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGull::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
