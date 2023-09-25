#include "MagicBall.h"
#include "GameInstance.h"
#include "Weapon_Player_Wand.h"

CMagicBall::CMagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMagicBall::CMagicBall(const CMagicBall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMagicBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMagicBall::Initialize(void* pArg)
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

	MAGICBALLINITDESC* initDesc = static_cast<MAGICBALLINITDESC*>(pArg);

	m_MagicBallDesc.eMagicGroup = initDesc->eMagicGroup;;
	m_MagicBallDesc.eMagicType = initDesc->eMagicType;
	m_MagicBallDesc.eBuffType = initDesc->eBuffType;
	m_MagicBallDesc.eMagicTag = initDesc->eMagicTag;
	m_MagicBallDesc.fDamage = initDesc->fDamage;
	m_MagicBallDesc.fDistance = initDesc->fDistance;
	m_MagicBallDesc.fInitLifeTime = initDesc->fLifeTime;
	m_TargetOffsetMatrix = initDesc->TargetOffsetMatrix;
	m_pWeaponMatrix = initDesc->pWeaponMatrix;
	m_WeaponOffsetMatrix = initDesc->WeaponOffsetMatrix;

	m_pTarget = initDesc->pTarget;
	Safe_AddRef(m_pTarget);

	m_MagicBallDesc.fLifeTime = m_MagicBallDesc.fInitLifeTime;
	m_pTransform->Set_Position(m_MagicBallDesc.vStartPosition);

	m_MagicBallDesc.vStartPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	
	m_CollisionDesc.eMagicGroup = m_MagicBallDesc.eMagicGroup;
	m_CollisionDesc.eMagicType = m_MagicBallDesc.eMagicType;
	m_CollisionDesc.eBuffType = m_MagicBallDesc.eBuffType;
	m_CollisionDesc.eMagicTag = m_MagicBallDesc.eMagicTag;
	m_CollisionDesc.fDamage = m_MagicBallDesc.fDamage;

	Set_CollisionData(&m_CollisionDesc);

	m_pTransform->Set_RigidBody(m_pRigidBody);

	return S_OK;
}

void CMagicBall::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	//시간이 흐름.
	if (m_MagicBallDesc.fLifeTime > 0)
		m_MagicBallDesc.fLifeTime -= fTimeDelta;

	//매직볼 상태에 따른 처리
	switch (m_eMagicBallState)
	{
		case Client::CMagicBall::MAGICBALL_STATE_BEGIN:
		{
			if (m_isFirstFrameInState)
			{
				Ready_Begin();
				m_isFirstFrameInState = false;
			}
				
			Tick_Begin(fTimeDelta);
			break;
		}
		
		case Client::CMagicBall::MAGICBALL_STATE_DRAWTRAIL:
		{
			if (m_isFirstFrameInState)
			{
				Ready_DrawMagic();
				m_isFirstFrameInState = false;
			}
				
			Tick_DrawMagic(fTimeDelta);
			break;
		}
		
		case Client::CMagicBall::MAGICBALL_STATE_CASTMAGIC:
		{
			if (m_isFirstFrameInState)
			{
				Ready_CastMagic();
				m_isFirstFrameInState = false;
			}
				
			Tick_CastMagic(fTimeDelta);
			break;
		}
		
		case Client::CMagicBall::MAGICBALL_STATE_DYING:
		{
			if (m_isFirstFrameInState)
			{
				Ready_Dying();
				m_isFirstFrameInState = false;
			}
				
			Tick_Dying(fTimeDelta);
			break;
		}
		
		case Client::CMagicBall::MAGICBALL_STATE_END:
		{
			//cout << "마법 죽어요" << endl;
			Set_ObjEvent(OBJ_DEAD);
			break;
		}
	}
}

void CMagicBall::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	if (m_pRigidBody != nullptr)
	{
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
	}
#endif // _DEBUG
}

void CMagicBall::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//cout << "Player Enter" << endl;
}

void CMagicBall::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	//cout << "stay" << endl;
}

void CMagicBall::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	//cout << "Exit" << endl;
}

HRESULT CMagicBall::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMagicBall Add_Component : (Com_Renderer)");
		return E_FAIL;
	}

	if (FAILED(Add_RigidBody()))
	{
		MSG_BOX("Failed CMagicBall SettingRigidBody : (Com_RigidBody)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMagicBall::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(0.2f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_MAGIC;
	RigidBodyDesc.eCollisionFlag = COL_ENEMY | COL_PLAYER | COL_NPC;
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

void CMagicBall::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pRenderer);
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pTarget);
	}
}
