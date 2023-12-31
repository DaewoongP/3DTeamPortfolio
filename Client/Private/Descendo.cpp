#include "Descendo.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CDescendo::CDescendo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CDescendo::CDescendo(const CDescendo& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CDescendo::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_MagicTrail_Winga_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_MagicTrail_Winga_Effect"),
			CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Wingardium/Wingardium.trail"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicTrail/BasicTrail.trail"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_RainBow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_RainBow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandFlare/RainBow/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_Spread")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_Spread")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandFlare/Spread/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandFlare/WandGlow/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlowLarge")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlowLarge")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_WandFlare/WandGlow_Large/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Accio_HitMain")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Accio_HitMain")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Accio/Hit_Main/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_Default_GlowBall")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Defatul_Default_GlowBall")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default/Default_GlowBall/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(iLevel, TEXT("Prototype_GameObject_Descendo_Down_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(iLevel, TEXT("Prototype_GameObject_Descendo_Down_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Descendo/Down_Effect/"), iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	//if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Decendo_MeshEffect")))
	//{
	//	if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Decendo_MeshEffect")
	//		, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Decendo/Decendo.ME"), m_iLevel))))
	//	{
	//		ENDINSTANCE;
	//		return E_FAIL;
	//	}
	//}
	ENDINSTANCE;


	return S_OK;
}

HRESULT CDescendo::Initialize(void* pArg)
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
	_float4 vColor = _float4(255.f / 255.f, 102.f / 255.f, 102.f / 255.f, 1);
	for (int i = 0; i < EFFECT_STATE_END; i++)
	{
		for (_uint j = 0; j < m_ParticleVec[i].size(); j++)
		{
			m_ParticleVec[i].data()[j]->Get_MainModuleRef().vStartColor = vColor;
		}

		for (_uint j = 0; j < m_TrailVec[i].size(); j++)
		{
			m_TrailVec[i].data()[j]->Set_Trail_HeadColor(vColor.xyz());
			m_TrailVec[i].data()[j]->Set_Trail_TailColor(vColor.xyz());
		}
	}

	Ready_Shake(30.0f, 2.0f, 0.1f);

	return S_OK;
}

void CDescendo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDescendo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CDescendo::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//���̶� �浹������?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag,TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
#pragma region ī�޶� ����ũ
		BEGININSTANCE;

		pGameInstance->Set_Shake(
			CCamera_Manager::SHAKE_PRIORITY_2,
			CCamera_Manager::SHAKE_TYPE_TRANSLATION,
			CCamera_Manager::SHAKE_AXIS_LOOK,
			CEase::IN_EXPO,
			5.0f,
			0.2f,
			Shake_Power(CollisionEventDesc.pOtherTransform->Get_Position()),
			CCamera_Manager::SHAKE_POWER_DECRECENDO);

		ENDINSTANCE;
#pragma endregion

	//	m_pMeshEffect->Play(m_vStartPosition);
	}
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CDescendo::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CDescendo::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CDescendo::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	CLight::LIGHTDESC LightDesc;
	LightDesc.eType = CLight::TYPE_POINT;
	LightDesc.fRange = 5.f;
	m_vLightColor = LightDesc.vDiffuse = ToColor(250.f, 48.f, 48.f, 192.f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPos = m_pTransform->Get_Position().TransCoord();
	pGameInstance->Add_Light(LightDesc, &m_pLight);
	Safe_Release(pGameInstance);

	return S_OK;
}

void CDescendo::Ready_Begin()
{
	__super::Ready_Begin();
}

void CDescendo::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CDescendo::Ready_CastMagic()
{
	Ready_SplineMove_Accio(m_TrailVec[EFFECT_STATE_MAIN][0],_float3(0,1,0));
	ADD_DECREASE_LIGHT(m_vStartPosition, 50.f, 0.1f, m_vLightColor);
	__super::Ready_CastMagic();
}

void CDescendo::Ready_Dying()
{
	m_ParticleVec[EFFECT_STATE_MAIN][4]->Disable();
	__super::Ready_Dying();
}

void CDescendo::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CDescendo::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CDescendo::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_TrailVec[EFFECT_STATE_MAIN][0]->Spline_Move(m_vSplineLerp[0], m_vStartPosition, m_vEndPosition, m_vSplineLerp[1], m_fLerpAcc);
		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
		m_ParticleVec[EFFECT_STATE_MAIN][4]->Get_Transform()->Set_Position(m_TrailVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Get_Position());
		m_pLight->Set_Position(m_TrailVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Get_Position().TransCoord());
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
}

void CDescendo::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}

HRESULT CDescendo::Add_Components()
{
	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_BasicCast_Wand_Trail_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_Wand_Glow_Effect"),
		TEXT("Com_WandGlow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_Wand_Glow_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	m_ParticleVec[EFFECT_STATE_MAIN].resize(5);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_MagicTrail_Winga_Effect"),
		TEXT("Com_MainTrail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_MagicTrail_Winga_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_WandFlare_RainBow"),
		TEXT("Com_RainBow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_WandFlare_RainBow)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_WandFlare_Spread"),
		TEXT("Com_Spread"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_WandFlare_Spread)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlow"),
		TEXT("Com_WandGlow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_WandFlare_WandGlow)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_WandFlare_WandGlowLarge"),
		TEXT("Com_WandGlowLarge"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_WandFlare_WandGlowLarge)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Defatul_Default_GlowBall"),
		TEXT("Com_MainEffect"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][4]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Defatul_Default_GlowBall)");
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(2);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Accio_HitMain"),
		TEXT("Com_HitDust_Effect"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Accio_HitMain)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Descendo_Down_Effect"),
		TEXT("Com_Down_Effect"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Descendo_Down_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	
	//if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Decendo_MeshEffect"),
	//	TEXT("Com_Mesh_Effect"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
	//{
	//	MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Decendo_MeshEffect)");
	//	__debugbreak();
	//	return E_FAIL;
	//}

	return S_OK;
}

CDescendo* CDescendo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CDescendo* pInstance = New CDescendo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CDescendo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDescendo::Clone(void* pArg)
{
	CDescendo* pInstance = New CDescendo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDescendo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDescendo::Free()
{
	__super::Free();
	if(m_isCloned)
	{
		Safe_Release(m_pMeshEffect);
	}
}
