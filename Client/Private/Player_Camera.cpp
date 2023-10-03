#include "Player_Camera.h"
#include "GameInstance.h"

CPlayer_Camera::CPlayer_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCamera(_pDevice, _pContext)
{

}

_float3 CPlayer_Camera::Get_CamLookXZ()
{
	BEGININSTANCE;

	_float3 vXZLook{};

	vXZLook = *pGameInstance->Get_CamLook();

	vXZLook = XMVectorSetY(vXZLook, 0.0f);

	vXZLook.Normalize();
	
	ENDINSTANCE;

	return vXZLook;
}

_float3 CPlayer_Camera::Get_CamRightXZ()	
{
	BEGININSTANCE;

	_float3 vXZRight{};

	_float3 vUp(0.0f, 1.0f, 0.0f);

	vXZRight = *pGameInstance->Get_CamRight();

	vXZRight = XMVectorSetY(vXZRight, 0.0f);

	vXZRight.Normalize();

	ENDINSTANCE;

	return vXZRight;
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

	
	m_pPlayerTransform = pCameraDesc->pPlayerTransform;
	Safe_AddRef(m_pPlayerTransform);
	
	//위치 초기화
	//Update_FollowMatrix();

	_float3 vPos = m_FollowTargetMatrix.Translation();
	m_pTransform->Set_Position(vPos);

	m_fFollowLimit = 0.001f;

	m_vAtStandard = _float3(0.0f, 0.0f, 1.0f);

	m_vEyeStandard = _float3(sinf(XMConvertToRadians(30.0f)), 0.0f, -cosf(XMConvertToRadians(30.f)));

	m_fEyeMaxDistance = m_fAtMaxDistance = 3.0f;
	m_fEyeMinDistance = m_fAtMinDistance = 1.0f;


	m_fTimeSpeed = 10.0f;

	m_fCameraHeight = 1.5f;

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
}

void CPlayer_Camera::Mouse_Input(_float _fTimeDelta)
{
	_long		dwMouseMove = 0;

	BEGININSTANCE;

	dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);

	if (dwMouseMove)
	{
		////회전 제한
		//_float3 vLook = m_pTransform->Get_Look();

		//vLook.Normalize();

		//_float3 vLookNoY = vLook;

		//vLookNoY.y = 0.0f;
		//vLookNoY.Normalize();

		//if (XMConvertToRadians(70.0f) >= fabsf(XMVectorGetX(XMVector3AngleBetweenNormals(vLookNoY, vLook))))
		//{
		//	_float3	vRight = m_pTransform->Get_Right();

		//	m_pTransform->Turn(vRight, dwMouseMove * _fTimeDelta * 0.1f);

		//	dwMouseMove = 0;
		//}

		_float3	vRight = m_pTransform->Get_Right();

		m_pTransform->Turn(vRight, dwMouseMove * _fTimeDelta * 0.1f);

		dwMouseMove = 0;
	}

	dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X);

	if (dwMouseMove)
	{
		_float3	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		m_pTransform->Turn(vUp, dwMouseMove * _fTimeDelta * 0.1f);

		dwMouseMove = 0;
	}

	ENDINSTANCE;
}

void CPlayer_Camera::Update_FollowMatrix()
{
	if (nullptr != m_pPlayerTransform)
	{
		m_FollowTargetMatrix = *m_pPlayerTransform->Get_WorldMatrixPtr();
	}
}

void CPlayer_Camera::Follow_Transform()
{
	BEGININSTANCE;

	//차이 벡터
	_float3 vTargetDir{};

	vTargetDir = XMVectorSetY(m_FollowTargetMatrix.Translation(), m_FollowTargetMatrix.Translation().y + m_fCameraHeight);

	vTargetDir -= m_pTransform->Get_Position();

	//거리 비교
	if (m_fFollowLimit < vTargetDir.Length())
	{
		//거리 만큼이동
		m_pTransform->Set_Speed((_float)vTargetDir.Length() * m_fTimeSpeed);

		_float fTick = pGameInstance->Get_World_Tick();

		//이동
		m_pTransform->Move_Direction(vTargetDir, fTick);
	}

	ENDINSTANCE;
}

