#include "..\Public\Door.h"
#include "GameInstance.h"

#include "Player.h"

CDoor::CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CDoor::CDoor(const CDoor& rhs)
	: CMapObject(rhs)
{
}

HRESULT CDoor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoor::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CDoor Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	// �÷��̾� ã��
	BEGININSTANCE;
	m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(iCurrentLevelIndex, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	ENDINSTANCE;

	return S_OK;
}

void CDoor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Door_Action(fTimeDelta);
}

void CDoor::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);

#ifdef _DEBUG
		for (auto& pRigidBody : m_RigidBodys)
		{
			m_pRenderer->Add_DebugGroup(pRigidBody);
		}
#endif // _DEBUG
	}
}

void CDoor::Door_Action(_float fTimeDelta)
{
	// �÷��̾�� �Ÿ� ��
	_float3 vPlayerPos = m_pPlayer->Get_PlayerPos();
	_float3 vDoorPos = m_pTransform->Get_Position();

	// ���� �÷��̾��� �Ÿ� ����
	_float3 vDistGap = { (vDoorPos.x - vPlayerPos.x) , (vDoorPos.y - vPlayerPos.y), (vDoorPos.z - vPlayerPos.z) };

	m_fDist_From_Player = sqrtf(vDistGap.x * vDistGap.x +
		vDistGap.y * vDistGap.y +
		vDistGap.z * vDistGap.z);

	// ���� ��� �������� ȸ���Ұ��� �Ǵ�
	// �÷��̾ �� �ٷ� �տ� ����(Ȥ�� �׿� ���ϰ� �����ٸ�)
	if (2.f >= m_fDist_From_Player && false == m_isDoorAction)
	{
		// �÷��̾�� ���� ������
		_float fDotPlayerAndDoor = XMVector3Dot(vDistGap, m_pTransform->Get_Look()).m128_f32[0];

		if (0.f > fDotPlayerAndDoor) // ���� ��
			m_eCurrentState = OPEN_INSIDE;
		else if (0.f < fDotPlayerAndDoor) // ���� ��
			m_eCurrentState = OPEN_OUTSIDE;

		m_isDoorAction = true;
	}

	// �÷��̾ �־����� ���� �������� ���� ����
	if (CLOSE_OUTSIDE == m_eCurrentState || CLOSE_INSIDE == m_eCurrentState)
	{
		// �÷��̾ �����ٸ� ���� ������ �ʵ��� return�� ����������.
		if (3.f >= m_fDist_From_Player)
			return;
	}

	// �Ǵܿ� ���� ���� �� ȸ��(����)
	if (true == m_isDoorAction)
	{
		if(OPEN_INSIDE == m_eCurrentState) // �ȿ��� ����
			m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(DOORTURN), fTimeDelta);
		else if(OPEN_OUTSIDE == m_eCurrentState) // �ۿ��� ����
			m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(-DOORTURN), fTimeDelta);
		else if (CLOSE_INSIDE == m_eCurrentState)  // �ȿ��� ����
			m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(-DOORTURN), fTimeDelta);
		else if (CLOSE_OUTSIDE == m_eCurrentState) // �ۿ��� ����
			m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(DOORTURN), fTimeDelta);

		// DOORTURN ��ŭ �������� ȸ���� ����
		m_fDoorSpinValue += DOORTURN * fTimeDelta;

		// ���� ���� ���¶�� ������ ���� ���¶�� �״�� ���� �̺�Ʈ���� ���
		if (DOORTURN <= m_fDoorSpinValue)
		{
			if (OPEN_INSIDE == m_eCurrentState)
			{
				m_eCurrentState = CLOSE_INSIDE;
				m_fDoorSpinValue = 0.f;
			}
				
			else if (OPEN_OUTSIDE == m_eCurrentState)
			{
				m_eCurrentState = CLOSE_OUTSIDE;
				m_fDoorSpinValue = 0.f;
			}

			else
			{
				m_eCurrentState = DOOR_END;
				m_isDoorAction = false;
				m_fDoorSpinValue = 0.f;
			}			
		}
	}
}

CDoor* CDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoor* pInstance = New CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoor::Clone(void* pArg)
{
	CDoor* pInstance = New CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDoor::Free()
{
	__super::Free();

	/*for (auto& pRigidBody : m_RigidBodys)
		Safe_Release(pRigidBody);
	m_RigidBodys.clear();*/

	Safe_Release(m_pShader);
	Safe_Release(m_pShadowShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pRenderer);
}
