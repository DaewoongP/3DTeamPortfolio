#include "..\Public\Transform.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_float4 CTransform::Get_Quaternion()
{
	_float4x4 RotationMatrix = m_WorldMatrix;
	_float4 vTrans = _float4(0.f, 0.f, 0.f, 1.f);
	memcpy(RotationMatrix.m[3], &vTrans, sizeof(_float4));

	return XMQuaternionRotationMatrix(RotationMatrix);
}

_float2 CTransform::Get_Trnaslation_To_UI_fXY(_uint iWinSizeX, _uint iWinSizeY) const
{
	_float2 vTranslation;

	vTranslation.x = m_WorldMatrix.Translation().x + iWinSizeX * 0.5f;
	vTranslation.y = -(m_WorldMatrix.Translation().y - iWinSizeY * 0.5f);

	return vTranslation;
}

_float4 CTransform::Get_QuaternionVector_From_Axis(_float3 vAxis, _float _fRadian)
{
	return XMQuaternionRotationAxis(XMVector3Normalize(vAxis), _fRadian);
}

_float4 CTransform::Get_QuaternionVector_RollPitchYaw(_float3 vRotation)
{
	return XMQuaternionRotationRollPitchYawFromVector(vRotation);
}

_float4 CTransform::Get_QuaternionVector_Roll(_float fRadian)
{
	return XMQuaternionRotationRollPitchYaw(fRadian, 0.f, 0.f);
}

_float4 CTransform::Get_QuaternionVector_Pitch(_float fRadian)
{
	return XMQuaternionRotationRollPitchYaw(0.f, fRadian, 0.f);
}

_float4 CTransform::Get_QuaternionVector_Yaw(_float fRadian)
{
	return XMQuaternionRotationRollPitchYaw(0.f, 0.f, fRadian);
}

void CTransform::Set_Scale(_float3 _vScale)
{
	_float3 vRight = Get_Right();
	_float3 vUp = Get_Up();
	_float3 vLook = Get_Look();
	
	Set_Right(XMVector3Normalize(vRight) * _vScale.x);
	Set_Up(XMVector3Normalize(vUp) * _vScale.y);
	Set_Look(XMVector3Normalize(vLook) * _vScale.z);
}

void CTransform::Set_Right(_float3 _vRight)
{
	memcpy(&m_WorldMatrix.m[0][0], &_vRight, sizeof(_float3));
}

void CTransform::Set_Up(_float3 _vUp)
{
	memcpy(&m_WorldMatrix.m[1][0], &_vUp, sizeof(_float3));
}

void CTransform::Set_Look(_float3 _vLook)
{
	memcpy(&m_WorldMatrix.m[2][0], &_vLook, sizeof(_float3));
}

void CTransform::Set_Position(_float3 _vPosition)
{
	memcpy(&m_WorldMatrix.m[3][0], &_vPosition, sizeof(_float3));
}

void CTransform::Set_WorldMatrix(_float4x4 _WorldMatrix)
{
	m_WorldMatrix = _WorldMatrix;
}

void CTransform::Add_Force(_float3 vDir, _float fJumpForce, _float fTimeDelta)
{
	m_vForce += vDir * fJumpForce / fTimeDelta;
}

HRESULT CTransform::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	return S_OK;
}

void CTransform::Tick(_float fTimeDelta)
{
	RigidBody(fTimeDelta);
}

void CTransform::RigidBody(_float fTimeDelta)
{
	if (false == m_isRigidBody ||
		nullptr == m_pNavigation)
		return;

	// 중력 F = G * M;
	m_vForce += m_vGravity * m_fMass;

	// 저항 F = -K * V
	m_vForce += m_vVelocity * m_fResistance * -1.f;

	// 가속도  A = F / M
	m_vAccel += m_vForce / m_fMass;

	// 속도  V = A * dt
	m_vVelocity += m_vAccel * fTimeDelta;

	Check_Move(Get_Position(), m_vVelocity);

	if (m_WorldMatrix._42 <= m_fGroundY)
	{
		m_vAccel.y = 0.f;
		m_vVelocity.y = 0.f;
		m_WorldMatrix._42 = m_fGroundY;
		m_isGrounded = true;
	}

	// 처리한 힘 초기화.
	ZEROMEM(&m_vForce);
}

