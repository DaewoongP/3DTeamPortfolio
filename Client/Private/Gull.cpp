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

	// 플레이어 찾기
	BEGININSTANCE;
	m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	Safe_AddRef(m_pPlayer);
	ENDINSTANCE;

	m_fRandTime = (rand() % 3) * 1.f;

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

	// 갈매기 인덱스 추출
	wstring wsGull(m_pTag);
	size_t findIndex = wsGull.find(TEXT("Gull_")) + 5;

	wstring wsModelIndex = wsGull.substr(findIndex);
	m_iGullIndex = stoi(wsModelIndex);

	// 플레이어가 다가가면 날아가는 갈매기
	if (10 > m_iGullIndex)
	{
		m_GullAnimIndex = GULL_IDLE2;
		m_pModel->Change_Animation((_uint)m_GullAnimIndex);
	}

	// 갈매기 v편대
	else
	{
		m_GullAnimIndex = GULL_FLY;
		m_pModel->Change_Animation((_uint)m_GullAnimIndex);
		m_vecOriginPos = m_pTransform->Get_Position();
	}	

	// 리지드 바디 초기화
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
		MSG_BOX("Failed CGull Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CGull::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fRandTimeAcc += fTimeDelta;

	// 플레이어가 다가가면 날아가는 갈매기
	if (10 > m_iGullIndex)
	{
		// 플레이어와 거리 비교
		_float3 vPlayerPos = m_pPlayer->Get_PlayerPos();
		_float3 vGullPos = m_pTransform->Get_Position();

		m_fDist_From_Player = sqrtf((vPlayerPos.x - vGullPos.x) * (vPlayerPos.x - vGullPos.x) +
			(vPlayerPos.y - vGullPos.y) * (vPlayerPos.y - vGullPos.y) +
			(vPlayerPos.z - vGullPos.z) * (vPlayerPos.z - vGullPos.z));

		Check_Dist_From_Player(fTimeDelta); // 플레이어 거리에 따른 애니메이션 변경
	}

	// 갈매기 v편대
	else
	{
		Fly_V_Squadron(fTimeDelta);
	}

	// 애니메이션이 변경되면 그때 애니메이션 삽입
	if (m_PreGullAnimIndex != m_GullAnimIndex)
	{
		m_PreGullAnimIndex = m_GullAnimIndex;
		m_pModel->Change_Animation(m_GullAnimIndex);
	}

	if (nullptr != m_pModel)
	{
		if (10 <= m_iGullIndex)
		{
			if (m_fRandTimeAcc >= m_fRandTime)
				m_pModel->Play_Animation(fTimeDelta,&m_SoundChannel, CModel::UPPERBODY, m_pTransform);
		}
		else
			m_pModel->Play_Animation(fTimeDelta,&m_SoundChannel, CModel::UPPERBODY, m_pTransform);
	}
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

	// 플레이어와 충돌했다면 채집 활성화
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = true;
}

void CGull::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// 플레이어와 충돌이 끝났다면 채집 비활성화
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = false;
}

HRESULT CGull::Render()
{
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

	// 비행 시작과 관련된 상태일 경우
	if (GULL_START_FLY == m_GullAnimIndex)
	{
		// 해당 애니메이션이 끝나면 바꿔버림
		if (true == m_pModel->Is_Finish_Animation())
		{
			m_GullAnimIndex = GULL_FLY;
		}
		return;
	}

	// 비행 중일 경우
	if (GULL_FLY == m_GullAnimIndex)
	{
		_float3 vLook = m_pTransform->Get_Look();
		_float3 vUp = m_pTransform->Get_Up() * -1.f;

		_float3 vDir = vLook + vUp;
		vDir += vLook;
		vDir += vLook;
		vDir += vLook;
		vDir.Normalize();

		m_pTransform->Move_Direction(vDir, fTimeDelta);
		m_fDeadTime += fTimeDelta;

		// 일정 시간이 지나면 사망처리
		if (10.f <= m_fDeadTime)
		{
			Set_ObjEvent(OBJ_DEAD);
		}
		return;
	}

	// 거리에 따른 애니메이션 처리
	// 일정 거리로 다가오면 경계
	if (5.f >= m_fDist_From_Player && false == m_isCol_with_Player)
	{
		m_GullAnimIndex = GULL_ALERT;
	}

	// 경계에서 더 다가오면 날아감
	else if (5.f >= m_fDist_From_Player && true == m_isCol_with_Player)
	{
		m_GullAnimIndex = GULL_START_FLY;
	}

	// 평상시
	else
	{
		m_GullAnimIndex = GULL_IDLE2;
	}
}

void CGull::Fly_V_Squadron(_float fTimeDelta)
{
	m_fFlyTime += fTimeDelta;

	m_pTransform->Go_Straight(fTimeDelta);

	if (20.f <= m_fFlyTime)
	{
		m_pTransform->Set_Position(m_vecOriginPos);
		m_fFlyTime = 0.f;
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
