#include "..\Public\MapObject.h"
#include "GameInstance.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapObject::CMapObject(const CMapObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CMapObject Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MAPOBJECTDESC* pMapObjectDesc = reinterpret_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix((*pMapObjectDesc).WorldMatrix);

	if (nullptr == pMapObjectDesc)
	{
		MSG_BOX("Object Desc is NULL");
		return E_FAIL;
	}

	if (FAILED(Add_Components(pMapObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CMapObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMapObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CMapObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; iMeshCount++)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);

		m_pShader->Begin("Mesh");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapObject::Add_Components(MAPOBJECTDESC* pMapObjectDesc)
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Model */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, pMapObjectDesc->wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 리지드 바디 초기화
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.fStaticFriction = 0.5f;
	RigidBodyDesc.fDynamicFriction = 0.5f;
	RigidBodyDesc.fRestitution = 0.f;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vInitRotation = m_pTransform->Get_Quaternion();

	vector<CMesh*> Meshes = *m_pModel->Get_MeshesVec();
	vector<_float3> Vertices;
	vector<_ushort> Indices;
	for (auto& pMesh : Meshes)
	{
		vector<_float3> MeshVertices = *pMesh->Get_VerticesPositionVec();
		// 버텍스 벡터에 벡터를 삽입하는 함수
		Vertices.insert(Vertices.end(), MeshVertices.begin(), MeshVertices.end());

		vector<_uint> MeshIndices = *pMesh->Get_IndicesVec();
		Indices.insert(Indices.end(), MeshIndices.begin(), MeshIndices.end());
	}
	
	// 피직스 메쉬 생성
	PxTriangleMeshDesc TriangleMeshDesc;
	TriangleMeshDesc.points.count = Vertices.size();
	TriangleMeshDesc.points.stride = sizeof(_float3);
	TriangleMeshDesc.points.data = Vertices.data();

	TriangleMeshDesc.triangles.count = Indices.size() / 3;
	TriangleMeshDesc.triangles.stride = 3 * sizeof(_ushort);
	TriangleMeshDesc.triangles.data = Indices.data();
	TriangleMeshDesc.flags = PxMeshFlag::e16_BIT_INDICES;

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

	Safe_Release(pGameInstance);
	
	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapObject::SetUp_ShaderResources()
{
	BEGININSTANCE; if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL; ENDINSTANCE;

	return S_OK;
}

CMapObject* CMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject* pInstance = New CMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject::Clone(void* pArg)
{
	CMapObject* pInstance = New CMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMapObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
