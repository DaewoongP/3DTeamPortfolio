#include "..\Public\Transform.h"
#include "RigidBody.h"
#include "GameInstance.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_float3 CTransform::Get_Scale_With_Negative() const
{
	_float3 vScale = { 1.f, 1.f, 1.f };

	if (0.f > m_WorldMatrix.m[0][0])
		vScale.x = Get_Right().Length() * -1.f;
	else
		vScale.x = Get_Right().Length();

	if (0.f > m_WorldMatrix.m[1][1])
		vScale.y = Get_Up().Length() * -1.f;
	else
		vScale.y = Get_Up().Length();

	if (0.f > m_WorldMatrix.m[2][2])
		vScale.z = Get_Look().Length() * -1.f;
	else
		vScale.z = Get_Look().Length();

	return vScale;
}

_float4 CTransform::Get_Quaternion()
{
	_float4x4 RotationMatrix = m_WorldMatrix;
	_float4 vTrans = _float4(0.f, 0.f, 0.f, 1.f);
	memcpy(RotationMatrix.m[3], &vTrans, sizeof(_float4));

	return XMQuaternionRotationMatrix(RotationMatrix);
}

_float2 CTransform::Get_Trnaslation_To_UI_fXY() const
{
	_float2 fTransflation;
	_uint g_iWinSizeX = 1280;
	_uint g_iWinSizeY = 720;

	fTransflation.x = m_WorldMatrix.Translation().x + g_iWinSizeX * 0.5f;
	fTransflation.y = -(m_WorldMatrix.Translation().y - g_iWinSizeY * 0.5f);

	return fTransflation;
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

_float3 CTransform::Get_Velocity()
{
	return m_vVelocity;
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

void CTransform::Set_Scale_No_Zero(_float3 _vScale)
{
	/* Tool에서 맵 오브젝트를 설치할 때 실수로 0의 값을 집어넣는
	경우를 막기위한 함수 */
	/* 추가로 음수값도 잘 받도록 조절 */

	_float3 vRight = Get_Right();
	_float3 vUp = Get_Up();
	_float3 vLook = Get_Look();

	// 값이 0이 안되도록 막음
	if (0.f == _vScale.x)
		_vScale.x = 0.1f;
	if (0.f == _vScale.y)
		_vScale.y = 0.1f;
	if (0.f == _vScale.z)
		_vScale.z = 0.1f;

	// 스케일 부호 고정
	if (0.f > vRight.x)
		_vScale.x *= -1.f;

	if (0.f > vUp.y)
		_vScale.y *= -1.f;

	if (0.f > vLook.z)
		_vScale.z *= -1.f;

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

	m_ubTransformChanged |= CHANGEFLAG::TRANSLATION;

}

void CTransform::Set_WorldMatrix(_float4x4 _WorldMatrix)
{
	m_WorldMatrix = _WorldMatrix;

	m_ubTransformChanged |= CHANGEFLAG::ROTATION | CHANGEFLAG::TRANSLATION;

	Update_Components();
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
	if (true == isnan(m_WorldMatrix.Right().x))
	{
		m_WorldMatrix._11 = 1.f; m_WorldMatrix._12 = 0.f; m_WorldMatrix._13 = 0.f; m_WorldMatrix._14 = 0.f;
		m_WorldMatrix._21 = 0.f; m_WorldMatrix._22 = 1.f; m_WorldMatrix._23 = 0.f; m_WorldMatrix._24 = 0.f;
		m_WorldMatrix._31 = 0.f; m_WorldMatrix._32 = 0.f; m_WorldMatrix._33 = 1.f; m_WorldMatrix._34 = 0.f;
	}

	m_vVelocity = (Get_Position() - m_vPrePosition) * 60.f; // 60frame 기준 고정 속도처리
	m_vPrePosition = Get_Position();

	Update_Components();
}

void CTransform::Move_Direction(_float3 vDirection, _float fTimeDelta)
{
	_float3 vPosition = Get_Position();

	vDirection.Normalize();

	vPosition += vDirection * m_fSpeed * fTimeDelta;

	Set_Position(vPosition);

	m_ubTransformChanged |= CHANGEFLAG::TRANSLATION;
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

void CTransform::Turn(_float3 vAxis, _float fTimeDelta)
{
	_float3 vRight = Get_Right();
	_float3 vUp = Get_Up();
	_float3 vLook = Get_Look();

	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(
		Get_QuaternionVector_From_Axis(vAxis, m_fRotationSpeed * fTimeDelta));

	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;
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

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;
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

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;
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

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;
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

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;
}

void CTransform::Set_Quaternion_With_Negative(_float4 vQuaternion)
{
	_float3 vScale = Get_Scale_With_Negative();

	_float3 vRight, vUp, vLook;

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;
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

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;
}

void CTransform::LookAt_Lerp(_float3 _vTarget, const _float& fTimeDelta, _bool _isDeleteY)
{
	_float3 vPosition = Get_Position();
	_float3 vLook = Get_Look();
	_float3 vDir = _vTarget - vPosition;

	if (true == _isDeleteY)
	{
		vLook.y = 0.f;
		vDir.y = 0.f;
	}
	vLook.Normalize();
	vDir.Normalize();

	vLook = XMVectorLerp(vLook, vDir, fTimeDelta * m_fRotationSpeed);
	vLook.Normalize();

	_float3 vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vRight.Normalize();
	_float3 vUp = XMVector3Cross(vLook, vRight);
	vUp.Normalize();

	_float3 vScale = Get_Scale();

	Set_Right(XMVector3Normalize(vRight) * vScale.x);
	Set_Up(XMVector3Normalize(vUp) * vScale.y);
	Set_Look(XMVector3Normalize(vLook) * vScale.z);

	m_ubTransformChanged |= CHANGEFLAG::ROTATION;
}

HRESULT CTransform::Write_File(HANDLE hFile, LPDWORD dwByte)
{
	if (false == WriteFile(hFile, &m_WorldMatrix, sizeof(_float4x4), dwByte, nullptr))
		return E_FAIL;
	if (false == WriteFile(hFile, &m_fSpeed, sizeof(_float), dwByte, nullptr))
		return E_FAIL;
	if (false == WriteFile(hFile, &m_fRotationSpeed, sizeof(_float), dwByte, nullptr))
		return E_FAIL;
	if (false == WriteFile(hFile, &m_ubTransformChanged, sizeof(_ubyte), dwByte, nullptr))
		return E_FAIL;
	if (false == WriteFile(hFile, &m_vPrePosition, sizeof(_float3), dwByte, nullptr))
		return E_FAIL;
	if (false == WriteFile(hFile, &m_vVelocity, sizeof(_float3), dwByte, nullptr))
		return E_FAIL;

	return S_OK;
}

HRESULT CTransform::Read_File(HANDLE hFile, LPDWORD dwByte)
{
	if (false == ReadFile(hFile, &m_WorldMatrix, sizeof(_float4x4), dwByte, nullptr))
		return E_FAIL;
	if (false == ReadFile(hFile, &m_fSpeed, sizeof(_float), dwByte, nullptr))
		return E_FAIL;
	if (false == ReadFile(hFile, &m_fRotationSpeed, sizeof(_float), dwByte, nullptr))
		return E_FAIL;
	if (false == ReadFile(hFile, &m_ubTransformChanged, sizeof(_ubyte), dwByte, nullptr))
		return E_FAIL;
	if (false == ReadFile(hFile, &m_vPrePosition, sizeof(_float3), dwByte, nullptr))
		return E_FAIL;
	if (false == ReadFile(hFile, &m_vVelocity, sizeof(_float3), dwByte, nullptr))
		return E_FAIL;

	return S_OK;
}

void CTransform::Update_Components()
{
	if (nullptr == m_pRigidBody)
		return;

	if (m_ubTransformChanged & CHANGEFLAG::TRANSLATION)
	{
		m_pRigidBody->Set_Position(Get_Position());
	}
	else
	{
		Set_Position(m_pRigidBody->Get_Position());
	}

	if (m_ubTransformChanged & CHANGEFLAG::ROTATION)
	{
		m_pRigidBody->Set_Rotation(Get_Quaternion());
	}
	else
	{
		Set_Quaternion(m_pRigidBody->Get_Rotation());
	}

	// Controller
	/*if (m_ubTransformChanged & CHANGEFLAG::TRANSLATION)
		controller->Translate(Get_Position());
	else
		m_Position = controller->GetPosition();*/

	m_ubTransformChanged = CHANGEFLAG::NONE;
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

	Safe_Release(m_pRigidBody);
}
