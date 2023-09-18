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
		//���콺 ��ŷ �������� �߻�
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
		//�̰� �ٵ� ���� ����ġ�� ���� ��������������.
		m_vTargetPosition = m_pTarget->Get_Position();
	}

	// �÷��̾ Ÿ���� ���� vector�� ����.
	_float3 vDir = XMVector3Normalize(m_vTargetPosition - m_MagicBallDesc.vStartPosition);
	// ������ ���� ����.
	_float3 tempAxis = _float3(1, 1, 1);
	// ����
	_float3	normal = XMVector3Cross(vDir, tempAxis);
	
	//���� ��θ�ŭ �ڷ� �̵��� ��
	m_vLerpWeight[0] = m_MagicBallDesc.vStartPosition - vDir;
	//������ ���� ���� ���ϰ�
	_float fRandom = Random_Generator(-20.f, 20.f);
	// ���� �������� Ƥ��.
	m_vLerpWeight[0] += _float3(normal.x * fRandom, normal.y  * fRandom, normal.z * fRandom);

	//���� ��θ�ŭ �ڷ� �̵��� ��
	m_vLerpWeight[1] = m_MagicBallDesc.vStartPosition + vDir;
	//������ ���� ���� ���ϰ�
	fRandom = Random_Generator(-20.f, 20.f);
	// ���� �������� Ƥ��.
	m_vLerpWeight[1] += _float3(normal.x * fRandom, normal.y * fRandom, normal.z * fRandom);
	m_pExplosiveEffect->ResetParticle();
	m_fTimeScalePerDistance = m_MagicBallDesc.fDistance / _float3(m_vTargetPosition - m_MagicBallDesc.vStartPosition).Length();

	return S_OK;
}

void CConfringo::Tick(_float fTimeDelta)
{
	if (m_fLerpAcc < 1)
	{
		// ���⼭ �R�R�� ��������.
		m_fLerpAcc += fTimeDelta / m_MagicBallDesc.fInitLifeTime * m_fTimeScalePerDistance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_pTrailEffect->Spline_Move(m_vLerpWeight[0],m_MagicBallDesc.vStartPosition, m_vTargetPosition, m_vLerpWeight[1], m_fLerpAcc);
	}
	else 
	{
		//������ ȿ����.
		if (!m_isExplosiveTrigger)
		{
			m_pExplosiveEffect->Play_Particle(m_vTargetPosition);
			m_isExplosiveTrigger = true;

			if(m_pExplosiveEffect->Get_Particle_State_End())
				Set_ObjEvent(OBJ_DEAD);
		}
		
	}
	__super::Tick(fTimeDelta);
}

void CConfringo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CConfringo::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
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
