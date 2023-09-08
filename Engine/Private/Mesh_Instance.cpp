#include "..\Public\Mesh_Instance.h"

CMesh_Instance::CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh_Instance::CMesh_Instance(const CMesh_Instance& rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
{
}

HRESULT CMesh_Instance::Initialize_Prototype(const Engine::MESH Mesh, _float4x4 PivotMatrix, _float4x4* pInstanceMatrix, _uint iNumInstance)
{
	m_iNumInstance = { iNumInstance }; // 인스턴스의 개수

	m_iMaterialIndex = Mesh.iMaterialIndex;
	m_iIndexCountPerInstance = Mesh.iNumFaces * 3;
	m_iNumVertexBuffers = { 2 };
	m_iNumVertices = Mesh.iNumVertices;
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { m_iIndexCountPerInstance * m_iNumInstance };
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT		hr = 0;

	hr = Ready_VertexBuffer_NonAnim(Mesh, pInstanceMatrix, PivotMatrix);

	FAILED_CHECK_RETURN(hr, E_FAIL);

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumFaces = { 0 };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		for (size_t i = 0; i < Mesh.iNumFaces; i++)
		{
			pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[0];
			pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[1];
			pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[2];
		}		
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
	int i = 0;
#pragma endregion

	return S_OK;
}

HRESULT CMesh_Instance::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint					iStrides[] = {
		m_iStride, //44
		m_iInstanceStride,//44
	};

	_uint					iOffset[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CMesh_Instance::Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4* pInstanceMatrix, _float4x4 PivotMatrix)
{
#pragma region INSTANCE_BUFFER
	{
		/* 인스턴스 정점 버퍼를 생성한다.(m_pVBInstance) */
		D3D11_BUFFER_DESC		BufferDesc;
		ZeroMemory(&BufferDesc, sizeof BufferDesc);

		m_iInstanceStride = sizeof(VTXINSTANCE);

		//인스턴스 몇개인지도 받아와야함..
		BufferDesc.ByteWidth = { m_iInstanceStride * m_iNumInstance };
		BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
		BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
		BufferDesc.StructureByteStride = { m_iInstanceStride };
		BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
		BufferDesc.MiscFlags = { 0 };

		VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

		for (size_t i = 0; i < m_iNumInstance; ++i)
		{
			pVertices[i].vRight = XMFLOAT4(pInstanceMatrix[i].m[0][0], pInstanceMatrix[i].m[0][1], pInstanceMatrix[i].m[0][2], pInstanceMatrix[i].m[0][3]);
			pVertices[i].vUp = XMFLOAT4(pInstanceMatrix[i].m[1][0], pInstanceMatrix[i].m[1][1], pInstanceMatrix[i].m[1][2], pInstanceMatrix[i].m[1][3]);
			pVertices[i].vLook = XMFLOAT4(pInstanceMatrix[i].m[2][0], pInstanceMatrix[i].m[2][1], pInstanceMatrix[i].m[2][2], pInstanceMatrix[i].m[2][3]);
			pVertices[i].vTranslation = XMFLOAT4(pInstanceMatrix[i].m[3][0], pInstanceMatrix[i].m[3][1], pInstanceMatrix[i].m[3][2], pInstanceMatrix[i].m[3][3]);
		}

		D3D11_SUBRESOURCE_DATA		SubResourceData;
		ZeroMemory(&SubResourceData, sizeof SubResourceData);

		SubResourceData.pSysMem = pVertices;

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_pVBInstance)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);
	}
#pragma endregion

#pragma region VERTEX_BUFFER
	{
		m_iStride = { sizeof(VTXMESH) };

		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

		m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
		m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
		m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
		m_BufferDesc.StructureByteStride = { m_iStride };
		m_BufferDesc.CPUAccessFlags = { 0 };
		m_BufferDesc.MiscFlags = { 0 };

		VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

		for (size_t i = 0; i < m_iNumVertices; i++)
		{
			memcpy(&pVertices[i].vPosition, &Mesh.vPositions[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vPosition,
				XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

			memcpy(&pVertices[i].vNormal, &Mesh.vNormals[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vNormal,
				XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

			memcpy(&pVertices[i].vTexCoord, &Mesh.vTexCoords[i], sizeof(_float2));
			memcpy(&pVertices[i].vTangent, &Mesh.vTangents[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vTangent,
				XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix));
		}

		ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
		m_SubResourceData.pSysMem = pVertices;

		if (FAILED(__super::Create_Buffer(&m_pVB)))
			return E_FAIL;
		Safe_Delete_Array(pVertices);
	}
#pragma endregion

	return S_OK;
}

CMesh_Instance* CMesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const Engine::MESH Mesh, _float4x4* pInstanceMatrix, _uint iNumInstance, _float4x4 PivotMatrix)
{
	CMesh_Instance* pInstance = new CMesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Mesh, PivotMatrix, pInstanceMatrix, iNumInstance)))
	{
		MSG_BOX("Failed to Created CMesh_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMesh_Instance::Clone(void* pArg)
{
	CMesh_Instance* pInstance = new CMesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMesh_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
