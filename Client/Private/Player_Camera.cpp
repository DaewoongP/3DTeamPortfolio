#include "Player_Camera.h"
#include "GameInstance.h"

CPlayer_Camera::CPlayer_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCamera(_pDevice, _pContext)
{

}


HRESULT CPlayer_Camera::Initialize(void* pArg)
{
	PLAYERCAMERADESC* pCameraDesc = (PLAYERCAMERADESC*)pArg;

	if (FAILED(CCamera::Initialize(pCameraDesc->CameraDesc)))
	{
		MSG_BOX("Failed Initialize Camera");
		
		return E_FAIL;
	}

	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	
	if (nullptr == m_pTransform)
	{
		MSG_BOX("Failed Create Player Camera Trensform");

		return E_FAIL;
	}

	m_pTransform->Set_RotationSpeed(1.0f);

	
	m_pFollowTargetMatrix = pCameraDesc->pFollowTargetMatrix;
	m_pFollowTargetBoneMatrix = pCameraDesc->pFollowTargetBoneMatrix;

	//��ġ �ʱ�ȭ
	Update_FollowMatrix();

	_float3 vPos = m_FollowTargetMatrix.Translation();
	m_pTransform->Set_Position(vPos);

	m_fFollowLimit = 0.001f;

	m_vAtStandard = _float3(0.0f, 0.0f, 1.0f);

	m_vEyeStandard = _float3(sinf(XMConvertToRadians(30.0f)), 0.0f, -cosf(XMConvertToRadians(30.f)));

	m_fEyeMaxDistance = m_fAtMaxDistance = 10.0f;
	m_fEyeMinDistance = m_fAtMinDistance = 1.0f;


	m_fTimeSpeed = 10.0f;

	return S_OK;
}

void CPlayer_Camera::Tick(const _float& _TimeDelta)
{
	Mouse_Input(_TimeDelta);
	
	Update_FollowMatrix();

	Follow_Transform();

	Eye_At_Distance();

	Update_Eye_At();

	BEGININSTANCE;

	pGameInstance->Set_Transform(
		CPipeLine::D3DTS_PROJ, 
		XMMatrixPerspectiveFovLH(
			m_CameraDesc.m_fFovY, 
			m_CameraDesc.m_fAspect, 
			m_CameraDesc.m_fNear, 
			m_CameraDesc.m_fFar));
	pGameInstance->Set_CameraFar(m_CameraDesc.m_fFar);

	ENDINSTANCE;

#ifdef _DEBUG
	
	//Tick_ImGui();

#endif // _DEBUG
}

#ifdef _DEBUG
void CPlayer_Camera::Tick_ImGui()
{
	ImGui::Begin("Test Player Camera");

	BEGININSTANCE;

	ImGui::DragFloat("Eye_Intersect_Distance", &m_fEyeIntersectDistance, 0.01f, 0.0f, 20.0f);
	ImGui::DragFloat("At_Intersect_Distance", &m_fAtIntersectDistance, 0.01f, 0.0f, 20.0f);

	ENDINSTANCE;

	ImGui::End();
}
#endif // _DEBUG

void CPlayer_Camera::Mouse_Input(_float _TimeDelta)
{
	_long		dwMouseMove = 0;

	BEGININSTANCE

		dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);

	if (dwMouseMove)
	{
		_float3	vRight = m_pTransform->Get_Right();

		m_pTransform->Turn(vRight, dwMouseMove * _TimeDelta * 0.1f);

		dwMouseMove = 0;
	}

	dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X);

	if (dwMouseMove)
	{
		_float3	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pTransform->Turn(vUp, dwMouseMove * _TimeDelta * 0.1f);

		dwMouseMove = 0;
	}

	ENDINSTANCE
}

void CPlayer_Camera::Update_FollowMatrix()
{
	if (nullptr != m_pFollowTargetBoneMatrix && nullptr != m_pFollowTargetMatrix)
	{
		m_FollowTargetMatrix = (*m_pFollowTargetBoneMatrix) * (*m_pFollowTargetMatrix);
	}
	else if (nullptr != m_pFollowTargetMatrix)
	{
		m_FollowTargetMatrix = (*m_pFollowTargetMatrix);
	}
	else
	{
		MSG_BOX("Do not have FollowMatrix");
	}
}

