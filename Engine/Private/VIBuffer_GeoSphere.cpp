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
	// TODO
	const float X = .525731112119133606f;
	const float Z = .850650808352039932f;

	VTXPOSNORTEX initialVertices[] = {
		{{-X, 0.0f, Z}, {-X, 0.0f, Z}, {0, 0}},
		{{X, 0.0f, Z}, {X, 0.0f, Z}, {1, 0}},
		{{-X, 0.0f, -Z}, {-X, 0.0f, -Z}, {0, 1}},
		{{X, 0.0f, -Z}, {X, 0.0f, -Z}, {1, 1}},
		{{0.0f, Z, X}, {0.0f, Z, X}, {0.5f, 0}},
		{{0.0f, Z, -X}, {0.0f, Z, -X}, {0.5f, 1}},
		{{0.0f, -Z, X}, {0.0f, -Z, X}, {0.5f, 1}},
		{{0.0f, -Z, -X}, {0.0f, -Z, -X}, {0.5f, 0}},
		{{Z, X, 0.0f}, {Z, X, 0.0f}, {1, 0.5f}},
		{{-Z, X, 0.0f}, {-Z, X, 0.0f}, {0, 0.5f}},
		{{Z, -X, 0.0f}, {Z, -X, 0.0f}, {1, 0.5f}},
		{{-Z, -X, 0.0f}, {-Z, -X, 0.0f}, {0, 0.5f}}
	};

	// reverse
	_ushort initialIndices[] = {
		1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
		1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
	};


	// 측지구 세분화
	vector<Vertex> vertices(begin(initialVertices), end(initialVertices));
	vector<Triangle> triangles;

	for (int i = 0; i < sizeof(initialIndices) / sizeof(_ushort); i += 3)
	{
		triangles.push_back({ initialIndices[i], initialIndices[i + 1], initialIndices[i + 2] });
	}

	for (int i = 0; i < m_iSubdivisionNum; ++i)
	{
		subdivide(vertices, triangles);
	}

	// 정규화
	for (auto& vertex : vertices)
	{
		float length = sqrt(vertex.vPosition.x * vertex.vPosition.x + vertex.vPosition.y * vertex.vPosition.y + vertex.vPosition.z * vertex.vPosition.z);
		vertex.vPosition.x /= length;
		vertex.vPosition.y /= length;
		vertex.vPosition.z /= length;
	}

	m_iNumVertices = static_cast<_uint>(vertices.size());
	m_iNumIndices = static_cast<_uint>(triangles.size() * 3);

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

	memcpy(pVertices, &vertices[0], sizeof(VTXPOSNORTEX) * m_iNumVertices);

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

	// 수정된 triangles 배열에서 데이터를 복사합니다.
	for (int i = 0, idx = 0; i < triangles.size(); ++i)
	{
		pIndices[idx++] = triangles[i].v1;
		pIndices[idx++] = triangles[i].v2;
		pIndices[idx++] = triangles[i].v3;
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

_uint CVIBuffer_GeoSphere::getMidpointIndex(vector<Vertex>& vertices, _uint index1, _uint index2)
{
	// Check if midpoint already exists in cache
	if (m_MidPointCache.find({ index1, index2 }) != m_MidPointCache.end())
		return m_MidPointCache[{index1, index2}];

	// Compute new midpoint vertex
	Vertex midpoint;
	midpoint.vPosition.x = (vertices[index1].vPosition.x + vertices[index2].vPosition.x) * 0.5f;
	midpoint.vPosition.y = (vertices[index1].vPosition.y + vertices[index2].vPosition.y) * 0.5f;
	midpoint.vPosition.z = (vertices[index1].vPosition.z + vertices[index2].vPosition.z) * 0.5f;

	// Average and set the UV coordinates
	midpoint.vTexCoord.x = (vertices[index1].vTexCoord.x + vertices[index2].vTexCoord.x) * 0.5f;
	midpoint.vTexCoord.y = (vertices[index1].vTexCoord.y + vertices[index2].vTexCoord.y) * 0.5f;

	// Average the normals (Note: This will need to be normalized if the normals are not unit vectors)
	XMVECTOR normal1 = XMLoadFloat3(&vertices[index1].vNormal);
	XMVECTOR normal2 = XMLoadFloat3(&vertices[index2].vNormal);
	XMVECTOR avgNormal = XMVector3Normalize(XMVectorAdd(normal1, normal2));
	XMStoreFloat3(&midpoint.vNormal, avgNormal);

	// Normalize vPosition to move it onto the sphere
	XMVECTOR pos = XMLoadFloat3(&midpoint.vPosition);
	pos = XMVector3Normalize(pos);
	XMStoreFloat3(&midpoint.vPosition, pos);

	vertices.push_back(midpoint);
	unsigned int newIndex = vertices.size() - 1;

	// Store in cache and return new index
	m_MidPointCache[{index1, index2}] = newIndex;
	m_MidPointCache[{index2, index1}] = newIndex;

	return newIndex;
}

void CVIBuffer_GeoSphere::subdivide(vector<Vertex>& vertices, vector<Triangle>& triangles)
{
	std::vector<Triangle> newTriangles;

	for (const Triangle& tri : triangles)
	{
		unsigned int mid1 = getMidpointIndex(vertices, tri.v1, tri.v2);
		unsigned int mid2 = getMidpointIndex(vertices, tri.v2, tri.v3);
		unsigned int mid3 = getMidpointIndex(vertices, tri.v1, tri.v3);

		newTriangles.push_back({ tri.v1, mid1, mid3 });
		newTriangles.push_back({ tri.v2, mid2, mid1 });
		newTriangles.push_back({ tri.v3, mid3, mid2 });
		newTriangles.push_back({ mid1, mid2, mid3 });
	}

	triangles = newTriangles;
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
