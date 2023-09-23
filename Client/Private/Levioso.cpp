#include "Levioso.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CLevioso::CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CLevioso::CLevioso(const CLevioso& rhs)
	: CMagicBall(rhs)
{
}

void CLevioso::TrailAction(_float3 vPosition, _float fTimeDelta)
{
	m_MagicTimer -= fTimeDelta;
	m_pWingardiumEffect->TrailAction(vPosition, fTimeDelta);
	m_fWingardiumEffectDeadTimer = 0.3f;

	m_fWingardiumEffectAccTime += fTimeDelta;
	if (m_fWingardiumEffectAccTime > 6.0f)
		m_fWingardiumEffectDeadTimer = -1;
}

HRESULT CLevioso::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevioso::Initialize(void* pArg)
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
	m_CollisionDesc.Action = bind(&CLevioso::TrailAction, this, placeholders::_1, placeholders::_2);

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
	
	m_pEffect->Ready_Spin(m_vTargetPosition, m_MagicBallDesc.vStartPosition, m_MagicBallDesc.fLifeTime, m_MagicBallDesc.fDistance);
	return S_OK;
}

void CLevioso::Tick(_float fTimeDelta)
{
	if (m_pEffect->Spin_Move(fTimeDelta))
	{
		//이동이 끝났고 윙가가 발동 안했다면?
		if (!m_bWingardiumActionTrigger)
		{
			m_bWingardiumActionTrigger = true;
			m_pWingardiumEffect->SetActionTrigger(m_bWingardiumActionTrigger);
			dynamic_cast<CGameObject*>(m_pTarget->Get_Owner())->On_Maigc_Throw_Data(&m_CollisionDesc);
		}
		else
		{
			m_fWingardiumEffectDeadTimer -= fTimeDelta;
			if (m_fWingardiumEffectDeadTimer < 0)
			{
				Set_ObjEvent(OBJ_DEAD);
			}
			//TrailAction(m_pTarget->Get_Position(), fTimeDelta);
		}
	}
	m_pTransform->Set_Position(m_pEffect->Get_Transform()->Get_Position());
	__super::Tick(fTimeDelta);
}

void CLevioso::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CLevioso::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CLevioso::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CLevioso::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CLevioso::Add_Components()
{
	return S_OK;
}

HRESULT CLevioso::Add_Effect()
{
	CDefault_MagicTraill_Effect::INITDESC initDesc;
	initDesc.vInitPosition = m_MagicBallDesc.vStartPosition;
	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_MagicTraill_Winga_Effect"), 
		TEXT("Com_Effect"), reinterpret_cast<CComponent**>(&m_pEffect),&initDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_MagicTraill_Winga_Effect)");
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Wingardium_Effect"),
		TEXT("Com_WingradiumEffect"), reinterpret_cast<CComponent**>(&m_pWingardiumEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Wingardium_Effect)");
		return E_FAIL;
	}

	return S_OK;
}

CLevioso* CLevioso::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevioso* pInstance = new CLevioso(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CLevioso");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLevioso::Clone(void* pArg)
{
	CLevioso* pInstance = new CLevioso(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CLevioso");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevioso::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pEffect);
		Safe_Release(m_pWingardiumEffect);
	}
}