void CTransform::Use_RigidBody(CNavigation* pNavigation, _float fLimitXZVelocity, _float fMass, _float fResistance)
{
	m_pNavigation = pNavigation;
	Safe_AddRef(m_pNavigation);

	m_isRigidBody = true;

	m_fLimitXZVelocity = fLimitXZVelocity;
	m_fMass = fMass;
	m_fResistance = fResistance;
}

void CTransform::Check_Move(_float3 vCurrentPosition, _float3 vVelocity)
{
	_float3 vXZOriginPosition = XMVectorSet(XMVectorGetX(vCurrentPosition), 0.f, XMVectorGetZ(vCurrentPosition), 1.f);
	_float3 vXZVelocity = XMVectorSet(XMVectorGetX(vVelocity), 0.f, XMVectorGetZ(vVelocity), 0.f);
	_float3 vDir = XMVector3Normalize(vXZVelocity);

	_float3 vXZPosition;
	vXZPosition = vXZOriginPosition + vXZVelocity;

	_bool		isMove = true;
	_float3		vNormal;
	_float3		vLineStartPoint, vLineDirection;
	ZEROMEM(&vNormal);
	ZEROMEM(&vLineStartPoint);
	ZEROMEM(&vLineDirection);

	// 움직일수 있는지 체크
	// X,Z로만 체크.
	isMove = m_pNavigation->Is_Move(vXZPosition, &vNormal, &m_eCurrentCellFlag);

	// 원래 포지션의 Y값에서 셀의 Y값 만큼 더해줌.
	if (true == isMove &&
		(CELL_MOVE == m_eCurrentCellFlag))
	{
		if (false == Check_CellY(vXZPosition, vCurrentPosition))
		{
			m_eCurrentCellFlag = m_ePreviousCellFlag;

			// 포지션 xz는 이전포지션, y는 계속 처리하게 만들어 셀보다 낮은곳에서 이동하려 할경우 막음.
			vCurrentPosition.y = vCurrentPosition.y + vVelocity.y;
			Set_Position(vCurrentPosition);
			return;
		}
	}

	_uint iExcept = { 0 };

	while (false == isMove)
	{
		vXZPosition = vXZOriginPosition + vDir - vNormal * vDir.Dot(vNormal) * vXZVelocity.Length();

		isMove = m_pNavigation->Is_Move(vXZPosition, &vNormal, &m_eCurrentCellFlag);

		if (++iExcept > 100)
		{
			vXZPosition = vXZOriginPosition;
			break;
		}
	}

	if (true == isMove &&
		(CELL_MOVE == m_eCurrentCellFlag))
	{
		Check_CellY(vXZPosition, vCurrentPosition);
	}


	_float3 vMovedPosition = _float3(vXZPosition.x, vCurrentPosition.y + vVelocity.y, vXZPosition.z);

	Set_Position(vMovedPosition);
	Update_CellFlag(vMovedPosition);
}

void CTransform::Move_Direction(_float3 vMoveDir, _float fTimeDelta)
{
	

	if (true == m_isRigidBody)
	{
		_float3 vXZDir;
		// 속도처리 있는 움직임.
		vXZDir = vMoveDir;
		vXZDir.y = 0.f;
		
		_float3 vVelocity = vXZDir * m_fSpeed * fTimeDelta;

		m_vVelocity += vVelocity;
		
		// x,z로만 상한선 처리.
		_float3 vLimitXZVelocity = m_vVelocity;
		vLimitXZVelocity.y = 0.f;

		if (vLimitXZVelocity.Length() > m_fLimitXZVelocity)
		{
			vLimitXZVelocity = XMVector3Normalize(vLimitXZVelocity) * m_fLimitXZVelocity;
			m_vVelocity.x = vLimitXZVelocity.x;
			m_vVelocity.z = vLimitXZVelocity.z;
		}
	}
	else
	{
		_float3 vDir;
		// 일반적인 움직임.
		vDir = XMVector3Normalize(vMoveDir);

		_float3		vPosition = Get_Position();

		vPosition += vDir * m_fSpeed * fTimeDelta;

		Set_Position(vPosition);
	}
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_float3 vLook = Get_Look();
	vLook.Normalize();
	Move_Direction(vLook, fTimeDelta);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_float3 vLook = Get_Look();
	vLook.Normalize();
	Move_Direction(-vLook, fTimeDelta);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_float3 vRight = Get_Right();
	vRight.Normalize();
	Move_Direction(-vRight, fTimeDelta);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_float3 vRight = Get_Right();
	vRight.Normalize();
	Move_Direction(vRight, fTimeDelta);
}

