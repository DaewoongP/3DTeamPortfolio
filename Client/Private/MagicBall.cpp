#include "MagicBall.h"
#include "GameInstance.h"
#include "MagicBallPool.h"
#include "Trail.h"

CMagicBall::CMagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMagicBall::CMagicBall(const CMagicBall& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CMagicBall::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;

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
	//실시간으로 갱신해줍니다.
	if(m_pTargetWorldMatrix!=nullptr)
		m_CurrentTargetMatrix = (*m_pTargetOffsetMatrix) * (*m_pTargetWorldMatrix);
	
	if(m_pWeaponWorldMatrix!=nullptr)
		m_CurrentWeaponMatrix = (*m_pWeaponOffsetMatrix) * (*m_pWeaponWorldMatrix);
	Tick_MagicBall_State(fTimeDelta);
	__super::Tick(fTimeDelta);
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
	//Set InitDesc to MagicBallDesc
	m_fLifeTime = InitDesc.fLifeTime;
	
	m_pTarget = InitDesc.pTarget;

	if (InitDesc.pTarget != nullptr)
	{
		m_pTargetWorldMatrix = InitDesc.pTarget->Get_Transform()->Get_WorldMatrixPtr();
		m_pTargetOffsetMatrix = InitDesc.pTarget->Get_Offset_MatrixPtr();
		m_CurrentTargetMatrix = (*m_pTargetOffsetMatrix) * (*m_pTargetWorldMatrix);
		m_vEndPosition = m_CurrentTargetMatrix.Translation();

	}
	else 
	{
		m_pTargetWorldMatrix = nullptr;
		m_pTargetOffsetMatrix = nullptr;
		m_CurrentTargetMatrix = XMMatrixIdentity();
		m_vEndPosition = _float3(0, 0, 0);
	}
	
	if (InitDesc.pWeapon != nullptr)
	{
		m_pWeaponWorldMatrix = InitDesc.pWeapon->Get_Transform()->Get_WorldMatrixPtr();
		m_pWeaponOffsetMatrix = InitDesc.pWeapon->Get_Offset_MatrixPtr();
		m_CurrentWeaponMatrix = (*m_pWeaponOffsetMatrix) * (*m_pWeaponWorldMatrix);
		m_vStartPosition = m_CurrentWeaponMatrix.Translation();
	}
	else 
	{
		m_pTargetWorldMatrix = nullptr;
		m_pTargetOffsetMatrix = nullptr;
		m_CurrentWeaponMatrix = XMMatrixIdentity();
		m_vStartPosition = _float3(0,0,0);
	}
	
	InitDesc.eCollisionFlag;
	
	m_pTransform->Set_Position(m_CurrentWeaponMatrix.Translation());

	//Set CollisionDesc
	m_CollisionDesc.eMagicGroup = InitDesc.eMagicGroup;
	m_CollisionDesc.eMagicType = InitDesc.eMagicType;
	m_CollisionDesc.eBuffType = InitDesc.eBuffType;
	m_CollisionDesc.eMagicTag = InitDesc.eMagicTag;
	m_CollisionDesc.iDamage = InitDesc.iDamage;

	Set_CollisionData(&m_CollisionDesc);
	m_eCollisionFlag = InitDesc.eCollisionFlag;

	m_pRigidBody->Set_CollisionFlag("Magic_Ball", m_eCollisionFlag);
	//상태 초기화
	Set_ObjEvent(OBJ_NONE);
	Set_MagicBallState(MAGICBALL_STATE_BEGIN);
	
	m_pRigidBody->Enable_Collision("Magic_Ball", this);
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
	m_fLerpAcc = 0.f;
	// 플레이어가 타겟을 보는 vector를 구함.
	_float3 vDir = XMVector3Normalize(m_vEndPosition - m_vStartPosition);
	// 임의의 축을 구함.
	_float3 tempAxis = _float3(1, 1, 1);
	// 외적
	_float3	normal = XMVector3Cross(vDir, tempAxis);

	//진행 경로만큼 뒤로 이동한 뒤
	m_vSplineLerp[0] = m_vStartPosition - vDir;
	//임의의 랜덤 값을 구하고
	_float fRandom = Random_Generator(-20.f, 20.f);
	// 외적 방향으로 튄다.
	m_vSplineLerp[0] += _float3(normal.x * fRandom, normal.y * fabsf(fRandom), normal.z * fRandom);

	//진행 경로만큼 뒤로 이동한 뒤
	m_vSplineLerp[1] = m_vStartPosition + vDir;
	//임의의 랜덤 값을 구하고
	fRandom = Random_Generator(-20.f, 20.f);
	// 외적 방향으로 튄다.
	m_vSplineLerp[1] += _float3(normal.x * fRandom, normal.y * fabsf(fRandom), normal.z * fRandom);
}

void CMagicBall::Ready_StraightMove(CTrail* pTrail)
{
	//타겟이 없으면?
	_float distance = 30.0f;
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
		m_vEndPosition = m_vStartPosition + vDirStartToPicked * distance;
	}

	m_fTimeScalePerDitance = distance / _float3(m_vEndPosition - m_vStartPosition).Length();
	pTrail->Reset_Trail(m_vStartPosition + _float3(0, 0.5f, 0), m_vStartPosition + _float3(0, -0.5f, 0));
	pTrail->Get_Transform()->Set_Position(m_vStartPosition);
}

void CMagicBall::Ready_SplineSpinMove(CTrail* pTrail,_float2 vSpinWeight, _float fSpinSpeed)
{
	Ready_SplineMove(pTrail);
	m_vSpinWeight = vSpinWeight;
	m_fSpinSpeed = fSpinSpeed;
}

void CMagicBall::Tick_MagicBall_State(_float fTimeDelta)
{
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
		Set_StartPosition();
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
		CMagicBallPool* pMagicBallPool = CMagicBallPool::GetInstance();
		Safe_AddRef(pMagicBallPool);
		pMagicBallPool->Return_Magic(this, m_CollisionDesc.eMagicTag);

		Safe_Release(pMagicBallPool);
		Set_ObjEvent(OBJ_DEAD);
		break;
	}
	}
}

void CMagicBall::Set_StartPosition()
{
	m_vStartPosition = m_CurrentWeaponMatrix.Translation();
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
	}
}
