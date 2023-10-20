#include "Projectile_White.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

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

	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Test")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Particle_Test")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Test/"), iLevel))))
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

	return S_OK;
}

void CProjectile_White::Ready_Begin()
{
	__super::Ready_Begin();
}

void CProjectile_White::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();

	m_pMeshEffect_Outer_Ball->Play(m_CurrentWeaponMatrix.Translation());
	m_pMeshEffect_Inner_Ball->Play(m_CurrentWeaponMatrix.Translation());
}

void CProjectile_White::Ready_CastMagic()
{
	//Ready_SplineMove(m_TrailVec[EFFECT_STATE_MAIN][0]);
	__super::Ready_CastMagic();
}

void CProjectile_White::Ready_Dying()
{
	__super::Ready_Dying();
}

void CProjectile_White::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CProjectile_White::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CProjectile_White::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
	//m_ParticleVec[EFFECT_STATE_MAIN][1]->Get_Transform()->LookAt(m_vEndPosition);
	//if (m_fLerpAcc != 1)
	//{
	//	m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
	//	if (m_fLerpAcc > 1)
	//		m_fLerpAcc = 1;
	//	m_TrailVec[EFFECT_STATE_MAIN][0]->Spline_Move(m_vSplineLerp[0], m_vStartPosition, m_vEndPosition, m_vSplineLerp[1], m_fLerpAcc);
	//	m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
	//}
	//else
	//{
	//	Do_MagicBallState_To_Next();
	//}

	Do_MagicBallState_To_Next();
}

void CProjectile_White::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}

HRESULT CProjectile_White::Add_Components()
{
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Test"),
		TEXT("Com_Test"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0])));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Outer_Ball"),
		TEXT("Com_MeshEffet_Outer_Ball"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Outer_Ball)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Inner_Ball"),
		TEXT("Com_MeshEffet_Inner_Ball"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Inner_Ball)));

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

	Safe_Release(m_pMeshEffect_Outer_Ball);
	Safe_Release(m_pMeshEffect_Inner_Ball);
}