void CTransform::Chase(_float3 vTargetPosition, _float fTimeDelta, _float fMinDistance)
{
	_float3 vPosition = Get_Position();
	_float3 vDir = vTargetPosition - vPosition;

	if (fMinDistance <= vDir.Length())
		Move_Direction(vDir, fTimeDelta);
}

void CTransform::Turn(_float3 vAxis, _float fTimeDelta)
{
	_float3 vRight = Get_Right();
	_float3 vUp = Get_Up();
	_float3 vLook = Get_Look();

	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(
		Get_QuaternionVector_From_Axis(vAxis, m_fRotationSpeed * fTimeDelta));
	
//	_float4x4 RotationMatrix = XMMatrixRotationAxis(XMVector3Normalize(vAxis), m_fRotationSpeed * fTimeDelta);

	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_float3 vAxis, _float fRadian, _float fTimeDelta)
{
	_float3 vRight = Get_Right();
	_float3 vUp = Get_Up();
	_float3 vLook = Get_Look();

	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(
		Get_QuaternionVector_From_Axis(vAxis, fRadian * fTimeDelta));

	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float3 vAxis, _float fRadian)
{
	_float3 vScale = Get_Scale();

	_float3 vRight, vUp, vLook;

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(
		Get_QuaternionVector_From_Axis(vAxis, fRadian));

	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float3 vRadians)
{
	_float3 vScale = Get_Scale();

	_float3 vRight, vUp, vLook;

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;
	
	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(
		Get_QuaternionVector_RollPitchYaw(vRadians));

	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Set_Quaternion(_float4 vQuaternion)
{
	_float3 vScale = Get_Scale();

	_float3 vRight, vUp, vLook;

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::LookAt(_float3 _vTarget, _bool _isDeleteY)
{
	_float3 vPosition = Get_Position();

	_float3 vLook = _vTarget - vPosition;
	if (true == _isDeleteY)
		vLook.y = 0.f;

	_float3 vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_float3 vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scale();

	Set_Right(XMVector3Normalize(vRight) * vScale.x);
	Set_Up(XMVector3Normalize(vUp) * vScale.y);
	Set_Look(XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Lerp(_float3 _vTarget, _bool _isDeleteY)
{
	/*_float3 vPosition = Get_Position();

	_float3 vDestLook = _vTarget - vPosition;
	if (true == _isDeleteY)
		vDestLook.y = 0.f;

	_float3 vLook = XMVectorLerp(Get_Look(), vDestLook, );

	_float3 vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_float3 vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scale();

	Set_Right(XMVector3Normalize(vRight) * vScale.x);
	Set_Up(XMVector3Normalize(vUp) * vScale.y);
	Set_Look(XMVector3Normalize(vLook) * vScale.z);

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;*/
}

void CTransform::LookTo(_float3 vTarget)
{
	_float3 vScaled = Get_Scale();

	_float3 vLook = XMVector3Normalize(vTarget) * vScaled.z;
	_float3 vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScaled.x;
	_float3 vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScaled.y;


	Set_Look(XMVector3Normalize(vLook));
	Set_Right(XMVector3Normalize(vRight));
	Set_Up(XMVector3Normalize(vUp));


}

_bool CTransform::Check_CellY(_float3 vXZPosition, _float3 vCurrentPosition)
{
	_float fCellY = m_pNavigation->Get_CurrentCellY(vCurrentPosition);

	if (fCellY > XMVectorGetY(vCurrentPosition) + m_fStepOffset)
	{
		return false;
	}
	else
	{
		m_fGroundY = fCellY;
		return true;
	}
}

void CTransform::Update_CellFlag(_float3 vCurrentPosition)
{
	if (m_ePreviousCellFlag != m_eCurrentCellFlag)
	{
		switch (m_eCurrentCellFlag)
		{
		case Engine::CELL_MOVE:
			m_fGroundY = m_pNavigation->Get_CurrentCellY(vCurrentPosition);
			break;
		case Engine::CELL_FALL:
			m_fGroundY = -999.f;
			break;
		}

		m_ePreviousCellFlag = m_eCurrentCellFlag;
	}
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

	Safe_Release(m_pNavigation);
}
