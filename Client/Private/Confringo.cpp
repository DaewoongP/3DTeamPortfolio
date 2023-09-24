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
	//메인 이펙트
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Trail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Confringo/Confringo.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Dust_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Dust_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Confringo/Dust"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	//종료 이펙트
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Confringo/FireBallDir"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Confringo/FireBallB"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Expread_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Expread_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Confringo/FireExpread"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Small_Expread_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Small_Expread_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Confringo/FireExpread_Sub"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	
	//완드
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Fire_Torch_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Fire_Torch_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Confringo/Fire_Torch"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicTrail/BasicCast/BasicTrail.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Twinkle_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Confringo_Twinkle_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/Twinkle"), m_iLevel))))
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
	return S_OK;
}

void CConfringo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandTouchEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandDustEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTwinklEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTrailEffect->Get_Transform()->Set_Position(vWandPosition);
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
		Set_MagicBallState(MAGICBALL_STATE_DYING);
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

void CConfringo::Ready_Begin()
{
	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandDustEffect->Get_EmissionModuleRef().Setting_PrevPos(vWandPosition);
	m_pWandTwinklEffect->Get_EmissionModuleRef().Setting_PrevPos(vWandPosition);

	m_pTrail->Disable();
	m_pTrailDustEffect->Disable();

	m_pExplosiveEffect[0]->Disable();
	m_pExplosiveEffect[1]->Disable();
	m_pExplosiveBigPartEffect->Disable();
	m_pExplosiveSmallPartEffect->Disable();

	m_pWandTouchEffect->Disable();
	m_pWandDustEffect->Disable();
	m_pWandTwinklEffect->Disable();
	m_pWandTrailEffect->Disable();
}

void CConfringo::Ready_DrawMagic()
{
	m_pWandTouchEffect->Enable();
	m_pWandDustEffect->Enable();
	m_pWandTwinklEffect->Enable();
	m_pWandTrailEffect->Enable();

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandTouchEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandDustEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTrailEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTwinklEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTrailEffect->Reset_Trail(_float3(vWandPosition) + _float3(0, 0.5f, 0), _float3(vWandPosition) + _float3(0, -0.5f, 0));
}

void CConfringo::Ready_CastMagic()
{
	if (m_pTarget == nullptr)
	{
		//마우스 피킹 지점으로 발사
		BEGININSTANCE;
		_float4 vMouseOrigin, vMouseDirection;
		_float3 vMouseWorldPickPosition, vDirStartToPicked;
		if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
		{
			ENDINSTANCE;
			return;
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

	m_pTrail->Reset_Trail(_float3(m_vStartPostion) + _float3(0, 0.5f, 0), _float3(m_vStartPostion) + _float3(0, -0.5f, 0));
	m_pTrail->Get_Transform()->Set_Position(m_vStartPostion);

	m_pTrailDustEffect->Get_EmissionModuleRef().Setting_PrevPos(m_vStartPostion);
	m_pTrailDustEffect->Get_Transform()->Set_Position(m_vStartPostion);

	m_pTrail->Enable();
	m_pTrailDustEffect->Enable();
}

void CConfringo::Ready_Dying()
{
	/*m_pWandTouchEffect->Disable();
	m_pWandDustEffect->Disable();
	m_pWandTwinklEffect->Disable();
	m_pWandTrailEffect->Disable();*/

	m_pExplosiveEffect[0]->Enable();
	m_pExplosiveEffect[1]->Enable();
	m_pExplosiveBigPartEffect->Enable();
	m_pExplosiveSmallPartEffect->Enable();

	m_pExplosiveEffect[0]->Play(m_pTrail->Get_Transform()->Get_Position());
	m_pExplosiveEffect[1]->Play(m_pTrail->Get_Transform()->Get_Position());
	m_pExplosiveBigPartEffect->Play(m_pTrail->Get_Transform()->Get_Position());
	m_pExplosiveSmallPartEffect->Play(m_pTrail->Get_Transform()->Get_Position());
}

void CConfringo::Tick_Begin(_float fTimeDelta)
{
	//콘프링고는 비긴 없습니다.
	Do_MagicBallState_To_Next();
}

void CConfringo::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CConfringo::Tick_CastMagic(_float fTimeDelta)
{
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_MagicBallDesc.fInitLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_pTrail->Spline_Spin_Move(m_vSplineLerp[0], m_vStartPostion, m_vTargetPosition, m_vSplineLerp[1], m_fLerpAcc);
		m_pTrailDustEffect->Get_Transform()->Set_Position(m_pTrail->Get_Transform()->Get_Position());
		m_pTransform->Set_Position(m_pTrail->Get_Transform()->Get_Position());
	}
	else 
	{
		Do_MagicBallState_To_Next();
	}
}

void CConfringo::Tick_Dying(_float fTimeDelta)
{
	if (!m_pExplosiveEffect[0]->IsEnable() &&
		!m_pExplosiveEffect[1]->IsEnable() &&
		!m_pExplosiveSmallPartEffect->IsEnable()&&
		!m_pExplosiveBigPartEffect->IsEnable())
		Do_MagicBallState_To_Next();
}


HRESULT CConfringo::Add_Components()
{
	return S_OK;
}

HRESULT CConfringo::Add_Effect()
{
	//메인
	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Confringo_Trail"), 
		TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pTrail))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Confringo_Trail)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Dust_Effect")
		, TEXT("Com_Main_Dust"), (CComponent**)&m_pTrailDustEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}

	//종료
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")
		, TEXT("Com_Explosive_Particle01"), (CComponent**)&m_pExplosiveEffect[0])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")
		, TEXT("Com_Explosive_Particle02"), (CComponent**)&m_pExplosiveEffect[1])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Expread_Effect")
		, TEXT("Com_END_Expread"), (CComponent**)&m_pExplosiveBigPartEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Small_Expread_Effect")
		, TEXT("Com_Small_Expread"), (CComponent**)&m_pExplosiveSmallPartEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}

	//완드
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Fire_Torch_Effect")
		, TEXT("Com_Fire_Torch"), (CComponent**)&m_pWandTouchEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Dust_Effect")
		, TEXT("Com_Dust_Effect"), (CComponent**)&m_pWandDustEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")
		, TEXT("Com_Twinkle_Effect"), (CComponent**)&m_pWandTwinklEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
		, TEXT("Com_Wand_Trail_Effect"), (CComponent**)&m_pWandTrailEffect)))
	{
		__debugbreak();
		return E_FAIL;
	}
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
		//메인
		Safe_Release(m_pTrail);
		Safe_Release(m_pTrailDustEffect);
		
		//종료
		Safe_Release(m_pExplosiveEffect[0]);
		Safe_Release(m_pExplosiveEffect[1]);
		Safe_Release(m_pExplosiveBigPartEffect);
		Safe_Release(m_pExplosiveSmallPartEffect);
		
		//완드
		Safe_Release(m_pWandTouchEffect);
		Safe_Release(m_pWandDustEffect);
		Safe_Release(m_pWandTwinklEffect);
		Safe_Release(m_pWandTrailEffect);
	}
}

