#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

HRESULT CFrustum::Initialize()
{
	m_vOriginal_Points[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginal_Points[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginal_Points[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginal_Points[3] = _float3(-1.f, -1.f, 0.f);
	m_vOriginal_Points[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginal_Points[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginal_Points[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginal_Points[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	/* 월드상의 절두체 평면을 구한다. */

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float3			vPoints[8];

	_float4x4			ProjMatrixInv = *pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	_float4x4			ViewMatrixInv = *pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	Safe_Release(pPipeLine);

	for (_uint i = 0; i < 8; ++i)
	{
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginal_Points[i]), ProjMatrixInv);
		vPoints[i] = XMVector3TransformCoord(vPoints[i], ViewMatrixInv);
	}

	m_vWorldPlanes[0] = XMPlaneFromPoints(vPoints[1], vPoints[5], vPoints[6]);
	m_vWorldPlanes[1] = XMPlaneFromPoints(vPoints[4], vPoints[0], vPoints[3]);
	m_vWorldPlanes[2] = XMPlaneFromPoints(vPoints[4], vPoints[5], vPoints[1]);
	m_vWorldPlanes[3] = XMPlaneFromPoints(vPoints[3], vPoints[2], vPoints[6]);
	m_vWorldPlanes[4] = XMPlaneFromPoints(vPoints[5], vPoints[4], vPoints[7]);
	m_vWorldPlanes[5] = XMPlaneFromPoints(vPoints[0], vPoints[1], vPoints[2]);
}

void CFrustum::Transform_ToLocalSpace(_float4x4 WorldMatrix)
{
	_float4x4		WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);

	for (_uint i = 0; i < 6; ++i)
	{
		m_vLocalPlanes[i] = XMPlaneTransform(m_vWorldPlanes[i], WorldMatrixInv);
	}
}

_bool CFrustum::isIn_WorldFrustum(_float4 vWorldPos, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos)))
		{
			return false;
		}
	}

	return true;
}

_bool CFrustum::isIn_LocalSpace(_float4 vLocalPos, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(m_vLocalPlanes[i], vLocalPos)))
			return false;
	}

	return true;
}

void CFrustum::Free()
{
}
