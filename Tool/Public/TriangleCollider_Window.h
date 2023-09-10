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
	/* 생성한 셀의 정보 */
	typedef struct tagColCellDesc
	{
		string m_strIndexName = { "" };
		array<_float3, CCell::POINT_END> m_Points;
		CVIBuffer_Cell* m_pBufferCom = { nullptr };
	}COLCELLDESC;

	/* 피킹된 셀의 정보 */
	typedef struct tagPickColDesc
	{
		/* 피킹한 셀의 인덱스 */
		_uint m_iCellIndex = { 0 };
		/* 피킹한 셀의 포인트 세개(A B C) 중 하나의 인덱스 */
		CCell::POINT m_ePoint = { CCell::POINT_END };
		/* CTriangleCollider_Window피킹한 셀과 카메라의 거리 */
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

	_bool Get_VertexIndex_By_Picking(_uint* pVertexIndex, _float3* pPickPosition);

private:
	/* 현재 생성한 셀 들의 정보 */
	vector<COLCELLDESC> m_Cells;
	/* 생성할 셀의 위치 정보 */
	array<_float3, CCell::POINT_END> m_vCell;
	/* 피킹한 콜라이더 구체를 가지고 있는 셀정보들 */
	vector<PICKCOLDESC> m_PickDatas;
	/* 현재 선택한 셀의 인덱스 */
	_uint m_iCurrentCellIndex = { 0 };
	/* 현재 피킹하고있는 포인트의 인덱스 (A,B,C)*/
	_uint m_iCurrentPickPointIndex = { 0 };

	/* 피킹한 위치 */
	_float4	m_vPickPos;
	/* 리스트 박스 인덱스 탐색용 변수 */
	_uint m_iSelecIndex = { 0 };
	_int m_iSelectOption = { 0 };
	/* 현재 셀 생성중인지 확인용 bool */
	_bool m_isMakingCell = { false };

private:
	CShader* m_pShaderCom = { nullptr };
	CTriangleColMesh* m_pTriangleColMesh = { nullptr };

public:
	/* 피킹한 구체를 가지고 있는 셀의 정보들을 반환 */
	_bool Pick_Spheres(const _float4& vOrigin, const _float4& vDirection, _Inout_ vector<PICKCOLDESC>& Out);

private:
	/* 셀 피킹 함수 */
	HRESULT Pick_Navigation(_float fTimeDelta);
	HRESULT Make_Cell();
	// 셀 변경모드가 켜져있을때 현재 네비게이션의 포지션을 변경할 수 있게하는 InputFloat3을 호출
	HRESULT CurrentNavigationPosition();
	HRESULT Navigation_List();
	HRESULT Delete_Cell();
	/* 셀들의 렌더 갱신 */
	HRESULT Remake_Cells();

private: /* For.FileSystem */
	HRESULT NavigationSaveLoad();
	HRESULT NavigationSaveButton();
	HRESULT NavigationWrite_File(const _tchar* pPath);
	HRESULT NavigationLoadButton();
	HRESULT NavigationRead_File(const _tchar* pFileName);

private:
	// 셀의 방향 처리(CCW로 변경)
	void CCWSort_Cell(_float3* pPoints);
	HRESULT Render_Cells();

public:
	static CTriangleCollider_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END