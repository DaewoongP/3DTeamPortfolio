#pragma once
/* =============================================== */
// 
//	정 : 장현우
//	부 :
//
/* =============================================== */

#include "Camera.h"

BEGIN(Engine)

//메인 -> 컷씬 -> 오프셋
//메인으로 돌아오는 러프는 메인이 수행하게끔 하겠다.
//액션도 매인이 가진다?

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	explicit CCamera_Manager() = default;
	virtual ~CCamera_Manager() = default;

	//for. Get, Set
public:
	//메인카메라가 돌지 않음을 확인하기위한 함수
	const _bool Get_MainCameraOff() const { return m_isMainCameraOff; }

private:
	//오프셋 카메라위치를 구하기위한 함수
	const _float4 Get_OffSetEye(OFFSETCAMERADESC& _OffSetCameraDesc);
	
	//for.public

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
	
	//for.protected

protected:

	//for.private
private:
	//queue<class CCamera*>	m_Cameras;

	//파이프라인
	class CPipeLine* m_pPipeLine;

	//매인 카메라
	class CCamera* m_pMainCamera;
	
	//for. CutScene
	//컷씬 카메라 구조체 (이전 값을 담기 위함)
	CUTSCENECAMERADESC m_PreviousCutSceneCameraDesc = {};
	
	//컷씬 카메라 구조체 큐
	queue<CUTSCENECAMERADESC>	m_CutSceneCameraDescs;

	//컷씬 시간 누적 값
	_float m_fCutSceneTimeAcc = { 0.0f };
	
	//컷씬 미리 저장해두는 구조체
	unordered_map<const _tchar*, vector<CUTSCENECAMERADESC>>	m_CutSceneCameraInfos;
	
	//for. OffSet
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

	//메인으로 돌아갈지 체크하기 위한 값
	_bool m_isMainCameraOff{ false };

private:
	//태그로 컷씬을 찾는다.
	vector<CUTSCENECAMERADESC>* Find_CutScene(const _tchar* _CutSceneTag);

	//태그로 오프셋카메라를 찾는다.
	vector<OFFSETCAMERADESC>* Find_OffSetCamera(const _tchar* _OffSetTag);

	//컷씬 재생
	void Play_CutScene(_float _TimeDelta);

	//오프셋 재생
	void Play_OffSetCamera(_float _TimeDelta);

	void MainCameraOff()
	{
		m_isMainCameraOff = true;
	}

public:
	virtual void Free() override;
};

END