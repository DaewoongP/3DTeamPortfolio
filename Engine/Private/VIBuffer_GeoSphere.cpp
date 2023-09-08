#include "VIBuffer_GeoSphere.h"

CVIBuffer_GeoSphere::CVIBuffer_GeoSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)\
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_GeoSphere::CVIBuffer_GeoSphere(const CVIBuffer_GeoSphere& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_GeoSphere::Initialize_Prototype()
{
	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = { 12 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { 60 };
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
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	// TODO
	const float X = .525731112119133606f;
	const float Z = .850650808352039932f;

	VTXPOSNORTEX initialVertices[] = {
		{{-X, 0.0f, Z}, {0, 0, 1}, {0, 0}},
		{{X, 0.0f, Z}, {0, 0, 1}, {1, 0}},
		{{-X, 0.0f, -Z}, {0, 0, -1}, {0, 1}},
		{{X, 0.0f, -Z}, {0, 0, -1}, {1, 1}},
		{{0.0f, Z, X}, {0, 1, 0}, {0.5f, 0}},
		{{0.0f, Z, -X}, {0, 1, 0}, {0.5f, 1}},
		{{0.0f, -Z, X}, {0, -1, 0}, {0.5f, 1}},
		{{0.0f, -Z, -X}, {0, -1, 0}, {0.5f, 0}},
		{{Z, X, 0.0f}, {1, 0, 0}, {1, 0.5f}},
		{{-Z, X, 0.0f}, {-1, 0, 0}, {0, 0.5f}},
		{{Z, -X, 0.0f}, {1, 0, 0}, {1, 0.5f}},
		{{-Z, -X, 0.0f}, {-1, 0, 0}, {0, 0.5f}}
	};

	memcpy(pVertices, initialVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

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

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	// TODO
	_ushort initialIndices[] = {
	0,4,1, 0,9,4, 9,5,4, 4,5,8, 4,8,1,
	8,10,1, 8,3,10, 5,3,8, 5,2,3, 2,7,3,
	7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
	6,1,10, 9,0,11, 9,11,2, 9,2,5, 7,2,11
	};

	memcpy(pIndices, initialIndices, sizeof(_ushort) * m_iNumIndices);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

CVIBuffer_GeoSphere* CVIBuffer_GeoSphere::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CVIBuffer_GeoSphere* pInstance = new CVIBuffer_GeoSphere(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Geometry_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer* CVIBuffer_GeoSphere::Clone(void* _pArg)
{
	CVIBuffer_GeoSphere* pInstance = new CVIBuffer_GeoSphere(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Geometry_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_GeoSphere::Free(void)
{
	__super::Free();
}
