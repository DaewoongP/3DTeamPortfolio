#include "AncientDoor_L.h"
#include "..\Public\Door.h"
#include "GameInstance.h"

#include "Player.h"

CAncientDoor_L::CAncientDoor_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CAncientDoor_L::CAncientDoor_L(const CAncientDoor_L& rhs)
	: CMapObject(rhs)
{
}

HRESULT CAncientDoor_L::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAncientDoor_L::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CAncientDoor_L Argument is NULL");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_pTransform->Set_RigidBody(m_pRigidBody);

	return S_OK;
}

HRESULT CAncientDoor_L::Initialize_Level(_uint iCurrentLevelIndex)
{
	/* Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, m_ObjectDesc.wszTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
	{
		MSG_BOX("Failed CAncientDoor_L Add_Component : (Com_Model)");
		__debugbreak();
		return E_FAIL;
	}

	//// ������ �ٵ� �ʱ�ȭ
	//CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	//RigidBodyDesc.isStatic = false;
	//RigidBodyDesc.isTrigger = false;
	//RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	//RigidBodyDesc.eConstraintFlag = CRigidBody::All;
	//RigidBodyDesc.fStaticFriction = 1.f;
	//RigidBodyDesc.fDynamicFriction = 1.f;
	//RigidBodyDesc.fRestitution = 0.f;
	//PxBoxGeometry MyGeometry = PxBoxGeometry(0.5f, 1.5f, 0.6f);
	//RigidBodyDesc.pGeometry = &MyGeometry;
	//RigidBodyDesc.vOffsetPosition = _float3(-0.35f, 0.75f, -0.15f);
	//RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(60.f), 0.f);
	//RigidBodyDesc.pOwnerObject = this;
	//RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 1.f, 1.f);
	//RigidBodyDesc.eThisCollsion = COL_STATIC;
	//RigidBodyDesc.eCollisionFlag = COL_ENEMY | COL_PLAYER;
	//strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "MapObject");
	//
	//if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
	//	TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	//{
	//	MSG_BOX("Failed CAncientDoor_L Add_Component : (Com_RigidBody)");
	//	__debugbreak();
	//	return E_FAIL;
	//}

	// �÷��̾� ã��
	BEGININSTANCE;
	m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(iCurrentLevelIndex, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	ENDINSTANCE;

	return S_OK;
}

void CAncientDoor_L::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Door_Action(fTimeDelta);
}

void CAncientDoor_L::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);

#ifdef _DEBUG
		//m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CAncientDoor_L::Door_Action(_float fTimeDelta)
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
	if (5.f >= m_fDist_From_Player && false == m_isDoorAction)
	{
		// �÷��̾�� ���� ������
		_float fDotPlayerAndDoor = XMVector3Dot(vDistGap, m_pTransform->Get_Look()).m128_f32[0];

		if (0.f > fDotPlayerAndDoor) // ���� ��
			m_eCurrentState = OPEN_INSIDE;
	
		m_isDoorAction = true;
	}

	// �÷��̾ �־����� ���� �������� ���� ����
	if (CLOSE_OUTSIDE == m_eCurrentState || CLOSE_INSIDE == m_eCurrentState)
	{
			return;
	}

	// �Ǵܿ� ���� ���� �� ȸ��(����)
	if (true == m_isDoorAction)
	{
		if (OPEN_INSIDE == m_eCurrentState) // �ȿ��� ����
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

CAncientDoor_L* CAncientDoor_L::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAncientDoor_L* pInstance = New CAncientDoor_L(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAncientDoor_L");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAncientDoor_L::Clone(void* pArg)
{
	CAncientDoor_L* pInstance = New CAncientDoor_L(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAncientDoor_L");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAncientDoor_L::Free()
{
	__super::Free();

	//Safe_Release(m_pRigidBody);
}