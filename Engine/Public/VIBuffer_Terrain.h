#pragma once
/* =============================================== */
// 
//	정 : 심정환
//	부 :
//
/* =============================================== */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
	enum POINT { LT, RT, RB, LB, POINT_END };

private:
	explicit CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	const _float3* Get_PosArray() const { return m_pPos; }
	const _uint* Get_Index() const { return m_pIndex; }
	const _uint Get_TerrainSizeX() const { return m_iTerrainSizeX; }
	const _uint Get_TerrainSizeZ() const { return m_iTerrainSizeZ; }


public:
	virtual HRESULT Initialize_Prototype(const _tchar * pHeightMap);
	virtual HRESULT Initialize_Prototype(_uint iTerrainSizeX, _uint iTerrainSizeY);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Culling(_Matrix WorldMatrix);
	HRESULT RemakeTerrain(const _tchar * pHeightMap);
	HRESULT RemakeTerrain(_uint iTerrainSizeX, _uint iTerrainSizeY);
	_bool	IsPicked(_float4 vRayOrigin, _float4 vRayDir, _float& fMinDist);
	_bool	IsPicked(_float4 vRayOrigin, _float4 vRayDir, _float4x4 WorldMatrix, _float& fMinDist);

private:
	_float3*			m_pPos = { nullptr };
	_uint*				m_pIndex = { nullptr };

	_uint				m_iTerrainSizeX = { 0 };
	_uint				m_iTerrainSizeZ = { 0 };

	class CQuadTree* m_pQuadTree = { nullptr };

private:
	void IntersectPoint(_float3 vPoints[POINT_END], _float4 RayOrigin, _float4 RayDir, _float& fDist);
	_bool isInFourPoint(_float3 LT, _float3 RT, _float3 RB, _float3 LB, _float4 RayOrigin, _float4 RayDir, _float& fDist);

public:
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pHeightMap);
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iTerrainSizeX, _uint iTerrainSizeY);	
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END