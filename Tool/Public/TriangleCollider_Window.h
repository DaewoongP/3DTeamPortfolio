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
	/* ������ ���� ���� */
	typedef struct tagColCellDesc
	{
		array<_float3, 3> m_Points;
		CVIBuffer_Cell* m_pBufferCom = { nullptr };
	}COLCELLDESC;

	/* ��ŷ�� ���� ���� */
	typedef struct tagPickColDesc
	{
		/* ��ŷ�� ���� �ε��� */
		_uint m_iCellIndex = { 0 };
		/* ��ŷ�� ���� ����Ʈ ����(A B C) �� �ϳ��� �ε��� */
		CCell::POINT m_ePoint = { CCell::POINT_END };
		/* CTriangleCollider_Window��ŷ�� ���� ī�޶��� �Ÿ� */
		_float m_fDistance = { 0.f };
	}PICKCOLDESC;

	enum OPTION { OPTION_NOTHING, OPTION_CREATE, OPTION_PICK };

private:
	explicit CTriangleCollider_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTriangleCollider_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void OpenFile_Button();
	void AddModel_Button();
	void Select_Model();

	// �𵨿��� ��ŷ�ϴ� �Լ���.
	_bool Get_VertexIndex_By_Picking(_Inout_ _float4* pPickPosition);

	// ������ ��ŷ�ϴ� �Լ���.
	_bool Pick_Spheres(const _float4& vOrigin, const _float4& vDirection, _Inout_ vector<PICKCOLDESC>& Out);

private:
	/* ���� ������ �� ���� ���� */
	vector<COLCELLDESC> m_Cells;
	/* ������ ���� ��ġ ���� */
	array<_float3, CCell::POINT_END> m_vCell;
	/* ��ŷ�� �ݶ��̴� ��ü�� ������ �ִ� �������� */
	vector<PICKCOLDESC> m_PickDatas;
	/* ���� ������ ���� �ε��� */
	_uint m_iCurrentCellIndex = { 0 };
	/* ���� ��ŷ�ϰ��ִ� ����Ʈ�� �ε��� (A,B,C)*/
	_uint m_iCurrentPickPointIndex = { 0 };

	/* ��ŷ�� ��ġ */
	_float4	m_vPickPos;
	/* ����Ʈ �ڽ� �ε��� Ž���� ���� */
	_uint m_iSelecIndex = { 0 };
	_int m_iSelectOption = { 0 };
	/* ���� �� ���������� Ȯ�ο� bool */
	_bool m_isMakingCell = { false };

	//�� ��������뵵��.
	_int			m_iMaxModelIndex = { 0 };
	_int		    m_iModelIndex = { 0 };
	vector<string>  m_vecModelList;
	vector<wstring> m_vecModelList_t;
	_tchar			m_wszCurrentDummyModelTag[MAX_PATH] = {};

private:
	CShader* m_pShaderCom = { nullptr };
	CTriangleColMesh* m_pTriangleColMesh = { nullptr };

private:
	HRESULT Read_File_Data(_tchar* pModelFilePath);
	HRESULT Release_Mesh(COLMESH* Mesh);
	HRESULT Create_COLCELL(COLMESH* PMesh);

	/* �� ��ŷ �Լ� */
	HRESULT Pick_Navigation(_float fTimeDelta);
	HRESULT Make_Cell();
	// �� �����尡 ���������� ���� �׺���̼��� �������� ������ �� �ְ��ϴ� InputFloat3�� ȣ��
	HRESULT CurrentNavigationPosition();
	HRESULT Navigation_List();
	HRESULT Delete_Cell();
	/* ������ ���� ���� */
	HRESULT Remake_Cells();

private:
	// ���� ���� ó��(CCW�� ����)
	void CCWSort_Cell(_float3* pPoints);
	HRESULT Render_Cells();

public:
	static CTriangleCollider_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END