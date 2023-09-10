#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"
#include "CustomModel.h"
#include "Dynamic_Mesh.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCustomModel;
END

BEGIN(Tool)

class CDummy_Cloth final : public CGameObject
{
private:
	explicit CDummy_Cloth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_Cloth(const CDummy_Cloth& rhs);
	virtual ~CDummy_Cloth() = default;

public:
	CCustomModel* Get_CustomModel() const { return m_pModelCom; }
	_uint Get_Max_NumMesh() const { return m_pModelCom->Get_NumMeshes(m_eMeshPartsType); }
	cloth::Cloth* Get_CurrentMesh_Cloth() const { 
		if (nullptr == m_pCurrent_Dynamic_Mesh) return nullptr;
		return m_pCurrent_Dynamic_Mesh->Get_Cloth();
	}
	vector<_float> Get_InvMasses() const { 
		if (nullptr == m_pCurrent_Dynamic_Mesh) return vector<_float>();
		return m_pCurrent_Dynamic_Mesh->Get_InvMasses(); 
	}
	_bool Get_VertexIndex_By_Picking(_Inout_ _uint* pVertexIndex, _Inout_ _float3* pPickPosition);
	void Set_Model_Component(CCustomModel::MESHTYPE _eMeshType, const _tchar* _pModelTag);
	void Set_MeshIndex(_uint _iMeshIndex);
	void Set_Testing(_bool _isTesting) { m_isTesting = _isTesting; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Reset_Position();
	HRESULT Remake_ClothMesh(vector<_float> InvMasses);

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CCustomModel*			m_pModelCom = { nullptr };
	CDynamic_Mesh*			m_pCurrent_Dynamic_Mesh = { nullptr };

private:
	CCustomModel::MESHTYPE	m_eMeshPartsType = { CCustomModel::MESH_END };
	_uint					m_iMeshIndex = { 0 };
	_bool					m_isRemakeMesh = { false };
	vector<_float>			m_InvMasses;

private:
	_bool					m_isTesting = { false };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CDummy_Cloth* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;	
};

END