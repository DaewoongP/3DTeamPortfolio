#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;
class CMapObject;
class CMapObject_Ins;

class CObject_Window final : public CImWindow
{
	enum DUMMY_MATRIX { DUMMY_SCALE, DUMMY_ROT, DUMMY_TRANS, DUMMY_END }; // 더미의 상태
	enum INSTALL_METHOD { ONE_METHOD, CONTINUOUS_METHOD, MULTI_METHOD, END_METHOD }; // 설치 방법

	typedef struct SaveObjectDesc
	{
		_float4x4 matTransform; // 상태 행렬
		_uint iTagLen; // 문자열 길이
		_tchar wszTag[MAX_PATH]; // 오브젝트 종류(모델 컴포넌트 이름)
	}SAVEOBJECTDESC;

	typedef struct SaveInsObjectDesc
	{
		_uint iInstanceCnt; // 인스턴스 개수
		_float4x4* pMatTransform; // 각각 상태 행렬들의 주소
		_float4x4 matTransform; // 상태 행렬
		_uint iTagLen; // 문자열 길이
		_tchar wszTag[MAX_PATH]; // 오브젝트 종류(모델 컴포넌트 이름)
	}SAVEINSOBJECTDESC;

private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// 메뉴 모음
	void Picking_Menu(); // 피킹 메뉴
	void Install_Object(_float3 vPos); // 오브젝트 설치 메뉴
	void Install_Continuous_Object(_float3 vPos); // 오브젝트 연속 설치 메뉴
	void Install_Multi_Object(_float3 vPos); // 오브젝트 다중(인스턴싱) 설치 메뉴
	void Select_Model(); // 모델 선택 메뉴
	void Current_MapObject(); // 현재 설치되어 있는 맵 오브젝트 확인
	void Save_Load_Menu(); // 오브젝트 세이브 로드 메뉴
	void Ins_Save_Load_Menu(); // 인스턴트 오브젝트 세이브 로드 메뉴
	void Delete_Object_Menu(); // 오브젝트 삭제 관련 메뉴
	void Mesh_Picking_Menu(); // 메쉬 피킹 메뉴
	void Delete_Picking_Object(); // 피킹한 오브젝트 삭제 메뉴
	HRESULT Save_MapObject(); // MapObject 저장
	HRESULT Load_MapObject(); // MapObject 로드
	HRESULT Save_MapObject_Ins(); // MapObject_Ins 저장
	HRESULT Load_MapObject_Ins(); // MapObject_Ins 로드

	void Deep_Copy_Name(const _tchar* wszName = nullptr); // 모델 이름 문자열 깊은 복사
	void Deep_Copy_Path(const _tchar* wszPath); // 모델 경로 문자열 깊은 복사
	void Deep_Copy_Tag(const _tchar* wszTag); // 맵 오브젝트 태그 문자열 깊은 복사
	const _tchar* Deep_Copy(const _tchar* wszString); // 들어오는 문자열을 깊은 복사하는 함수

	_float3 Find_PickingPos(); // 지형의 피킹 위치를 찾는 함수
	HRESULT Create_Dummy(); // Dummy 생성
	HRESULT Delete_Dummy(); // Dummy 삭제
	HRESULT Save_Model_Path(_uint iType, const _tchar* pFilePath); // 프로그램이 종료될 때 호출시켜서 모델의 경로들을 저장함

private:
	// 메뉴 On Off _bool 변수 모음
	_bool m_isCheckPicking = { false };
	_bool m_isSelectModel = { false };
	_bool m_isCurrentMapObject = { false };
	_bool m_isSaveLoad = { false };
	_bool m_isInsSaveLoad = { false };
	_bool m_isInstallObject = { true };
	_bool m_isPickingObject = { false };

	// 버튼 On Off _bool 변수 모음
	_bool m_isDeleteObject = { false };

	// Radio 버튼 _int 변수 모음
	_int m_iInstallMethod = { 0 }; // MapObject 설치 방법 변경 변수

	CMapDummy* m_pDummy = { nullptr }; // 생성해둔 Dummy의 주소
	CMapObject* m_pObject = { nullptr }; // 설치할 MapObject의 주소
	CMapObject_Ins* m_pObjIns = { nullptr }; // 설치할 MapObejct_Ins의 주소
	_uint m_iMapObjectIndex = { 0 }; // 현재 맵에 설치된 맵 오브젝트의 개수

	string m_strCurrentModel = { "Dummy" }; // 현재 활성화된 모델 이름, 초기값은 더미
	_float m_vDummyMatrix[DUMMY_END][3];	// 더미의 상태 행렬, 이 값을 .ddd 파일에 저장함

	_int m_iModelIndex = { 0 }; // 선택된 모델 인덱스
	_int m_iTagIndex = { 0 }; // 선택된 맵 오브젝트 태그 인덱스
	vector<string> m_vecModelList; // 현재 추가해둔 모델 이름 리스트
	vector<const _tchar*> m_vecModelList_t; // 모델 이름을 _tchar로 저장해둠
	vector<const _tchar*> m_vecModelPath_t; // 모델 경로를 _tchar로 저장해둠
	vector<wstring> m_vecMapObjectTag; // 맵에 추가한 오브젝트들의 넘버링 태그를 저장
	vector<string> m_vecObjectTag_s; // 넘버링 태그를 string으로 저장함
 	vector<SAVEOBJECTDESC> m_vecSaveObject; // 저장할 맵 오브젝트에 대한 정보

	vector<SAVEINSOBJECTDESC> m_vecSaveInsObject; // 저장할 인스턴싱한 맵 오브젝트에 대한 정보

	_bool m_bOne = { true };

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END