#pragma once
/* =============================================== */
// 
//	정 : 장현우
//	부 :
//
/* =============================================== */

#include "Camera.h"
#include "Ease.h"

BEGIN(Engine)

//메인 -> 컷씬 -> 오프셋
//메인으로 돌아오는 러프는 메인이 수행하게끔 하겠다.
//액션도 매인이 가진다?

class ENGINE_DLL CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
public:
	enum SHAKE_AXIS
	{
		SHAKE_AXIS_RIGHT,
		SHAKE_AXIS_UP,
		SHAKE_AXIS_LOOK,
		SHAKE_AXIS_SET,
		SHAKE_AXIS_END
	};
	enum SHAKE_TYPE
	{
		SHAKE_TYPE_TRANSLATION,
		SHAKE_TYPE_ROTATION,
		SHAKE_TYPE_END
	};
	enum SHAKE_POWER 
	{
		SHAKE_POWER_CRECENDO,
		SHAKE_POWER_DECRECENDO,
		SHAKE_POWER_CRECENDO_DECRECENDO,
		SHAKE_POWER_END
	};

private:
	explicit CCamera_Manager() = default;
	virtual ~CCamera_Manager() = default;

	//for. Get, Set
public:
	//메인카메라가 돌지 않음을 확인하기위한 함수
	const _bool Get_MainCameraOff() const { return m_isMainCameraOff; }
	
	//쉐이크 시작 함수

	//타입, 축, 그래프, 주기, 크기, 지속시간, 특정 축
	void Set_Shake(
		SHAKE_TYPE _eType = SHAKE_TYPE_TRANSLATION,
		SHAKE_AXIS _eAxis = SHAKE_AXIS_LOOK,
		CEase::EASE _eEase = CEase::IN_SINE,
		_float _fSpeed = 1.0f,
		_float _Duration = 1.0f,
		_float _fPower = 1.0f,
		SHAKE_POWER _ePower = SHAKE_POWER_CRECENDO_DECRECENDO,
		_float3 _vAxisSet = _float3());
	
#ifdef _DEBUG
public:
	void Set_DebugCam(_bool isCam) { m_isDebugCam = isCam; }
#endif // _DEBUG

private:
	//오프셋 카메라위치를 구하기위한 함수
	const _float4 Get_OffSetEye(OFFSETCAMERADESC & _OffSetCameraDesc);

	//for.public

public:
	void Tick(_float _TimeDelta);

	//쉐이크를 위한 틱
	void Late_Tick(_float _TimeDelta);

public:
	//엔진 초기화
	HRESULT Initialize_CameraManager();

	//컷씬 카메라 데이터를 담는다.
	HRESULT Read_CutSceneCamera(const _tchar * _CutSceneTag, const _tchar * _CutScenePath);

	//컷씬 재생을 위한 큐 추가
	HRESULT Add_CutScene(const _tchar * _CutSceneTag);

	//컷씬 한컷씩 추가
	HRESULT Add_CutScene(CUTSCENECAMERADESC & _CutSceneCameraDesc);

	//오프셋 카메라 데이터를 담는다.
	HRESULT Read_OffSetCamera(const _tchar * _OffSetTag, const _tchar * _OffSetPath);

	//오프셋 재생을 위한 큐 추가
	HRESULT Add_OffSetCamera(const _tchar * _OffSetTag);

	//카메라 추가
	HRESULT Add_Camera(const _tchar * _CameraTag, class CCamera* _pCamera);

	//카메라 변경
	HRESULT Set_Camera(const _tchar * _CameraTag);

	//카메라 찾기
	class CCamera* Find_Camera(const _tchar * _CameraTag);

	//중지
	void Stop_CutScene();

	//카메라, 컷씬 초기화
	HRESULT Clear();


	//for.protected

protected:

	//for.private
private:
	//queue<class CCamera*>	m_Cameras;

	//파이프라인
	class CPipeLine* m_pPipeLine = { nullptr };

	//현제 카메라
	class CCamera* m_pCurrentCamera = { nullptr };

	//카메라 컨테이너
	unordered_map<const _tchar*, class CCamera*>	m_Cameras;

	//for. CutScene
	//컷씬 카메라 구조체 (이전 값을 담기 위함)
	CUTSCENECAMERADESC m_PreviousCutSceneCameraDesc = {};

	//컷씬 카메라 구조체 큐
	queue<CUTSCENECAMERADESC>	m_CutSceneCameraDescs;

	//구면 러프용으로 담아둘 리스트(추가 삭제에 용이)
	list<CUTSCENECAMERADESC> m_SplineData;

	//구면 러프용으로 담아둘 백터(인덱스 접근에 용이)(사이즈를 정해 놓고 사용할 것)
	vector<list<CUTSCENECAMERADESC>::iterator> m_SplineDataIndexAccess;

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

#ifdef _DEBUG
private:
	_bool		m_isDebugCam = { false };
#endif // _DEBUG

	//쉐이크
private:
	_float m_fShakeDuration = { 0.0f };
	_float m_fShakeTimeAcc = { 0.0f };
	_float m_fShakePower = { 0.0f };
	
	_float3 m_vShake_Axis_Set = { _float3() };

	SHAKE_AXIS m_eShake_Axis = { SHAKE_AXIS_END };
	SHAKE_TYPE m_eShake_Type = { SHAKE_TYPE_END };
	SHAKE_POWER m_eShake_Power = { SHAKE_POWER_END};

	CEase::EASE m_eEase = { CEase::EASE_END };

	//진동 주기
	_float m_fShakeSpeed = { 0.0f };

private:
	//태그로 컷씬을 찾는다.
	vector<CUTSCENECAMERADESC>* Find_CutScene(const _tchar* _CutSceneTag);

	//태그로 오프셋카메라를 찾는다.
	vector<OFFSETCAMERADESC>* Find_OffSetCamera(const _tchar* _OffSetTag);

	//컷씬 재생
	void Play_CutScene(_float _TimeDelta);

	//컷씬 재생 Spline
	void Play_Spline_CutScene(_float _TimeDelta);

	//오프셋 재생
	void Play_OffSetCamera(_float _TimeDelta);

	void MainCameraOff()
	{
		m_isMainCameraOff = true;
	}

	//list의 원소를 벡터에 연결
	void Connect_List_To_Vector();
	
	void CutScene_Do_Not_Lerp_Update(CUTSCENECAMERADESC _CutSceneCameraDesc);
	void CutScene_Lerp_Update(CUTSCENECAMERADESC _CutSceneCameraDescStart, CUTSCENECAMERADESC _CutSceneCameraDescEnd);


	//쉐이크
	void Shake_Update(_float _TimeDelta);

public:
	virtual void Free() override;
};

END