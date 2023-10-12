#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CDummy;

class CMonster_Window final : public CImWindow
{
	enum DUMMY_MATRIX { DUMMY_SCALE, DUMMY_ROT, DUMMY_TRANS, DUMMY_END }; // 더미의 상태

	typedef struct MonsterDesc
	{
		CDummy* pDummy = { nullptr };
		wstring wstrTag = { TEXT("") };					// 몬스터 종류(Golem, Dugbog, Troll...)
		_char szComponentTag[MAX_PATH] = { "" };		// 몬스터의 컴포넌트 태그
		wstring wstrPrototypeModelTag = { TEXT("") };	// 모델 프로토타입 태그
		wstring wstrModelFilePath = { TEXT("") };		// 모델 파일 패스
		_float4x4 matTransform;							// 상태 행렬
	}MONSTERDATA;

private:
	explicit CMonster_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMonster_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// 메뉴 모음
	void Picking_Menu();		// 피킹 메뉴
	void Show_MonsterTable();	// 현재 설치되어 있는 맵 오브젝트 확인
	void Save_Load_Menu();		// 오브젝트 세이브 로드 메뉴
	HRESULT Save_Monsters(const wstring& wstrMapDataName); // MapObject 저장
	HRESULT Load_Monsters(const wstring& wstrMapDataPath); // MapObject 로드

	_float3 Find_PickingPos();									// 지형의 피킹 위치를 찾는 함수
	HRESULT Create_Dummy();										// Dummy 생성
	HRESULT Delete_Dummy();										// Dummy 삭제
	HRESULT Load_Model_Prototypes(const wstring& wstrFilePath); // 프로그램이 시작될 때 호출시켜서 객체의 프로토타입 생성

private:
	// 메뉴 On Off _bool 변수 모음
	_bool m_isSaveLoad = { false };
	_bool m_isCheckPicking = { false };
	_bool m_isCreateMonster = { false };
	_bool m_isCurrentMapObject = { false };

	// 마우스 피킹 관련 변수
	_bool m_isLockMouseMove = { false };			// 마우스 움직임 제한 변수
	_float3 m_vLockingMousePos = { 0.f, 0.f, 0.f }; // 제한된 마우스 위치

	// 가변 데이터
	vector<MONSTERDATA> m_MonsterDatas;
	CDummy* m_pCurrentDummy = { nullptr };	// 현재 몬스터 더미 포인터
	_int m_iTableIndex = { 0 };				// 오브젝트 테이블에 선택된 오브젝트의 인덱스
	_int m_iCountIndex = { 0 };
	_float m_vDummyMatrix[DUMMY_END][3];	// 몬스터 더미의 상태 행렬

	// 초기 세팅된 데이터
	vector<wstring> m_wstrModelPaths;
	vector<wstring> m_wstrMonsterTags;
	vector<wstring> m_wstrPrototypeModelTags;
	_int m_iPrototypeTableIndex = { 0 }; // 초기 세팅된 데이터에 접근할 용도의 인덱스

	// 세이브 로드용 데이터
	string m_strLoadFilePath = { "" };
	_char m_szDataName[MAX_PATH] = { "" };

public:
	static CMonster_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END