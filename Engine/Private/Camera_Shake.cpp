#include "..\Public\Camera_Shake.h"
#include "Camera_Manager.h"
#include "PipeLine.h"

CCamera_Shake::CCamera_Shake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext),
	m_pCamera_Manager(CCamera_Manager::GetInstance()),
	m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pCamera_Manager);
	Safe_AddRef(m_pPipeLine);
}

CCamera_Shake::CCamera_Shake(const CCamera_Shake& rhs)
	: CComponent(rhs),
	m_pCamera_Manager(rhs.m_pCamera_Manager),
	m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pCamera_Manager);
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera_Shake::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Shake::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Failed Initialize CCamera_Shake");
		return E_FAIL;
	}

	m_Camera_Shake_Desc = *static_cast<CAMERA_SHAKE_DESC*>(pArg);

	if (true == m_Camera_Shake_Desc.isDistanceOption && nullptr == m_Camera_Shake_Desc.pTransform)
	{
		MSG_BOX("Failed Initialize CCamera_Shake");
		return E_FAIL;
	}

	Safe_AddRef(m_Camera_Shake_Desc.pTransform);

	return S_OK;
}

void CCamera_Shake::Ready_Shake(_float _fMax, _float _fMin, _float _fPower)
{
	m_fMaxCameraShakeDistance = _fMax;
	m_fMinCameraShakeDistance = _fMin;
	m_fShakePower = _fPower;
}

void CCamera_Shake::RandomRightAxisShake()
{
	Shake_Power();

	_float3 vRandomAxis = *m_pPipeLine->Get_CamRight();

	vRandomAxis.Normalize();

	_float3 vLook = *m_pPipeLine->Get_CamLook();

	_float fRandomRadian = _float(rand() % 30);

	fRandomRadian *= _float((rand() % 2) - 1);

	vRandomAxis = XMVector3TransformNormal(vRandomAxis, XMMatrixRotationAxis(vLook, XMConvertToRadians(fRandomRadian)));

	m_pCamera_Manager->Set_Shake(
		m_Camera_Shake_Desc.eShake_Priority,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Type,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis,
		m_Camera_Shake_Desc.Shake_Info_Desc.eEase,
		m_Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed,
		m_Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration,
		m_Camera_Shake_Desc.Shake_Info_Desc.fShakePower,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Power,
		vRandomAxis);
}

void CCamera_Shake::RandomUpAxisShake()
{
	Shake_Power();

	_float3 vRandomAxis = *m_pPipeLine->Get_CamUp();

	vRandomAxis.Normalize();

	_float3 vLook = *m_pPipeLine->Get_CamLook();

	_float fRandomRadian = _float(rand() % 30);

	fRandomRadian *= _float((rand() % 2) - 1);

	vRandomAxis = XMVector3TransformNormal(vRandomAxis, XMMatrixRotationAxis(vLook, XMConvertToRadians(fRandomRadian)));

	m_pCamera_Manager->Set_Shake(
		m_Camera_Shake_Desc.eShake_Priority,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Type,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis,
		m_Camera_Shake_Desc.Shake_Info_Desc.eEase,
		m_Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed,
		m_Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration,
		m_Camera_Shake_Desc.Shake_Info_Desc.fShakePower,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Power,
		vRandomAxis);
}

void CCamera_Shake::GoAndComeBackShake()
{
	Shake_Power();

	m_pCamera_Manager->Set_Shake(
		m_Camera_Shake_Desc.eShake_Priority,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Type,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis,
		m_Camera_Shake_Desc.Shake_Info_Desc.eEase,
		1.0f / m_Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration,
		m_Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration,
		m_Camera_Shake_Desc.Shake_Info_Desc.fShakePower,
		m_Camera_Shake_Desc.Shake_Info_Desc.eShake_Power,
		m_Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set);
}




void CCamera_Shake::Shake_Power()
{
	if (false == m_Camera_Shake_Desc.isDistanceOption)
	{
		return;
	}

	_float fDistance = XMVectorGetX(XMVector3Length(m_pPipeLine->Get_CamPosition()->xyz() - m_Camera_Shake_Desc.pTransform->Get_Position()));

	if (m_fMinCameraShakeDistance >= fDistance)
	{
		m_fDistanceRatio = 1.0f;
	}
	else if (m_fMaxCameraShakeDistance <= fDistance)
	{
		m_fDistanceRatio = 0.0f;
	}
	else
	{
		fDistance -= m_fMinCameraShakeDistance;
		_float fRatioDistance = m_fMaxCameraShakeDistance - m_fMinCameraShakeDistance;

		m_fDistanceRatio = 1.0f - fDistance / fRatioDistance;
	}

	m_Camera_Shake_Desc.Shake_Info_Desc.fShakePower = m_fDistanceRatio * m_fShakePower;

	return;
}

CCamera_Shake* CCamera_Shake::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Shake* pInstance = New CCamera_Shake(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		__debugbreak();
		MSG_BOX("Failed to Created CCamera_Shake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCamera_Shake::Clone(void* pArg)
{
	CCamera_Shake* pInstance = New CCamera_Shake(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Shake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Shake::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		Safe_Release(m_Camera_Shake_Desc.pTransform);
	}
	
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pPipeLine);
}
