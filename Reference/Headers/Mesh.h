#pragma once
/* =============================================== */
// 
//	Á¤ : ¾ÈÃ¶¹Î
//	ºÎ :
//
/* =============================================== */

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMesh : public CVIBuffer
{
protected:
	explicit CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }
	void	Get_Matrices(CModel::BONES Bones, _Inout_ _float4x4* pMatrices, _float4x4 PivotMatrix);
	vector<_float3>* Get_VerticesPositionVec() { return &m_VerticesPositionVec; }
	vector<PxU32>* Get_IndicesVec() { return &m_IndicesVec; }

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const CModel::BONES& Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

protected:
	_tchar			m_szName[MAX_PATH] = TEXT("");
	_uint			m_iMaterialIndex = { 0 };

protected:
	_uint			m_iNumBones = { 0 };
	vector<_uint>	m_BoneIndices;

	// Physx ConvexMesh »ý¼º¿ë
	vector<_float3> m_VerticesPositionVec;
	vector<PxU32> m_IndicesVec;

protected:
	virtual HRESULT Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4 PivotMatrix);
	virtual HRESULT Ready_VertexBuffer_Anim(const Engine::MESH Mesh, const CModel::BONES& Bones);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const CModel::BONES& Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END