#pragma once

#include "Base.h"
#include "Tool_Defines.h"


BEGIN(Tool)

class CCamera_Point;
#ifdef _DEBUG
class CCamera_Line;
#endif
class CCutScene_Camera_Tool :
    public CBase
{
public:
	enum CUTSCENEPOINT
	{
		CUTSCENEPOINT_SELECT,
		CUTSCENEPOINT_CREATE,
		CUTSCENEPOINT_DELETE,
		CUTSCENEPOINT_END
	};

	enum CUTSCENE_EYE_OR_AT
	{
		CUTSCENE_AT,
		CUTSCENE_EYE,
		CUTSCENE_END
	};

	typedef struct tagCameraPointInfoDesc
	{
		CCamera_Point* pEyePoint{ nullptr };
		CCamera_Point* pAtPoint{ nullptr };
		_bool isLerp{ true };
		_float fDuration{ 1.0f };
		//축, 특정값 추가 예정(구면 보간)
	}CAMERAPOINTINFODESC;

private:
	explicit CCutScene_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutScene_Camera_Tool() = default;

public:
	HRESULT Initialize(void* pArg);
	void Tick(_float _fTimeDelta);
	HRESULT Render();




private:
	
	//포인트를 생성 하기 위함
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	//컷씬에 들어가는 데이터
	list<CAMERAPOINTINFODESC> m_CameraInfoList;
	//원본 At
	list<CCamera_Point*> m_OriginAtList;

	//검색된 반복자
	list<CAMERAPOINTINFODESC>::iterator m_CurrentIterater = { m_CameraInfoList.end() };

	//현제 선택된 오브젝트(수정하기 위함)
	CCamera_Point* m_pCurrentPoint = { nullptr };

	//현제 선택된 AtPoint(복사하기 위함)
	CCamera_Point* m_pAtCurrentPoint = { nullptr };

	//생성 가이드 용
	CCamera_Point* m_pCreateGuidePoint = { nullptr };
	
	//IMGUI 라디오 버튼용
	_int m_iPointRadio = { 0 };
	//Eye,At 구분용
	_int m_iEyeOrAt = { 0 };

	//포인트 생성 거리
	_float m_fDistance = { 10.0f };

	//초기화 더블체크 용
	_bool m_isClearDoubleCheck{ false };

#ifdef _DEBUG

	//Eye 라인
	CCamera_Line* m_pEyeLine = { nullptr };

	//At 라인
	CCamera_Line* m_pAtLine = { nullptr };

	//Look 라인
	CCamera_Line* m_pLookLine = { nullptr };
	
#endif

	//Line_Update용
	_bool m_isLineUpdate{ false };

private:
	//마우스로 위치 수정 기능
	void Fix_Point();

	//리스트 초기화 객체 초기화
	void Clear_CutSceneList();

	//AtPoint 바꾸기
	void Change_AtPoint(CCamera_Point* _pAtPoint);

private:
	//클릭 시 RayPos, RayDir 반환
	void Return_RayInfo(_Out_ _float4& _vRayPos, _Out_ _float4& _vRayDir);
	
	//생성 위치
	_float4 Point_Create_Position(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

private:
	//생성
	void Create_CameraInfo(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	void Create_OriginAt(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

private:
	//전체 순회 선택
	_bool Select_Eye_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);
	_bool Select_OriginAt_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

private:
	//삭제 (오브젝트 선택보다 늦게 돌려야 한다.)
	void Delete_Eye_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);
	void Delete_OriginAt_Point(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

private:
	//카메라 포인트 및 인포 수정
	void Fix_CameraPointInfo(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//생성 관련 업데이트
	void Create_Tick(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//선택 관련 업데이트
	void Select_Tick(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//삭제 관련 업데이트
	void Delete_Tick(_In_ _float4 _vRayPos, _In_ _float4 _vRayDir);

	//리스트 업데이트
	void List_Tick(_float _TimeDelta);
	
	void Clear_DoubleCheck();

	HRESULT Save_CutSceneInfo(const _tchar* _wszFilePath);

	HRESULT Load_CutSceneInfo(const _tchar* _wszFilePath);

	void Save_And_Load();

	void Play_CutScene();

	void Add_CutScene(const CAMERAPOINTINFODESC& _CameraPointInfoDesc);

#ifdef _DEBUG

	//라인 준비
	HRESULT Ready_Line();

	//추가 및 수정 시에
	HRESULT Line_Update();

	//Eye라인 업데이트
	HRESULT EyeLine_Update();

	//At라인 업데이트
	HRESULT AtLine_Update();

	//Look라인 업데이트
	HRESULT LookLine_Update();

#endif
public:
	static CCutScene_Camera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void * pArg = nullptr);
	virtual void Free() override;

};

END