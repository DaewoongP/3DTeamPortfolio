#include "EnergyBall.h"

#include "GameInstance.h"

#include "MagicBall.h"
#include "MagicSlot.h"
#include "Protego.h"

CEnergyBall::CEnergyBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnergyBall::CEnergyBall(const CEnergyBall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnergyBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_EnergyBall_ChargeDistortion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_EnergyBall_ChargeDistortion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/EnergyBall/ChargeDistortion/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_EnergyBall_ChargeDarkBall")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_EnergyBall_ChargeDarkBall")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/EnergyBall/ChargeDarkBall/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod01")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GodRod01.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod02")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GodRod02.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt01")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooBolt01.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt02")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooBolt02.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA01")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooStapleA01.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA02")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooStapleA02.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB01")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooStapleB01.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB02")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooStapleB02.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl01")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooSwirl01.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl02")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooSwirl02.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl03")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl03")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooSwirl03.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl03")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl03")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooSwirl03.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl04")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl04")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooSwirl04.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl05")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl05")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Conjured/GooSwirl05.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Projectile_White_Inner_Ball")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Projectile_White_Inner_Ball")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Projectile_White/Inner_Ball.ME"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CEnergyBall::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CEnergyBall Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Make_Magics()))
		return E_FAIL;

	/* Set Class Parameters */
	ENERGYBALLINITDESC* pInitDesc = static_cast<ENERGYBALLINITDESC*>(pArg);
	m_fActionProtegoTime = pInitDesc->fActionProtegoTime;
	m_DeathFunction = pInitDesc->DeathFunction;

	m_pTransform->Set_Position(pInitDesc->vPosition);
	m_pTransform->Set_Scale(_float3(0.5f, 0.5f, 0.5f));

	m_pMeshEffect_Inner_Ball->Get_Transform()->Set_Scale(_float3(1.3f, 1.3f, 1.3f));
	m_pMeshEffect_Inner_Ball->Set_LifeTime(7.f);
	return S_OK;
}

void CEnergyBall::Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Tick(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	
	if (true == m_isFirst)
	{
		if (false == m_isEffectStart)
		{
			m_isEffectStart = true;
			m_pParticle_EnergyBall_ChargeDistortion->Play(m_pTransform->Get_Position());
			m_pParticle_EnergyBall_ChargeDarkBall->Play(m_pTransform->Get_Position());
			for (auto& Conjureds : m_pMeshEffect_Conjured)
			{
				if (nullptr == Conjureds)
					continue;

				Conjureds->Play(m_pTransform->Get_Position());
			}
				
			//m_pMesheffect->Play();
		}
		
		if (m_fActionProtegoTime < m_fTimeAcc)
		{
			m_pParticle_EnergyBall_ChargeDistortion->Stop();
			m_pParticle_EnergyBall_ChargeDarkBall->Stop();
			m_pMeshEffect_Inner_Ball->Play(m_pTransform->Get_Position());
			CMagicBall* pMagicBall = m_pMagicSlot->Action_Magic_Basic(1, this, nullptr, COL_MAGIC, COL_SHIELD_ENEMY);
			static_cast<CProtego*>(pMagicBall)->Set_Scale(6.7f);
			m_isFirst = false;
		}
	}

	if (9.5f <= m_fTimeAcc)
	{
		m_pMeshEffect_Inner_Ball->Stop();
		m_isEnable = false;
	}
		

	if (true == m_isDead)
	{
		m_pMeshEffect_Inner_Ball->Stop();
		_bool isFinishDeathFunction = m_DeathFunction(fTimeDelta);
		if (true == isFinishDeathFunction)
			m_isEnable = false;
	}
}

