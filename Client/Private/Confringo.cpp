#include "Confringo.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CConfringo::CConfringo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CConfringo::CConfringo(const CConfringo& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CConfringo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CConfringo::Initialize(void* pArg)
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
		//이거 근데 몹의 발위치로 지금 설정돼있을듯함.
		m_vTargetPosition = m_pTarget->Get_Position();
	}

	//Ready for Spline Lerp
	m_pTrailEffect->Ready_Spline(m_vTargetPosition, m_MagicBallDesc.vStartPosition, m_MagicBallDesc.fLifeTime, m_MagicBallDesc.fDistance);
	m_pWandDustEffect->Set_Position(m_vTargetPosition);
	m_pExplosiveEffect->ResetParticle();
	return S_OK;
}

void CConfringo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	//사망처리 걸리면 재생
	if (m_isExplosiveTrigger)
	{
		//파티클 끝나면 없애줘.
		//if (m_pExplosiveEffect->Get_Particle_State_End())
		Set_ObjEvent(OBJ_DEAD);
		return;
	}
	//이동이 끝나면 true를 리턴해줄거임.
	if (m_pTrailEffect->Spline_Move(fTimeDelta))
	{
		//터지는 효과임.
		if (!m_isExplosiveTrigger)
		{
			m_pExplosiveEffect->Play_Particle(m_vTargetPosition);
			m_isExplosiveTrigger = true;
		}
	}
	m_pTransform->Set_Position(m_pTrailEffect->Get_Transform()->Get_Position());
}

void CConfringo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CConfringo::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		//사망처리 드갑니다.
		m_isExplosiveTrigger = true;
		m_pExplosiveEffect->Play_Particle(m_pTrailEffect->Get_Transform()->Get_Position());
	}
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CConfringo::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CConfringo::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CConfringo::Add_Components()
{
	return S_OK;
}

HRESULT CConfringo::Add_Effect()
{
	CDefault_MagicTraill_Effect::INITDESC initDesc;
	initDesc.vInitPosition = m_MagicBallDesc.vStartPosition;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_MagicTraill_Confringo_Effect"), 
		TEXT("Com_TrailEffect"), reinterpret_cast<CComponent**>(&m_pTrailEffect),&initDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_MagicTraill_Confringo_Effect)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Confringo_Explosive_Effect"),
		TEXT("Com_ExplosiveEffect"), reinterpret_cast<CComponent**>(&m_pExplosiveEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Confringo_Explosive_Effect)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Confringo_WandDust_Effect"),
		TEXT("Com_WandDustEffect"), reinterpret_cast<CComponent**>(&m_pWandDustEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Confringo_WandDust_Effect)");
		return E_FAIL;
	}
	
	return S_OK;
}

CConfringo* CConfringo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CConfringo* pInstance = new CConfringo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CConfringo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CConfringo::Clone(void* pArg)
{
	CConfringo* pInstance = new CConfringo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CConfringo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CConfringo::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pTrailEffect);
		Safe_Release(m_pExplosiveEffect);
		Safe_Release(m_pWandDustEffect);
	}
}
