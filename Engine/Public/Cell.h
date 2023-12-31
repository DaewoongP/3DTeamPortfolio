#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 : 주성환
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };

private:
	explicit CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	const _int Get_CellIndex() const { return m_iIndex; }
	CELLFLAG Get_CellFlag() const { return m_eCellFlag; }
	_float3 Get_Point(POINT ePoint) { return m_vPoints[ePoint]; }
	void SetUp_Neighbor(NEIGHBOR eNeighbor, CCell* pNeighbor) { m_iNeighborIndices[eNeighbor] = pNeighbor->m_iIndex; }

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);

public:
	_bool Compare_Points(_float3 vSourPoint, _float3 vDestPoint);
	_bool Is_In(_float3 vPosition, _Inout_ _float3* pNormal, _Inout_ _int* pNeighborIndex, _Inout_ CELLFLAG* pFlag);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	_int					m_iIndex = { 0 };
	_float3					m_vPoints[POINT_END];
	_float3					m_vNormals[NEIGHBOR_END];
	_int					m_iNeighborIndices[NEIGHBOR_END] = { -1, -1, -1 };

	CELLFLAG				m_eCellFlag = { CELL_MOVE };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free();
};

END