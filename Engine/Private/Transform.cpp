#include "..\Public\Transform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_isRigidBody(rhs.m_isRigidBody)
	, m_vGravity(rhs.m_vGravity)
	, m_vVelocity(rhs.m_vVelocity)
	, m_vForce(rhs.m_vForce)
	, m_vAccel(rhs.m_vAccel)
	, m_fMass(rhs.m_fMass)
	, m_fResistance(rhs.m_fResistance)
{
}

_float3 CTransform::Get_Scale() const
{
	return _float3(Get_Right().Length(), Get_Up().Length(), Get_Look().Length());
}

_float3 CTransform::Get_Right() const
{
	return m_WorldMatrix.Right();
}

_float3 CTransform::Get_Up() const
{
	return m_WorldMatrix.Up();
}

_float3 CTransform::Get_Look() const
{
	return m_WorldMatrix.Look();
}

_float3 CTransform::Get_Translation() const
{
	return m_WorldMatrix.Translation();
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
	__super::Tick(fTimeDelta);
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
