#include "..\Public\VIBuffer_Point_Instance.h"
#include "Texture.h"
#include "PipeLine.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance & rhs)
	: CVIBuffer_Instance(rhs)
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void * pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Point Num Instance null");
		return E_FAIL;
	}

	m_iNumInstance = *reinterpret_cast<_uint*>(pArg);

	if (FAILED(Make_Buffers()))
		return E_FAIL;
	
	vector<_float4x4> InitializeMatrices;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float4x4 InitMatrix = XMMatrixIdentity();

		InitializeMatrices.push_back(InitMatrix);
	}

	if (FAILED(__super::Initialize(InitializeMatrices.data())))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Make_Buffers()
{
	m_iIndexCountPerInstance = 1;
	m_iNumVertexBuffers = { 2 };
	m_iStride = { sizeof(VTXPOINT) };
	m_iNumVertices = { 1 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { m_iIndexCountPerInstance * m_iNumInstance };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOINT* pVertices = new VTXPOINT;
	ZeroMemory(pVertices, sizeof(VTXPOINT));

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);
	pVertices->vPSize = _float2(1.f, 1.f);

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

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

CVIBuffer_Point_Instance * CVIBuffer_Point_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Point_Instance*	pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Point_Instance::Clone(void * pArg)
{
	CVIBuffer_Point_Instance*	pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

}
