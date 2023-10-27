#include "..\Public\MapObject_Ins.h"
#include "GameInstance.h"

CMapObject_Ins::CMapObject_Ins(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapObject_Ins::CMapObject_Ins(const CMapObject_Ins& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMapObject_Ins::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject_Ins::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CMapObject_Ins Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ObjectDesc = *static_cast<MAPOJBECTINSDESC*>(pArg);

	if (FAILED(Add_Components()))
		return E_FAIL;

	_float3	vPos = _float3(m_ObjectDesc.WorldMatrix._41, m_ObjectDesc.WorldMatrix._42, m_ObjectDesc.WorldMatrix._43);

	m_pTransform->Set_Position(vPos);

	m_vEmissive = _float4(0.f, 0.f, 0.f, 0.f);

	return S_OK;
}

HRESULT CMapObject_Ins::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject_Ins Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 리지드 바디 초기화
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.fStaticFriction = 1.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.vDebugColor = _float4(0.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.eThisCollsion = COL_STATIC;
	RigidBodyDesc.eCollisionFlag = COL_ITEM;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Instance");

	vector<CMesh_Instance*> Meshes = m_pModel->Get_Meshes();
	vector<_float3> Vertices;
	vector<PxU32> Indices;
	_float4x4 WorldMatrix = m_pTransform->Get_WorldMatrix();

	for (auto& pMesh : Meshes)
	{
		vector<_float3> MeshVertices = pMesh->Get_VertexPositions();
		vector<_float4x4> InstanceMatries = pMesh->Get_InstanceMatrices();

		_uint iIndex = { 0 };
		for (auto& InstanceMatrix : InstanceMatries)
		{
			Vertices.clear();
			Indices.clear();

			for (auto& MeshVetex : MeshVertices)
			{
				_float3 vInstanceVertex = XMVector3TransformCoord(MeshVetex, InstanceMatrix);
				_float3 vWorldVertex = XMVector3TransformCoord(vInstanceVertex, WorldMatrix);
				Vertices.push_back(vWorldVertex);
			}

			vector<PxU32> MeshIndices = pMesh->Get_Indices();

			for (size_t i = 0; i < MeshIndices.size(); ++i)
			{
				Indices.push_back(MeshIndices[i]);
			}

			// 피직스 메쉬 생성
			PxTriangleMeshDesc TriangleMeshDesc;
			TriangleMeshDesc.points.count = Vertices.size();
			TriangleMeshDesc.points.stride = sizeof(_float3);
			TriangleMeshDesc.points.data = Vertices.data();

			TriangleMeshDesc.triangles.count = Indices.size() / 3;
			TriangleMeshDesc.triangles.stride = 3 * sizeof(PxU32);
			TriangleMeshDesc.triangles.data = Indices.data();

			PxTolerancesScale PxScale;
			PxCookingParams PxParams(PxScale);
			PxDefaultMemoryOutputStream DefaultWriteBuffer;
			if (!PxCookTriangleMesh(PxParams, TriangleMeshDesc, DefaultWriteBuffer))
			{
				MSG_BOX("Failed Create Triangle Mesh");
				return E_FAIL;
			}

			PxPhysics* pPhysX = pGameInstance->Get_Physics();

			PxDefaultMemoryInputData DefaultReadBuffer(DefaultWriteBuffer.getData(), DefaultWriteBuffer.getSize());
			PxTriangleMeshGeometry TriangleMeshGeoMetry = PxTriangleMeshGeometry(pPhysX->createTriangleMesh(DefaultReadBuffer));
			RigidBodyDesc.pGeometry = &TriangleMeshGeoMetry;
			wstring randstr = TEXT("Com_RigidBody") + Generate_HashtagW();
			/* Com_RigidBody */
			CComponent* pRigidBody = { nullptr };
			if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
				randstr.c_str(), reinterpret_cast<CComponent**>(&pRigidBody), &RigidBodyDesc)))
			{
				MSG_BOX("Failed CMapObject Add_Component : (Com_RigidBody)");
				__debugbreak();
				return E_FAIL;
			}

			m_RigidBodys.push_back(pRigidBody);
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMapObject_Ins::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMapObject_Ins::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);

#ifdef _DEBUG
		for (auto& pRigidBody : m_RigidBodys)
		{
			m_pRenderer->Add_DebugGroup(pRigidBody);
		}
#endif // _DEBUG
	}
}

HRESULT CMapObject_Ins::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();
	vector<CMesh_Instance*> Meshes = m_pModel->Get_Meshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		if (wcswcs(Meshes[iMeshCount]->Get_MeshName(), TEXT("Glass")) && true == g_isNight)
		{
			m_vEmissive = _float4(0.5f, 0.5f, 0.45f, 0.5f);
			if (FAILED(m_pShader->Bind_RawValue("g_vBloom", &m_vEmissive, sizeof(_float4))))
				return E_FAIL;
		}
		else if (wcswcs(Meshes[iMeshCount]->Get_MeshName(), TEXT("SUB_GEN_A_EXT_LOD1_Alphas")) && true == g_isNight)
		{
			m_vEmissive = _float4(0.5f, 0.5f, 0.45f, 0.5f);
			if (FAILED(m_pShader->Bind_RawValue("g_vBloom", &m_vEmissive, sizeof(_float4))))
				return E_FAIL;
		}
		else
		{
			m_vEmissive = _float4(0.f, 0.f, 0.f, 0.f);
			if (FAILED(m_pShader->Bind_RawValue("g_vBloom", &m_vEmissive, sizeof(_float4))))
				return E_FAIL;
		}

		m_pShader->Begin("Mesh_No_Cull");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapObject_Ins::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_Material(m_pShadowShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);

		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapObject_Ins::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMapObject_Ins Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CMapObject_Ins Add_Component : (Com_Shader)");
		__debugbreak();
		return E_FAIL;
	}
	
	/* Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowMeshInstance"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		MSG_BOX("Failed CMapObject_Ins Add_Component : (Com_ShadowShader)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapObject_Ins::SetUp_ShaderResources()
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

HRESULT CMapObject_Ins::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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

CMapObject_Ins* CMapObject_Ins::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject_Ins* pInstance = New CMapObject_Ins(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMapObject_Ins");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject_Ins::Clone(void* pArg)
{
	CMapObject_Ins* pInstance = New CMapObject_Ins(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMapObject_Ins");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapObject_Ins::Free()
{
	__super::Free();

	for (auto& pRigidBody : m_RigidBodys)
		Safe_Release(pRigidBody);
	m_RigidBodys.clear();

	Safe_Release(m_pShadowShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
