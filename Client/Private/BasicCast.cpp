#include "BasicCast.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CBasicCast::CBasicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CBasicCast::CBasicCast(const CBasicCast& rhs)
	: CMagicBall(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CBasicCast::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/DefaultConeBoom"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/Default_SphereTrace"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BasicCast/DefaultConeEmit"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicTrail/BasicTrail.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;


	return S_OK;
}

HRESULT CBasicCast::Initialize(void* pArg)
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

void CBasicCast::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandEffect->Get_Transform()->Set_Position(vWandPosition);
	m_WandTrail->Get_Transform()->Set_Position(vWandPosition);
}

void CBasicCast::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBasicCast::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag,TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CBasicCast::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CBasicCast::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

void CBasicCast::Ready_Begin()
{
	m_pHitEffect->Disable();
	m_pWandEffect->Disable();
	m_pFinalAttackEffect->Disable();
	m_pTrailEffect->Disable();
	m_WandTrail->Disable();
}

void CBasicCast::Ready_DrawMagic()
{
	m_WandTrail->Enable();
	m_pWandEffect->Enable();

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_WandTrail->Get_Transform()->Set_Position(vWandPosition);
	m_pWandEffect->Get_Transform()->Set_Position(vWandPosition);
	m_WandTrail->Reset_Trail(_float3(vWandPosition) + _float3(0, 0.5f, 0), _float3(vWandPosition) + _float3(0, -0.5f, 0));
}

void CBasicCast::Ready_CastMagic()
{
	m_pTrailEffect->Enable();
	if (m_pTarget == nullptr)
	{
		BEGININSTANCE;
		_float4 vMouseOrigin, vMouseDirection;
		_float3 vMouseWorldPickPosition, vDirStartToPicked;
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
	m_pTrailEffect->Ready_Spline(m_vTargetPosition, m_MagicBallDesc.vStartPosition, m_MagicBallDesc.fInitLifeTime, m_MagicBallDesc.fDistance);
}

void CBasicCast::Ready_Dying()
{
	m_WandTrail->Disable();
	m_pWandEffect->Disable();

	m_pHitEffect->Get_Transform()->Set_Position(m_pTrailEffect->Get_Transform()->Get_Position());
	m_pHitEffect->Enable();
	m_pHitEffect->Play();
}

void CBasicCast::Tick_Begin(_float fTimeDelta)
{
	//베이직 캐스트는 비긴 없습니다.
	Do_MagicBallState_To_Next();
}

void CBasicCast::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CBasicCast::Tick_CastMagic(_float fTimeDelta)
{
	//스플라인 움직임이 끝나면 터지는 이펙트 재생하겠습니다.
	if (m_pTrailEffect->Spline_Move(fTimeDelta))
	{
		Do_MagicBallState_To_Next();
	}
	m_pTransform->Set_Position(m_pTrailEffect->Get_Transform()->Get_Position());
}

void CBasicCast::Tick_Dying(_float fTimeDelta)
{
	//사망하러 가겠습니다.
	if (!m_pHitEffect->IsEnable())
	{
		Do_MagicBallState_To_Next();
	}
}

HRESULT CBasicCast::Add_Components()
{
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_DefaultConeBoom_Particle")
		, TEXT("Com_DefaultConeBoom_Particle"), (CComponent**)&m_pHitEffect)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Default_SphereTrace_Particle")
		, TEXT("Com_Default_SphereTrace_Particle"), (CComponent**)&m_pWandEffect)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_DefaultConeEmit_Particle")
		, TEXT("Com_DefaultConeEmit_Particle"), (CComponent**)&m_pFinalAttackEffect)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBasicCast::Add_Effect()
{
	CDefault_MagicTraill_Effect::INITDESC initDesc;
	initDesc.vInitPosition = m_MagicBallDesc.vStartPosition;
	if (FAILED(CComposite::Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_MagicTraill_BasicCast_Effect"),
		TEXT("Com_TrailEffect"), reinterpret_cast<CComponent**>(&m_pTrailEffect), &initDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_MagicTrail_BasicCast_Effect)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_WandTrail))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_BasicCast_Wand_Trail_Effect)");
		return E_FAIL;
	}

	return S_OK;
}

CBasicCast* CBasicCast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CBasicCast* pInstance = New CBasicCast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CBasicCast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBasicCast::Clone(void* pArg)
{
	CBasicCast* pInstance = New CBasicCast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBasicCast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBasicCast::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_WandTrail);
		Safe_Release(m_pTrailEffect);
		Safe_Release(m_pHitEffect);
		Safe_Release(m_pWandEffect);
		Safe_Release(m_pFinalAttackEffect);
	}
	
}
