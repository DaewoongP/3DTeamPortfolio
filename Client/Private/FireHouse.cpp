#include "..\Public\FireHouse.h"
#include "GameInstance.h"

CFireHouse::CFireHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CFireHouse::CFireHouse(const CFireHouse& rhs)
	: CMapObject(rhs)
{
}

HRESULT CFireHouse::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireHouse::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CFireHouse Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireHouse::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// 건물 종류 파악
	wstring wsTargetName(TEXT("Prototype_Component_Model_"));
	wstring wsModelName(m_ObjectDesc.wszTag);

	_uint iLength = wsTargetName.size();
	wsModelName = wsModelName.substr(iLength);

	wstring wsF(TEXT("SM_HM_Gen_F"));
	wstring wsG(TEXT("SM_HM_Gen_G"));
	wstring wsHoney(TEXT("SM_HM_Honeydukes"));
	wstring wsQuill(TEXT("SM_HM_Quill"));
	wstring wsTea(TEXT("SM_HM_TeaShop"));
	wstring wsDB(TEXT("SM_HM_DB_GR"));

	if (0 == lstrcmp(wsModelName.c_str(), wsF.c_str()))
	{
		m_eHouseType = HOUSE_GEN_F;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsG.c_str()))
	{
		m_eHouseType = HOUSE_GEN_G;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsHoney.c_str()))
	{
		m_eHouseType = HOUSE_HONEY;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsQuill.c_str()))
	{
		m_eHouseType = HOUSE_QUILL;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsTea.c_str()))
	{
		m_eHouseType = HOUSE_TEASHOP;
	}
	else if (0 == lstrcmp(wsModelName.c_str(), wsDB.c_str()))
	{
		m_eHouseType = HOUSE_DB_GR;
	}

	if (HOUSE_DB_GR != m_eHouseType)
	{
		vector<class CMesh*> Meshes = *m_pModel->Get_MeshesVec();
		//vector<_float3> FirePosition;
		for (auto& pMesh : Meshes)
		{
			vector<_float3> Positions = *pMesh->Get_VerticesPositionVec();
			for (_uint i = 0; i < MAX_FIRES; ++i)
			{
				_uint iRandIndex = _uint(Random_Generator(0.f, _float(Positions.size())));
				m_FirePosition.push_back(XMVector3TransformCoord(Positions[iRandIndex], m_pTransform->Get_WorldMatrix()));
			}
		}

		for (auto& vPosition : m_FirePosition)
		{
			CParticleSystem* pParticle = { nullptr };
			wstring wstrTag = TEXT("Com_Fire_Particle") + Generate_HashtagW();
			if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_House_FireEffect")
				, wstrTag.c_str(), (CComponent**)&pParticle)))
			{
				__debugbreak();
				return E_FAIL;
			}
			pParticle->Get_MainModuleRef().fSimulationSpeed = Random_Generator(0.1f, 1.5f);
			pParticle->Get_MainModuleRef().fStartSize = Random_Generator(1.f, 4.f);
			//pParticle->Play(vPosition);
			m_Particles.push_back(pParticle);

			wstrTag = TEXT("Com_Fire_Distortion") + Generate_HashtagW();
			if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_House_Fire_Distortion")
				, wstrTag.c_str(), (CComponent**)&pParticle)))
			{
				__debugbreak();
				return E_FAIL;
			}

			//pParticle->Play(vPosition);
			m_Particles.push_back(pParticle);

			wstrTag = TEXT("Com_Fire_Smoke") + Generate_HashtagW();
			if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_House_Fire_Smoke")
				, wstrTag.c_str(), (CComponent**)&pParticle)))
			{
				__debugbreak();
				return E_FAIL;
			}

			//pParticle->Play(vPosition);

			m_Particles.push_back(pParticle);
		}
	}

#pragma region 리지드 바디 생성
	BEGININSTANCE;
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
	RigidBodyDesc.eCollisionFlag = COL_ENEMY | COL_PLAYER | COL_NPC | COL_ITEM;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "MapObject");

	vector<CMesh*> ModelMeshes = *m_pModel->Get_MeshesVec();
	vector<_float3> Vertices;
	vector<PxU32> Indices;
	_uint iIndex = { 0 };

	m_vMaxPoint = _float3(-9999999.f, -9999999.f, -9999999.f);
	m_vMinPoint = _float3(9999999.f, 9999999.f, 9999999.f);

	for (auto& pMesh : ModelMeshes)
	{
		Vertices.clear();
		Indices.clear();

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

	ENDINSTANCE;
#pragma endregion 리지드 바디 생성

	return S_OK;
}

void CFireHouse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(true == m_isFireOn)
		FireOn();
}

void CFireHouse::Late_Tick(_float fTimeDelta)
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

HRESULT CFireHouse::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModel->Bind_Material(m_pShader, "g_NormalTexture", iMeshCount, NORMALS);

		m_pShader->Begin("Mesh_No_Cull");

		if (FAILED(m_pModel->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

void CFireHouse::FireOn()
{
	_uint iPosIndex = 0;

	for (auto& iter : m_Particles)
	{
		iter->Play(m_FirePosition[iPosIndex++]);
	}

	m_isFireOn = false;
}

CFireHouse* CFireHouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireHouse* pInstance = New CFireHouse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFireHouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFireHouse::Clone(void* pArg)
{
	CFireHouse* pInstance = New CFireHouse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFireHouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireHouse::Free()
{
	__super::Free();

	for (auto& pParticle : m_Particles)
	{
		Safe_Release(pParticle);
	}

	m_Particles.clear();
}
