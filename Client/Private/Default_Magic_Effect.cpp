
#include "Default_Magic_Effect.h"
#include "GameInstance.h"

CDefault_Magic_Effect::CDefault_Magic_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDefault_Magic_Effect::CDefault_Magic_Effect(const CDefault_Magic_Effect& rhs)
	: CGameObject(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CDefault_Magic_Effect::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	m_iLevel = iLevel;

	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_TestTrail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_TestTrail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_TestParticle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_TestParticle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Default_Spell"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CDefault_Magic_Effect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));
	m_pTransform->Set_Speed(20.f);
	m_pParticleTransform = m_pParticleSystem->Get_Transform();
	Safe_AddRef(m_pParticleTransform);

	m_pTrailTransform = m_pTrail->Get_Transform();
	Safe_AddRef(m_pTrailTransform);
	
	return S_OK;
}

void CDefault_Magic_Effect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

#ifdef _DEBUG
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_UPARROW))
	{
		m_pTransform->Go_Straight(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHTARROW))
	{
		m_pTransform->Go_Right(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFTARROW))
	{
		m_pTransform->Go_Left(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_DOWNARROW))
	{
		m_pTransform->Go_Backward(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE))
	{
		Play_Particle(m_pTransform->Get_Position());
	}

	Safe_Release(pGameInstance);
#endif // DEBUG
}

void CDefault_Magic_Effect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	m_pTrailTransform->Set_Position(m_pTransform->Get_Position());
}

void CDefault_Magic_Effect::Play_Particle(_float3 vPos)
{
	m_pParticleSystem->Play();
	m_pParticleTransform->Set_Position(vPos);
}

void CDefault_Magic_Effect::Set_Position(_float3 vPos)
{
	m_pTransform->Set_Position(vPos);
}

void CDefault_Magic_Effect::Enable_Trail(_bool _isEnable)
{
	(true == _isEnable) ? m_pTrail->Enable() : m_pTrail->Disable();
}

HRESULT CDefault_Magic_Effect::Add_Components()
{
	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_TestTrail")
		, TEXT("Com_Trail"), (CComponent**)&m_pTrail)))
		return E_FAIL;

	if (FAILED(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_TestParticle")
		, TEXT("Com_Particle"), (CComponent**)&m_pParticleSystem)))
		return E_FAIL;

	return S_OK;
}

CDefault_Magic_Effect* CDefault_Magic_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CDefault_Magic_Effect* pInstance = New CDefault_Magic_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CDefault_Magic_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefault_Magic_Effect::Clone(void* pArg)
{
	CDefault_Magic_Effect* pInstance = New CDefault_Magic_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDefault_Magic_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefault_Magic_Effect::Free()
{
	if (true == m_isCloned)
	{
		__super::Free();

		Safe_Release(m_pTrail);
		Safe_Release(m_pParticleSystem);

		Safe_Release(m_pParticleTransform);
		Safe_Release(m_pTrailTransform);
	}
}
