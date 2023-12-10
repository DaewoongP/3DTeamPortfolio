#include "..\Public\Debug_Render_Box.h"

CDebug_Render_Box::CDebug_Render_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CDebug_Render_Box::CDebug_Render_Box(const CDebug_Render_Box& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CDebug_Render_Box::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Extents arg null");
		return E_FAIL;
	}

	BOXDESC BoxDesc = *reinterpret_cast<BOXDESC*>(pArg);
		
	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOS) };
	m_iNumVertices = { 8 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { 36 };
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

	VTXPOS* pVertices = New VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-BoxDesc.vExtents.x, BoxDesc.vExtents.y, -BoxDesc.vExtents.z);
	pVertices[1].vPosition = _float3(BoxDesc.vExtents.x, BoxDesc.vExtents.y, -BoxDesc.vExtents.z);
	pVertices[2].vPosition = _float3(BoxDesc.vExtents.x, -BoxDesc.vExtents.y, -BoxDesc.vExtents.z);
	pVertices[3].vPosition = _float3(-BoxDesc.vExtents.x, -BoxDesc.vExtents.y, -BoxDesc.vExtents.z);
	pVertices[4].vPosition = _float3(-BoxDesc.vExtents.x, BoxDesc.vExtents.y, BoxDesc.vExtents.z);
	pVertices[5].vPosition = _float3(BoxDesc.vExtents.x, BoxDesc.vExtents.y, BoxDesc.vExtents.z);
	pVertices[6].vPosition = _float3(BoxDesc.vExtents.x, -BoxDesc.vExtents.y, BoxDesc.vExtents.z);
	pVertices[7].vPosition = _float3(-BoxDesc.vExtents.x, -BoxDesc.vExtents.y, BoxDesc.vExtents.z);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), 
			XMMatrixRotationQuaternion(BoxDesc.vOffsetRotation)) + XMLoadFloat3(&BoxDesc.vOffsetPosition));
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

	_ushort* pIndices = New _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 1;
	pIndices[1] = 5;
	pIndices[2] = 6;

	pIndices[3] = 1;
	pIndices[4] = 6;
	pIndices[5] = 2;

	pIndices[6] = 4;
	pIndices[7] = 0;
	pIndices[8] = 3;

	pIndices[9] = 4;
	pIndices[10] = 3;
	pIndices[11] = 7;

	pIndices[12] = 4;
	pIndices[13] = 5;
	pIndices[14] = 1;

	pIndices[15] = 4;
	pIndices[16] = 1;
	pIndices[17] = 0;

	pIndices[18] = 3;
	pIndices[19] = 2;
	pIndices[20] = 6;

	pIndices[21] = 3;
	pIndices[22] = 6;
	pIndices[23] = 7;

	pIndices[24] = 7;
	pIndices[25] = 6;
	pIndices[26] = 5;

	pIndices[27] = 7;
	pIndices[28] = 5;
	pIndices[29] = 4;

	pIndices[30] = 0;
	pIndices[31] = 1;
	pIndices[32] = 2;

	pIndices[33] = 0;
	pIndices[34] = 2;
	pIndices[35] = 3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

CDebug_Render_Box* CDebug_Render_Box::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_Render_Box* pInstance = New CDebug_Render_Box(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDebug_Render_Box");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CDebug_Render_Box::Clone(void* pArg)
{
	CDebug_Render_Box* pInstance = New CDebug_Render_Box(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDebug_Render_Box");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDebug_Render_Box::Free()
{
	__super::Free();

}
