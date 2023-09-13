#pragma once
#include "ImWindow.h"
#include "TriangleColMesh.h"
#include "Cell.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Cell;
class CBounding_Sphere;
END

BEGIN(Tool)

class  CTriangleCollider_Window final : public CImWindow
{
public:
	typedef struct tagColCellDesc
	{
		_uint			m_iIndices[3] = {};
		CVIBuffer_Cell* m_pBufferCom = { nullptr };
	}COLCELLDESC;

	/* ��ŷ�� ���� ���� */
	typedef struct tagPickColDesc
	{
		_uint m_iCellIndex = { 0 };
		CCell::POINT m_ePoint = { CCell::POINT_END };
		_float m_fDistance = { 0.f };
	}PICKCOLDESC;

	enum OPTION { OPTION_NOTHING, OPTION_CREATE, OPTION_PICK };
	enum EDITMOVEOPTION { EDIT_X, EDIT_Y, EDIT_Z, EDIT_END};

private:
	explicit CTriangleCollider_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTriangleCollider_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void OpenFile_Button();
	void AddModel_Button();

	// �𵨿��� ��ŷ�ϴ� �Լ���.
	_bool Get_VertexIndex_By_Picking(_Inout_ _float4* pPickPosition);

	// ������ ��ŷ�ϴ� �Լ���.
	_bool Pick_Spheres(const _float4& vOrigin, const _float4& vDirection, _uint* iIndex);
	
	void EditVertices(_float fTimeDelta);
	HRESULT Save_MeshData();
private:
	vector<_float3>		m_Vertices;
	vector<COLCELLDESC> m_Cells;
	_uint m_vCell[3] = {};
	_uint m_iCurrentCellIndex = { 0 };
	_uint m_iCurrentPickPointIndex = { 0 };

	_float4	m_vPickPos;
	_uint m_vPickIndex = { 0 };
	_int m_iSelectOption = { 0 };
	/* ���� �� ���������� Ȯ�ο� bool */
	_bool m_isMakingCell = { false };

	//��������
	EDITMOVEOPTION m_EidtMoveType = EDIT_END;
	_bool		   m_isMoveTrigger = { false };

	//�� ��������뵵��.
	_int			m_iMaxModelIndex = { 0 };
	_int		    m_iModelIndex = { 0 };
	vector<string>  m_vecModelList;
	vector<wstring> m_vecModelList_t;
	_tchar			m_wszCurrentDummyModelTag[MAX_PATH] = {};

	//�� ���� �뵵��
	_tchar			m_CurrentFilePath_COL[MAX_PATH] = {};
private:
	CShader* m_pShaderCom = { nullptr };
	CTriangleColMesh* m_pTriangleColMesh = { nullptr };

private:
	HRESULT Read_File_Data(_tchar* pModelFilePath);
	HRESULT Release_Mesh(COLMESH* Mesh);
	HRESULT Create_COLCELL(COLMESH* PMesh);
	HRESULT Make_Cell();
	HRESULT ReMake_Cell();

private:
	// ���� ���� ó��(CCW�� ����)
	void CCWSort_Cell(_uint* pPoints);
	HRESULT Render_Cells();

public:
	static CTriangleCollider_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END