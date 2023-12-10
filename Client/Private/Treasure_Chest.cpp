#include "..\Public\Treasure_Chest.h"
#include "GameInstance.h"

#include "Player.h"
#include "Player_Information.h"
#include "Inventory.h"

#include "UI_Interaction.h"

CTreasure_Chest::CTreasure_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTreasure_Chest::CTreasure_Chest(const CTreasure_Chest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTreasure_Chest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasure_Chest::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CTreasure_Chest Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ObjectDesc = *reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);

	if (FAILED(Add_Components()))
		return E_FAIL;

	// 플레이어 찾기
	BEGININSTANCE;
	m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	m_pPlayerInformation = m_pPlayer->Get_Player_Information();
	Safe_AddRef(m_pPlayer);
	Safe_AddRef(m_pPlayerInformation);
	ENDINSTANCE;

	if (FAILED(CComposite::Add_Component(LEVEL_SMITH, TEXT("Prototype_Component_Particle_ChestLight"),
		TEXT("Com_Effect0"), reinterpret_cast<CComponent**>(&m_pEffect0))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_Component_Particle_ChestLight)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_SMITH, TEXT("Prototype_Component_Particle_ChestParticle"),
		TEXT("Com_Effect1"), reinterpret_cast<CComponent**>(&m_pEffect1))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_Component_Particle_ChestParticle)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_SMITH, TEXT("Prototype_Component_Particle_ChestLight_ME"),
		TEXT("Com_Effect2"), reinterpret_cast<CComponent**>(&m_pEffect2))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_Component_Particle_ChestLight_ME)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTreasure_Chest::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CTreasure_Chest Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// 리지드 바디 초기화
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.fStaticFriction = 1.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry MyGeometry = PxSphereGeometry(2.f);
	RigidBodyDesc.pGeometry = &MyGeometry;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.vDebugColor = _float4(0.f, 1.f, 1.f, 1.f);
	RigidBodyDesc.eThisCollsion = COL_STATIC;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Gatherer");

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CDoor Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CTreasure_Chest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	// 일정 거리안으로 들어왔을 때
	if (true == m_isCol_with_Player && nullptr != m_pModel && true == m_isGetItem)
	{
		// 버튼 UI가
		m_pUI_Interaction->Tick(fTimeDelta);

		BEGININSTANCE;  // 버튼을 누르면 동작(한번만)
		if (pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN) && true == m_isGetItem)
		{
			m_isGetItem = false;

			// 소리와 이펙트
			pGameInstance->Play_Sound(TEXT("ChestOpen.wav"), 1.f);
			m_pEffect0->Play(m_pTransform->Get_Position());
			m_pEffect1->Play(m_pTransform->Get_Position());
			m_pEffect2->Play(m_pTransform->Get_Position());

			// 인벤토리 획득 처리
			m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_LACEWING_FLIES);
			m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_KNOTGRASS);
			m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_MONGREL_FUR);
			m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_SPIDER_FANG);
			m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_DITTANY_LEAVES);
			m_pPlayerInformation->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_FLUXWEED_STEM);
		}

		ENDINSTANCE;
	}

	// 닫혀있는 상태
	if (true == m_isGetItem)
	{
		m_pModel->Play_Animation(0.f, &m_SoundChannel, CModel::UPPERBODY, m_pTransform);
	}

	// 열리는 상태
	else
	{
		m_pModel->Play_Animation(fTimeDelta,&m_SoundChannel, CModel::UPPERBODY, m_pTransform);
	}
}

void CTreasure_Chest::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	BEGININSTANCE; 

	if (true == m_isCol_with_Player && nullptr != m_pModel && true == m_isGetItem)
	{
		m_pUI_Interaction->Late_Tick(fTimeDelta);
	}
	
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

void CTreasure_Chest::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// 플레이어와 충돌했다면 수집 활성화
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = true;
}

void CTreasure_Chest::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wsCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wsPlayer(TEXT("Player_Default"));

	// 플레이어와 충돌이 끝났다면 수집 비활성화
	if (0 == lstrcmp(wsCollisionTag.c_str(), wsPlayer.c_str()))
		m_isCol_with_Player = false;
}

HRESULT CTreasure_Chest::Render()
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

HRESULT CTreasure_Chest::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

HRESULT CTreasure_Chest::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTreasure_Chest Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CTreasure_Chest Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CTreasure_Chest Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_UI_Interaction */
	BEGININSTANCE;

	CUI_Interaction::INTERACTIONDESC pDesc;
	lstrcpy(pDesc.m_wszName, TEXT("보물상자"));
	lstrcpy(pDesc.m_wszFunc, TEXT("열기"));
	m_UIMatirx = m_pTransform->Get_WorldMatrix();
	m_UIMatirx._42 += 1.25f;
	pDesc.m_WorldMatrix = &m_UIMatirx;

	m_pUI_Interaction = static_cast<CUI_Interaction*>(pGameInstance->Clone_Component(LEVEL_STATIC,
		TEXT("Prototype_GameObject_UI_Interaction"), &pDesc));

	ENDINSTANCE;

	return S_OK;
}

HRESULT CTreasure_Chest::SetUp_ShaderResources()
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

HRESULT CTreasure_Chest::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

void CTreasure_Chest::Check_MinMaxPoint(_float3 vPoint)
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

CTreasure_Chest* CTreasure_Chest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTreasure_Chest* pInstance = New CTreasure_Chest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTreasure_Chest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreasure_Chest::Clone(void* pArg)
{
	CTreasure_Chest* pInstance = New CTreasure_Chest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTreasure_Chest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTreasure_Chest::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerInformation);

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pEffect0);
	Safe_Release(m_pEffect1);
	Safe_Release(m_pEffect2);

	Safe_Release(m_pUI_Interaction);
}
