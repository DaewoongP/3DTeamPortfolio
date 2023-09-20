#include "..\Public\VIBuffer_Rect_Trail.h"
#include "Shader.h"
#include "Texture.h"
#include "PipeLine.h"
#include "Calculator.h"

CVIBuffer_Rect_Trail::CVIBuffer_Rect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect_Trail::CVIBuffer_Rect_Trail(const CVIBuffer_Rect_Trail& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect_Trail::Reset_Trail()
{
	std::lock_guard<std::mutex> lock(mtx);
	// Local Position
	_float3 vHighPos = ((*m_TrailDesc.pHighLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	_float3 vLowPos = ((*m_TrailDesc.pLowLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	
	// World Position
	_float3 vHighWorldPos = XMVector3TransformCoord(vHighPos, *m_TrailDesc.pWorldMatrix);
	_float3 vLowWorldPos = XMVector3TransformCoord(vLowPos, *m_TrailDesc.pWorldMatrix);
	
	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		if (1 == i % 2)
			pData[i].vPosition = vHighPos;
		else
			pData[i].vPosition = vLowPos;
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Rect_Trail::Reset_Trail(_float3 vHighPos, _float3 vLowPos)
{
	std::lock_guard<std::mutex> lock(mtx);

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		if (1 == i % 2)
			pData[i].vPosition = vHighPos;
		else
			pData[i].vPosition = vLowPos;
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Rect_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Rect_Trail::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Trail Desc NULL");
		return E_FAIL;
	}

	m_TrailDesc = *reinterpret_cast<TRAILDESC*>(pArg);

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = { 2 * (m_TrailDesc.iTrailNum + 1) };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { 6 * m_TrailDesc.iTrailNum };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOSTEX* pVertices = New VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	// Local Position
	_float3 vHighPos = ((*m_TrailDesc.pHighLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	_float3 vLowPos = ((*m_TrailDesc.pLowLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();

	// World Position
	_float3 vHighWorldPos = XMVector3TransformCoord(vHighPos, *m_TrailDesc.pWorldMatrix);
	_float3 vLowWorldPos = XMVector3TransformCoord(vLowPos, *m_TrailDesc.pWorldMatrix);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		// 홀수 ... 9 7 5 3 1
		_uint iVertexIndex = { 0 };
		if (1 == i % 2)
		{
			iVertexIndex = (i - 1) >> 1;
			pVertices[i].vPosition = vHighWorldPos;
			pVertices[i].vTexCoord = _float2(1.f * (_float)iVertexIndex / m_TrailDesc.iTrailNum, 1.f);
		}
		// 짝수 ... 8 6 4 2 0
		else
		{
			iVertexIndex = i >> 1;
			pVertices[i].vPosition = vLowWorldPos;
			pVertices[i].vTexCoord = _float2(1.f * (_float)iVertexIndex / m_TrailDesc.iTrailNum, 0.f);
		}
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
	/*
	9 7 5 3 1
	8 6 4 2 0
	*/

	/*
	3 1 0 / 3 0 2 , 5 3 2 / 5 2 4 , 7 5 4 / 7 4 6
	*/
	_uint iIndex = { 0 };
	for (_uint i = 0; i < m_iNumIndices;)
	{
		pIndices[i++] = iIndex + 3;
		pIndices[i++] = iIndex + 1;
		pIndices[i++] = iIndex + 0;

		pIndices[i++] = iIndex + 3;
		pIndices[i++] = iIndex + 0;
		pIndices[i++] = iIndex + 2;

		iIndex += 2;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

void CVIBuffer_Rect_Trail::Tick()
{
	std::lock_guard<std::mutex> lock(mtx);

	// Local Position
	_float3 vHighPos = ((*m_TrailDesc.pHighLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	_float3 vLowPos = ((*m_TrailDesc.pLowLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	
	// World Position
	_float3 vHighWorldPos = XMVector3TransformCoord(vHighPos, *m_TrailDesc.pWorldMatrix);
	_float3 vLowWorldPos = XMVector3TransformCoord(vLowPos, *m_TrailDesc.pWorldMatrix);

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	//자르기
	_int iHeadIndex, iTailIndex,iBodyCount = { 0 };
	iHeadIndex = 0;
	iTailIndex = 0;

	//꼬리 찾을
	for (_uint i = m_iNumVertices - 1; i >= 2; i-=2)
	{
		if (!XMVector3Equal(_float3(pData[i].vPosition), _float3(pData[i - 2].vPosition)))
		{
			iTailIndex = i;
			break;
		}
		pData[i].vTexCoord = { 1.f ,0.f };
		pData[i-1].vTexCoord = { 1.f ,1.f };
	}
	
	//머릴 찾을
	for (_uint i = 1; i < m_iNumVertices -2 ; i+=2)
	{
		if (!XMVector3Equal(_float3(pData[i].vPosition), _float3(pData[i + 2].vPosition)))
		{
			iHeadIndex = i;
			break;
		}
		pData[i].vTexCoord = { 0.f ,0.f };
		pData[i-1].vTexCoord = { 0.f ,1.f };
	}

	iBodyCount = iTailIndex - iHeadIndex;
	if (iBodyCount != 0)
	{
		for (_int i = iTailIndex; i >= iHeadIndex-1; i-=2)
		{
			float t = static_cast<float>(i - iHeadIndex) / static_cast<float>(iBodyCount);
			pData[i].vTexCoord = { t ,0.f };
			pData[i-1].vTexCoord = { t ,1.f };
		}
	}

	for (_uint i = m_iNumVertices - 1; i >= 2 ; --i)
	{
		pData[i].vPosition = pData[i - 2].vPosition;
	}

	// 0번에 Low 월드 포지션을 대입한다
	pData[0].vPosition = vLowWorldPos;
	// 1번에 High 월드 포지션을 대입한다
	pData[1].vPosition = vHighWorldPos;

	m_pContext->Unmap(m_pVB, 0);
}

//void CVIBuffer_Rect_Trail::Tick_Spline()
//{
//	std::lock_guard<std::mutex> lock(mtx);
//
//	// Local Position
//	_float3 vHighPos = ((*m_TrailDesc.pHighLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
//	_float3 vLowPos = ((*m_TrailDesc.pLowLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
//
//	// World Position
//	_float3 vHighWorldPos = XMVector3TransformCoord(vHighPos, *m_TrailDesc.pWorldMatrix);
//	_float3 vLowWorldPos = XMVector3TransformCoord(vLowPos, *m_TrailDesc.pWorldMatrix);
//
//	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
//
//	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);
//
//	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);
//
//
//	//몸통의 크기를 구함.
//	_int iHeadIndex = { 0 };
//	_int iTailIndex = { 0 };
//	_int iBodyCount = { 0 };
//
//	//꼬리 찾을
//	for (_uint i = m_iNumVertices - 1; i >= 2; i -= 2)
//	{
//		if (!XMVector3Equal(_float3(pData[i].vPosition), _float3(pData[i - 2].vPosition)))
//		{
//			iTailIndex = i;
//			break;
//		}
//	}
//	//머릴 찾을
//	for (_uint i = 1; i < m_iNumVertices - 2; i += 2)
//	{
//		if (!XMVector3Equal(_float3(pData[i].vPosition), _float3(pData[i + 2].vPosition)))
//		{
//			iHeadIndex = i;
//			break;
//		}
//	}
//
//	iBodyCount = iTailIndex - iHeadIndex;
//
//	//정점보다 바디가 작으면(무조건 첫 이동임.)
//	if (iBodyCount < m_iNumVertices)
//	{
//		//그냥 전체 러프
//		for (_uint i = 0; i < m_iNumVertices; i += 2)
//		{
//			pData[i].vPosition = _float3(XMVectorLerp(_float3(pData[i].vPosition), vLowWorldPos, (_float)(m_iNumVertices - i)/ (_float)m_iNumVertices));
//			pData[i+1].vPosition = _float3(XMVectorLerp(_float3(pData[i+1].vPosition), vHighWorldPos, (_float)(m_iNumVertices - (i + 1)) / (_float)m_iNumVertices));
//		}
//	}
//	else
//	{
//		//아니면, 꼬리부터 한칸씩 땡겨서 이동하고, 남은 친구들은 내가 써먹을거임.
//		_uint iIndex = 0;
//		for (_uint i = m_iNumVertices - 1; i >= 2; i-=2)
//		{
//			if (m_iNumVertices / 2 < iIndex)
//				break;
//			pData[i].vPosition = pData[i - 2 - iIndex].vPosition;
//			pData[i-1].vPosition = pData[(i - 1) - 2 - iIndex].vPosition;
//			iIndex += 2;
//		}
//
//		_uint iCnt = 0;
//		for(_uint i = iIndex; i < m_iNumVertices;i += 2)
//		{
//			_float fValue = ((m_iNumVertices - iIndex) - iCnt) / (m_iNumVertices - iIndex);
//			pData[i].vPosition = _float3(XMVectorCatmullRom(_float3(pData[iIndex-2].vPosition), _float3(pData[iIndex].vPosition), _float3(pData[0].vPosition), vLowWorldPos, fValue));
//			pData[i + 1].vPosition = _float3(XMVectorCatmullRom(_float3(pData[iIndex - 2].vPosition), _float3(pData[iIndex].vPosition), _float3(pData[1].vPosition), vLowWorldPos, fValue));
//			iCnt++;
//		}
//	}
//	m_pContext->Unmap(m_pVB, 0);
//}

HRESULT CVIBuffer_Rect_Trail::Render(const _char* pConstantName, _float4 vColor, CShader* pShader, const _char* pPassName)
{
	if (FAILED(Setup_ShaderResources(pShader)))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue(pConstantName, &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Begin(pPassName)))
		return E_FAIL;

	return __super::Render();
}

HRESULT CVIBuffer_Rect_Trail::Render(const _char* pConstantName, CTexture* pTexture, CShader* pShader, const _char* pPassName)
{
	if (FAILED(Setup_ShaderResources(pShader)))
		return E_FAIL;

	if (FAILED(pTexture->Bind_ShaderResource(pShader, "g_Texture")))
		return E_FAIL;

	if (FAILED(pShader->Begin(pPassName)))
		return E_FAIL;

	return __super::Render();
}

HRESULT CVIBuffer_Rect_Trail::Render()
{
	return __super::Render();
}

HRESULT CVIBuffer_Rect_Trail::Setup_ShaderResources(class CShader* pShader)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	// 월드를 곱해서 쉐이더로 전달해줄거라 따로 처리하지않음.
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	return S_OK;
}

CVIBuffer_Rect_Trail* CVIBuffer_Rect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect_Trail* pInstance = New CVIBuffer_Rect_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Rect_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Trail::Clone(void* pArg)
{
	CVIBuffer_Rect_Trail* pInstance = New CVIBuffer_Rect_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Rect_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Trail::Free()
{
	__super::Free();
}
