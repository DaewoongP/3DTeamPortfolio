#include "../Debug_Render_HeightField.h"

CDebug_Render_HeightField::CDebug_Render_HeightField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CDebug_Render_HeightField::CDebug_Render_HeightField(const CDebug_Render_HeightField& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CDebug_Render_HeightField::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Plane Desc arg null");
		return E_FAIL;
	}

	HEIGHTFIELDDESC HeightFieldDesc = *reinterpret_cast<HEIGHTFIELDDESC*>(pArg);

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOS) };
	m_iNumVertices = { (HeightFieldDesc.iNumRows + 1) * (HeightFieldDesc.iNumColumns + 1) };
	m_iIndexStride = { sizeof(_uint) };
	m_iNumIndices = { HeightFieldDesc.iNumRows * HeightFieldDesc.iNumColumns * 6 };
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// VertexBuffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOS* pVertices = New VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	vector<_float3> Positions;
	Positions.reserve(m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&HeightFieldDesc.pPositions[i]),
			XMMatrixRotationQuaternion(HeightFieldDesc.vOffsetRotation)) + XMLoadFloat3(&HeightFieldDesc.vOffsetPosition));		
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	//IndexBuffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_uint* pIndices = New _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < HeightFieldDesc.iNumColumns; ++i)
	{
		for (_uint j = 0; j < HeightFieldDesc.iNumRows; j++)
		{
			_uint		iIndex = i * (HeightFieldDesc.iNumRows + 1) + j;

			_uint		iIndices[4] = {
				iIndex + HeightFieldDesc.iNumRows + 1,
				iIndex + HeightFieldDesc.iNumRows + 2,
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

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

CDebug_Render_HeightField* CDebug_Render_HeightField::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_Render_HeightField* pInstance = New CDebug_Render_HeightField(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDebug_Render_HeightField");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CDebug_Render_HeightField::Clone(void* pArg)
{
	CDebug_Render_HeightField* pInstance = New CDebug_Render_HeightField(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDebug_Render_HeightField");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDebug_Render_HeightField::Free()
{
	__super::Free();
}
