#include "..\Public\Breath_Effect.h"
#include "GameInstance.h"
#include "CoolTime.h"
CBreath_Effect::CBreath_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBreath_Effect::CBreath_Effect(const CBreath_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CBreath_Effect::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_iLevel = iLevel;

	BEGININSTANCE;

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedFire")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedFire")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Breath/RedFire/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedFire")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedFire")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Breath/RedFire/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_BlackFire")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_BlackFire")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Breath/BlackFire/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_WhiteFire")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_WhiteFire")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Breath/WhiteFire/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedBurn")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedBurn")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Breath/RedBurn/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CBreath_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pAttachedTime = CCoolTime::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CBreath_Effect::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	ADD_IMGUI([&] {Tick_Imgui(fTimeDelta); });
#endif // _DEBUG

	if (false == m_isEnable)
		return;

	__super::Tick(fTimeDelta);

	m_pParticle_Breath_RedFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_RedFire->Get_Transform()->Set_Position(*m_pStartPos);

	m_pParticle_Breath_BlackFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_BlackFire->Get_Transform()->Set_Position(*m_pStartPos);

	m_pParticle_Breath_WhiteFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_WhiteFire->Get_Transform()->Set_Position(*m_pStartPos);

	if (false == m_isStart && false == m_pAttachedTime->IsEnable())
	{
		m_pParticle_Breath_RedBurn->Play(*m_pTargetPos);
		m_isStart = true;
	}

	if (true == m_isStart)
	{
		m_pParticle_Breath_RedBurn->Get_Transform()->Set_Position(*m_pTargetPos);
	}
}

void CBreath_Effect::Late_Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Late_Tick(fTimeDelta);
}

void CBreath_Effect::Play(const _float3* pStartPos, _float3* pTargetPos, _float fTerrainYPos)
{
	if (true == is_Null(pStartPos, pTargetPos))
		return;

	m_isEnable = true;
	m_pStartPos = pStartPos;
	m_pTargetPos = pTargetPos;
	m_isStart = false;

	// 시간 = 속력 / 거리
	_float fAttachedTime = (*m_pStartPos - *m_pTargetPos).Length() / m_pParticle_Breath_RedFire->Get_MainModuleRef().fStartSpeed;
	m_pAttachedTime->Set_MaxCoolTime(fAttachedTime);
	m_pAttachedTime->Play_CoolTime();

	m_pParticle_Breath_RedFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_RedFire->Play(*m_pStartPos);

	m_pParticle_Breath_BlackFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_BlackFire->Play(*m_pStartPos);

	m_pParticle_Breath_WhiteFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_WhiteFire->Play(*m_pStartPos);
}

void CBreath_Effect::Stop()
{
	m_pParticle_Breath_RedFire->Stop();
	m_pParticle_Breath_BlackFire->Stop();
	m_pParticle_Breath_WhiteFire->Stop();
	m_pParticle_Breath_RedBurn->Stop();
}

void CBreath_Effect::Reset()
{
}

#ifdef _DEBUG
void CBreath_Effect::Tick_Imgui(_float fTimeDelta)
{
	CImGui_Manager::NextWindow_LeftBottom();
	ImGui::Begin("Breath_Effect");
	ImGui::End();
}
_bool CBreath_Effect::is_Null(const _float3* pStartPos, _float3* pTargetPos)
{
	return nullptr == pStartPos || nullptr == pTargetPos;
}
#endif // _DEBUG

HRESULT CBreath_Effect::Add_Components()
{
	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedFire")
		, TEXT("Com_Particle_Breath_RedFire"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_RedFire)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_BlackFire")
		, TEXT("Com_Particle_Breath_BlackFire"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_BlackFire)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_WhiteFire")
		, TEXT("Com_Particle_Breath_WhiteFire"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_WhiteFire)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedBurn")
		, TEXT("Com_Particle_Breath_RedBurn "), reinterpret_cast<CComponent**>(&m_pParticle_Breath_RedBurn)));

	return S_OK;
}

CBreath_Effect* CBreath_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CBreath_Effect* pInstance = New CBreath_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Create Brath_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBreath_Effect::Clone(void* pArg)
{
	CBreath_Effect* pInstance = New CBreath_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone Brath_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreath_Effect::Free(void)
{
	__super::Free();

	Safe_Release(m_pParticle_Breath_RedFire);
	Safe_Release(m_pParticle_Breath_BlackFire);
	Safe_Release(m_pParticle_Breath_WhiteFire);
	Safe_Release(m_pParticle_Breath_RedBurn);
	Safe_Release(m_pAttachedTime);
}
