#pragma once

// 정 : 심정환
// static mesh 인스턴싱을 위한 Mesh 클래스

#include "VIBuffer.h"

BEGIN(Engine)

class CMesh_Instance final : public CVIBuffer
{
private:
	explicit CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMesh_Instance(const CMesh_Instance& rhs);
	virtual ~CMesh_Instance() = default;

public:
	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }

public:
	virtual HRESULT Initialize_Prototype(const Engine::MESH Mesh, _float4x4 PivotMatrix, _float4x4* pInstanceMatrix, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	ID3D11Buffer*				m_pVBInstance = { nullptr };

	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };	
	_uint						m_iNumInstance = { 0 };
	_uint						m_iMaterialIndex = { 0 };

private:
	HRESULT Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4* pInstanceMatrix, _float4x4 PivotMatrix);

public:
	static CMesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext * pContext, 
		const Engine::MESH Mesh, _float4x4* pInstanceMatrix, _uint iNumInstance, _float4x4 PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END