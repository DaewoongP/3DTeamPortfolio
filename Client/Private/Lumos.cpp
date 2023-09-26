#include "Lumos.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CLumos::CLumos(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CLumos::CLumos(const CLumos& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CLumos::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;
	BEGININSTANCE;

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lumos_Wand_Glow_Red_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lumos_Wand_Glow_Red_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lumos/Red_Light"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lumos_Wand_Glow_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Lumos_Wand_Glow_Effect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Lumos/Glow"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;


	return S_OK;
}

HRESULT CLumos::Initialize(void* pArg)
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

void CLumos::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fEndTimer -= fTimeDelta;

	if(m_fEndTimer<=0)
		Set_MagicBallState(MAGICBALL_STATE_DYING);

	//위치 동기화
	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandGlowEffect->Get_Transform()->Set_Position(vWandPosition);
	m_pWandGlowRedEffect->Get_Transform()->Set_Position(vWandPosition);
}

void CLumos::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CLumos::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
}

void CLumos::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CLumos::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

void CLumos::Lumos_Tick(_float fTimeDelta)
{
	m_fEndTimer = 0.3f;
}

void CLumos::Ready_Begin()
{
	m_pWandGlowEffect->Disable();
	m_pWandGlowRedEffect->Disable();
}

void CLumos::Ready_DrawMagic()
{
}

void CLumos::Ready_CastMagic()
{
	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	m_pWandGlowEffect->Enable();
	m_pWandGlowEffect->Play(vWandPosition);
	m_pWandGlowRedEffect->Enable();
	m_pWandGlowRedEffect->Play(vWandPosition);
}

void CLumos::Ready_Dying()
{
	//여기에 자연스레 사라지게 해주기 만들기
}

void CLumos::Tick_Begin(_float fTimeDelta)
{
	//베이직 캐스트는 비긴 없습니다.
	Do_MagicBallState_To_Next();
}

void CLumos::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CLumos::Tick_CastMagic(_float fTimeDelta)
{
 
}

void CLumos::Tick_Dying(_float fTimeDelta)
{
	//사망하러 가겠습니다.
	if (!m_pWandGlowEffect->IsEnable()&&
		!m_pWandGlowRedEffect->IsEnable())
	{
		Do_MagicBallState_To_Next();
	}
}

HRESULT CLumos::Add_Components()
{
	return S_OK;
}

HRESULT CLumos::Add_Effect()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Lumos_Wand_Glow_Effect"),
		TEXT("Com_WandGlow"), reinterpret_cast<CComponent**>(&m_pWandGlowEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Lumos_Wand_Glow_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Lumos_Wand_Glow_Red_Effect"),
		TEXT("Com_WandGlowRed"), reinterpret_cast<CComponent**>(&m_pWandGlowRedEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Lumos_Wand_Glow_Red_Effect)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

CLumos* CLumos::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CLumos* pInstance = New CLumos(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CLumos");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLumos::Clone(void* pArg)
{
	CLumos* pInstance = New CLumos(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLumos");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLumos::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pWandGlowEffect);
		Safe_Release(m_pWandGlowRedEffect);
	}
	
}
