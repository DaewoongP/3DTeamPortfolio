#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CDynamic_Mesh final : public CVIBuffer
{
private:
	explicit CDynamic_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDynamic_Mesh(const CDynamic_Mesh& rhs);
	virtual ~CDynamic_Mesh() = default;

public:
	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }
	void	Get_Matrices(CModel::BONES Bones, _Inout_ _float4x4 * pMatrices, _float4x4 PivotMatrix);

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const CModel::BONES & Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	void Tick(_float fTimeDelta);

private:
	_tchar			m_szName[MAX_PATH] = TEXT("");
	_uint			m_iMaterialIndex = { 0 };
	CModel::TYPE	m_eType = { CModel::TYPE_END };

private:
	_uint			m_iNumBones = { 0 };
	vector<_uint>	m_BoneIndices;

private: /* For. Cloth */
	cloth::Fabric*		m_pFabric = { nullptr };
	cloth::Cloth*		m_pCloth = { nullptr };
	cloth::Solver*		m_pSolver = { nullptr };
	vector<_float3>		m_VertexPositions;
	vector<_ulong>		m_Indices;
	vector<_float>		m_InvMasses;

private:
	HRESULT Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4 PivotMatrix);
	HRESULT Ready_VertexBuffer_Anim(const Engine::MESH Mesh, const CModel::BONES & Bones);
	HRESULT Initialize_ClothMesh();

public:
	static CDynamic_Mesh* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eType, const CModel::BONES & Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END