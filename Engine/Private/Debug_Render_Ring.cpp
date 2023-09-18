#include "..\Public\Debug_Render_Ring.h"

CDebug_Render_Ring::CDebug_Render_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CDebug_Render_Ring::CDebug_Render_Ring(const CDebug_Render_Ring& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CDebug_Render_Ring::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Ring Desc arg null");
		return E_FAIL;
	}

	RINGDESC RingDesc = *reinterpret_cast<RINGDESC*>(pArg);
	_uint iRingSegments = { 32 };

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOS) };
	m_iNumVertices = { iRingSegments + 1 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { m_iNumVertices * 2 };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOS* pVertices = New VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	/* Init */
	_float fAngleDelta = XMConvertToRadians(360.f) / _float(iRingSegments);

	_float3 vCosDelta = XMVectorReplicate(cosf(fAngleDelta));
	_float3 vSinDelta = XMVectorReplicate(sinf(fAngleDelta));
	_float3 vIncrementalSin = XMVectorZero();
	_float3 vIncrementalCos = _float3(1.f, 1.f, 1.f);

	// origin = center
	// Axis 두개설정해서 ex) xz평면 식으로 처리하나봄.
	_float fRadius = RingDesc.fRadius;
	_float3 vOrigin = RingDesc.vOffsetPosition;
	_float4 vQuat = RingDesc.vOffsetRotation;

	_float3 vMajorAxis = XMVector3TransformNormal(RingDesc.vMajorAxis * fRadius, XMMatrixRotationQuaternion(vQuat));
	_float3 vMinorAxis = XMVector3TransformNormal(RingDesc.vMinorAxis * fRadius, XMMatrixRotationQuaternion(vQuat));

	for (_uint i = 0; i < iRingSegments; ++i)
	{
		_float3 vPos = XMVectorMultiplyAdd(vMajorAxis, vIncrementalCos, vOrigin);
		vPos = XMVectorMultiplyAdd(vMinorAxis, vIncrementalSin, vPos);
		pVertices[i].vPosition = vPos;

		_float3 vNewCos = vIncrementalCos * vCosDelta - vIncrementalSin * vSinDelta;
		_float3 vNewSin = vIncrementalCos * vSinDelta + vIncrementalSin * vCosDelta;
		vIncrementalCos = vNewCos;
		vIncrementalSin = vNewSin;
	}

	pVertices[iRingSegments] = pVertices[0];

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ushort* pIndices = New _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = i;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

CDebug_Render_Ring* CDebug_Render_Ring::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_Render_Ring* pInstance = New CDebug_Render_Ring(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDebug_Render_Ring");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CDebug_Render_Ring::Clone(void* pArg)
{
	CDebug_Render_Ring* pInstance = New CDebug_Render_Ring(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDebug_Render_Ring");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDebug_Render_Ring::Free()
{
	__super::Free();

}
