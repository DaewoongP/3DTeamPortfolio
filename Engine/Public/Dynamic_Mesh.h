#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 :
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
	vector<_float3> Get_VertexPositions() const { return m_VertexPositions; }
	vector<_ulong> Get_Indices() const { return m_Indices; }
	vector<_float> Get_InvMasses() const { return m_InvMasses; }
	void Set_Spheres(vector<pair<_float3, _float>> _Spheres) { m_ClothSpheres = _Spheres; }
	// 바람을 실시간으로 조정하여 처리해야함. (트랜스폼의 반대방향으로 처리하면 좋을듯.)
	void Set_WindVelocity(_float3 vWindVelocity);

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const CModel::BONES & Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix, HANDLE hFile);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	// 바람 등으로 움직였던 옷을 초기위치로 다시 세팅함.
	void Reset_Position();
	HRESULT Remake_ClothMesh(vector<_float> InvMasses);

private: /* For. Cloth */
	cloth::Fabric*		m_pFabric = { nullptr };
	cloth::Cloth*		m_pCloth = { nullptr };
	cloth::Solver*		m_pSolver = { nullptr };
	vector<_float3>		m_VertexPositions;
	vector<_ulong>		m_Indices;
	vector<_float>		m_InvMasses;
	CModel::TYPE		m_eType = { CModel::TYPE_END };
	_bool				m_isRemakeMesh = { false };
	vector<pair<_float3, _float>> m_ClothSpheres;

private:
	vector<VTXANIMMESH>		m_AnimVertices;
	vector<VTXANIMMESH>		m_InitAnimVertices;
	vector<VTXMESH>			m_NonAnimVertices;

private:
	_bool					m_isDownFrame = { false };

private:
	virtual HRESULT Ready_VertexBuffer_NonAnim(const Engine::MESH Mesh, _float4x4 PivotMatrix) override;
	virtual HRESULT Ready_VertexBuffer_Anim(const Engine::MESH Mesh, const CModel::BONES & Bones) override;
	HRESULT Initialize_ClothMesh();
	HRESULT Initialize_ClothMesh(HANDLE hFile);
	HRESULT Read_ClothData(HANDLE hFile);
	void Clear_ClothMesh();

public:
	static CDynamic_Mesh* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eType, const CModel::BONES & Bones, const Engine::MESH Mesh, _float4x4 PivotMatrix, HANDLE hFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END