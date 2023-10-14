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
{
}

HRESULT CFinisher::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

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
	return S_OK;
}

void CFinisher::Tick(_float fTimeDelta)
{
	if (m_pTargetWorldMatrix != nullptr)
		m_CurrentTargetMatrix = /*(*m_pTargetOffsetMatrix) * */(*m_pTargetWorldMatrix);

	if (m_pWeaponWorldMatrix != nullptr)
		m_CurrentWeaponMatrix = (*m_pWeaponOffsetMatrix) * (*m_pWeaponWorldMatrix);
	Tick_MagicBall_State(fTimeDelta);
	CGameObject::Tick(fTimeDelta);
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
	for (int i = 0; i < 3; i++)
	{
		m_pTrail[i]->Disable();
	}
	return S_OK;
}

void CFinisher::Ready_Begin()
{
	__super::Ready_Begin();
}

void CFinisher::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CFinisher::Ready_CastMagic()
{
	_float distance = 30.0f;
	if (m_pTarget == nullptr)
	{
		BEGININSTANCE;
		_float4 vMouseOrigin, vMouseDirection;
		_float3 vMouseWorldPickPosition, vDirStartToPicked;
		//마우스 피킹지점 찾기
		if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
		{
			Safe_Release(pGameInstance);
			ENDINSTANCE;
			return;
		}
		ENDINSTANCE;

		//가상의 지점으로 레이를 쏨.
		vMouseWorldPickPosition = vMouseOrigin.xyz() + vMouseDirection.xyz() * 100;
		//방향을 구함
		vDirStartToPicked = (vMouseWorldPickPosition - m_vStartPosition);
		vDirStartToPicked.Normalize();
		//목표지점
		m_vEndPosition = m_vStartPosition + vDirStartToPicked * distance;
	}

	for (int i = 0; i < 3; i++)
	{
		_float3 vWeight[2];
		vWeight[0] = _float3(-2.f, -0.5f, -2.f);
		vWeight[1] = _float3(2.f,0.5f, 2.f);

		m_pTrail[i]->Set_Threshold(0.2f);
		m_pTrail[i]->Ready_LightningStrike(m_vEndPosition + _float3(0, 20, 0), m_vEndPosition - _float3(0,10,0), vWeight, 20);
		m_pTrail[i]->Enable(m_vEndPosition);
	}
	m_pTransform->Set_Position(m_vEndPosition);
}

void CFinisher::Ready_Dying()
{
	for (int i = 0; i < m_TrailVec[EFFECT_STATE_HIT].size(); i++)
	{
		m_TrailVec[EFFECT_STATE_HIT].data()[i]->Enable(m_vEndPosition);
	}
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_HIT].size(); i++)
	{
		m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Enable(m_vEndPosition);
		m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Play(m_vEndPosition);
	}
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	pGameInstance->Add_InstanceLight(m_vEndPosition, 50.f, 1.f, _float4(1.f, 1.f, 1.f, 1.f));
	Safe_Release(pGameInstance);
}

void CFinisher::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CFinisher::Tick_DrawMagic(_float fTimeDelta)
{

}

void CFinisher::Tick_CastMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CFinisher::Tick_Dying(_float fTimeDelta)
{
	//트레일 이어주기
	for (int i = 0; i < m_TrailVec[EFFECT_STATE_WAND].size(); i++)
	{
		m_TrailVec[EFFECT_STATE_WAND].data()[i]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	}
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_WAND].size(); i++)
	{
		m_ParticleVec[EFFECT_STATE_WAND].data()[i]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	}

	if (!m_ParticleVec[EFFECT_STATE_HIT].data()[6]->IsEnable())
		Do_MagicBallState_To_Next();
}

HRESULT CFinisher::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
		, TEXT("Com_Wand_Trail"), (CComponent**)&m_TrailVec[EFFECT_STATE_WAND][0])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Wand_Lightning_Effect")
		, TEXT("Com_Wand_Lightning"), (CComponent**)&m_ParticleVec[EFFECT_STATE_WAND][0])))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(7);
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Spark_Blue_Effect")
		, TEXT("Com_Spark_Blue_Effect"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][0])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Spark_Green_Effect")
		, TEXT("Com_Spark_Green_Effect"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][1])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Spark_Red_Effect")
		, TEXT("Com_Spark_Red_Effect"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][2])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Lightning_Line_Effect")
		, TEXT("Com_Line_Effect"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][3])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Lightning_Flare_Center_Effect")
		, TEXT("Com_FlareCenter_Effect"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][4])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Lightning_Flare_Spread_Effect")
		, TEXT("Com_FlareSpread_Effect"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][5])))
	{
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_Lightning_Dust_Effect")
		, TEXT("Com_Dust_Effect"), (CComponent**)&m_ParticleVec[EFFECT_STATE_HIT][6])))
	{
		__debugbreak();
		return E_FAIL;
	}

	//별도 
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
	}
}
