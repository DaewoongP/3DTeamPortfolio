#include "..\Public\Mesh.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_BoneIndices(rhs.m_BoneIndices)
{
	lstrcpy(m_szName, rhs.m_szName);
}

void CMesh::Get_Matrices(CModel::BONES Bones, _float4x4* pMatrices, _float4x4 PivotMatrix)
{
	_uint		iIndex = 0;

	for (auto iBoneIndex : m_BoneIndices)
	{
		_float4x4 OffsetMatrix = Bones[iBoneIndex]->Get_OffsetMatrix();
		_float4x4 CombinedMatrix = Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
		pMatrices[iIndex++] = OffsetMatrix * CombinedMatrix * PivotMatrix;
	}
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, const CModel::BONES& Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix)
{
	m_iMaterialIndex = Mesh.iMaterialIndex;
	lstrcpy(m_szName, Mesh.szName);
	m_iNumVertexBuffers = { 1 };
	m_iNumVertices = { Mesh.iNumVertices };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { Mesh.iNumFaces * 3 };
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == eType)
	{
		hr = Ready_VertexBuffer_NonAnim(Mesh, PivotMatrix);
	}
	else
	{
		hr = Ready_VertexBuffer_Anim(Mesh, Bones);
	}
	
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

	for (_uint i = 0; i < Mesh.iNumFaces; ++i)
	{
		pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[0];
		pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[1];
		pIndices[iNumFaces++] = Mesh.Faces[i].iIndices[2];
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4 PivotMatrix)
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

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(const Engine::MESH Mesh, const CModel::BONES& Bones)
{
	m_iStride = { sizeof(VTXANIMMESH) };

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };


	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &Mesh.vPositions[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &Mesh.vNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexCoord, &Mesh.vTexCoords[i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &Mesh.vTangents[i], sizeof(_float3));
	}

	m_iNumBones = Mesh.iNumBones;

	for (_uint i = 0; i < Mesh.iNumBones; ++i)
	{
		Engine::BONE Bone = Mesh.Bones[i];

		_uint iIndex = { 0 };

		auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue) {
			if (!lstrcmp(Bone.szName, pValue->Get_Name()))
				return true;
			else
			{
				++iIndex;
				return false;
			}
		});
		
		_float4x4		OffsetMatrix;
		memcpy(&OffsetMatrix, &Bone.OffsetMatrix, sizeof(_float4x4));

		Bones[iIndex]->Set_OffsetMatrix(OffsetMatrix);

		m_BoneIndices.push_back(iIndex);

		for (_uint j = 0; j < Bone.iNumWeights; ++j)
		{
			Engine::WEIGHT VertexWeight = Bone.Weights[j];
			
			if (0.f == pVertices[VertexWeight.iVertexId].vBlendWeights.x)
			{
				pVertices[VertexWeight.iVertexId].vBlendIndices.x = i;
				pVertices[VertexWeight.iVertexId].vBlendWeights.x = VertexWeight.fWeight;
			}
			else if (0.f == pVertices[VertexWeight.iVertexId].vBlendWeights.y)
			{
				pVertices[VertexWeight.iVertexId].vBlendIndices.y = i;
				pVertices[VertexWeight.iVertexId].vBlendWeights.y = VertexWeight.fWeight;
			}
			else if (0.f == pVertices[VertexWeight.iVertexId].vBlendWeights.z)
			{
				pVertices[VertexWeight.iVertexId].vBlendIndices.z = i;
				pVertices[VertexWeight.iVertexId].vBlendWeights.z = VertexWeight.fWeight;
			}
			else if (0.f == pVertices[VertexWeight.iVertexId].vBlendWeights.w)
			{
				pVertices[VertexWeight.iVertexId].vBlendIndices.w = i;
				pVertices[VertexWeight.iVertexId].vBlendWeights.w = VertexWeight.fWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint	iBoneIndex = 0;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue)
			{
				if (!lstrcmp(m_szName, pValue->Get_Name()))
					return true;
				else
				{
					++iBoneIndex;
					return false;
				}
			});


		m_BoneIndices.push_back(iBoneIndex);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const CModel::BONES& Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, Bones, Mesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh::Free()
{
	__super::Free();
}
