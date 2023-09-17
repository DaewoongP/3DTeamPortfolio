#include "../Public/VIBuffer_Color_Instance.h"
#include "PipeLine.h"

CVIBuffer_Color_Instance::CVIBuffer_Color_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Color_Instance::CVIBuffer_Color_Instance(const CVIBuffer_Color_Instance& rhs)
	: CVIBuffer(rhs)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iDrawNum(rhs.m_iDrawNum)
{
}

HRESULT CVIBuffer_Color_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Color_Instance::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Initialize Matrix NULL");
		return E_FAIL;
	}

	vector<_float4x4> InitializeMatrix;
	InitializeMatrix.resize(m_iNumInstance);
	memcpy(InitializeMatrix.data(), pArg, sizeof(_float4x4) * m_iNumInstance);
	m_iDrawNum = m_iNumInstance;

	D3D11_BUFFER_DESC BufferDesc;
	ZEROMEM(&BufferDesc);

	m_iInstanceStride = sizeof(VTXCOLINSTANCE);
	BufferDesc.ByteWidth = { m_iInstanceStride * m_iNumInstance };
	BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	BufferDesc.StructureByteStride = { m_iInstanceStride };
	BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	BufferDesc.MiscFlags = { 0 };

	VTXCOLINSTANCE* pVertices = new VTXCOLINSTANCE[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		memcpy(&pVertices[i].vRight, InitializeMatrix[i].m[0], sizeof(_float4));
		memcpy(&pVertices[i].vUp, InitializeMatrix[i].m[1], sizeof(_float4));
		memcpy(&pVertices[i].vLook, InitializeMatrix[i].m[2], sizeof(_float4));
		memcpy(&pVertices[i].vTranslation, InitializeMatrix[i].m[3], sizeof(_float4));
		pVertices->vColor = { 1.f, 1.f, 1.f, 1.f };
	}

	D3D11_SUBRESOURCE_DATA		SubResourceData;
	ZeroMemory(&SubResourceData, sizeof SubResourceData);

	SubResourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

void CVIBuffer_Color_Instance::Tick(VTXCOLINSTANCE* pInstances, _int iRenderedParticleNum, _bool isAlphaBlend, _float4x4 AlphaBlendObjectWorldMatrixInverse)
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
		Sort_AlphaBlend(pInstances, iRenderedParticleNum, AlphaBlendObjectWorldMatrixInverse);
	}

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXCOLINSTANCE* pVTXCOLINSTANCE = static_cast<VTXCOLINSTANCE*>(MappedSubResource.pData);

	for (_uint i = 0; i < iRenderedParticleNum; ++i)
	{
		memcpy(&pVTXCOLINSTANCE[i].vRight, &pInstances[i].vRight, sizeof(_float4));
		memcpy(&pVTXCOLINSTANCE[i].vUp, &pInstances[i].vUp, sizeof(_float4));
		memcpy(&pVTXCOLINSTANCE[i].vLook, &pInstances[i].vLook, sizeof(_float4));
		memcpy(&pVTXCOLINSTANCE[i].vTranslation, &pInstances[i].vTranslation, sizeof(_float4));
		memcpy(&pVTXCOLINSTANCE[i].vColor, &pInstances[i].vColor, sizeof(_float4));
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Color_Instance::Sort_AlphaBlend(VTXCOLINSTANCE* pInstances, _int iRenderedParticleNum, _float4x4 AlphaBlendObjectWorldMatrixInverse)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4 vCamPos = XMLoadFloat4(pPipeLine->Get_CamPosition());
	_float4 vCamLocalPos = XMVector3TransformCoord(vCamPos, AlphaBlendObjectWorldMatrixInverse);

	Safe_Release(pPipeLine);

	vector<VTXCOLINSTANCE> ColorInstances;
	ColorInstances.resize(iRenderedParticleNum);

	memcpy(ColorInstances.data(), pInstances, sizeof(VTXCOLINSTANCE) * iRenderedParticleNum);

	sort(ColorInstances.begin(), ColorInstances.end(), [vCamLocalPos](VTXCOLINSTANCE SourInstance, VTXCOLINSTANCE DestInstance) {
		_float4 vSourLocalPos, vDestLocalPos;
		memcpy(&vSourLocalPos, &SourInstance.vTranslation, sizeof(_float4));
		memcpy(&vDestLocalPos, &DestInstance.vTranslation, sizeof(_float4));

		_float4 vSour = vSourLocalPos - vCamLocalPos;
		_float4 vDest = vDestLocalPos - vCamLocalPos;

		// 내림차순 (멀리있는거부터 그림.)
		if (vSour.Length() > vDest.Length())
			return true;

		return false;
		});

	memcpy(pInstances, ColorInstances.data(), sizeof(VTXCOLINSTANCE) * iRenderedParticleNum);
}

void CVIBuffer_Color_Instance::Set_DrawNum(_uint iDrawNum)
{
	if (iDrawNum > m_iNumInstance)
		return;

	m_iDrawNum = iDrawNum;
}

_uint CVIBuffer_Color_Instance::Get_DrawNum()
{
	return m_iDrawNum;
}

HRESULT CVIBuffer_Color_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint					iOffset[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iDrawNum, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Color_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
