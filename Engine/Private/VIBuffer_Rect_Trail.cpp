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
	m_pVertices = New VTXPOSTEX[rhs.m_iNumVertices];
	memcpy(m_pVertices, rhs.m_pVertices, sizeof(VTXPOSTEX) * rhs.m_iNumVertices);
}

HRESULT CVIBuffer_Rect_Trail::Reset_Trail()
{
	// Local Position
	_float3 vHighPos = ((*m_TrailDesc.pHighLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	_float3 vLowPos = ((*m_TrailDesc.pLowLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();

	// World Position
	_float3 vHighWorldPos = XMVector3TransformCoord(vHighPos, *m_TrailDesc.pWorldMatrix);
	_float3 vLowWorldPos = XMVector3TransformCoord(vLowPos, *m_TrailDesc.pWorldMatrix);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		if (1 == i % 2)
			m_pVertices[i].vPosition = vHighPos;
		else
			m_pVertices[i].vPosition = vLowPos;
	}

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	memcpy(pData, m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Rect_Trail::Reset_Trail(_float3 vHighPos, _float3 vLowPos)
{
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		if (1 == i % 2)
			m_pVertices[i].vPosition = vHighPos;
		else
			m_pVertices[i].vPosition = vLowPos;
	}

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	memcpy(pData, m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Rect_Trail::Initialize_Prototype(_uint iTrailNum)
{
	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = { 2 * (iTrailNum + 1) };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { 6 * iTrailNum };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	m_pVertices = New VTXPOSTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	//// Local Position
	//_float3 vHighPos = ((*m_TrailDesc.pHighLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	//_float3 vLowPos = ((*m_TrailDesc.pLowLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();

	//// World Position
	//_float3 vHighWorldPos = XMVector3TransformCoord(vHighPos, *m_TrailDesc.pWorldMatrix);
	//_float3 vLowWorldPos = XMVector3TransformCoord(vLowPos, *m_TrailDesc.pWorldMatrix);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		// 홀수 ... 9 7 5 3 1
		_uint iVertexIndex = { 0 };
		if (1 == i % 2)
		{
			iVertexIndex = (i - 1) >> 1;
			m_pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
			m_pVertices[i].vTexCoord = _float2(1.f * (_float)iVertexIndex / iTrailNum, 1.f);
		}
		// 짝수 ... 8 6 4 2 0
		else
		{
			iVertexIndex = i >> 1;
			m_pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
			m_pVertices[i].vTexCoord = _float2(1.f * (_float)iVertexIndex / iTrailNum, 0.f);
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pVertices;

#pragma endregion

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

	// 프로토타입에서 미리 처리해놓은 값들 얕은복사로 다가져오고
	// 버텍스버퍼만 따로 생성해줌.
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Rect_Trail::Tick()
{
	// Local Position
	_float3 vHighPos = ((*m_TrailDesc.pHighLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	_float3 vLowPos = ((*m_TrailDesc.pLowLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();

	// World Position
	_float3 vHighWorldPos = XMVector3TransformCoord(vHighPos, *m_TrailDesc.pWorldMatrix);
	_float3 vLowWorldPos = XMVector3TransformCoord(vLowPos, *m_TrailDesc.pWorldMatrix);

	//자르기
	_int iHeadIndex, iTailIndex, iBodyCount = { 0 };
	iHeadIndex = 0;
	iTailIndex = 0;

	//꼬리 찾을
	for (_uint i = m_iNumVertices - 1; i >= 2; i -= 2)
	{
		if (!XMVector3Equal(_float3(m_pVertices[i].vPosition), _float3(m_pVertices[i - 2].vPosition)))
		{
			iTailIndex = i;
			break;
		}
		m_pVertices[i].vTexCoord = { 1.f ,0.f };
		m_pVertices[i - 1].vTexCoord = { 1.f ,1.f };
	}

	//머릴 찾을
	for (_uint i = 1; i < m_iNumVertices - 2; i += 2)
	{
		if (!XMVector3Equal(_float3(m_pVertices[i].vPosition), _float3(m_pVertices[i + 2].vPosition)))
		{
			iHeadIndex = i;
			break;
		}
		m_pVertices[i].vTexCoord = { 0.f ,0.f };
		m_pVertices[i - 1].vTexCoord = { 0.f ,1.f };
	}

	iBodyCount = iTailIndex - iHeadIndex;
	if (iBodyCount != 0)
	{
		for (_int i = iTailIndex; i >= iHeadIndex - 1; i -= 2)
		{
			_float t = static_cast<_float>(i - iHeadIndex) / static_cast<_float>(iBodyCount);
			m_pVertices[i].vTexCoord = { t ,0.f };
			m_pVertices[i - 1].vTexCoord = { t ,1.f };
		}
	}

	//헤드가 움직였고, 몸체가 있다면?
	if (iHeadIndex > 1 && iBodyCount != m_iNumVertices)
	{
		for (_uint i = m_iNumVertices - 1; i >= 2; i -= 2)
		{
			if (iTailIndex < i) // 꼬리보다 뒤라면?(지금 안움직이는 꼬리)
			{
				//현재 꼬리의 위치 - 2 (뒤 프레임과 위치가 다른 친구) 위치로 맞춰준다.
				m_pVertices[i].vPosition = m_pVertices[iTailIndex - 2].vPosition;
				m_pVertices[i - 1].vPosition = m_pVertices[iTailIndex - 3].vPosition;
			}
			else
			{
				// 머리보다 작으면? (뭉쳐있는 거니까 굳이 계산 안해도 됨)
				if (i < iHeadIndex)
					break;
				// 머리보다 작지 않으면, 몸통부분이니까 앞프레임으로 땡겨줌.
				m_pVertices[i].vPosition = m_pVertices[i - 2].vPosition;
				m_pVertices[i - 1].vPosition = m_pVertices[(i - 1) - 2].vPosition;
			}
		}
	}
	else
	{
		for (_uint i = m_iNumVertices - 1; i >= 2; --i)
		{
			m_pVertices[i].vPosition = m_pVertices[i - 2].vPosition;
		}
	}

	// 0번에 Low 월드 포지션을 대입한다
	m_pVertices[0].vPosition = vLowWorldPos;
	// 1번에 High 월드 포지션을 대입한다
	m_pVertices[1].vPosition = vHighWorldPos;

	std::lock_guard<std::mutex> lock(mtx);

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	memcpy(pData, m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Rect_Trail::Tick_Spline(_float DistPer)
{
	// Local Position
	_float3 vHighPos = ((*m_TrailDesc.pHighLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();
	_float3 vLowPos = ((*m_TrailDesc.pLowLocalMatrix) * (*m_TrailDesc.pPivotMatrix)).Translation();

	// World Position
	_float3 vHighWorldPos = XMVector3TransformCoord(vHighPos, *m_TrailDesc.pWorldMatrix);
	_float3 vLowWorldPos = XMVector3TransformCoord(vLowPos, *m_TrailDesc.pWorldMatrix);

	_int iHeadIndex, iTailIndex, iBodyCount = { 0 };
	iHeadIndex = 0;
	iTailIndex = 0;

	for (_uint i = m_iNumVertices - 1; i >= 2; i -= 2)
	{
		if (!XMVector3Equal(_float3(m_pVertices[i].vPosition), _float3(m_pVertices[i - 2].vPosition)))
		{
			iTailIndex = i;
			break;
		}
	}

	for (_uint i = 1; i < m_iNumVertices - 2; i += 2)
	{
		if (!XMVector3Equal(_float3(m_pVertices[i].vPosition), _float3(m_pVertices[i + 2].vPosition)))
		{
			iHeadIndex = i;
			break;
		}
	}

	iBodyCount = iTailIndex - iHeadIndex;

	//얼만큼 이동했는지 이동값을 받아옵니다.
	_float fDist = _float3(m_pVertices[0].vPosition - vLowWorldPos).Length();
	//거리별 땅겨줄 프레임임.
	_float fDistPer = DistPer;
	//얼만큼 땅길건지 정하는거임.
	_uint HeadFallowIdx = _uint(fDist / fDistPer) * 2;

	//미리 그만큼 땡겨놓을거임.
	for (_uint i = m_iNumVertices - 1; i >= HeadFallowIdx + 2; --i)
	{
		m_pVertices[i].vPosition = m_pVertices[i - 2 - HeadFallowIdx].vPosition;
	}
	//0번부터 땅길 인덱스까지 데꼬오는거임,
	for (_uint i = 0; i < HeadFallowIdx; i += 2)
	{
		XMVectorCatmullRom(_float3(m_pVertices[i + 2].vPosition), _float3(m_pVertices[0].vPosition),
			vLowWorldPos, vLowWorldPos, (fDistPer * i) / (fDist));
		XMVectorCatmullRom(_float3(m_pVertices[(i + 1) + 2].vPosition), _float3(m_pVertices[1].vPosition),
			vHighWorldPos, vHighWorldPos, (fDistPer * i) / (fDist));
	}

	std::lock_guard<std::mutex> lock(mtx);

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	memcpy(pData, m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Rect_Trail::Tick_Lightning(vector<_float3>* posVec)
{
	_uint iConunt = m_iNumVertices / (posVec->size()-1);
	_uint iVectorSize = posVec->size()-1;
	_uint iPositionNum = { 0 };

	//모든 정점을 순회하며 위치를 변경해줍니다.
	for (_uint i = 0; i < m_iNumVertices; i+=2)
	{
		// idx = i(현재 정점) / icount(pos당 감당하는 정점의 수)
		_uint idx = i/(iConunt);
		_float t = (i % (iConunt))/ (_float)iConunt;

		m_pVertices[i].vPosition =
			_float3(XMVectorCatmullRom(posVec->data()[(idx ==0)?(0):(idx -1)],
				posVec->data()[idx], 
				posVec->data()[(idx +1> iVectorSize - 2)? (iVectorSize - 2) : (idx + 1)],
				posVec->data()[((idx +2)> iVectorSize - 2)? (iVectorSize-2): (idx +2)] - _float3(0,0,1),
				t)) + (*m_TrailDesc.pHighLocalMatrix).Translation();
		m_pVertices[i].vTexCoord = _float2((_float)(i / 2) * 2 / m_iNumVertices, 1.f);
		
		m_pVertices[i + 1].vPosition =
			_float3(XMVectorCatmullRom(posVec->data()[(idx == 0) ? (0) : (idx - 1)],
				posVec->data()[idx],
				posVec->data()[(idx + 1 > iVectorSize - 2) ? (iVectorSize - 2) : (idx + 1)],
				posVec->data()[((idx + 2) > iVectorSize - 2) ? (iVectorSize - 2) : (idx + 2)] - _float3(0, 0, 1),
				t))+ (*m_TrailDesc.pLowLocalMatrix).Translation();
		m_pVertices[i+1].vTexCoord = _float2((_float)((i+1)/2) * 2/ m_iNumVertices, 0.f);
	}

	std::lock_guard<std::mutex> lock(mtx);

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	VTXPOSTEX* pData = static_cast<VTXPOSTEX*>(MappedSubResource.pData);

	memcpy(pData, m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	m_pContext->Unmap(m_pVB, 0);
}

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

CVIBuffer_Rect_Trail* CVIBuffer_Rect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iTrailNum)
{
	CVIBuffer_Rect_Trail* pInstance = New CVIBuffer_Rect_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iTrailNum)))
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

	Safe_Delete_Array(m_pVertices);
}
