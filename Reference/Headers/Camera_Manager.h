#pragma once
/* =============================================== */
// 
//	�� : ������
//	�� :
//
/* =============================================== */

#include "Camera.h"

BEGIN(Engine)

//���� -> �ƾ� -> ������
//�������� ���ƿ��� ������ ������ �����ϰԲ� �ϰڴ�.
//�׼ǵ� ������ ������?

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	explicit CCamera_Manager() = default;
	virtual ~CCamera_Manager() = default;

	//for. Get, Set
public:
	//����ī�޶� ���� ������ Ȯ���ϱ����� �Լ�
	const _bool Get_MainCameraOff() const { return m_isMainCameraOff; }

#ifdef _DEBUG
public:
	void Set_DebugCam(_bool isCam) { m_isDebugCam = isCam; }
#endif // _DEBUG

private:
	//������ ī�޶���ġ�� ���ϱ����� �Լ�
	const _float4 Get_OffSetEye(OFFSETCAMERADESC& _OffSetCameraDesc);
	
	//for.public

public:
	void Tick(_float _TimeDelta);

public:
	//���� �ʱ�ȭ
	HRESULT Initialize_CameraManager();

	//�ƾ� ī�޶� �����͸� ��´�.
	HRESULT Read_CutSceneCamera(const _tchar* _CutSceneTag, const _tchar* _CutScenePath);

	//�ƾ� ����� ���� ť �߰�
	HRESULT Add_CutScene(const _tchar* _CutSceneTag);

	//�ƾ� ���ƾ� �߰�
	HRESULT Add_CutScene(CUTSCENECAMERADESC & _CutSceneCameraDesc);

	//������ ī�޶� �����͸� ��´�.
	HRESULT Read_OffSetCamera(const _tchar* _OffSetTag, const _tchar* _OffSetPath);
	
	//������ ����� ���� ť �߰�
	HRESULT Add_OffSetCamera(const _tchar* _OffSetTag);

	//ī�޶� �߰�
	HRESULT Add_Camera(const _tchar* _CameraTag, class CCamera* _pCamera);

	//ī�޶� ����
	HRESULT Set_Camera(const _tchar* _CameraTag);

	//ī�޶� ã��
	class CCamera* Find_Camera(const _tchar* _CameraTag);

	//����
	void Stop_CutScene();

	//ī�޶�, �ƾ� �ʱ�ȭ
	HRESULT Clear();


	//for.protected

protected:

	//for.private
private:
	//queue<class CCamera*>	m_Cameras;

	//����������
	class CPipeLine* m_pPipeLine = { nullptr };

	//���� ī�޶�
	class CCamera* m_pCurrentCamera = { nullptr };
	
	//ī�޶� �����̳�
	unordered_map<const _tchar*, class CCamera*>	m_Cameras;

	//for. CutScene
	//�ƾ� ī�޶� ����ü (���� ���� ��� ����)
	CUTSCENECAMERADESC m_PreviousCutSceneCameraDesc = {};
	
	//�ƾ� ī�޶� ����ü ť
	queue<CUTSCENECAMERADESC>	m_CutSceneCameraDescs;

	//���� ���������� ��Ƶ� ����Ʈ(�߰� ������ ����)
	list<CUTSCENECAMERADESC> m_SplineData;

	//���� ���������� ��Ƶ� ����(�ε��� ���ٿ� ����)(����� ���� ���� ����� ��)
	vector<list<CUTSCENECAMERADESC>::iterator> m_SplineDataIndexAccess;
	
	//�ƾ� �ð� ���� ��
	_float m_fCutSceneTimeAcc = { 0.0f };
	
	//�ƾ� �̸� �����صδ� ����ü
	unordered_map<const _tchar*, vector<CUTSCENECAMERADESC>>	m_CutSceneCameraInfos;
	
	//for. OffSet
	//������ ī�޶� ����ü (���� ���� ��� ����) 
	//(�ַ� ��ü�� ���߰ų� ���󰥶� ����� ��)
	OFFSETCAMERADESC m_PreviousOffSetCameraDesc = {};

	//������ ī�޶� ����ü ť
	queue<OFFSETCAMERADESC>	m_OffSetCameraDescs;

	

	//������ �ð� ���� ��
	_float m_fOffSetTimeAcc = { 0.0f };

	//������ �̸� �����صδ� ����ü
	unordered_map<const _tchar*, vector<OFFSETCAMERADESC>>	m_OffSetCameraInfos;

	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjectionMatrix;

	//�������� ���ư��� üũ�ϱ� ���� ��
	_bool m_isMainCameraOff{ false };

#ifdef _DEBUG
private:
	_bool		m_isDebugCam = { false };
#endif // _DEBUG

private:
	//�±׷� �ƾ��� ã�´�.
	vector<CUTSCENECAMERADESC>* Find_CutScene(const _tchar* _CutSceneTag);

	//�±׷� ������ī�޶� ã�´�.
	vector<OFFSETCAMERADESC>* Find_OffSetCamera(const _tchar* _OffSetTag);

	//�ƾ� ���
	void Play_CutScene(_float _TimeDelta);

	//�ƾ� ��� Spline
	void Play_Spline_CutScene(_float _TimeDelta);

	//������ ���
	void Play_OffSetCamera(_float _TimeDelta);

	void MainCameraOff()
	{
		m_isMainCameraOff = true;
	}

	//list�� ���Ҹ� ���Ϳ� ����
	void Connect_List_To_Vector();
	
	void CutScene_Do_Not_Lerp_Update(CUTSCENECAMERADESC _CutSceneCameraDesc);
	void CutScene_Lerp_Update(CUTSCENECAMERADESC _CutSceneCameraDescStart, CUTSCENECAMERADESC _CutSceneCameraDescEnd);


public:
	virtual void Free() override;
};

END