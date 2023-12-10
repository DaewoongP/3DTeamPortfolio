#include "..\Public\VIBuffer_Rect_Instance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs)
	: CVIBuffer_Instance(rhs)
	, m_InstanceDesc(rhs.m_InstanceDesc)
	, m_pInstanceSpeed(rhs.m_pInstanceSpeed)
	, m_pInstancePos(rhs.m_pInstancePos)
{
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype(const INSTANCEDESC* pInstanceDesc, _uint iNumInstance)
{
	m_iNumInstance = { iNumInstance }; // �ν��Ͻ��� ����
	if (FAILED(Make_InstanceLogic(pInstanceDesc)))
		return E_FAIL;

	m_iIndexCountPerInstance = { 6 }; // �簢��(�ν��Ͻ�) �ϳ� �׸��µ� �ʿ��� �ε��� ��
	m_iNumVertexBuffers = { 2 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = { 4 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { m_iIndexCountPerInstance * m_iNumInstance };
	m_eFormat = { DXGI_FORMAT_R16_UINT };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT }; // ���� ���� ��ü�� �����̴� ���� �ƴ�, �ν��Ͻ����ۿ��� ����� ���� ������� ���̴����� ó��.
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };


	VTXPOSTEX* pVertices = New VTXPOSTEX[4];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * 4);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexCoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexCoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexCoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexCoord = _float2(0.f, 1.f);

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

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void* pArg)
{
	vector<_float4x4> InitializeMatrices;
	
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float4x4 InitMatrix;
		XMStoreFloat4x4(&InitMatrix, XMMatrixIdentity());

		memcpy(InitMatrix.m[3], &m_pInstancePos[i], sizeof(_float4));
		InitializeMatrices.push_back(InitMatrix);
	}
	
	if (FAILED(__super::Initialize(InitializeMatrices.data())))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Rect_Instance::Tick(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		static_cast<VTXINSTANCE*>(MappedSubResource.pData)[i].vTranslation.x = m_pInstancePos[i].x;
		static_cast<VTXINSTANCE*>(MappedSubResource.pData)[i].vTranslation.z = m_pInstancePos[i].z;
		static_cast<VTXINSTANCE*>(MappedSubResource.pData)[i].vTranslation.y -= m_pInstanceSpeed[i] * fTimeDelta;

		if (0 >= static_cast<VTXINSTANCE*>(MappedSubResource.pData)[i].vTranslation.y)
			static_cast<VTXINSTANCE*>(MappedSubResource.pData)[i].vTranslation.y = m_pInstancePos[i].y;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Rect_Instance::Make_InstanceLogic(const INSTANCEDESC* pInstanceDesc)
{
	m_InstanceDesc = *pInstanceDesc;

	// � '��ü'������ ������ ���·� �׷��ֱ� ���� �����Ҵ��Ͽ� �����͸� �����ϴ� ���·� ���� (�������� ó��.).
	m_pInstanceSpeed = New _float[m_iNumInstance];
	m_pInstancePos = New _float4[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pInstanceSpeed[i] = _float(rand() % (m_InstanceDesc.vSpeed.y + 1 - m_InstanceDesc.vSpeed.x) + m_InstanceDesc.vSpeed.x);
		m_pInstancePos[i] = _float4(rand() % (_uint(m_InstanceDesc.vExtents.x) + 1) - m_InstanceDesc.vExtents.x * 0.5f,
			rand() % (_uint(m_InstanceDesc.vExtents.y) + 1) - m_InstanceDesc.vExtents.y * 0.5f + m_InstanceDesc.fHeight,
			rand() % (_uint(m_InstanceDesc.vExtents.z) + 1) - m_InstanceDesc.vExtents.z * 0.5f,
			1.f);
	}

	return S_OK;
}

CVIBuffer_Rect_Instance* CVIBuffer_Rect_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCEDESC* pInstanceDesc, _uint iNumInstance)
{
	CVIBuffer_Rect_Instance* pInstance = New CVIBuffer_Rect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc, iNumInstance)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Instance::Clone(void* pArg)
{
	CVIBuffer_Rect_Instance* pInstance = New CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();

	if (!m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceSpeed);
		Safe_Delete_Array(m_pInstancePos);
	}
}
