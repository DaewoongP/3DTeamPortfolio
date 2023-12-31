#include "..\Public\VIBuffer_Terrain.h"
#include "Frustum.h"
#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_pPos(rhs.m_pPos)
	, m_pIndex(rhs.m_pIndex)
	, m_pQuadTree(rhs.m_pQuadTree)
	, m_iTerrainSizeX(rhs.m_iTerrainSizeX)
	, m_iTerrainSizeZ(rhs.m_iTerrainSizeZ)
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMap)
{
	_ulong			dwByte = { 0 };
	HANDLE			hFile = CreateFile(pHeightMap, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	BITMAPFILEHEADER	fh;
	BITMAPINFOHEADER	ih;

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	_uint iNumVerticesX = { (_uint)ih.biWidth };
	_uint iNumVerticesZ = { (_uint)ih.biHeight };

	m_iTerrainSizeX = iNumVerticesX;
	m_iTerrainSizeZ = iNumVerticesZ;

	_ulong* pPixel = New _ulong[iNumVerticesX * iNumVerticesZ];
	ZeroMemory(pPixel, sizeof(_ulong) * iNumVerticesX * iNumVerticesZ);

	ReadFile(hFile, pPixel, sizeof(_ulong) * iNumVerticesX * iNumVerticesZ, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = { iNumVerticesX * iNumVerticesZ };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2 * 3 };
	m_eFormat = { DXGI_FORMAT_R32_UINT };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

#pragma region VERTEX_BUFFER

	VTXPOSNORTEX* pVertices = New VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	m_pPos = New _float3[m_iNumVertices];
	ZeroMemory(m_pPos, sizeof(_float3) * m_iNumVertices);

	// Bmp 로드
	DirectX::ScratchImage image;
	HRESULT hr = DirectX::LoadFromWICFile(pHeightMap, DirectX::WIC_FLAGS_NONE, nullptr, image);

	// 이미지 데이터 추출
	const DirectX::Image* img = image.GetImage(0, 0, 0);

	// Height Map 생성 (예: R 채널 값 사용)
	std::vector<float> heightMap(img->height * img->width);

	if (SUCCEEDED(hr))	{
		
		for (size_t i = 0; i < img->height * img->width; ++i)
		{
			heightMap[i] = static_cast<float>(img->pixels[i * 4]); // R 채널 값을 사용
		}
	}

	for (_uint i = 0; i < iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < iNumVerticesX; j++)
		{
			_uint		iIndex = i * iNumVerticesX + j;

			//m_pPos[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 10.f, (_float)i);
			m_pPos[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, (heightMap[iIndex]) / 1.f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexCoord = _float2((_float)j / (iNumVerticesX - 1.f), i / (iNumVerticesZ - 1.f));
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEX_BUFFER

	_ulong* pIndices = New _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	m_pIndex = New _uint[m_iNumIndices];
	ZeroMemory(m_pIndex, sizeof(_uint) * m_iNumIndices);
	//memcpy(m_pIndex, pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + iNumVerticesX,
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			XMVECTOR	vSour, vDest, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) -
				XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) -
				XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) -
				XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) -
				XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion

	m_pQuadTree = CQuadTree::Create(iNumVerticesX * iNumVerticesZ - iNumVerticesX,
		iNumVerticesX * iNumVerticesZ - 1,
		iNumVerticesX - 1,
		0);

	if (FAILED(m_pQuadTree->Make_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iTerrainSizeX, _uint iTerrainSizeY)
{
	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = { (iTerrainSizeX + 1) * (iTerrainSizeY + 1)};
	m_iIndexStride = { sizeof(_uint) };
	m_iNumIndices = { iTerrainSizeX * iTerrainSizeY * 6 };
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iTerrainSizeX = iTerrainSizeX;
	m_iTerrainSizeZ = iTerrainSizeY;

	// VertexBuffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOSNORTEX* pVertices = New VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	m_pPos = New _float3[m_iNumVertices];
	
	for (_uint iVertexY = 0; iVertexY < iTerrainSizeY + 1; ++iVertexY)
	{
		for (_uint iVertexX = 0; iVertexX < iTerrainSizeX + 1; ++iVertexX)
		{
			_uint iIndex = iVertexX + ((iTerrainSizeX + 1) * iVertexY);
			pVertices[iIndex].vPosition = _float3((_float)iVertexX, 0.f, (_float)iVertexY);

			m_pPos[iIndex] = _float3((_float)iVertexX, 0.f, (_float)iVertexY);

			pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);

			pVertices[iIndex].vTexCoord = _float2(
					(_float)iVertexX / iTerrainSizeX,
					(_float)iVertexY / iTerrainSizeY);
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	//IndexBuffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	_uint* pIndices = New _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < iTerrainSizeY; ++i)
	{
		for (_uint j = 0; j < iTerrainSizeX; j++)
		{
			_uint		iIndex = i * (iTerrainSizeX + 1) + j;

			_uint		iIndices[4] = {
				iIndex + iTerrainSizeX + 1,
				iIndex + iTerrainSizeX + 2,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
		}
	}

	m_pIndex = New _uint[m_iNumIndices];
	memcpy(m_pIndex, pIndices, sizeof(_uint) * m_iNumIndices);
	

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	m_pQuadTree = CQuadTree::Create(iTerrainSizeX * iTerrainSizeY - iTerrainSizeY,
		iTerrainSizeX * iTerrainSizeY - 1,
		iTerrainSizeX - 1,
		0);
	
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Terrain::Culling(_Matrix WorldMatrix)
{
	std::lock_guard<std::mutex> lock(mtx);

	CFrustum* pFrustum = CFrustum::GetInstance();
	Safe_AddRef(pFrustum);

	pFrustum->Transform_ToLocalSpace(WorldMatrix);

	_uint		iNumIndices = { 0 };

	m_pQuadTree->Culling(pFrustum, m_pPos, m_pIndex, &iNumIndices);

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pIndex, sizeof(_uint) * iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;

	Safe_Release(pFrustum);
}

HRESULT CVIBuffer_Terrain::RemakeTerrain(const _tchar* pHeightMap)
{
	Safe_Delete_Array(m_pPos);
	Safe_Delete_Array(m_pIndex);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Safe_Release(m_pQuadTree);
	if (FAILED(Initialize_Prototype(pHeightMap)))
		return E_FAIL;
	return S_OK;
}

HRESULT CVIBuffer_Terrain::RemakeTerrain(_uint iTerrainSizeX, _uint iTerrainSizeY)
{
	Safe_Delete_Array(m_pPos);
	Safe_Delete_Array(m_pIndex);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Safe_Release(m_pQuadTree);
	if (FAILED(Initialize_Prototype(iTerrainSizeX, iTerrainSizeY)))
		return E_FAIL;
	return S_OK;
}

_bool CVIBuffer_Terrain::IsPicked(_float4 vRayOrigin, _float4 vRayDir, _float& fMinDist)
{
	_float3 vPoint[POINT_END];
	_float3 vCenter;
	vPoint[LT] = _float3(0.f, 0.f, (_float)(m_iTerrainSizeZ - 1));
	vPoint[RT] = _float3((_float)(m_iTerrainSizeX - 1), 0.f, (_float)(m_iTerrainSizeZ - 1));
	vPoint[RB] = _float3((_float)(m_iTerrainSizeX - 1), 0.f, 0.f);
	vPoint[LB] = _float3(0.f, 0.f, 0.f);

	if (false == isInFourPoint(vPoint[LT], vPoint[RT]
		, vPoint[RB], vPoint[LB], vRayOrigin, vRayDir, fMinDist))
	{
		return false;
	}

	IntersectPoint(vPoint, vRayOrigin, vRayDir, fMinDist);

	return true;
}

_bool CVIBuffer_Terrain::IsPicked(_float4 vRayOrigin, _float4 vRayDir, _float4x4 WorldMatrix, _float& fMinDist)
{
	_float3 vPoint[POINT_END];
	_float3 vCenter;
	vPoint[LT] = _float3(0.f, 0.f, (_float)(m_iTerrainSizeZ - 1));
	vPoint[RT] = _float3((_float)(m_iTerrainSizeX - 1), 0.f, (_float)(m_iTerrainSizeZ - 1));
	vPoint[RB] = _float3((_float)(m_iTerrainSizeX - 1), 0.f, 0.f);
	vPoint[LB] = _float3(0.f, 0.f, 0.f);

	for (_uint i = 0; i < POINT_END; ++i)
	{
		vPoint[i] = XMVector3TransformCoord(vPoint[i], WorldMatrix);
	}

	if (false == isInFourPoint(vPoint[LT], vPoint[RT]
		, vPoint[RB], vPoint[LB], vRayOrigin, vRayDir, fMinDist))
	{
		return false;
	}

	IntersectPoint(vPoint, vRayOrigin, vRayDir, fMinDist);

	return true;
}

void CVIBuffer_Terrain::IntersectPoint(_float3 vPoints[POINT_END], _float4 RayOrigin, _float4 RayDir, _float& fDist)
{
	// 모서리 길이가 1.f
	if ((vPoints[RT].x - vPoints[LT].x) == 1.f)
	{
		return;
	}

	enum POINT_SUB { SUB_LT, SUB_TC, SUB_RT, SUB_LC, SUB_C, SUB_RC, SUB_LB, SUB_BC, SUB_RB, SUB_END };

	_float3 vPoint[SUB_END];
	vPoint[SUB_LT] = (vPoints[LT]);
	vPoint[SUB_TC] = ((vPoints[LT]) + (vPoints[RT])) / 2;
	vPoint[SUB_RT] = (vPoints[RT]);
	vPoint[SUB_LC] = ((vPoints[LT]) + (vPoints[LB])) / 2;
	vPoint[SUB_C] = ((vPoints[LT]) + (vPoints[RB])) / 2;
	vPoint[SUB_RC] = ((vPoints[RT]) + (vPoints[RB])) / 2;
	vPoint[SUB_LB] = (vPoints[LB]);
	vPoint[SUB_BC] = ((vPoints[LB]) + (vPoints[RB])) / 2;
	vPoint[SUB_RB] = (vPoints[RB]);

	if (isInFourPoint(vPoint[SUB_LT], vPoint[SUB_TC], vPoint[SUB_LC], vPoint[SUB_C], RayOrigin, RayDir, fDist))
	{
		_float3 vSubPoints[POINT_END];
		vSubPoints[LT] = vPoint[SUB_LT];
		vSubPoints[RT] = vPoint[SUB_TC];
		vSubPoints[LB] = vPoint[SUB_LC];
		vSubPoints[RB] = vPoint[SUB_C];
		IntersectPoint(vSubPoints, RayOrigin, RayDir, fDist);
		return;
	}

	if (isInFourPoint(vPoint[SUB_TC], vPoint[SUB_RT], vPoint[SUB_C], vPoint[SUB_RC], RayOrigin, RayDir, fDist))
	{
		_float3 vSubPoints[POINT_END];
		vSubPoints[LT] = vPoint[SUB_TC];
		vSubPoints[RT] = vPoint[SUB_RT];
		vSubPoints[LB] = vPoint[SUB_C];
		vSubPoints[RB] = vPoint[SUB_RC];
		IntersectPoint(vSubPoints, RayOrigin, RayDir, fDist);
		return;
	}

	if (isInFourPoint(vPoint[SUB_LC], vPoint[SUB_C], vPoint[SUB_LB], vPoint[SUB_BC], RayOrigin, RayDir, fDist))
	{
		_float3 vSubPoints[POINT_END];
		vSubPoints[LT] = vPoint[SUB_LC];
		vSubPoints[RT] = vPoint[SUB_C];
		vSubPoints[LB] = vPoint[SUB_LB];
		vSubPoints[RB] = vPoint[SUB_BC];
		IntersectPoint(vSubPoints, RayOrigin, RayDir, fDist);
		return;
	}

	if (isInFourPoint(vPoint[SUB_C], vPoint[SUB_RC], vPoint[SUB_BC], vPoint[SUB_RB], RayOrigin, RayDir, fDist))
	{
		_float3 vSubPoints[POINT_END];
		vSubPoints[LT] = vPoint[SUB_C];
		vSubPoints[RT] = vPoint[SUB_RC];
		vSubPoints[LB] = vPoint[SUB_BC];
		vSubPoints[RB] = vPoint[SUB_RB];
		IntersectPoint(vSubPoints, RayOrigin, RayDir, fDist);
		return;
	}
}

_bool CVIBuffer_Terrain::isInFourPoint(_float3 LT, _float3 RT, _float3 RB, _float3 LB, _float4 RayOrigin, _float4 RayDir, _float& fDist)
{
	_float fTemp;
	if (TriangleTests::Intersects(RayOrigin, RayDir, LT, RT, RB, fTemp))
	{
		fDist = fTemp;
		return true;
	}

	if (TriangleTests::Intersects(RayOrigin, RayDir, LT, RB, LB, fTemp))
	{
		fDist = fTemp;
		return true;
	}

	return false;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMap)
{
	CVIBuffer_Terrain* pInstance = New CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMap)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iTerrainSizeX, _uint iTerrainSizeY)
{
	CVIBuffer_Terrain* pInstance = New CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iTerrainSizeX, iTerrainSizeY)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = New CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pPos);
		Safe_Delete_Array(m_pIndex);
	}	

	Safe_Release(m_pQuadTree);
}
