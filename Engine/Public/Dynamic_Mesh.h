#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "Mesh.h"

BEGIN(Engine)

class ENGINE_DLL CDynamic_Mesh final : public CMesh
{
private:
	explicit CDynamic_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDynamic_Mesh(const CDynamic_Mesh& rhs);
	virtual ~CDynamic_Mesh() = default;

public:
	cloth::Cloth* Get_Cloth() const { return m_pCloth; }

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const CModel::BONES & Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix, const _tchar* szClothDataFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	// �ٶ� ������ �������� ���� �ʱ���ġ�� �ٽ� ������.
	void Reset_Position();

private: /* For. Cloth */
	cloth::Fabric*		m_pFabric = { nullptr };
	cloth::Cloth*		m_pCloth = { nullptr };
	cloth::Solver*		m_pSolver = { nullptr };
	vector<_float3>		m_VertexPositions;
	vector<_ulong>		m_Indices;
	vector<_float>		m_InvMasses;
	CModel::TYPE		m_eType = { CModel::TYPE_END };

private:
	vector<VTXANIMMESH>		m_AnimVertices;
	vector<VTXMESH>			m_NonAnimVertices;

private:
	virtual HRESULT Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4 PivotMatrix) override;
	virtual HRESULT Ready_VertexBuffer_Anim(const Engine::MESH Mesh, const CModel::BONES & Bones) override;
	HRESULT Initialize_ClothMesh();

public:
	static CDynamic_Mesh* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eType, const CModel::BONES & Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix, const _tchar* szClothDataFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END