#pragma once

// 정 : 심정환
// static mesh 인스턴싱을 위한 Mesh 클래스

#include "Model.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMesh_Instance final : public CVIBuffer
{
private:
	explicit CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMesh_Instance(const CMesh_Instance& rhs);
	virtual ~CMesh_Instance() = default;

public:
	_tchar* Get_MeshName() { return m_szName; }
	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }
	void	Get_Matrices(CModel::BONES Bones, _Inout_ _float4x4* pMatrices, _float4x4 PivotMatrix);
	vector<_float4x4>			Get_InstanceMatrices() { return m_InstanceMatrixVec; }
	vector<_float3>				Get_VertexPositions() { return m_VerticesPositionVec; }
	vector<PxU32>				Get_Indices() { return m_IndicesVec; };

public:
	virtual HRESULT Initialize_Prototype(const Engine::MESH Mesh, _float4x4 PivotMatrix, _float4x4* pInstanceMatrix, _uint iNumInstance, const CModel::BONES& Bones, CModel::TYPE eType = CModel::TYPE_NONANIM);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	ID3D11Buffer*				m_pVBInstance = { nullptr };

	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };	
	_uint						m_iNumInstance = { 0 };
	_uint						m_iMaterialIndex = { 0 };

	_tchar						m_szName[MAX_PATH] = TEXT("");

private:
	_uint			m_iNumBones = { 0 };
	vector<_uint>	m_BoneIndices;

	vector<_float4x4>			m_InstanceMatrixVec;
	vector<_float3>				m_VerticesPositionVec;
	vector<PxU32>				m_IndicesVec;

private:
	HRESULT Ready_VertexBuffer_Anim(const Engine::MESH Mesh, _float4x4* pInstanceMatrix, const CModel::BONES& Bones, _float4x4 PivotMatrix);
	HRESULT Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4* pInstanceMatrix, _float4x4 PivotMatrix);

public:
	static CMesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext * pContext, 
		const Engine::MESH Mesh, _float4x4* pInstanceMatrix, _uint iNumInstance, const CModel::BONES& Bones, _float4x4 PivotMatrix, CModel::TYPE eType = CModel::TYPE_NONANIM);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END