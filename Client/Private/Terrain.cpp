#include "..\Public\Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pBuffer->Culling(m_pTransform->Get_WorldMatrix());

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CTerrain::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin("Terrain");

	if (FAILED(m_pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Shader)");
		return E_FAIL;
	}

	/* Com_Buffer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBuffer))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Buffer)");
		return E_FAIL;
	}

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.vInitPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.5f;
	RigidBodyDesc.fDynamicFriction = 0.5f;
	RigidBodyDesc.fRestitution = 0.f;
	
	PxHeightFieldDesc HeightFieldDesc;
	HeightFieldDesc.format = PxHeightFieldFormat::eS16_TM;
	HeightFieldDesc.nbRows = m_pBuffer->Get_TerrainSizeX(); // 가로 개수
	HeightFieldDesc.nbColumns = m_pBuffer->Get_TerrainSizeZ(); // 세로 개수
	
	PxHeightFieldSample* pSample = New PxHeightFieldSample[sizeof(PxHeightFieldSample) * HeightFieldDesc.nbRows * HeightFieldDesc.nbColumns];
	const _float3* pPosArray = m_pBuffer->Get_PosArray();

	for (_uint i = 0; i < HeightFieldDesc.nbColumns; ++i)
	{
		for (_uint j = 0; j < HeightFieldDesc.nbRows; ++j)
		{
			_uint iIndex = j + HeightFieldDesc.nbRows * i;
			_uint iTest = i + HeightFieldDesc.nbColumns * j;
			pSample[iTest].height = pPosArray[iIndex].y;
		}
	}

	HeightFieldDesc.samples.data = pSample; // 버텍스 높이값
	HeightFieldDesc.samples.stride = sizeof(PxHeightFieldSample); // Stride
	HeightFieldDesc.flags = PxHeightFieldFlags();

	PxHeightField* pHeightField = PxCreateHeightField(HeightFieldDesc);//, pPhysX->getPhysicsInsertionCallback());
	Safe_Delete_Array(pSample);
	
	PxHeightFieldGeometry GeoMetry(pHeightField, PxMeshGeometryFlags(), 1.f, 1.f, 1.f);
	RigidBodyDesc.pGeometry = &GeoMetry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	RigidBodyDesc.vDebugColor = _float4(0.f, 1.f, 0.f, 1.f);
	RigidBodyDesc.pOwnerObject = this;

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
	{
		MSG_BOX("Failed CTerrain Add_Component : (Com_Texture)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_BrushTexture", 1)))
		return E_FAIL;

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = New CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = New CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pRenderer);
}
