#include "Diffindo.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CDiffindo::CDiffindo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CDiffindo::CDiffindo(const CDiffindo& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CDiffindo::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;

	//디핀도는 이펙트가 매쉬밖에 없는 느낌이더라..
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Trail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Diffindo/Diffindo.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Ball_Twinkle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Ball_Twinkle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Diffindo/Ball_Twinkle"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Cast_Twinkle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Cast_Twinkle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Diffindo/Cast_Twinkle"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Dust")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Dust")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Diffindo/Dust"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Move_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Move_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Diffindo/Move_Particle"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_MeshEffect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_MeshEffect")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Diffindo/Diffindo.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;


	return S_OK;
}

HRESULT CDiffindo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Player Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Player Add_Components");

		return E_FAIL;
	}
	m_pTransform->Set_Speed(30);
	return S_OK;
}

void CDiffindo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDiffindo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CDiffindo::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CDiffindo::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CDiffindo::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CDiffindo::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	return S_OK;
}

void CDiffindo::Ready_Begin()
{
	__super::Ready_Begin();
}

void CDiffindo::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CDiffindo::Ready_CastMagic()
{
	if (m_pTarget == nullptr)
	{
		BEGININSTANCE;
		_float4 vMouseOrigin, vMouseDirection;
		_float3 vMouseWorldPickPosition, vDirStartToPicked;
		//마우스 피킹지점 찾기
		if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
		{
			ENDINSTANCE;
			return;
		}
		ENDINSTANCE;

		//가상의 지점으로 레이를 쏨.
		vMouseWorldPickPosition = vMouseOrigin.xyz() + vMouseDirection.xyz() * 100;
		//방향을 구함
		vDirStartToPicked = (vMouseWorldPickPosition - m_vStartPosition);
		vDirStartToPicked.Normalize();
		//목표지점
		m_vEndPosition = m_vStartPosition + vDirStartToPicked * 30.f;
	}

	m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_Transform()->LookAt(m_vEndPosition);
	m_ParticleVec[EFFECT_STATE_MAIN][1]->Get_Transform()->LookAt(m_vEndPosition);

	m_fTimeScalePerDitance = 30.f / _float3(m_vEndPosition - m_vStartPosition).Length();
	m_pMeshEffect->Play(m_vStartPosition);
	m_pMeshEffect->Get_Transform()->LookAt(m_vEndPosition);
	__super::Ready_CastMagic();
}

void CDiffindo::Ready_Dying()
{
	__super::Ready_Dying();
}

void CDiffindo::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CDiffindo::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CDiffindo::Tick_CastMagic(_float fTimeDelta)
{
	//__super::Tick_CastMagic(fTimeDelta);
	m_fLerpAcc += fTimeDelta / m_fLifeTime;
	//애는 러프 안시키고 그쪽 방향으로 무지성 이동 시켜줄거임.
	
	m_pMeshEffect->Get_Transform()->Go_Straight(fTimeDelta);
	m_pTransform->Set_Position(m_pMeshEffect->Get_Transform()->Get_Position());
	m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Set_Position(m_pTransform->Get_Position());
	m_ParticleVec[EFFECT_STATE_MAIN][1]->Get_Transform()->Set_Position(m_pTransform->Get_Position());

	m_ParticleVec[EFFECT_STATE_MAIN][2]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	_float distance =m_pMeshEffect->Get_Transform()->Get_Speed()* m_fLerpAcc;

	if (distance>30)
	{
		Do_MagicBallState_To_Next();
	}
}

void CDiffindo::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}

HRESULT CDiffindo::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Diffindo_Trail"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Trail)");
		__debugbreak();
		return E_FAIL;
	}
	
	m_ParticleVec[EFFECT_STATE_MAIN].resize(4);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Diffindo_Ball_Twinkle"),
		TEXT("Com_Ball_Twinkle"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Ball_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Diffindo_Dust"),
		TEXT("Com_Wand_Dust"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Dust)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Diffindo_Cast_Twinkle"),
		TEXT("Com_Cast_Twinkle"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Cast_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Diffindo_Move_Particle"),
		TEXT("Com_Move_Particle"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Move_Particle)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Diffindo_MeshEffect"),
		TEXT("Com_Mesh_Effect"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_MeshEffect)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDiffindo::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxBoxGeometry BoxGeometry = PxBoxGeometry(2.f,0.1f,2.f);
	//PxSphereGeometry SphereGeometry = PxSphereGeometry(3.f);
	RigidBodyDesc.pGeometry = &BoxGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_MAGIC;
	RigidBodyDesc.eCollisionFlag = m_eCollisionFlag;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Magic_Ball");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}

	return S_OK;
}

CDiffindo* CDiffindo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CDiffindo* pInstance = New CDiffindo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CDiffindo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDiffindo::Clone(void* pArg)
{
	CDiffindo* pInstance = New CDiffindo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDiffindo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDiffindo::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		Safe_Release(m_pMeshEffect);
	}
}
