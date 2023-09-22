#include "Finisher.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CFinisher::CFinisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CFinisher::CFinisher(const CFinisher& rhs)
	: CMagicBall(rhs)
	, m_iLevel(rhs.m_iLevel)
{
}

HRESULT CFinisher::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;
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

	if (FAILED(Add_Effect()))
	{
		MSG_BOX("Failed Player Add_Effect");

		return E_FAIL;
	}

	if (m_pTarget == nullptr)
	{
		//마우스 피킹 지점으로 발사
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
		vDirStartToPicked = (vMouseWorldPickPosition - m_MagicBallDesc.vStartPosition);
		vDirStartToPicked.Normalize();
		m_vTargetPosition = vDirStartToPicked * m_MagicBallDesc.fDistance;
	}
	else 
	{
		m_vTargetPosition = m_pTarget->Get_Position() + m_TargetOffsetMatrix.Translation();
	}

	_float3 vWeight[2];
	vWeight[0] = _float3(-0.05f, -0.05f, -0.05f);
	vWeight[1] = _float3(0.05f, 0.05f, 0.05f);

	for (int i = 0; i < 3; i++)
	{
		m_pTrail[i]->Set_Threshold(0.0f);
		m_pTrail[i]->Ready_LightningStrike(m_vTargetPosition + _float3(0, 10, 0), m_vTargetPosition, vWeight, 10);
		m_pTrail[i]->Reset_Trail(m_vTargetPosition + _float3(0, 10, 0) + _float3(0, 0.5f, 0), m_vTargetPosition + _float3(0, 10, 0) + _float3(0, -0.5f, 0));
		m_pTrail[i]->Get_Transform()->Set_Position(m_vTargetPosition + _float3(0, 10, 0));
	}
	return S_OK;
}

void CFinisher::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
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

HRESULT CFinisher::Add_Components()
{
	return S_OK;
}

HRESULT CFinisher::Add_Effect()
{
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Traill_Lightning_Effect"), 
		TEXT("Com_LightningTrail01"), reinterpret_cast<CComponent**>(&m_pTrail[0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Traill_Lightning_Effect)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Traill_Lightning_Effect"),
		TEXT("Com_LightningTrail02"), reinterpret_cast<CComponent**>(&m_pTrail[1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Traill_Lightning_Effect)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Traill_Lightning_Effect"),
		TEXT("Com_LightningTrail03"), reinterpret_cast<CComponent**>(&m_pTrail[2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Traill_Lightning_Effect)");
		return E_FAIL;
	}
	return S_OK;
}

CFinisher* CFinisher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CFinisher* pInstance = new CFinisher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CFinisher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFinisher::Clone(void* pArg)
{
	CFinisher* pInstance = new CFinisher(*this);

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