void CPlayer_Camera::Eye_At_Distance()
{
	//상태에 따라 거리가 바뀐다.
	
	//충돌된 거리가 필요하다.

	//상태또는 거리 충돌거리가 구현되면 추가 한다.

	//일단 확인용으로 imgui를 사용한다.

	BEGININSTANCE;
	
	//eye
	//충돌해서 값이 있고, 최대 거리보다 길다면
	if (0.0f != m_fEyeIntersectDistance && 
		m_fEyeMaxDistance > m_fEyeIntersectDistance)
	{
		//차이 구하고
		_float fDistance = m_fEyeDistance - m_fEyeIntersectDistance;

		//뺀다
		m_fEyeDistance -= fDistance * pGameInstance->Get_World_Tick() * m_fTimeSpeed;
	}
	//같지 않다면
	else if (m_fEyeDistance != m_fEyeMaxDistance)
	{
		//차이 구하고
		_float fDistance = m_fEyeMaxDistance - m_fEyeDistance;

		//더한다. 
		m_fEyeDistance += fDistance * pGameInstance->Get_World_Tick() * m_fTimeSpeed;
	}
	else
	{

	}

	Clamp(m_fEyeDistance, m_fEyeMinDistance, m_fEyeMaxDistance);

	//At
	//충돌해서 값이 있고, 최대 거리보다 길다면
	if (0.0f != m_fAtIntersectDistance && 
		m_fAtMaxDistance > m_fAtIntersectDistance)
	{
		//차이 구하고
		_float fDistance = m_fAtDistance - m_fAtIntersectDistance;

		//뺀다
		m_fAtDistance -= fDistance * pGameInstance->Get_World_Tick() * m_fTimeSpeed;
	}
	//같지 않다면
	else if (m_fAtDistance != m_fAtMaxDistance)
	{
		//차이 구하고
		_float fDistance = m_fAtMaxDistance - m_fAtDistance;

		//더한다. 
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
	//최종적으로 던지는 곳

	//Eye
	_float3 vEye = m_vEyeStandard;
	vEye *= m_fEyeDistance;
	vEye = vEye.Transform(vEye, m_pTransform->Get_WorldMatrix());

	//At
	_float3 vAt = m_vAtStandard;
	vAt *= m_fAtDistance;

	vAt = vAt.Transform(vAt, m_pTransform->Get_WorldMatrix());

	_float3 vUp = _float3(0.0f, 1.0f, 0.0f);

	BEGININSTANCE;

	while (false == IsValid_CameraPos(vEye, vUp))
	{
		//Eye
		_float3 vEye = m_vEyeStandard;
		vEye *= m_fEyeDistance;
		vEye = vEye.Transform(vEye, m_pTransform->Get_WorldMatrix());

		//At
		_float3 vAt = m_vAtStandard;
		vAt *= m_fAtDistance;

		vAt = vAt.Transform(vAt, m_pTransform->Get_WorldMatrix());

		_float3 vUp = _float3(0.0f, 1.0f, 0.0f);
	}
	
	pGameInstance->Set_Transform(
		CPipeLine::D3DTS_VIEW, 
		XMMatrixLookAtLH(vEye, vAt, vUp));

	ENDINSTANCE;
}

_bool CPlayer_Camera::IsValid_CameraPos(_float3 vEye, _float3 vUp)
{
	if (XMVector3Equal(vEye, XMVectorZero()) ||
		XMVector3IsInfinite(vEye) ||
		XMVector3Equal(vUp, XMVectorZero()) ||
		XMVector3IsInfinite(vUp))
	{
		return false;
	}
	
	return true;
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
	Safe_Release(m_pPlayerTransform);
}