void CPlayer_Camera::Follow_Transform()
{
	BEGININSTANCE;

	//���� ����
	_float3 vTargetDir{};

	vTargetDir = m_FollowTargetMatrix.Translation();

	vTargetDir -= m_pTransform->Get_Position();

	//�Ÿ� ��
	if (m_fFollowLimit < vTargetDir.Length())
	{
		//�Ÿ� ��ŭ�̵�
		m_pTransform->Set_Speed((_float)pow((double)vTargetDir.Length(), 2.0));

		//�̵�
		m_pTransform->Move_Direction(vTargetDir, pGameInstance->Get_World_Tick());
	}

	ENDINSTANCE;
}

void CPlayer_Camera::Eye_At_Distance()
{
	//���¿� ���� �Ÿ��� �ٲ��.
	
	//�浹�� �Ÿ��� �ʿ��ϴ�.

	//���¶Ǵ� �Ÿ� �浹�Ÿ��� �����Ǹ� �߰� �Ѵ�.

	//�ϴ� Ȯ�ο����� imgui�� ����Ѵ�.

	BEGININSTANCE;
	
	//eye
	//�浹�ؼ� ���� �ְ�, �ִ� �Ÿ����� ��ٸ�
	if (0.0f != m_fEyeIntersectDistance && 
		m_fEyeMaxDistance > m_fEyeIntersectDistance)
	{
		//���� ���ϰ�
		_float fDistance = m_fEyeDistance - m_fEyeIntersectDistance;

		//����
		m_fEyeDistance -= fDistance * pGameInstance->Get_World_Tick() * m_fTimeSpeed;
	}
	//���� �ʴٸ�
	else if (m_fEyeDistance != m_fEyeMaxDistance)
	{
		//���� ���ϰ�
		_float fDistance = m_fEyeMaxDistance - m_fEyeDistance;

		//���Ѵ�. 
		m_fEyeDistance += fDistance * pGameInstance->Get_World_Tick() * m_fTimeSpeed;
	}
	else
	{

	}

	Clamp(m_fEyeDistance, m_fEyeMinDistance, m_fEyeMaxDistance);

	//At
	//�浹�ؼ� ���� �ְ�, �ִ� �Ÿ����� ��ٸ�
	if (0.0f != m_fAtIntersectDistance && 
		m_fAtMaxDistance > m_fAtIntersectDistance)
	{
		//���� ���ϰ�
		_float fDistance = m_fAtDistance - m_fAtIntersectDistance;

		//����
		m_fAtDistance -= fDistance * pGameInstance->Get_World_Tick() * m_fTimeSpeed;
	}
	//���� �ʴٸ�
	else if (m_fAtDistance != m_fAtMaxDistance)
	{
		//���� ���ϰ�
		_float fDistance = m_fAtMaxDistance - m_fAtDistance;

		//���Ѵ�. 
		m_fAtDistance += fDistance * pGameInstance->Get_World_Tick() * m_fTimeSpeed;
	}
	else
	{

	}

	Clamp(m_fAtDistance, m_fAtMinDistance, m_fAtMaxDistance);
	
	ENDINSTANCE;
}

void CPlayer_Camera::Update_Eye_At()
{
	//���������� ������ ��

	//Eye
	_float3 vEye = m_vEyeStandard;
	vEye *= m_fEyeDistance;
	vEye = vEye.Transform(vEye, m_pTransform->Get_WorldMatrix());

	//At
	_float3 vAt = m_vAtStandard;
	vAt *= m_fAtDistance;

	vAt = vAt.Transform(vAt, m_pTransform->Get_WorldMatrix());

	_float3 vUp = _float3(0.0f, 1.0f ,0.0f);

	BEGININSTANCE;

	pGameInstance->Set_Transform(
		CPipeLine::D3DTS_VIEW, 
		XMMatrixLookAtLH(vEye, vAt, vUp));

	ENDINSTANCE;
}


CPlayer_Camera* CPlayer_Camera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
	CPlayer_Camera* pInstance = New CPlayer_Camera(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create CPlayer_Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Camera::Free()
{
	CCamera::Free();

	Safe_Release(m_pTransform);
}