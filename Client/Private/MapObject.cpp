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

	m_ObjectDesc = *static_cast<MAPOBJECTDESC*>(pArg);
	m_pTransform->Set_WorldMatrix(m_ObjectDesc.WorldMatrix);

	// 음수 값이 있을 경우 Cull 모드를 None로 바꾼다.
	if (0.f >= m_pTransform->Get_Scale_With_Negative().x ||
		0.f >= m_pTransform->Get_Scale_With_Negative().y ||
		0.f >= m_pTransform->Get_Scale_With_Negative().z)
	{
		m_isCull = false;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
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
	RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 1.f, 1.f);
	RigidBodyDesc.eThisCollsion = COL_STATIC;
	RigidBodyDesc.eCollisionFlag = COL_ENEMY | COL_PLAYER;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "MapObject");

	vector<CMesh*> Meshes = *m_pModel->Get_MeshesVec();
	vector<_float3> Vertices;
	vector<PxU32> Indices;
	_uint iIndex = { 0 };

	m_vMaxPoint = _float3(-9999999.f, -9999999.f, -9999999.f);
	m_vMinPoint = _float3(9999999.f, 9999999.f, 9999999.f);

	for (auto& pMesh : Meshes)
	{
		vector<_float3> MeshVertices = *pMesh->Get_VerticesPositionVec();

		for (auto& MeshVetex : MeshVertices)
		{
			_float3 vWorldVertex = XMVector3TransformCoord(MeshVetex, m_ObjectDesc.WorldMatrix);
			Vertices.push_back(vWorldVertex);
			Check_MinMaxPoint(vWorldVertex);
		}

		vector<PxU32> MeshIndices = *pMesh->Get_IndicesVec();

		for (size_t i = 0; i < MeshIndices.size(); ++i)
		{
			Indices.push_back(MeshIndices[i] + iIndex);
		}

		iIndex += Vertices.size();
	}

	m_vCenterPoint = (m_vMaxPoint + m_vMinPoint) * 0.5f;
	m_fRadius = Vector3::Distance(m_vMaxPoint, m_vCenterPoint);

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

	Safe_Release(pGameInstance);

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CMapObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMapObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
		if (pGameInstance->isIn_WorldFrustum(m_vCenterPoint.TransCoord(), m_fRadius))
		{
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		}
		
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}

	Safe_Release(pGameInstance);
}

HRESULT CMapObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

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

HRESULT CMapObject::Render_Depth()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		if (FAILED(m_pShadowShader->Begin("Shadow")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapObject::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Renderer)");
		__debugbreak();
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

	return S_OK;
}

HRESULT CMapObject::SetUp_ShaderResources()
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

HRESULT CMapObject::SetUp_ShadowShaderResources()
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

void CMapObject::Check_MinMaxPoint(_float3 vPoint)
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
	Safe_Release(m_pShader);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
