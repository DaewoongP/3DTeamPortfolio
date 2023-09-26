#include "Finisher.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CFinisher::CFinisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CFinisher::CFinisher(const CFinisher& rhs)
	: CMagicBall(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CFinisher::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Trail_Lightning_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Trail_Lightning_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Lightning/Lightning.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Spark_Blue_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Spark_Blue_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lightning/Spark_Blue"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Spark_Green_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Spark_Green_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lightning/Spark_Green"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Spark_Red_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Spark_Red_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lightning/Spark_Red"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lightning_Flare_Center_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lightning_Flare_Center_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lightning/Flare_Center"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lightning_Line_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lightning_Line_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lightning/LineParticle"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lightning_Flare_Spread_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lightning_Flare_Spread_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lightning/Flare_Spread"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lightning_Dust_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lightning_Dust_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lightning/Dust"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Wand_Lightning_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Wand_Lightning_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lightning/Wand_Lightning"), m_iLevel))))
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

HRESULT CFinisher::Initialize(void* pArg)
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

void CFinisher::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandLightningParticle->Get_Transform()->Set_Position(vWandPosition);
	m_pWandTrail->Get_Transform()->Set_Position(vWandPosition);
}

void CFinisher::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CFinisher::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}
}

void CFinisher::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CFinisher::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CFinisher::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	return S_OK;
}

void CFinisher::Ready_Begin()
{
	for (int i = 0; i < 3; i++)
	{
		//m_pTrail[i]->Disable();
	}

	 m_LightningSparkEffect_Green->Disable();
	 m_LightningSparkEffect_Blue->Disable();
	 m_LightningSparkEffect_Red->Disable();

	 m_LineParticle->Disable();
	 m_FlareCenterParticle->Disable();
	 m_FlareSpreadParticle->Disable();
	 m_DustParticle->Disable();

	 m_pWandLightningParticle->Disable();
	 m_pWandTrail->Disable();
}

void CFinisher::Ready_DrawMagic()
{
	m_pWandLightningParticle->Enable();
	m_pWandTrail->Enable();

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandTrail->Reset_Trail(_float3(vWandPosition) + _float3(0, 0.5f, 0), _float3(vWandPosition) + _float3(0, -0.5f, 0));
}

void CFinisher::Ready_CastMagic()
{
	if (m_pTarget == nullptr)
	{
		//마우스 피킹 지점으로 발사
		BEGININSTANCE;
		_float4 vMouseOrigin, vMouseDirection;
		_float3 vMouseWorldPickPosition, vDirStartToPicked;
		if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
		{
			Safe_Release(pGameInstance);
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

	for (int i = 0; i < 3; i++)
	{
		_float3 vWeight[2];
		vWeight[0] = _float3(-2.f, -0.5f, -2.f);
		vWeight[1] = _float3(2.f,0.5f, 2.f);

		m_pTrail[i]->Set_Threshold(0.2f);
		m_pTrail[i]->Ready_LightningStrike(m_pTarget->Get_Position() + _float3(0, 10, 0), m_pTarget->Get_Position(), vWeight, 10);
		//m_pTrail[i]->Reset_Trail(m_pTarget->Get_Position() + _float3(0, 10, 0) + _float3(0, 2.f, 0), m_pTarget->Get_Position() + _float3(0, 10, 0) + _float3(0, -2.f, 0));
		//m_pTrail[i]->Get_Transform()->Set_Position(m_pTarget->Get_Position() + _float3(0, 10, 0));
		m_pTrail[i]->Enable();
	}

	m_LightningSparkEffect_Blue->Enable();
	m_LightningSparkEffect_Green->Enable();
	m_LightningSparkEffect_Red->Enable();
	m_LineParticle->Enable();
	m_FlareCenterParticle->Enable();
	m_FlareSpreadParticle->Enable();
	m_DustParticle->Enable();

	m_LightningSparkEffect_Blue->Play(m_pTarget->Get_Position());
	m_LightningSparkEffect_Green->Play(m_pTarget->Get_Position());
	m_LightningSparkEffect_Red->Play(m_pTarget->Get_Position());
	m_LineParticle->Play(m_pTarget->Get_Position());
	m_FlareCenterParticle->Play(m_pTarget->Get_Position());
	m_FlareSpreadParticle->Play(m_pTarget->Get_Position() + _float3(0, 0.5f, 0));
	m_DustParticle->Play(m_pTarget->Get_Position());
	m_pTransform->Set_Position(m_pTarget->Get_Position());
}

void CFinisher::Ready_Dying()
{
}

void CFinisher::Tick_Begin(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CFinisher::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CFinisher::Tick_CastMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CFinisher::Tick_Dying(_float fTimeDelta)
{
	//if(!m_DustParticle->IsEnable())
	//	Do_MagicBallState_To_Next();
}

HRESULT CFinisher::Add_Components()
{
	return S_OK;
}

HRESULT CFinisher::Add_Effect()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trail_Lightning_Effect"),
		TEXT("Com_LightningTrail01"), reinterpret_cast<CComponent**>(&m_pTrail[0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Trail_Lightning_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trail_Lightning_Effect"),
		TEXT("Com_LightningTrail02"), reinterpret_cast<CComponent**>(&m_pTrail[1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Trail_Lightning_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trail_Lightning_Effect"),
		TEXT("Com_LightningTrail03"), reinterpret_cast<CComponent**>(&m_pTrail[2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Trail_Lightning_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Spark_Blue_Effect")
		, TEXT("Com_Spark_Blue_Effect"), (CComponent**)&m_LightningSparkEffect_Blue)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Spark_Green_Effect")
		, TEXT("Com_Spark_Green_Effect"), (CComponent**)&m_LightningSparkEffect_Green)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Spark_Red_Effect")
		, TEXT("Com_Spark_Red_Effect"), (CComponent**)&m_LightningSparkEffect_Red)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Lightning_Line_Effect")
		, TEXT("Com_Line_Effect"), (CComponent**)&m_LineParticle)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Lightning_Flare_Center_Effect")
		, TEXT("Com_FlareCenter_Effect"), (CComponent**)&m_FlareCenterParticle)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Lightning_Flare_Spread_Effect")
		, TEXT("Com_FlareSpread_Effect"), (CComponent**)&m_FlareSpreadParticle)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Lightning_Dust_Effect")
		, TEXT("Com_Dust_Effect"), (CComponent**)&m_DustParticle)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Wand_Lightning_Effect")
		, TEXT("Com_Wand_Lightning"), (CComponent**)&m_pWandLightningParticle)))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
		, TEXT("Com_Wand_Trail"), (CComponent**)&m_pWandTrail)))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

CFinisher* CFinisher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CFinisher* pInstance = New CFinisher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CFinisher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFinisher::Clone(void* pArg)
{
	CFinisher* pInstance = New CFinisher(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFinisher");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFinisher::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pTrail[0]);
		Safe_Release(m_pTrail[1]);
		Safe_Release(m_pTrail[2]);

		Safe_Release(m_LightningSparkEffect_Blue);
		Safe_Release(m_LightningSparkEffect_Red);
		Safe_Release(m_LightningSparkEffect_Green);

		Safe_Release(m_LineParticle);
		Safe_Release(m_FlareCenterParticle);
		Safe_Release(m_FlareSpreadParticle);
		Safe_Release(m_DustParticle);

		Safe_Release(m_pWandLightningParticle);
		Safe_Release(m_pWandTrail);
	}
}
