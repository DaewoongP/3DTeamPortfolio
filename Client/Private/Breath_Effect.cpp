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

	//if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Breath")))
	//{
	//	if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Breath")
	//		, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Breath/Breath.ME"), m_iLevel))))
	//	{
	//		__debugbreak();
	//		ENDINSTANCE;
	//		return E_FAIL;
	//	}
	//}

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

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_WhiteBurn")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_WhiteBurn")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Breath/WhiteBurn/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_Vomit")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_Vomit")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Breath/Vomit/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_Distortion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_Distortion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Breath/Distortion/"), m_iLevel))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Breath/"));

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
	m_pStopTime = CCoolTime::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CBreath_Effect::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//ADD_IMGUI([&] {Tick_Imgui(fTimeDelta); });
#endif // _DEBUG

	if (false == m_isEnable)
		return;

	__super::Tick(fTimeDelta);
	m_pAttachedTime->Tick(fTimeDelta);
	m_pStopTime->Tick(fTimeDelta);
	//m_pMeshEffect_Breath->Get_Transform()->LookAt(*m_pTargetPos);
	//m_pMeshEffect_Breath->Get_Transform()->Set_Position(*m_pStartPos);

	m_pParticle_Breath_RedFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_RedFire->Get_Transform()->Set_Position(*m_pStartPos);

	m_pParticle_Breath_WhiteFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_WhiteFire->Get_Transform()->Set_Position(*m_pStartPos);

	m_pParticle_Breath_Vomit->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_Vomit->Get_Transform()->Set_Position(*m_pStartPos);

	if (false == m_isBurnStart && false == m_pAttachedTime->IsEnable())
	{
		//m_pParticle_Breath_RedBurn->Get_ShapeModuleRef().ShapeMatrix = m_pParticle_Breath_RedBurn->Get_ShapeModuleRef().ShapeMatrixInit;
		_float3 vDir, vReflectDir;

		vDir = *m_pTargetPos - *m_pStartPos;
		_float3::Reflect(vDir, _float3(0.f, 1.f, 0.f), vReflectDir);
		m_pParticle_Breath_RedBurn->Get_ShapeModuleRef().Set_ShapeLook(*m_pTargetPos, vReflectDir);

		m_pParticle_Breath_RedBurn->Play(*m_pTargetPos);
		m_pParticle_Breath_WhiteBurn->Play(*m_pTargetPos);
		m_pParticle_Breath_Distortion->Play(*m_pTargetPos);
		m_isBurnStart = true;
	}

	if (true == m_isBurnStart)
	{
		//_float3 vDir, vReflectDir;
		//_float3 vStartPos = *m_pStartPos;
		//_float3 vTargetPos = *m_pTargetPos;
		//vDir = vTargetPos - vStartPos;

		m_pParticle_Breath_RedBurn->Get_Transform()->Set_Position(*m_pTargetPos);
		m_pParticle_Breath_WhiteBurn->Get_Transform()->Set_Position(*m_pTargetPos);
		m_pParticle_Breath_Distortion->Get_Transform()->Set_Position(*m_pTargetPos);
		m_pParticle_Breath_Distortion->Get_Transform()->LookAt(*m_pTargetPos, true);
		//_float3::Reflect(vDir, _float3::Up, vReflectDir);
		//m_pParticle_Breath_RedBurn->Get_ShapeModuleRef().Set_ShapeLook(_float3::Zero, vReflectDir);
	}

	if (true == m_isBurnStop && false == m_pStopTime->IsEnable())
	{
		m_pParticle_Breath_RedBurn->Stop();
		m_pParticle_Breath_WhiteBurn->Stop();
		m_pParticle_Breath_Distortion->Stop();
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_iChannel = pGameInstance->Play_Sound(TEXT("BreathBurn.wav"), 0.7f);
	Safe_Release(pGameInstance);

	m_isEnable = true;
	m_pStartPos = pStartPos;
	m_pTargetPos = pTargetPos;
	m_isBurnStart = false;
	m_isBurnStop = false;

	//m_pMeshEffect_Breath->Play(*pStartPos);

	// 시간 = 속력 / 거리
	_float fAttachedTime = (*m_pStartPos - *m_pTargetPos).Length() / m_pParticle_Breath_RedFire->Get_MainModuleRef().fStartSpeed;
	m_pStopTime->Set_MaxCoolTime(fAttachedTime);
	m_pAttachedTime->Set_MaxCoolTime(fAttachedTime);
	m_pAttachedTime->Play_CoolTime();

	m_pParticle_Breath_RedFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_RedFire->Play(*m_pStartPos);

	m_pParticle_Breath_WhiteFire->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_WhiteFire->Play(*m_pStartPos);

	m_pParticle_Breath_Vomit->Get_ShapeModuleRef().Set_ShapeLook(*m_pStartPos, *m_pTargetPos);
	m_pParticle_Breath_Vomit->Play(*m_pStartPos);
}

void CBreath_Effect::Stop()
{
	m_isBurnStop = true;
	m_pStopTime->Play_CoolTime();
	m_pParticle_Breath_RedFire->Stop();
	m_pParticle_Breath_WhiteFire->Stop();
	m_pParticle_Breath_Vomit->Stop();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	pGameInstance->Stop_Sound(m_iChannel);
	Safe_Release(pGameInstance);
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
#endif // _DEBUG

_bool CBreath_Effect::is_Null(const _float3* pStartPos, _float3* pTargetPos)
{
	return nullptr == pStartPos || nullptr == pTargetPos;
}

HRESULT CBreath_Effect::Add_Components()
{
	//FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_MeshEffect_Breath")
	//	, TEXT("Com_MeshEffect_Breath"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Breath)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedFire")
		, TEXT("Com_Particle_Breath_RedFire"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_RedFire)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_WhiteFire")
		, TEXT("Com_Particle_Breath_WhiteFire"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_WhiteFire)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_RedBurn")
		, TEXT("Com_Particle_Breath_RedBurn"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_RedBurn)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_WhiteBurn")
		, TEXT("Com_Particle_Breath_WhiteBurn"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_WhiteBurn)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_Vomit")
		, TEXT("Comit_Particle_Breath_Vomit"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_Vomit)));

	FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Particle_Breath_Distortion")
		, TEXT("Comit_Particle_Breath_Distortion"), reinterpret_cast<CComponent**>(&m_pParticle_Breath_Distortion)));

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

	//Safe_Release(m_pMeshEffect_Breath);
	Safe_Release(m_pParticle_Breath_RedFire);
	Safe_Release(m_pParticle_Breath_WhiteFire);
	Safe_Release(m_pParticle_Breath_RedBurn);
	Safe_Release(m_pParticle_Breath_WhiteBurn);
	Safe_Release(m_pParticle_Breath_Vomit);
	Safe_Release(m_pParticle_Breath_Distortion);
	Safe_Release(m_pAttachedTime);
	Safe_Release(m_pStopTime);
}
