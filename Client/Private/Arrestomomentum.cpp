#include "Arrestomomentum.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

Arrestomomentum::Arrestomomentum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

Arrestomomentum::Arrestomomentum(const Arrestomomentum& rhs)
	: CMagicBall(rhs)
{
}

HRESULT Arrestomomentum::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Flare_Center")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Flare_Center")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Arrestomomentum/Flare_Center"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Flare_Horizontal")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Flare_Horizontal")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Arrestomomentum/Flare_Horizontal"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Spark")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Spark")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Arrestomomentum/Spark"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Hit_Center")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Hit_Center")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Arrestomomentum/Hit_Center"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Hit_Flare")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Arres_Hit_Flare")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Arrestomomentum/Hit_Flare"), m_iLevel))))
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

HRESULT Arrestomomentum::Initialize(void* pArg)
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

void Arrestomomentum::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void Arrestomomentum::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void Arrestomomentum::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//¸÷ÀÌ¶û Ãæµ¹ÇßÀ¸¸é?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}
}

void Arrestomomentum::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void Arrestomomentum::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT Arrestomomentum::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	return S_OK;
}

void Arrestomomentum::Ready_Begin()
{
	__super::Ready_Begin();
}

void Arrestomomentum::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void Arrestomomentum::Ready_CastMagic()
{
	m_pTransform->Set_Position(m_vEndPosition);
	__super::Ready_CastMagic();
}

void Arrestomomentum::Ready_Dying()
{
	__super::Ready_Dying();
}

void Arrestomomentum::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void Arrestomomentum::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void Arrestomomentum::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
}

void Arrestomomentum::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}

HRESULT Arrestomomentum::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	m_ParticleVec[EFFECT_STATE_WAND].resize(3);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
		, TEXT("Com_Wand_Trail"), (CComponent**)&m_TrailVec[EFFECT_STATE_WAND][0])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Flare_Center")
		, TEXT("Com_Wand_CenterFlare"), (CComponent**)&m_ParticleVec[EFFECT_STATE_WAND][0])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Flare_Horizontal")
		, TEXT("Com_Wand_HorizontalFlare"), (CComponent**)&m_ParticleVec[EFFECT_STATE_WAND][1])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Arres_Wand_Spark")
		, TEXT("Com_Wand_Spark"), (CComponent**)&m_ParticleVec[EFFECT_STATE_WAND][2])))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(2);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Arres_Hit_Center")
		, TEXT("Com_Hit_Center"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][0])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Arres_Hit_Flare")
		, TEXT("Com_Hit_Flare"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][1])))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Arrestomomentum::Add_Effect()
{
	return S_OK;
}

Arrestomomentum* Arrestomomentum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	Arrestomomentum* pInstance = New Arrestomomentum(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created Arrestomomentum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* Arrestomomentum::Clone(void* pArg)
{
	Arrestomomentum* pInstance = New Arrestomomentum(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned Arrestomomentum");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Arrestomomentum::Free()
{
	__super::Free();
}
