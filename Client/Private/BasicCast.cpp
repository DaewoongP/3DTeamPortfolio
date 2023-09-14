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

	//만약 타겟이 존재하지 않는다면?
	if (m_pTarget == nullptr)
	{
		// 마우스가 찍는 방향이 곧 마법 발사 방향이다.
		// 마우스origin,dir을 가져와 origin에서 dir만큼 마우스 origin 기준 dist의 크기로 이동.
		// 플레이어가 그 점을 보는 방향벡터를 만들어 플레이어 기준 dist의 크기로 이동.

		BEGININSTANCE;
		_float4 vMouseOrigin, vMouseDirection;
		_float3 vMouseWorldPickPosition, vDirStartToPicked;
		if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
		{
			Safe_Release(pGameInstance);
			return false;
		}
		ENDINSTANCE;

		vMouseWorldPickPosition = vMouseOrigin.xyz() + vMouseDirection.xyz() * 10000;
		vDirStartToPicked = (vMouseWorldPickPosition - m_vStartPosition);
		vDirStartToPicked.Normalize();
		m_vTargetPosition = vDirStartToPicked * m_fDistance;
	}
	else 
	{
		//이거 근데 몹의 발위치로 지금 설정돼있을듯함.
		m_vTargetPosition = m_pTarget->Get_Position();
	}

	// 플레이어가 타겟을 보는 vector를 구함.
	_float3 vDir = m_vTargetPosition - m_vStartPosition;
	vDir.Normalize();

	// 그 vector를 플레이어 기준 반대방향으로 1만큼 이동한 뒤 랜덤값을 잡아줌
	while (m_vLerpWeight[0].Length() < m_vStartPosition.Length())
	{
		m_vLerpWeight[0] = m_vStartPosition - vDir;
		m_vLerpWeight[0] += _float3(Random_Generator(-20.f, 20.f), Random_Generator(-20.f, 20.f), Random_Generator(-20.f, 20.f));
	}
	

	// 그 vector를 타겟 기준 정방향으로 1만큼 이동한 뒤 랜덤값을 잡아줌
	while (m_vLerpWeight[1].Length() < m_vStartPosition.Length())
	{
		m_vLerpWeight[1] = m_vTargetPosition + vDir;
		m_vLerpWeight[1] += _float3(Random_Generator(-20.f, 20.f), Random_Generator(-20.f, 20.f), Random_Generator(-20.f, 20.f));
	}

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
		if(m_pEffect->IsEnable())
			Set_ObjEvent(OBJ_DEAD);
	}
	__super::Tick(fTimeDelta);
}

void CBasicCast::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBasicCast::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CBasicCast::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CBasicCast::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
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
