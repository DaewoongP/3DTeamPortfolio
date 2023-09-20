#include "VIBuffer_UVSphere.h"

CVIBuffer_UVSphere::CVIBuffer_UVSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)\
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_UVSphere::CVIBuffer_UVSphere(const CVIBuffer_UVSphere& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_UVSphere::Initialize_Prototype()
{
	_uint iStacks = 30; // 위도
	_uint iSlices = 30; // 경도
	_float fRadius = 0.5f;

	m_iNumVertices = (iStacks + 1) * (iSlices + 1);
	m_iNumIndices = iStacks * iSlices * 6;
	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iIndexStride = { sizeof(_ushort) };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
	_ushort* pIndices = new _ushort[m_iNumIndices];

	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	//TODO
	// Vertex Buffer 생성
	for (_uint iStack = 0; iStack <= iStacks; ++iStack)
	{
		float phi = (XM_PI * iStack) / iStacks;
		float y = cosf(phi);
		float radius = sinf(phi);

		for (_uint iSlice = 0; iSlice <= iSlices; ++iSlice)
		{
			float theta = (2.0f * XM_PI * iSlice) / iSlices;
			float x = cosf(theta);
			float z = sinf(theta);

			_uint iVertex = iStack * (iSlices + 1) + iSlice;

			pVertices[iVertex].vPosition = _float3(x * fRadius * radius, y * fRadius, z * fRadius * radius);
			//pVertices[iVertex].vNormal = pVertices[iVertex].vPosition - _float3(0.f, 0.f, 0.f);
			pVertices[iVertex].vTexCoord = _float2((float)iSlice / iSlices, 1.0f - (float)iStack / iStacks);
		}
	}

	// Normal
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMVECTOR direction = XMLoadFloat3(&pVertices[i].vPosition);
		XMVECTOR normal = XMVector3Normalize(direction);
		XMStoreFloat3(&pVertices[i].vNormal, normal);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	// TODO
	_uint iIndex = 0;
	for (_uint iStack = 0; iStack < iStacks; ++iStack)
	{
		for (_uint iSlice = 0; iSlice < iSlices; ++iSlice)
		{
			pIndices[iIndex++] = (iStack + 1) * (iSlices + 1) + (iSlice + 1);
			pIndices[iIndex++] = (iStack + 1) * (iSlices + 1) + iSlice;
			pIndices[iIndex++] = iStack * (iSlices + 1) + iSlice;

			pIndices[iIndex++] = iStack * (iSlices + 1) + (iSlice + 1);
			pIndices[iIndex++] = (iStack + 1) * (iSlices + 1) + (iSlice + 1);
			pIndices[iIndex++] = iStack * (iSlices + 1) + iSlice;
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}
#pragma endregion

	// 버텍스 및 인덱스 메모리 해제
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

CVIBuffer_UVSphere* CVIBuffer_UVSphere::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CVIBuffer_UVSphere* pInstance = new CVIBuffer_UVSphere(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Geometry_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer* CVIBuffer_UVSphere::Clone(void* _pArg)
{
	CVIBuffer_UVSphere* pInstance = new CVIBuffer_UVSphere(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Geometry_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_UVSphere::Free(void)
{
	__super::Free();
}
