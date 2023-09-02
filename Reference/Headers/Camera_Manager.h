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

private:
	//������ ī�޶���ġ�� ���ϱ����� �Լ�
	const _float4 Get_OffSetEye(OFFSETCAMERADESC& _OffSetCameraDesc);
	
	//for.public

public:
	void Tick(_float _TimeDelta);
	void Late_Tick(_float _TimeDelta);


public:
	//���� �ʱ�ȭ
	HRESULT Initialize_CameraManager();

	//���� ī�޶� �־��ش�.
	HRESULT Add_MainCamera(class CCamera* _pMainCamera);

	//�ƾ� ī�޶� �����͸� ��´�.
	HRESULT Read_CutSceneCamera(const _tchar* _CutSceneTag, const _tchar* _CutScenePath);

	//�ƾ� ����� ���� ť �߰�
	HRESULT Add_CutScene(const _tchar* _CutSceneTag);

	//������ ī�޶� �����͸� ��´�.
	HRESULT Read_OffSetCamera(const _tchar* _OffSetTag, const _tchar* _OffSetPath);
	
	//������ ����� ���� ť �߰�
	HRESULT Add_OffSetCamera(const _tchar* _OffSetTag);
	
	//for.protected

protected:

	//for.private
private:
	//queue<class CCamera*>	m_Cameras;

	//����������
	class CPipeLine* m_pPipeLine;

	//���� ī�޶�
	class CCamera* m_pMainCamera;
	
	//for. CutScene
	//�ƾ� ī�޶� ����ü (���� ���� ��� ����)
	CUTSCENECAMERADESC m_PreviousCutSceneCameraDesc = {};
	
	//�ƾ� ī�޶� ����ü ť
	queue<CUTSCENECAMERADESC>	m_CutSceneCameraDescs;

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

private:
	//�±׷� �ƾ��� ã�´�.
	vector<CUTSCENECAMERADESC>* Find_CutScene(const _tchar* _CutSceneTag);

	//�±׷� ������ī�޶� ã�´�.
	vector<OFFSETCAMERADESC>* Find_OffSetCamera(const _tchar* _OffSetTag);

	//�ƾ� ���
	void Play_CutScene(_float _TimeDelta);

	//������ ���
	void Play_OffSetCamera(_float _TimeDelta);

	void MainCameraOff()
	{
		m_isMainCameraOff = true;
	}

public:
	virtual void Free() override;
};

END