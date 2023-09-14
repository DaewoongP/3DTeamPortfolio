#include "BasicCast.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CBasicCast::CBasicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CBasicCast::CBasicCast(const CBasicCast& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CBasicCast::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBasicCast::Initialize(void* pArg)
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

	m_vTargetPosition = _float3(0, 2, 30);

	// 플레이어가 타겟을 보는 vector를 구함.
	_float3 vDir = m_vTargetPosition - m_vStartPosition;
	vDir.Normalize();

	// 그 vector를 플레이어 기준 반대방향으로 1만큼 이동한 뒤 랜덤값을 잡아줌
	m_vLerpWeight[0] = m_vStartPosition - vDir;
	m_vLerpWeight[0] += _float3(Random_Generator(-100.f, 100.f), 0, 0);

	// 그 vector를 타겟 기준 정방향으로 1만큼 이동한 뒤 랜덤값을 잡아줌
	m_vLerpWeight[1] = m_vTargetPosition + vDir;
	m_vLerpWeight[1] += _float3(Random_Generator(-100.f, 100.f), 0, 0);

	return S_OK;
}

void CBasicCast::Tick(_float fTimeDelta)
{
	if (m_fLiftTime > 0)
	{
		m_fLerpAcc += fTimeDelta / m_fInitLiftTime;
		_float3 movedPos = XMVectorCatmullRom(m_vLerpWeight[0], m_vStartPosition, m_vTargetPosition, m_vLerpWeight[1], m_fLerpAcc);
		if (m_pTransform != nullptr)
			m_pTransform->Set_Position(movedPos);

		if (m_pEffectTrans != nullptr)
			m_pEffectTrans->Set_Position(m_pTransform->Get_Position());
	}
	else 
	{
		if (!m_bDeadTrigger)
		{
			m_bDeadTrigger = true;
			m_pEffect->Play_Particle(_float3(0,0,0));
		}
	}
	__super::Tick(fTimeDelta);
}

void CBasicCast::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBasicCast::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionEnter(CollisionDesc);
}

void CBasicCast::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionStay(CollisionDesc);
}

void CBasicCast::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionExit(CollisionDesc);
}

HRESULT CBasicCast::Add_Components()
{
	return S_OK;
}

HRESULT CBasicCast::Add_Effect()
{
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Default_Magic_Effect"), 
		TEXT("Layer_Effect"), reinterpret_cast<CComponent**>(&m_pEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Default_Magic_Effect)");
		return E_FAIL;
	}

	m_pEffectTrans = m_pEffect->Get_Transform();
	Safe_AddRef(m_pEffectTrans);
	return S_OK;
}

CBasicCast* CBasicCast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBasicCast* pInstance = new CBasicCast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBasicCast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBasicCast::Clone(void* pArg)
{
	CBasicCast* pInstance = new CBasicCast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBasicCast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBasicCast::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pEffectTrans);
		Safe_Release(m_pEffect);
	}
	
}
