#include "..\Public\Transform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

void CTransform::Set_Scale(_float3 _vScale)
{
	Set_Right(Get_Right() * _vScale.x);
	Set_Up(Get_Up() * _vScale.y);
	Set_Look(Get_Look() * _vScale.z);
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
}

void CTransform::Move_Direction(_float3 vDirection, _float fTimeDelta)
{
	_float3 vPosition = Get_Translation();
	
	vDirection.Normalize();
	
	vPosition += vDirection * m_fSpeed * fTimeDelta;

	Set_Position(vPosition);
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

	_float4x4 RotationMatrix = XMMatrixRotationAxis(XMVector3Normalize(vAxis), m_fRotationSpeed * fTimeDelta);

	Set_Right(XMVector3TransformNormal(vRight, RotationMatrix));
	Set_Up(XMVector3TransformNormal(vUp, RotationMatrix));
	Set_Look(XMVector3TransformNormal(vLook, RotationMatrix));
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
}
