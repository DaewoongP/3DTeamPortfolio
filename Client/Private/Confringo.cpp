#include "Confringo.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CConfringo::CConfringo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CConfringo::CConfringo(const CConfringo& rhs)
	: CMagicBall(rhs)
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

	m_pTrailDustEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pExplosiveEffect[0]->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pExplosiveEffect[1]->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pExplosiveBigPartEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pExplosiveSmallPartEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pWandTouchEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pWandDustEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pWandTwinklEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));

	m_pMainTrail->Disable();
	m_pWandDustEffect->Disable();

	m_pExplosiveEffect[0]->Disable();
	m_pExplosiveEffect[1]->Disable();
	m_pExplosiveBigPartEffect->Disable();
	m_pExplosiveSmallPartEffect->Disable();

	m_pWandTouchEffect->Disable();
	m_pWandDustEffect->Disable();
	m_pWandTwinklEffect->Disable();
	m_pWandTrail->Disable();

	return S_OK;
}

void CConfringo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandTouchEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandDustEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTwinklEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTrail->Get_Transform()->Set_Position(vWandPosition);
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

HRESULT CConfringo::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	m_fLerpAcc = 0.0f;
	m_pMainTrail->Disable();
	m_pWandDustEffect->Disable();

	m_pExplosiveEffect[0]->Disable();
	m_pExplosiveEffect[1]->Disable();
	m_pExplosiveBigPartEffect->Disable();
	m_pExplosiveSmallPartEffect->Disable();

	m_pWandTouchEffect->Disable();
	m_pWandDustEffect->Disable();
	m_pWandTwinklEffect->Disable();
	m_pWandTrail->Disable();
	return S_OK;
}

void CConfringo::Ready_Begin()
{
}

void CConfringo::Ready_DrawMagic()
{
	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandTouchEffect->Enable(vWandPosition);
	m_pWandDustEffect->Enable(vWandPosition);
	m_pWandTwinklEffect->Enable(vWandPosition);
	m_pWandTrail->Enable(vWandPosition);
}

void CConfringo::Ready_CastMagic()
{
	Ready_SplineSpinMove(m_pMainTrail,_float2(0.2f, 0.20f),10.f);
	m_pMainTrail->Enable(m_vStartPostion);
	m_pWandDustEffect->Enable(m_vStartPostion);
}

void CConfringo::Ready_Dying()
{
	m_pExplosiveEffect[0]->Enable(m_pMainTrail->Get_Transform()->Get_Position());
	m_pExplosiveEffect[1]->Enable(m_pMainTrail->Get_Transform()->Get_Position());
	m_pExplosiveBigPartEffect->Enable(m_pMainTrail->Get_Transform()->Get_Position());
	m_pExplosiveSmallPartEffect->Enable(m_pMainTrail->Get_Transform()->Get_Position());
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
		m_pMainTrail->Spline_Spin_Move(m_vSplineLerp[0], m_vStartPostion, m_vTargetPosition, m_vSplineLerp[1], m_vSpinWeight, m_fSpinSpeed, m_fLerpAcc);
		m_pWandDustEffect->Get_Transform()->Set_Position(m_pMainTrail->Get_Transform()->Get_Position());
		m_pTransform->Set_Position(m_pMainTrail->Get_Transform()->Get_Position());
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
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Confringo_Trail"),
		TEXT("Com_Trail"), reinterpret_cast<CComponent**>(&m_pMainTrail))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Confringo_Trail)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Dust_Effect")
		, TEXT("Com_Main_Dust"),  reinterpret_cast<CComponent**>(&m_pTrailDustEffect))))
	{
		__debugbreak();
		return E_FAIL;
	}

	//종료
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireBallDir_Effect")
		, TEXT("Com_Explosive_Particle01"), reinterpret_cast<CComponent**>(&m_pExplosiveEffect[0]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireBallB_Effect")
		, TEXT("Com_Explosive_Particle02"), reinterpret_cast<CComponent**>(&m_pExplosiveEffect[1]))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Expread_Effect")
		, TEXT("Com_END_Expread"), reinterpret_cast<CComponent**>(&m_pExplosiveBigPartEffect))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Small_Expread_Effect")
		, TEXT("Com_Small_Expread"), reinterpret_cast<CComponent**>(&m_pExplosiveSmallPartEffect))))
	{
		__debugbreak();
		return E_FAIL;
	}

	//완드
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Fire_Torch_Effect")
		, TEXT("Com_Fire_Torch"), reinterpret_cast<CComponent**>(&m_pWandTouchEffect))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Confringo_Dust_Effect")
		, TEXT("Com_Dust_Effect"), reinterpret_cast<CComponent**>(&m_pWandDustEffect))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")
		, TEXT("Com_Twinkle_Effect"), reinterpret_cast<CComponent**>(&m_pWandTwinklEffect))))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
		, TEXT("Com_Wand_Trail_Effect"), reinterpret_cast<CComponent**>(&m_pWandTrail))))
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
		Safe_Release(m_pMainTrail);
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
		Safe_Release(m_pWandTrail);
	}
}

