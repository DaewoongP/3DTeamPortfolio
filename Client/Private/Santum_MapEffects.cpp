#include "..\Public\Santum_MapEffects.h"
#include "GameInstance.h"
CSantum_MapEffects::CSantum_MapEffects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSantum_MapEffects::CSantum_MapEffects(const CSantum_MapEffects& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSantum_MapEffects::Initialize_Prototype()
{
	std::lock_guard<std::mutex> lock(mtx);

	__super::Initialize_Prototype();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

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



	/// <summary>
	/// ////////////////////
	/// </summary>
	/// <returns></returns>

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid1")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid1")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Sanctum/Liquid/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid2")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid2")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Sanctum/Liquid2/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid3")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid3")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Sanctum/Liquid3/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid4")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid4")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Sanctum/Liquid4/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid5")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid5")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Sanctum/Liquid5/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid6")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid6")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Sanctum/Liquid6/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid7")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Liquid7")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Sanctum/Liquid7/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CSantum_MapEffects::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CSantum_MapEffects::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& MeshEffect : m_MapMeshEffects)
	{
		MeshEffect.fTimeAcc += fTimeDelta;

		if (MeshEffect.fTimeAcc >= MeshEffect.fResetTime)
		{
			_float3 vPos;
			if (true == m_isPhase1)
			{
				vPos.x = Random_Generator(-31.f - 20.f, -31.f + 20.f);
				vPos.y = Random_Generator(-12.f - 20.f, -12.f);
				vPos.z = Random_Generator(134.f - 20.f, 134.f + 20.f);
			}
			else
			{
				vPos.x = Random_Generator(-31.762f - 20.f, -31.762f + 20.f);
				vPos.y = Random_Generator(-41.934f - 20.f, -41.934f);
				vPos.z = Random_Generator(240.366f - 20.f, 240.366f + 20.f);
			}
			MeshEffect.MeshEffect->Play(vPos);
			MeshEffect.fTimeAcc = 0.f;
		}
	}
	
	for (auto& ParticleEffect : m_MapParticleEffects)
	{
		ParticleEffect.fTimeAcc += fTimeDelta;

		if (ParticleEffect.fTimeAcc >= ParticleEffect.fResetTime)
		{
			_float3 vPos;
			if (true == m_isPhase1)
			{
				vPos.x = Random_Generator(-31.f - 20.f, -31.f + 20.f);
				vPos.y = Random_Generator(-12.f - 20.f, -12.f);
				vPos.z = Random_Generator(134.f - 20.f, 134.f + 20.f);
			}
			else
			{
				vPos.x = Random_Generator(-31.762f - 20.f, -31.762f + 20.f);
				vPos.y = Random_Generator(-41.934f - 20.f, -41.934f);
				vPos.z = Random_Generator(240.366f - 20.f, 240.366f + 20.f);
			}

			_float4x4 matRotX = _float4x4::MatrixRotationX(XMConvertToRadians(Random_Generator(0.f, 360.f)));
			ParticleEffect.ParticleEffect->Get_Transform()->Set_WorldMatrix(matRotX);
			ParticleEffect.ParticleEffect->Play(vPos);
			ParticleEffect.fTimeAcc = 0.f;
		}
	}
}

void CSantum_MapEffects::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CSantum_MapEffects::Add_Components()
{
	m_MapMeshEffects.resize(28);
	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod01")
		, TEXT("veverv"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[0].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod02")
		, TEXT("23rfsdf"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[1].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt01")
		, TEXT("xcvxcvqdsf"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[2].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt02")
		, TEXT("2r13fec2w"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[3].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA01")
		, TEXT("4tgbrbheger`"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[4].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA02")
		, TEXT("456456fgdg"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[5].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB01")
		, TEXT("xcvdcwe"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[6].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB02")
		, TEXT("567i5urgh"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[7].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl01")
		, TEXT("cvbrfwefwe"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[8].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl02")
		, TEXT("rtb5b6n73r"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[9].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl03")
		, TEXT("yhnynergerf"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[10].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl04")
		, TEXT("345yertfghfvbcvb"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[11].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl05")
		, TEXT("nmnmfbvhfh"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[12].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl05")
		, TEXT("hnj5j7j7j7"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[13].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl05")
		, TEXT("sdc2454556567"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[14].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod01")
		, TEXT("123"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[15].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GodRod02")
		, TEXT("234"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[16].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt01")
		, TEXT("345"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[17].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooBolt02")
		, TEXT("456"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[18].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA01")
		, TEXT("789"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[19].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleA02")
		, TEXT("123412345123r"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[20].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB01")
		, TEXT("sdfsdfdsfsdf"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[21].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooStapleB02")
		, TEXT("sdfsdfdsfsdf"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[22].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl01")
		, TEXT("Com_svcvkmskvmeSpiralx3_2"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[23].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl02")
		, TEXT("tynntyevrf"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[24].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl03")
		, TEXT("wdcerg4t6yu46h"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[25].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl04")
		, TEXT("ascvbvbncvbnmcvbnm"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[26].MeshEffect)));

	FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_MeshEffect_Conjured_GooSwirl05")
		, TEXT("zxcvwertg346h5eyhnertyh"), reinterpret_cast<CComponent**>(&m_MapMeshEffects[27].MeshEffect)));

	m_MapParticleEffects.resize(21);
	_uint i = 0;
	_uint iLiquidIndex = 1;
	for (auto& MapParticleEffects : m_MapParticleEffects)
	{
		wstring wstrProtoTag = TEXT("Prototype_GameObject_Particle_Liquid") + to_wstring(iLiquidIndex);
		FAILED_CHECK(CComposite::Add_Component(LEVEL_SANCTUM, wstrProtoTag.data()
			, Generate_HashtagW().data(), reinterpret_cast<CComponent**>(&m_MapParticleEffects[i].ParticleEffect)));
		++iLiquidIndex;
		++i;
		if (7 < iLiquidIndex)
			iLiquidIndex = 1;
	}

	for (auto& MeshEffect : m_MapMeshEffects)
	{
		MeshEffect.fResetTime = Random_Generator(15.f, 25.f);
	}

	for (auto& ParticleEffect : m_MapParticleEffects)
	{
		ParticleEffect.ParticleEffect->Disable();
		ParticleEffect.fResetTime = Random_Generator(15.f, 25.f);
	}

	return S_OK;
}

CSantum_MapEffects* CSantum_MapEffects::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSantum_MapEffects* pInstance = New CSantum_MapEffects(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSantum_MapEffects");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSantum_MapEffects::Clone(void* pArg)
{
	CSantum_MapEffects* pInstance = New CSantum_MapEffects(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSantum_MapEffects");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSantum_MapEffects::Free(void)
{
	__super::Free();

	for (auto& MeshEffect : m_MapMeshEffects)
	{
		Safe_Release(MeshEffect.MeshEffect);
	}
	m_MapMeshEffects.clear();

	for (auto& ParticleEffect : m_MapParticleEffects)
	{
		Safe_Release(ParticleEffect.ParticleEffect);
	}
	m_MapMeshEffects.clear();
}