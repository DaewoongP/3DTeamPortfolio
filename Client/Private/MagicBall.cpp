#include "MagicBall.h"
#include "GameInstance.h"

#include "MagicBallPool.h"

CMagicBall::CMagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMagicBall::CMagicBall(const CMagicBall& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
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
				m_pRigidBody->Disable_Collision("Magic_Ball");
				m_isFirstFrameInState = false;
			}

			Tick_Dying(fTimeDelta);
			break;
		}
		
		case Client::CMagicBall::MAGICBALL_STATE_END:
		{
			cout << "마법 죽어요" << endl;
			CMagicBallPool* pMagicBallPool = CMagicBallPool::GetInstance();
			Safe_AddRef(pMagicBallPool);
			pMagicBallPool->Return_Magic(this, m_MagicBallDesc.eMagicTag);

			Safe_Release(pMagicBallPool);
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
}

void CMagicBall::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
}

void CMagicBall::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
}

HRESULT CMagicBall::Reset(MAGICBALLINITDESC& InitDesc)
{
	m_MagicBallDesc.eMagicGroup = InitDesc.eMagicGroup;
	m_MagicBallDesc.eMagicType = InitDesc.eMagicType;
	m_MagicBallDesc.eBuffType = InitDesc.eBuffType;
	m_MagicBallDesc.eMagicTag = InitDesc.eMagicTag;
	m_MagicBallDesc.iDamage = InitDesc.iDamage;
	m_MagicBallDesc.fDistance = InitDesc.fDistance;
	m_MagicBallDesc.fInitLifeTime = InitDesc.fLifeTime;
	m_TargetOffsetMatrix = InitDesc.TargetOffsetMatrix;
	m_pWeaponMatrix = InitDesc.pWeaponMatrix;
	m_WeaponOffsetMatrix = InitDesc.WeaponOffsetMatrix;

	Safe_Release(m_pTarget);
	m_pTarget = InitDesc.pTarget;
	Safe_AddRef(m_pTarget);

	m_MagicBallDesc.fLifeTime = m_MagicBallDesc.fInitLifeTime;
	m_pTransform->Set_Position(m_MagicBallDesc.vStartPosition);

	m_MagicBallDesc.vStartPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();

	m_CollisionDesc.eMagicGroup = m_MagicBallDesc.eMagicGroup;
	m_CollisionDesc.eMagicType = m_MagicBallDesc.eMagicType;
	m_CollisionDesc.eBuffType = m_MagicBallDesc.eBuffType;
	m_CollisionDesc.eMagicTag = m_MagicBallDesc.eMagicTag;
	m_CollisionDesc.iDamage = m_MagicBallDesc.iDamage;

	Set_CollisionData(&m_CollisionDesc);
	m_eCollisionFlag = InitDesc.eCollisionFlag;

	m_pRigidBody->Set_CollisionFlag("Magic_Ball", m_eCollisionFlag);
	//상태 초기화
	Set_ObjEvent(OBJ_NONE);
	m_eMagicBallState = MAGICBALL_STATE_BEGIN;
	
	m_pRigidBody->Enable_Collision("Magic_Ball");
	m_pRigidBody->Set_ThisCollision(this);
	m_pTransform->Set_WorldMatrix(XMMatrixIdentity());

	return S_OK;
}

void CMagicBall::Ready_SpinMove(CTrail* pTrail,_float2 vSpinWeight, _float fSpinSpeed)
{
	Ready_StraightMove(pTrail);
	m_vSpinWeight = vSpinWeight;
	m_fSpinSpeed = fSpinSpeed;
}

void CMagicBall::Ready_SplineMove(CTrail* pTrail)
{
	Ready_StraightMove(pTrail);

	//Ready for Spline Lerp
	m_vStartPostion = m_MagicBallDesc.vStartPosition;
	m_fLerpAcc = 0.f;
	// 플레이어가 타겟을 보는 vector를 구함.
	_float3 vDir = XMVector3Normalize(m_vTargetPosition - m_vStartPostion);
	// 임의의 축을 구함.
	_float3 tempAxis = _float3(1, 1, 1);
	// 외적
	_float3	normal = XMVector3Cross(vDir, tempAxis);

	//진행 경로만큼 뒤로 이동한 뒤
	m_vSplineLerp[0] = m_vStartPostion - vDir;
	//임의의 랜덤 값을 구하고
	_float fRandom = Random_Generator(-20.f, 20.f);
	// 외적 방향으로 튄다.
	m_vSplineLerp[0] += _float3(normal.x * fRandom, normal.y * fabsf(fRandom), normal.z * fRandom);

	//진행 경로만큼 뒤로 이동한 뒤
	m_vSplineLerp[1] = m_vStartPostion + vDir;
	//임의의 랜덤 값을 구하고
	fRandom = Random_Generator(-20.f, 20.f);
	// 외적 방향으로 튄다.
	m_vSplineLerp[1] += _float3(normal.x * fRandom, normal.y * fabsf(fRandom), normal.z * fRandom);
}

void CMagicBall::Ready_StraightMove(CTrail* pTrail)
{
	//타겟이 있다면
	if (m_pTarget == nullptr)
	{
		BEGININSTANCE;
		_float4 vMouseOrigin, vMouseDirection;
		_float3 vMouseWorldPickPosition, vDirStartToPicked;
		//마우스 피킹지점 찾기
		if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
		{
			Safe_Release(pGameInstance);
			ENDINSTANCE;
			return;
		}
		ENDINSTANCE;

		vMouseWorldPickPosition = vMouseOrigin.xyz() + vMouseDirection.xyz() * 100;
		vDirStartToPicked = (vMouseWorldPickPosition - m_MagicBallDesc.vStartPosition);
		vDirStartToPicked.Normalize();
		m_vTargetPosition = m_MagicBallDesc.vStartPosition + vDirStartToPicked * m_MagicBallDesc.fDistance;
	}
	else
	{
		m_vTargetPosition = m_pTarget->Get_Position() + m_TargetOffsetMatrix.Translation();
	}
	// 설정 거리/실제 거리
	m_vStartPostion = m_MagicBallDesc.vStartPosition;
	m_fTimeScalePerDitance = m_MagicBallDesc.fDistance / _float3(m_vTargetPosition - m_vStartPostion).Length();
	pTrail->Reset_Trail(_float3(m_vStartPostion) + _float3(0, 0.5f, 0), _float3(m_vStartPostion) + _float3(0, -0.5f, 0));
	pTrail->Get_Transform()->Set_Position(m_vStartPostion);
}

void CMagicBall::Ready_SplineSpinMove(CTrail* pTrail,_float2 vSpinWeight, _float fSpinSpeed)
{
	Ready_SplineMove(pTrail);
	m_vSpinWeight = vSpinWeight;
	m_fSpinSpeed = fSpinSpeed;
}

HRESULT CMagicBall::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		MSG_BOX("Failed CMagicBall Add_Component : (Com_Renderer)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(Add_RigidBody()))
	{
		MSG_BOX("Failed CMagicBall SettingRigidBody : (Com_RigidBody)");
		__debugbreak();
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
	RigidBodyDesc.eCollisionFlag = m_eCollisionFlag;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Magic_Ball");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		__debugbreak();
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
