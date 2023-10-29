#include "Projectile_White.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"
#include "MeshEffect.h"

CProjectile_White::CProjectile_White(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CProjectile_White::CProjectile_White(const CProjectile_White& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CProjectile_White::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;
	m_iLevel = iLevel;

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_Bullet_Trace")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_Bullet_Trace")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Projectile_Black/BulletTrace/"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_SplineUp")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_SplineUp")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Projectile_Black/SplineUp/"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}


	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_Trail")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Projectile_Black/BulletTrail"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}


	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_MeshEffect_Projectile_Black_SplineUp")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_MeshEffect_Projectile_Black_SplineUp")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_Black/SplineUp.ME"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Trail_Projectile_Black")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Trail_Projectile_Black")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Projectile_Black/Projectile_Black.trail"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_MeshEffect_Projectile_Black")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_MeshEffect_Projectile_Black")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_Black/Projectile_Black.ME"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CProjectile_White::Initialize(void* pArg)
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

	m_InitMatrix_Projectile_Black = m_pMeshEffect_Projectile_Black->Get_Transform()->Get_WorldMatrix();
	for (_uint i = 0; i < 6; ++i)
	{
		m_InitMatrix_SplineUp[i] = m_pMeshEffect_SplineUp[i]->Get_Transform()->Get_WorldMatrix();
	}
	
	return S_OK;
}

void CProjectile_White::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CProjectile_White::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CProjectile_White::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//플레이어랑 충돌했으면?
	//if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Player_Default")) != nullptr)
	//{
	//	Set_MagicBallState(MAGICBALL_STATE_DYING);
	//}
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CProjectile_White::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CProjectile_White::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CProjectile_White::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);

	// Light
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	CLight::LIGHTDESC LightDesc;
	LightDesc.eType = CLight::TYPE_POINT;
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPos = m_pTransform->Get_Position().TransCoord();
	pGameInstance->Add_Light(LightDesc, &m_pLight);
	Safe_Release(pGameInstance);

	m_pMeshEffect_Projectile_Black->Get_Transform()->Set_WorldMatrix(m_InitMatrix_Projectile_Black);
	for (_uint i = 0; i < 6; ++i)
	{
		m_pMeshEffect_SplineUp[i]->Get_Transform()->Set_WorldMatrix(m_InitMatrix_SplineUp[i]);
	}
	return S_OK;
}

void CProjectile_White::Ready_Begin()
{
	Ready_StraightMove(m_TrailVec[EFFECT_STATE_MAIN][0]);
	__super::Ready_Begin();
}

void CProjectile_White::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CProjectile_White::Ready_CastMagic()
{
	__super::Ready_CastMagic();
	m_pMeshEffect_Projectile_Black->Play(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN][0]->Play(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN][1]->Play(m_CurrentWeaponMatrix.Translation());

}

void CProjectile_White::Ready_Dying()
{
	m_ParticleVec[EFFECT_STATE_MAIN][1]->Stop();

	for (auto& SplineUp : m_pMeshEffect_SplineUp)
	{
		SplineUp->Play(m_vEndPosition);
	}
	__super::Ready_Dying();
}

void CProjectile_White::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CProjectile_White::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
	__super::Tick_DrawMagic(fTimeDelta);
}

void CProjectile_White::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
	//m_ParticleVec[EFFECT_STATE_MAIN][1]->Get_Transform()->LookAt(m_vEndPosition);
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		_float3 vLerpPos = _float3::Lerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc);

		_float3 vTrailPos = m_TrailVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Get_Position();
		
			m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_Transform()->LookAt(m_vStartPosition);
			m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_ShapeModuleRef().Set_ShapeLook(m_vEndPosition, m_vStartPosition);
			m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Set_Position(vTrailPos);
		
		m_pMeshEffect_Projectile_Black->Get_Transform()->Set_Position(vLerpPos);
		m_pMeshEffect_Projectile_Black->Get_Transform()->LookAt(m_vEndPosition);
		m_ParticleVec[EFFECT_STATE_MAIN][1]->Get_Transform()->Set_Position(m_pMeshEffect_Projectile_Black->Get_Transform()->Get_Position());


		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
}

void CProjectile_White::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
	m_ParticleVec[EFFECT_STATE_HIT][0]->Get_Transform()->Set_Position(m_pMeshEffect_Projectile_Black->Get_Transform()->Get_Position());
}

HRESULT CProjectile_White::Add_Components()
{
	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Trail_Projectile_Black"),
			TEXT("Com_Trail_Projectile_Black"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0])));

	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Projectile_Black"),
		TEXT("Com_MeshEffect_Projectile_Black"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Projectile_Black)));

	m_ParticleVec[EFFECT_STATE_MAIN].resize(2);
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_Bullet_Trace"),
		TEXT("Com_Particle_Projectile_Black_Bullet_Trace"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0])));
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_Trail"),
		TEXT("Com_Particle_Projectile_Black_Trail"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][1])));



	m_ParticleVec[EFFECT_STATE_HIT].resize(1);
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Projectile_Black_SplineUp"),
		TEXT("Com_Particle_Projectile_Black_SplineUp"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0])));

	for (_uint i = 0; i < 6; ++i)
	{
		FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Projectile_Black_SplineUp"),
			Generate_HashtagW().data(), reinterpret_cast<CComponent**>(&m_pMeshEffect_SplineUp[i])));
		m_pMeshEffect_SplineUp[i]->Set_StartRot(_float3(0.f, i * 60.f, 0.f));
		m_pMeshEffect_SplineUp[i]->Set_EndRot(_float3(0.f, i * 60.f, 0.f));
	}
	
	return S_OK;
}

CProjectile_White* CProjectile_White::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CProjectile_White* pInstance = New CProjectile_White(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CProjectile_White");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_White::Clone(void* pArg)
{
	CProjectile_White* pInstance = New CProjectile_White(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProjectile_White");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProjectile_White::Free()
{
	__super::Free();

	Safe_Release(m_pMeshEffect_Projectile_Black);
	for (auto& MeshEffect : m_pMeshEffect_SplineUp)
		Safe_Release(MeshEffect);
}
