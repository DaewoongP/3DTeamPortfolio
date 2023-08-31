#pragma once
#include "Camera.h"

BEGIN(Engine)

//메인 -> 컷씬 -> 오프셋


class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	explicit CCamera_Manager() = default;
	virtual ~CCamera_Manager() = default;

	//4. Get, Set

	//5. public

public:
	void Tick(_float _TimeDelta);
	void Late_Tick(_float _TimeDelta);


public:
	//엔진 초기화
	HRESULT Initialize_CameraManager();

	//매인 카메라를 넣어준다.
	HRESULT Add_MainCamera(class CCamera* _pMainCamera);

	//컷씬 카메라 데이터를 담는다.
	HRESULT Read_CutSceneCamera(const _tchar* _CutSceneTag, const _tchar* _CutScenePath);

	//컷씬 재생을 위한 큐 추가
	HRESULT Add_CutScene(const _tchar* _CutSceneTag);

	//오프셋 카메라 데이터를 담는다.
	HRESULT Read_OffSetCamera(const _tchar* _OffSetTag, const _tchar* _OffSetPath);
	
	//오프셋 재생을 위한 큐 추가
	HRESULT Add_OffSetCamera(const _tchar* _OffSetTag);

private:
	//queue<class CCamera*>	m_Cameras;

	//파이프라인
	class CPipeLine* m_pPipeLine;

	//매인 카메라
	class CCamera* m_pMainCamera;
	
	//컷씬 카메라 구조체 (이전 값을 담기 위함)
	CUTSCENECAMERADESC m_PreviousCutSceneCameraDesc = {};
	
	//컷씬 카메라 구조체 큐
	queue<CUTSCENECAMERADESC>	m_CutSceneCameraDescs;

	//컷씬 시간 누적 값
	_float m_fCutSceneTimeAcc = { 0.0f };
	
	//컷씬 미리 저장해두는 구조체
	unordered_map<const _tchar*, vector<CUTSCENECAMERADESC>>	m_CutSceneCameraInfos;
	
	//오프셋 카메라 구조체 (이전 값을 담기 위함) 
	//(주로 객체를 비추거나 따라갈때 사용할 듯)
	OFFSETCAMERADESC m_PreviousOffSetCameraDesc = {};

	//오프셋 카메라 구조체 큐
	queue<OFFSETCAMERADESC>	m_OffSetCameraDescs;

	//오프셋 시간 누적 값
	_float m_fOffSetTimeAcc = { 0.0f };

	//오프셋 미리 저장해두는 구조체
	unordered_map<const _tchar*, vector<OFFSETCAMERADESC>>	m_OffSetCameraInfos;

	
	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjectionMatrix;



private:
	//태그로 컷씬을 찾는다.
	vector<CUTSCENECAMERADESC>* Find_CutScene(const _tchar* _CutSceneTag);

	//태그로 오프셋카메라를 찾는다.
	vector<OFFSETCAMERADESC>* Find_OffSetCamera(const _tchar* _OffSetTag);

	//컷씬 재생
	void Play_CutScene(_float _TimeDelta);

	//오프셋 재생
	void Play_OffSetCamera(_float _TimeDelta);

public:
	virtual void Free() override;
};

END