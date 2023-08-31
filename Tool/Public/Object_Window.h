#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;
class CMapObject;

class CObject_Window final : public CImWindow
{
	typedef struct SaveObjectDesc
	{
		//_float4x4 matTransform; // 상태 행렬
		_float3 vPos; // 위치 값
		_tchar wszTag[MAX_PATH]; // 오브젝트 종류
	}SAVEOBJECTDESC;

private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Picking_Menu(); // 피킹 메뉴
	void Install_Object(_float3 vPos); // 오브젝트 설치 메뉴
	void Select_Model(); // 모델 선택 메뉴
	void Save_Load_Menu(); // 세이브 로드 메뉴
	HRESULT Save_MapObject();
	HRESULT Load_MapObject();

	void Deep_Copy_Name(); // 모델 이름 문자열 깊은 복사
	void Deep_Copy_Path(const _tchar* wszPath); // 모델 경로 문자열 깊은 복사
	void Deep_Copy_Tag(const _tchar* wszTag); // 맵 오브젝트 태그 문자열 깊은 복사

	_float3 Find_PickingPos(); // 지형의 피킹 위치를 찾는 함수
	HRESULT Create_Dummy(); // 최초 1번 Dummy 생성

private:
	CMapDummy* m_pDummy = { nullptr }; // 생성해둔 Dummy의 주소
	CMapObject* m_pObject = { nullptr }; // 설치할 MapObject의 주소
	_uint m_iMapObjectIndex = { 0 }; // 현재 맵에 설치된 맵 오브젝트의 개수

	string m_strCurrentModel = { "Prototype_Component_Model_Tree" }; // 현재 활성화된 모델 이름, 초기값은 더미

	_int m_iModelIndex = { 0 }; // 선택된 모델 인덱스
	vector<string> m_vecModelList; // 현재 추가해둔 모델 이름 리스트
	vector<const _tchar*> m_vecModelList_t; // 모델 이름 리스트를 _tchar로 저장해둠
	vector<const _tchar*> m_vecModelPath_t; // 그 모델의 경로를 _tchar로 저장해둠
	vector<const _tchar*> m_vecMapObjectTag_t; // 맵에 추가한 오브젝트들의 정보

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END