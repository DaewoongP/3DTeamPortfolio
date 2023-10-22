#include "Projectile_Black.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CProjectile_Black::CProjectile_Black(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CProjectile_Black::CProjectile_Black(const CProjectile_Black& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CProjectile_Black::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;
	m_iLevel = iLevel;

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_StartDistortion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_StartDistortion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Projectile_White/StartDistortion/"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_White_BulletTrace")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_White_BulletTrace")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Projectile_White/BulletTrace/"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_White_HitBoom")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Projectile_White_HitBoom")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Projectile_White/HitBoom/"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Trail_Projectile_White")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Trail_Projectile_White")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Projectile_White/Projectile_White_Trail.trail"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_MeshEffect_Outer_Ball")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_MeshEffect_Outer_Ball")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_White/Outer_Ball.ME"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_MeshEffect_Inner_Ball")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_MeshEffect_Inner_Ball")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_White/Inner_Ball.ME"), iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_StartBoom")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_StartBoom")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_White/StartBoom.ME"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_EndBoom")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_EndBoom")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_White/EndBoom.ME"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CProjectile_Black::Initialize(void* pArg)
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
	return S_OK;
}

void CProjectile_Black::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CProjectile_Black::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CProjectile_Black::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//플레이어랑 충돌했으면?
	//if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Player_Default")) != nullptr)
	//{
	//	Set_MagicBallState(MAGICBALL_STATE_DYING);
	//}
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CProjectile_Black::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CProjectile_Black::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CProjectile_Black::Reset(MAGICBALLINITDESC& InitDesc)
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

	return S_OK;
}

void CProjectile_Black::Ready_Begin()
{
	Ready_StraightMove(m_TrailVec[EFFECT_STATE_MAIN][0]);
	__super::Ready_Begin();
}

void CProjectile_Black::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CProjectile_Black::Ready_CastMagic()
{
	__super::Ready_CastMagic();
	m_pMeshEffect_Outer_Ball->Play(m_CurrentWeaponMatrix.Translation());
	m_pMeshEffect_Inner_Ball->Play(m_CurrentWeaponMatrix.Translation());
	m_pMeshEffect_StartBoom->Play(m_CurrentWeaponMatrix.Translation());
}

void CProjectile_Black::Ready_Dying()
{
	m_pParticle_Projectile_White_BulletTrace->Stop();
	m_pMeshEffect_EndBoom->Play(m_pMeshEffect_Outer_Ball->Get_Transform()->Get_Position());
	__super::Ready_Dying();
}

void CProjectile_Black::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CProjectile_Black::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
	__super::Tick_DrawMagic(fTimeDelta);
}

void CProjectile_Black::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
	//m_ParticleVec[EFFECT_STATE_MAIN][1]->Get_Transform()->LookAt(m_vEndPosition);
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;

		_float3 vLerpPos = _float3::Lerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc);
		m_TrailVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Set_Position(vLerpPos);
		_float3 vTrailPos = m_TrailVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Get_Position();
		m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_Transform()->LookAt(m_vStartPosition);
		m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_ShapeModuleRef().Set_ShapeLook(m_vEndPosition, m_vStartPosition);
		m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Set_Position(vTrailPos);
		m_pMeshEffect_Outer_Ball->Get_Transform()->Set_Position(vTrailPos);
		m_pMeshEffect_Inner_Ball->Get_Transform()->Set_Position(vTrailPos);
		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
	}
	else
	{
		m_pMeshEffect_Outer_Ball->Stop();
		m_pMeshEffect_Inner_Ball->Stop();
		Do_MagicBallState_To_Next();
	}
}

void CProjectile_Black::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}

HRESULT CProjectile_Black::Add_Components()
{
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_StartDistortion"),
		TEXT("Com_Particle_StartDistortion"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0])));

	m_ParticleVec[EFFECT_STATE_MAIN].resize(1);
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Projectile_White_BulletTrace"),
		TEXT("Com_Particle_Projectile_White_BulletTrace"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0])));

	m_ParticleVec[EFFECT_STATE_HIT].resize(1);
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Projectile_White_HitBoom"),
		TEXT("Com_Particle_Projectile_HitBoom"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0])));

	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	FAILED_CHECK(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Trail_Projectile_White"),
		TEXT("Com_Trail_Projectile_White"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0])));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Outer_Ball"),
		TEXT("Com_MeshEffet_Outer_Ball"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Outer_Ball)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Inner_Ball"),
		TEXT("Com_MeshEffet_Inner_Ball"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Inner_Ball)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_StartBoom"),
		TEXT("Com_MeshEffet_StartBoom"), reinterpret_cast<CComponent**>(&m_pMeshEffect_StartBoom)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_EndBoom"),
		TEXT("Com_MeshEffect_EndBoom"), reinterpret_cast<CComponent**>(&m_pMeshEffect_EndBoom)));

	return S_OK;
}

CProjectile_Black* CProjectile_Black::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CProjectile_Black* pInstance = New CProjectile_Black(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CProjectile_Black");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile_Black::Clone(void* pArg)
{
	CProjectile_Black* pInstance = New CProjectile_Black(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProjectile_Black");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProjectile_Black::Free()
{
	__super::Free();

	Safe_Release(m_pMeshEffect_Outer_Ball);
	Safe_Release(m_pMeshEffect_Inner_Ball);
	Safe_Release(m_pMeshEffect_StartBoom);
	Safe_Release(m_pMeshEffect_EndBoom);
	Safe_Release(m_pParticle_Projectile_White_BulletTrace);
}
