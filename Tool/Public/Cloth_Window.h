#pragma once
#include "ImWindow.h"
#include "CustomModel.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
END

BEGIN(Tool)
class CDummy_Cloth;
class CCamera_Free;

class CCloth_Window final : public CImWindow
{
	typedef struct tagVertex
	{
		_char* pVertexName = { nullptr };
		_uint iVertexIndex = { 0 };
		_float3 vVertexPosition;
		CCollider* pCollider = { nullptr };
	}CLOTHVERTEX;

private:
	explicit CCloth_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCloth_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CDummy_Cloth*					m_pDummy_Cloth = { nullptr };
	CCamera_Free*					m_pCamera_Free = { nullptr };
	cloth::Cloth*					m_pCurrent_Cloth = { nullptr };
	CCollider*						m_pCollider = { nullptr };
	CRenderer*						m_pRenderer = { nullptr };

private:
	_tchar							m_wszDummyName[MAX_STR] = TEXT("");
	CCustomModel::MESHTYPE			m_eMeshType = { CCustomModel::MESH_END };
	_int							m_iMeshIndex = { 0 };
	_bool							m_isPickMesh = { false };
	_bool							m_isPickCollider = { false };
	_bool							m_isMeshHighLight = { false };
	// 현재 피킹된 버텍스의 인덱스와 포지션을 저장하는 배열
	vector<CLOTHVERTEX>				m_ClothVertices;
	_int							m_iClothVertexListIndex = { 0 };

private:
	void Radio_Select_MeshType(_float fTimeDelta);
	void Open_Model(_float fTimeDelta);
	void Input_Mesh_Index(_float fTimeDelta);
	_bool isValid_Mesh_Index(_int iIndex);
	void Pick_Mesh(_float fTimeDelta);
	void Pick_Collider(_float fTimeDelta);
	void Current_Picked_Vertices(_float fTimeDelta);
	void Input_Options(_float fTimeDelta);
	_bool isValid_Dummy();

public:
	static CCloth_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize);
	virtual void Free(void) override;
};

END