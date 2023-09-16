#include "..\Public\Debug_Render_TriangleMesh.h"

CDebug_Render_TriangleMesh::CDebug_Render_TriangleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CDebug_Render_TriangleMesh::CDebug_Render_TriangleMesh(const CDebug_Render_TriangleMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CDebug_Render_TriangleMesh::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("TriMesh arg null");
		return E_FAIL;
	}

	TRIANGLEMESHDESC TriangleMeshDesc = *reinterpret_cast<TRIANGLEMESHDESC*>(pArg);

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOS) };
	m_iNumVertices = { TriangleMeshDesc.iNum * 3 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { TriangleMeshDesc.iNum * 3 };
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

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vPosition = TriangleMeshDesc.pTriangles[i];

		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition),
			XMMatrixRotationQuaternion(TriangleMeshDesc.vOffsetRotation)) + XMLoadFloat3(&TriangleMeshDesc.vOffsetPosition));
	}

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

CDebug_Render_TriangleMesh* CDebug_Render_TriangleMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_Render_TriangleMesh* pInstance = new CDebug_Render_TriangleMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDebug_Render_TriangleMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CDebug_Render_TriangleMesh::Clone(void* pArg)
{
	CDebug_Render_TriangleMesh* pInstance = new CDebug_Render_TriangleMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDebug_Render_TriangleMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDebug_Render_TriangleMesh::Free()
{
	__super::Free();
}
