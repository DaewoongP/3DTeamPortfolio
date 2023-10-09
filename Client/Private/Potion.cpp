#include "..\Public\Potion.h"
#include "GameInstance.h"
#include "CoolTime.h"
#include "Player.h"
#include "Player_Information.h"
#include "Transform.h"
#include "CustomModel.h"

CPotion::CPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPotion::CPotion(const CPotion& rhs)
	: CGameObject(rhs)
	, m_PotionCreateDesc(rhs.m_PotionCreateDesc)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CPotion::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;

	// 아이템 정보
	m_PotionCreateDesc.eModelType = CModel::TYPE_NONANIM; // 모델 타입

	_float4x4 ScaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	_float4x4 RotMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(-90.f),
		XMConvertToRadians(0.f),
		XMConvertToRadians(180.f)));
	m_PotionCreateDesc.PivotMatrix = ScaleMatrix * RotMatrix;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Find_And_Add_Model(m_pDevice, m_pContext, m_iLevel, m_PotionCreateDesc.eModelType
		, m_PotionCreateDesc.wstrModelPath.data(), m_PotionCreateDesc.PivotMatrix), E_FAIL);

	Safe_Release(pGameInstance);
	
	return S_OK;
}

HRESULT CPotion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);

	// 수명 
	m_pLifeTime = CCoolTime::Create(m_pDevice, m_pContext);
	m_pLifeTime->Set_MaxCoolTime(5.f);

	// 손에 붙어있는 시간
	m_pAttachedTime = CCoolTime::Create(m_pDevice, m_pContext);
	m_pAttachedTime->Set_MaxCoolTime(2.f);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 모든 레벨을 조사해서 플레이어 주소를 가져옴.
	for (_uint i = 0; i < LEVEL_END; ++i)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(
			i
			, TEXT("Layer_Player")
			, TEXT("GameObject_Player")));

		if (m_pPlayer != nullptr)
			break;
	}

	// 플레이어 주소에 대한 유효성 검사.
	if (nullptr == m_pPlayer)
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	// 필요한 플레이어의 컴포넌트들을 받아옴.
	m_pPlayerModel = static_cast<CCustomModel*>(m_pPlayer->Find_Component(TEXT("Com_Model_CustomModel_Player")));
	m_pPlayerTransform = m_pPlayer->Get_Transform();
	m_pPlayerInformation = static_cast<CPlayer_Information*>(m_pPlayer->Find_Component(TEXT("Com_Player_Information")));


	// 파츠 값들 설정
	const CBone* pBone = m_pPlayerModel->Get_Bone(TEXT("SKT_LeftHand"));
	if (nullptr == pBone)
		return E_FAIL;
	m_OffsetMatrix = pBone->Get_OffsetMatrix();
	m_PivotMatrix = m_pPlayerModel->Get_PivotFloat4x4();
	m_pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	m_pParentWorldMatrix = m_pPlayerTransform->Get_WorldMatrixPtr();

	m_pLifeTime->Play_CoolTime();
	m_pAttachedTime->Play_CoolTime();
	return S_OK;
}

void CPotion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pLifeTime->Tick(fTimeDelta);
	m_pAttachedTime->Tick(fTimeDelta);
	
	if (false == m_pLifeTime->IsEnable())
	{
		Set_ObjEvent(OBJ_EVENT::OBJ_DEAD);
	}
}

void CPotion::Late_Tick(_float fTimeDelta)
{
	// 파츠 로직
	if (true == m_pAttachedTime->IsEnable())
	{		
		_float4x4 BoneMatrix;

		/* 부모(뼈의 오프셋 * 뼈의 컴바인드 * 피벗) */
		BoneMatrix = m_OffsetMatrix * (*m_pCombindTransformationMatrix) * m_PivotMatrix;

		// 스케일 제거.
		BoneMatrix.Right(XMVector3Normalize(BoneMatrix.Right()));
		BoneMatrix.Up(XMVector3Normalize(BoneMatrix.Up()));
		BoneMatrix.Look(XMVector3Normalize(BoneMatrix.Look()));

		_float4x4 CombinedWorldMatrix;
		CombinedWorldMatrix = BoneMatrix * (*m_pParentWorldMatrix);
		m_pTransform->Set_WorldMatrix(CombinedWorldMatrix);
	}
	else
	{
		m_pRigidBody->Enable_Collision("Potion", this);
	}
	
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
		
}

HRESULT CPotion::Render()
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", i, DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Material(m_pShader, "g_NormalTexture", i, NORMALS)))
			return E_FAIL;

		if (FAILED(m_pShader->Begin("Mesh")))
			return E_FAIL;

		if (FAILED(m_pModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPotion::Render_Depth()
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

HRESULT CPotion::SetUp_ShadowShaderResources()
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

HRESULT CPotion::Set_ShaderResources()
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShader)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CPotion] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CPotion::Add_Components()
{
	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")
		, TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer)));
	
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, ToPrototypeTag(TEXT("Prototype_Component_Model"), m_PotionCreateDesc.wstrModelPath.data()).c_str()
		, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)));
	
	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh")
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader)));
	/* Com_ShadowShader */
	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader)));
	FAILED_CHECK(Add_RigidBody());

	return S_OK;
}

HRESULT CPotion::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = true;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.8f;
	PxCapsuleGeometry MyGeometry = PxCapsuleGeometry(0.05f, 0.1f);
	RigidBodyDesc.pGeometry = &MyGeometry;
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(90.f), 0.f);
	RigidBodyDesc.vOffsetPosition = _float3(0.f, -0.01f, 0.f);
	RigidBodyDesc.eThisCollsion = COL_ITEM;
	RigidBodyDesc.eCollisionFlag = COL_STATIC | COL_ITEM;
	RigidBodyDesc.pOwnerObject = this;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Potion");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		return E_FAIL;
	}

	m_pRigidBody->Disable_Collision("Potion");
	
	return S_OK;
}

void CPotion::Free(void)
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
		Safe_Release(m_pModel);
		Safe_Release(m_pShader);
		Safe_Release(m_pShadowShader);
		Safe_Release(m_pLifeTime);
		Safe_Release(m_pAttachedTime);

		Safe_Release(m_pRigidBody);
	}
}
