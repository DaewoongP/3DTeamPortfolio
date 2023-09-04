#include "..\Public\VIBuffer_Rect_Color_Instance.h"
#include "Texture.h"
#include "PipeLine.h"

CVIBuffer_Rect_Color_Instance::CVIBuffer_Rect_Color_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Color_Instance(pDevice, pContext)
{
}

CVIBuffer_Rect_Color_Instance::CVIBuffer_Rect_Color_Instance(const CVIBuffer_Rect_Color_Instance& rhs)
	: CVIBuffer_Color_Instance(rhs)
{
}

HRESULT CVIBuffer_Rect_Color_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Rect_Color_Instance::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Point Num Instance null");
		return E_FAIL;
	}

	// 버퍼 파괴하고 재생성하려고 넣은 코드.
	Safe_Release(m_pVBInstance);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

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

void CVIBuffer_Rect_Color_Instance::Tick(COLORINSTANCE* pInstances, _int iRenderedParticleNum, _bool isAlphaBlend, _float4x4 AlphaBlendObjectWorldMatrixInverse)
{
	if (nullptr == pInstances)
		return;

	if (0 == iRenderedParticleNum)
		return;

	// iRenderedParticleNum에 -1을 넣으면 인스턴스 숫자만큼 렌더링
	// iRenderedParticleNum의 숫자가 더 크면 오류이므로 예외처리.
	if (-1 == iRenderedParticleNum && iRenderedParticleNum > m_iNumInstance)
	{
		iRenderedParticleNum = m_iNumInstance;
	}

	if (true == isAlphaBlend)
	{
		Sort_AlphaBlend(pInstances, AlphaBlendObjectWorldMatrixInverse);
	}

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXCOLINSTANCE* pVtxInstance = static_cast<VTXCOLINSTANCE*>(MappedSubResource.pData);

	for (_uint i = 0; i < iRenderedParticleNum; ++i)
	{
		memcpy(&pVtxInstance[i].vRight, &pInstances[i].InstanceLocalMatrix.m[0], sizeof(_float4));
		memcpy(&pVtxInstance[i].vUp, &pInstances[i].InstanceLocalMatrix.m[1], sizeof(_float4));
		memcpy(&pVtxInstance[i].vLook, &pInstances[i].InstanceLocalMatrix.m[2], sizeof(_float4));
		memcpy(&pVtxInstance[i].vTranslation, &pInstances[i].InstanceLocalMatrix.m[3], sizeof(_float4));
		memcpy(&pVtxInstance[i].vColor, &pInstances[i].vInstanceColor, sizeof(_float4));
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Rect_Color_Instance::Sort_AlphaBlend(COLORINSTANCE* pInstances, _float4x4 AlphaBlendObjectWorldMatrixInverse)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4 vCamPos = XMLoadFloat4(pPipeLine->Get_CamPosition());
	_float4 vCamLocalPos = XMVector3TransformCoord(vCamPos, AlphaBlendObjectWorldMatrixInverse);

	Safe_Release(pPipeLine);

	vector<COLORINSTANCE> ColorInstances;
	ColorInstances.resize(m_iNumInstance);

	memcpy(ColorInstances.data(), pInstances, sizeof(COLORINSTANCE) * m_iNumInstance);

	sort(ColorInstances.begin(), ColorInstances.end(), [vCamLocalPos](COLORINSTANCE SourInstance, COLORINSTANCE DestInstance) {
		_float4 vSourLocalPos, vDestLocalPos;
		memcpy(&vSourLocalPos, SourInstance.InstanceLocalMatrix.m[3], sizeof(_float4));
		memcpy(&vDestLocalPos, DestInstance.InstanceLocalMatrix.m[3], sizeof(_float4));

		_float4 vSour = vSourLocalPos - vCamLocalPos;
		_float4 vDest = vDestLocalPos - vCamLocalPos;

		// 내림차순 (멀리있는거부터 그림.)
		if (vSour.Length() > vDest.Length())
			return true;

		return false;
		});

	memcpy(pInstances, ColorInstances.data(), sizeof(COLORINSTANCE) * m_iNumInstance);
}

HRESULT CVIBuffer_Rect_Color_Instance::Make_Buffers()
{
	m_iIndexCountPerInstance = 6;
	m_iNumVertexBuffers = { 2 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = { 4 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { m_iIndexCountPerInstance * m_iNumInstance };
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

	VTXPOSTEX* pVertices = new VTXPOSTEX[4];
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

	_ushort* pIndices = new _ushort[m_iNumIndices];
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

CVIBuffer_Rect_Color_Instance* CVIBuffer_Rect_Color_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect_Color_Instance* pInstance = new CVIBuffer_Rect_Color_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Rect_Color_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Color_Instance::Clone(void* pArg)
{
	CVIBuffer_Rect_Color_Instance* pInstance = new CVIBuffer_Rect_Color_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Rect_Color_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Color_Instance::Free()
{
	__super::Free();

}
