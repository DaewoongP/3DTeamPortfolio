#include "..\Public\LightStand.h"
#include "GameInstance.h"

#include "MagicBall.h"

CLightStand::CLightStand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CLightStand::CLightStand(const CLightStand& rhs)
	: CMapObject(rhs)
{
}

HRESULT CLightStand::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightStand::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CLightStand Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_LightStand_Fire"),
		TEXT("Com_Torch"), reinterpret_cast<CComponent**>(&m_pEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_LightStand_Fire)");
		__debugbreak();
		return E_FAIL;
	}

	m_pEffect->Stop();
	m_pEffect->Get_MainModuleRef().fSimulationSpeed = Random_Generator(0.5f, 1.0f);

	return S_OK;
}

HRESULT CLightStand::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CMapObject Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(2.f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_ENEMY;
	RigidBodyDesc.eCollisionFlag = COL_MAGIC;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Torch");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CMagicBall Add_Component : (Com_RigidBody)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CLightStand::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	BEGININSTANCE;
	if (true == m_isFireOn && false == pGameInstance->Is_SoundPlaying(m_iSound0))
	{
		if(false == pGameInstance->Is_SoundPlaying(m_iSound1))
			m_iSound1 = pGameInstance->Play_Sound(TEXT("Firing.wav"), 0.25f* pGameInstance->Get_SoundPower(m_pTransform->Get_Position()));
	}

	pGameInstance->Set_ChannelVolume(m_iSound1, 0.25f * pGameInstance->Get_SoundPower(m_pTransform->Get_Position()));
	
	ENDINSTANCE;
}

void CLightStand::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		if (nullptr != m_pRigidBody)
			m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CLightStand::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		if (eBuff == BUFF_CONFRINGO)
		{
			BEGININSTANCE;
			m_iSound0 = pGameInstance->Play_Sound(TEXT("FireOn.wav"), 0.5f);
			ENDINSTANCE;

			LightOn();
			m_isFireOn = true;
		}
	}
}

void CLightStand::LightOn()
{
	m_pEffect->Play(m_pTransform->Get_Position() + _float3(0, 2.5f, 0));

	// 자체 발광 추가
	CLight::LIGHTDESC		LightDescHork;
	ZeroMemory(&LightDescHork, sizeof LightDescHork);
	
	LightDescHork.eType = CLight::TYPE_POINT;
	LightDescHork.vPos = m_pTransform->Get_Position().TransCoord() + _float3(0, 2.5f, 0);
	LightDescHork.fRange = 5.f;
	
	LightDescHork.vDiffuse = _float4(1.f, (166.f / 255.f), (76.f / 255.f), 1.f);
	LightDescHork.vAmbient = LightDescHork.vDiffuse;
	LightDescHork.vSpecular = LightDescHork.vDiffuse;
	
	BEGININSTANCE;
	pGameInstance->Add_Light(LightDescHork, nullptr);
	ENDINSTANCE;
}

CLightStand* CLightStand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightStand* pInstance = New CLightStand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLightStand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightStand::Clone(void* pArg)
{
	CLightStand* pInstance = New CLightStand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLightStand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLightStand::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
	Safe_Release(m_pRigidBody);
}
