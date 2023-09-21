#include "Confringo.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CConfringo::CConfringo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CConfringo::CConfringo(const CConfringo& rhs)
	: CMagicBall(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CConfringo::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireBallDir"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/FireBallB"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CConfringo::Initialize(void* pArg)
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

	if (FAILED(Add_Effect()))
	{
		MSG_BOX("Failed Player Add_Effect");

		return E_FAIL;
	}

	if (m_pTarget == nullptr)
	{
		//마우스 피킹 지점으로 발사
		BEGININSTANCE;
		_float4 vMouseOrigin, vMouseDirection;
		_float3 vMouseWorldPickPosition, vDirStartToPicked;
		if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
		{
			Safe_Release(pGameInstance);
			return false;
		}
		ENDINSTANCE;

		vMouseWorldPickPosition = vMouseOrigin.xyz() + vMouseDirection.xyz() * 10000;
		vDirStartToPicked = (vMouseWorldPickPosition - m_MagicBallDesc.vStartPosition);
		vDirStartToPicked.Normalize();
		m_vTargetPosition = vDirStartToPicked * m_MagicBallDesc.fDistance;
	}
	else 
	{
		m_vTargetPosition = m_pTarget->Get_Position() + m_TargetOffsetMatrix.Translation();
	}

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
	m_fTimeScalePerDitance = m_MagicBallDesc.fDistance / _float3(m_vTargetPosition - m_vStartPostion).Length();

	m_pTrail->Reset_Trail(_float3(m_vStartPostion) + _float3(0,0.5f,0), _float3(m_vStartPostion)+ _float3(0, -0.5f, 0));
	m_pTrail->Get_Transform()->Set_Position(m_vStartPostion);
	//m_pTrail->Set_Pass("Confringo");
	return S_OK;
}

void CConfringo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	//사망처리 걸리면 재생
	if (m_isExplosiveTrigger)
	{
		//파티클 끝나면 없애줘.
		if (!m_pExplosiveEffect[0]->IsEnable()&&
			!m_pExplosiveEffect[1]->IsEnable())
			Set_ObjEvent(OBJ_DEAD);
		return;
	}
	//이동이 끝나면 true를 리턴해줄거임.
	if (m_fLerpAcc == 1)
	{
		//터지는 효과임.
		if (!m_isExplosiveTrigger)
		{
			m_pExplosiveEffect[0]->Get_Transform()->Set_Position(m_pTrail->Get_Transform()->Get_Position());
			m_pExplosiveEffect[1]->Get_Transform()->Set_Position(m_pTrail->Get_Transform()->Get_Position());
			m_pExplosiveEffect[0]->Play();
			m_pExplosiveEffect[1]->Play();
			m_isExplosiveTrigger = true;
		}
	}
	else 
	{
		m_fLerpAcc += fTimeDelta / m_MagicBallDesc.fInitLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_pTrail->Tick(fTimeDelta);
		m_pTrail->Spline_Move(m_vSplineLerp[0],m_vStartPostion, m_vTargetPosition, m_vSplineLerp[1], m_fLerpAcc);
		m_pTransform->Set_Position(m_pTrail->Get_Transform()->Get_Position());
	}
}

void CConfringo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CConfringo::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		//사망처리 드갑니다.
		m_isExplosiveTrigger = true;
		m_pExplosiveEffect[0]->Get_Transform()->Set_Position(m_pTrail->Get_Transform()->Get_Position());
		m_pExplosiveEffect[1]->Get_Transform()->Set_Position(m_pTrail->Get_Transform()->Get_Position());
		m_pExplosiveEffect[0]->Play();
		m_pExplosiveEffect[1]->Play();
	}
	
}

void CConfringo::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CConfringo::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CConfringo::Add_Components()
{
	return S_OK;
}

HRESULT CConfringo::Add_Effect()
{
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Traill_Confringo_Effect"), 
		TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pTrail))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Traill_Confringo_Effect)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")
		, TEXT("Com_Explosive_Particle01"), (CComponent**)&m_pExplosiveEffect[0])))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")
		, TEXT("Com_Explosive_Particle02"), (CComponent**)&m_pExplosiveEffect[1])))
		return E_FAIL;
	
	return S_OK;
}

CConfringo* CConfringo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CConfringo* pInstance = new CConfringo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CConfringo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CConfringo::Clone(void* pArg)
{
	CConfringo* pInstance = new CConfringo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CConfringo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CConfringo::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pTrail);
		Safe_Release(m_pExplosiveEffect[0]);
		Safe_Release(m_pExplosiveEffect[1]);
		Safe_Release(m_pWandDustEffect);
	}
}