void CEnergyBall::Late_Tick(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CEnergyBall::Render()
{

	return S_OK;
}

void CEnergyBall::Reset(const ENERGYBALLINITDESC& tagResetDesc)
{
	m_isFirst = true;
	m_isEnable = true;
	m_isDead = false;
	m_fTimeAcc = 0.f;
	m_fActionProtegoTime = tagResetDesc.fActionProtegoTime;
	m_pTransform->Set_Position(tagResetDesc.vPosition);
	m_DeathFunction = tagResetDesc.DeathFunction;
	m_isEffectStart = false;
	Make_Magics();
}

void CEnergyBall::Set_Protego_Collision(CTransform* pTransform, CEnemy::ATTACKTYPE eType) const
{
	if (eType == CEnemy::ATTACK_BREAK || eType == CEnemy::ATTACK_SUPERBREAK)
	{
		m_isDead = true;
	}
}

HRESULT CEnergyBall::Make_Magics()
{
	////Basic Magic Protego
	//{
	//	_uint iMagicType = rand() % 3 + 1; // 1 ~ 3(Yellow, Purple, Red)

	//	CMagic::MAGICDESC magicInitDesc;
	//	magicInitDesc.eBuffType = BUFF_PROTEGO;
	//	magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
	//	magicInitDesc.eMagicType = (CMagic::MAGIC_TYPE)iMagicType;
	//	magicInitDesc.eMagicTag = PROTEGO;
	//	magicInitDesc.fInitCoolTime = 0.f;
	//	magicInitDesc.iDamage = 0;
	//	magicInitDesc.fLifeTime = 7.f;
	//	m_pMagicSlot->Add_Magics(magicInitDesc);

	//	m_pMeshEffect_Inner_Ball->Set_LifeTime(7.f);
	//	switch (magicInitDesc.eMagicType)
	//	{
	//	case Client::CMagic::MT_NOTHING:
	//		__debugbreak();
	//		break;
	//	case Client::CMagic::MT_YELLOW:
	//		m_pMeshEffect_Inner_Ball->Set_StartColor(_float4(1.f, 1.f, 0.f, 1.f));
	//		m_pMeshEffect_Inner_Ball->Set_EndColor(_float4(1.f, 1.f, 0.f, 1.f));
	//		//m_vColor1 = { 1.f, 1.f, 0.f, 1.f };
	//		//m_vColor2 = { 1.f, 0.5f, 0.f, 1.f };
	//		break;
	//	case Client::CMagic::MT_PURPLE:
	//		m_pMeshEffect_Inner_Ball->Set_StartColor(_float4(0.f, 0.f, 1.f, 1.f));
	//		m_pMeshEffect_Inner_Ball->Set_EndColor(_float4(0.f, 0.f, 1.f, 1.f));
	//		/*m_vColor1 = { 0.f, 0.f, 1.f, 1.f };
	//		m_vColor2 = { 0.5f, 0.f, 0.5f, 1.f };*/
	//		break;
	//	case Client::CMagic::MT_RED:
	//		m_pMeshEffect_Inner_Ball->Set_StartColor(_float4(1.f, 0.f, 0.f, 1.f));
	//		m_pMeshEffect_Inner_Ball->Set_EndColor(_float4(1.f, 0.f, 0.f, 1.f));
	//		/*m_vColor1 = { 1.f, 0.f, 0.f, 1.f };
	//		m_vColor2 = { 1.f, 0.5f, 0.f, 1.f };*/
	//		break;
	//	case Client::CMagic::MT_ALL:
	//		m_pMeshEffect_Inner_Ball->Set_StartColor(_float4(0.f, 0.f, 1.f, 1.f));
	//		m_pMeshEffect_Inner_Ball->Set_EndColor(_float4(0.f, 0.f, 1.f, 1.f));
	//		/*m_vColor1 = { 0.f, 0.f, 1.f, 1.f };
	//		m_vColor2 = { 1.f, 0.f, 1.f, 1.f };*/
	//		break;
	//	default:
	//		__debugbreak();
	//		break;
	//	}
	//}

	return S_OK;
}

HRESULT CEnergyBall::Add_Components()
{
	/* For.MagicSlot */
	FAILED_CHECK(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
		TEXT("Com_MagicSlot"), reinterpret_cast<CComponent**>(&m_pMagicSlot)));

	/* For. Partices */
	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_EnergyBall_ChargeDistortion"),
		TEXT("Com_Particle_EnergyBall_ChargeDistortion"), reinterpret_cast<CComponent**>(&m_pParticle_EnergyBall_ChargeDistortion)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_EnergyBall_ChargeDarkBall"),
		TEXT("Com_Particle_EnergyBall_ChargeDarkBall"), reinterpret_cast<CComponent**>(&m_pParticle_EnergyBall_ChargeDarkBall)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod01"),
		TEXT("Com_MeshEffect_Conjured_GodRod01"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[0])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod02"),
		TEXT("Com_MeshEffect_Conjured_GodRod02"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[1])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt01"),
		TEXT("Com_MeshEffect_Conjured_GooBolt01"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[2])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt02"),
		TEXT("Com_MeshEffect_Conjured_GooBolt02"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[3])));

	//FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA02"),
	//	TEXT("Com_MeshEffect_Conjured_GooStapleA01"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[4])));

	//FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA02"),
	//	TEXT("Com_MeshEffect_Conjured_GooStapleA02"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[5])));

	//FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB01"),
	//	TEXT("Com_MeshEffect_Conjured_GooStapleB01"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[6])));

	//FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB02"),
	//	TEXT("Com_MeshEffect_Conjured_GooStapleB02"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[7])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl01"),
		TEXT("Com_MeshEffect_Conjured_GooSwirl01"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[8])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl02"),
		TEXT("Com_MeshEffect_Conjured_GooSwirl02"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[9])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl03"),
		TEXT("Com_MeshEffect_Conjured_GooSwirl03"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[10])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl04"),
		TEXT("Com_MeshEffect_Conjured_GooSwirl04"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[11])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl05"),
		TEXT("Com_MeshEffect_Conjured_GooSwirl05"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Conjured[12])));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Projectile_White_Inner_Ball"),
		TEXT("Com_MeshEffect_Projectile_White_Inner_Ball"), reinterpret_cast<CComponent**>(&m_pMeshEffect_Inner_Ball)));


	return S_OK;
}

CEnergyBall* CEnergyBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnergyBall* pInstance = New CEnergyBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnergyBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnergyBall::Clone(void* pArg)
{
	CEnergyBall* pInstance = New CEnergyBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnergyBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnergyBall::Free()
{
	__super::Free();

	Safe_Release(m_pMagicSlot);
	Safe_Release(m_pParticle_EnergyBall_ChargeDistortion);
	Safe_Release(m_pParticle_EnergyBall_ChargeDarkBall);
	Safe_Release(m_pMeshEffect_Inner_Ball);
}
