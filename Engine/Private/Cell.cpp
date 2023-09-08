#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)

{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

	_float3		vLine;

	vLine = pPoints[POINT_B] - pPoints[POINT_A];
	m_vNormals[NEIGHBOR_AB] = _float3(vLine.z * -1.f, 0.f, vLine.x);

	vLine = pPoints[POINT_C] - pPoints[POINT_B];
	m_vNormals[NEIGHBOR_BC] = _float3(vLine.z * -1.f, 0.f, vLine.x);

	vLine = pPoints[POINT_A] - pPoints[POINT_C];
	m_vNormals[NEIGHBOR_CA] = _float3(vLine.z * -1.f, 0.f, vLine.x);

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(_float3 vSourPoint, _float3 vDestPoint)
{
	if (m_vPoints[POINT_A] == vSourPoint)
	{
		if (m_vPoints[POINT_B] == vDestPoint)
		{
			return true;
		}
		if (m_vPoints[POINT_C] == vDestPoint)
		{
			return true;
		}
	}

	if (m_vPoints[POINT_B] == vSourPoint)
	{
		if (m_vPoints[POINT_C] ==  vDestPoint)
		{
			return true;
		}
		if (m_vPoints[POINT_A] == vDestPoint)
		{
			return true;
		}
	}

	if (m_vPoints[POINT_C] == vSourPoint)
	{
		if (m_vPoints[POINT_A] == vDestPoint)
		{
			return true;
		}
		if (m_vPoints[POINT_B] == vDestPoint)
		{
			return true;
		}
	}

	return false;
}

_bool CCell::is_In(_float3 vPosition, _int* pNeighborIndex)
{
	for (size_t i = 0; i < NEIGHBOR_END; i++)
	{
		_float3		vDir = XMVector3Normalize(vPosition - m_vPoints[i]);
		_float3		vNormal = XMVector3Normalize(m_vNormals[i]);
		
		if (0 < vDir.Dot(vNormal))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer->Render();

	return S_OK;
}

#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
