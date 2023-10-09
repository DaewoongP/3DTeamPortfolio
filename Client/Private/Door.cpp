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

	// 플레이어 찾기
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
	// 플레이어와 거리 비교
	_float3 vPlayerPos = m_pPlayer->Get_PlayerPos();
	_float3 vDoorPos = m_pTransform->Get_Position();

	// 문과 플레이어의 거리 차이
	_float3 vDistGap = { (vDoorPos.x - vPlayerPos.x) , (vDoorPos.y - vPlayerPos.y), (vDoorPos.z - vPlayerPos.z) };

	m_fDist_From_Player = sqrtf(vDistGap.x * vDistGap.x +
		vDistGap.y * vDistGap.y +
		vDistGap.z * vDistGap.z);

	// 문이 어느 방향으로 회전할건지 판단
	// 플레이어가 문 바로 앞에 서면(혹은 그에 준하게 가깝다면)
	if (2.f >= m_fDist_From_Player && false == m_isDoorAction)
	{
		// 플레이어와 문의 내적값
		_float fDotPlayerAndDoor = XMVector3Dot(vDistGap, m_pTransform->Get_Look()).m128_f32[0];

		if (0.f > fDotPlayerAndDoor) // 나갈 때
			m_eCurrentState = OPEN_INSIDE;
		else if (0.f < fDotPlayerAndDoor) // 들어올 때
			m_eCurrentState = OPEN_OUTSIDE;

		m_isDoorAction = true;
	}

	// 플레이어가 멀어져야 문이 닫히도록 조건 설정
	if (CLOSE_OUTSIDE == m_eCurrentState || CLOSE_INSIDE == m_eCurrentState)
	{
		// 플레이어가 가깝다면 문이 닫히지 않도록 return을 때려버린다.
		if (3.f >= m_fDist_From_Player)
			return;
	}

	// 판단에 따른 실제 문 회전(열림)
	if (true == m_isDoorAction)
	{
		if(OPEN_INSIDE == m_eCurrentState) // 안에서 열림
			m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(DOORTURN), fTimeDelta);
		else if(OPEN_OUTSIDE == m_eCurrentState) // 밖에서 열림
			m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(-DOORTURN), fTimeDelta);
		else if (CLOSE_INSIDE == m_eCurrentState)  // 안에서 닫힘
			m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(-DOORTURN), fTimeDelta);
		else if (CLOSE_OUTSIDE == m_eCurrentState) // 밖에서 닫힘
			m_pTransform->Turn(_float3(0.f, 1.f, 0.f), XMConvertToRadians(DOORTURN), fTimeDelta);

		// DOORTURN 만큼 돌았으면 회전을 멈춤
		m_fDoorSpinValue += DOORTURN * fTimeDelta;

		// 문이 열린 상태라면 닫히고 닫힌 상태라면 그대로 다음 이벤트까지 대기
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
