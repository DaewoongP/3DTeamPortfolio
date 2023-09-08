#include "..\Public\Navigation.h"
#include "Cell.h"
#include "GameObject.h"

#ifdef _DEBUG
#include "Shader.h"
#include "PipeLine.h"
#endif

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

}

_float CNavigation::Get_CurrentCellY(_float3 vPosition) const
{
	if (0 > m_iCurrentIndex)
		return 0.f;

	_float3 vPointA = m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT_A);
	_float3 vPointB = m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT_B);
	_float3 vPointC = m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT_C);

	_float4 vCurrentPlane = XMPlaneFromPoints(vPointA, vPointB, vPointC);

	// y = (-ax -cz -d) / b
	return -1.f * vCurrentPlane.x * vPosition.x +
		-1.f * vCurrentPlane.z * vPosition.z +
		-1.f * vCurrentPlane.w / vCurrentPlane.y;
}

CELLFLAG CNavigation::Get_CurrentCellFlag() const
{
	if (0 > m_iCurrentIndex)
	{
		MSG_BOX("Failed Get_CurrentCellFlag");
		return CELLFLAG();
	}

	return m_Cells[m_iCurrentIndex]->Get_CellFlag();
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFiles)
{
	_ulong		dwByte = { 0 };

	HANDLE		hFile = CreateFile(pNavigationDataFiles, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[CCell::POINT_END];

	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, _uint(m_Cells.size()));
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == m_pOwner)
	{
		MSG_BOX("NULL Owner");
		return E_FAIL;
	}
	_float3 vPosition = static_cast<CGameObject*>(m_pOwner)->Get_Transform()->Get_Position();

	Find_MyCell(vPosition);

	return S_OK;
}

_bool CNavigation::Is_Move(_float3 vPosition, _Inout_ _float3* pNormal, _Inout_ CELLFLAG* pFlag)
{
	_int		iNeighborIndex = -1;
	_float3		vXZPosition = XMVectorSet(XMVectorGetX(vPosition), 0.f, XMVectorGetZ(vPosition), 1.f);

	if (true == m_Cells[m_iCurrentIndex]->Is_In(vXZPosition, pNormal, &iNeighborIndex, pFlag))
	{
		return true;
	}
	else
	{
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->Is_In(vXZPosition, pNormal, &iNeighborIndex, pFlag))
					break;
			}

			m_iCurrentIndex = iNeighborIndex;

			return true;
		}
		else
			return false;
	}

	return false;
}

HRESULT CNavigation::Find_MyCell(_float3 vPosition)
{
	for (auto& pCell : m_Cells)
	{
		if (true == pCell->Is_In(vPosition, nullptr, nullptr, nullptr))
			m_iInitialIndex = m_iCurrentIndex = pCell->Get_CellIndex();
	}

	return S_OK;
}


#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	_float4		vColor = -1 == m_iCurrentIndex ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (-1 == m_iCurrentIndex)
	{

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();
	}
	else
	{
		WorldMatrix._42 = 0.1f;
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_iCurrentIndex]->Render();
	}

	return S_OK;
}
#endif // _DEBUG

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFiles)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFiles)))
	{
		MSG_BOX("Failed to Created CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
