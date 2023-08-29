#include "Camera_Manager.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CCamera_Manager);

void CCamera_Manager::Tick(_float _TimeDelta)
{
	NULL_CHECK_RETURN(m_pMainCamera, );

	m_pMainCamera->Tick(_TimeDelta);

	//�ƾ��� ��� �� �� �� �ִٸ�.
	if (false == m_CutSceneCameraDescs.empty())
	{
		//��� �Ѵ�.
		Play_CutScene(_TimeDelta);
	}
	//�ƾ� ���� �켱 ���� -> ������
	else if (false == m_OffSetCameraDescs.empty())
	{
		Play_OffSetCamera(_TimeDelta);
	}
	//������ ���� �켱 ���� -> �׼�
	else if (true)
	{

	}
}

void CCamera_Manager::Late_Tick(_float _TimeDelta)
{
}

HRESULT CCamera_Manager::Initialize_CameraManager()
{
	m_pPipeLine = CPipeLine::GetInstance(); 
	return S_OK;
}

HRESULT CCamera_Manager::Add_MainCamera(CCamera* _pMainCamera)
{
	NULL_CHECK_RETURN(_pMainCamera, E_FAIL);

	//Ȥ���ִٸ� �����̹Ƿ� ���۷��� ī��Ʈ ���⼭ ����
	if (nullptr != m_pMainCamera)
	{
		Safe_Release(m_pMainCamera);
		m_pMainCamera = nullptr;
	}

	m_pMainCamera = _pMainCamera;

	return S_OK;
}

HRESULT CCamera_Manager::Read_CutSceneCamera(const _tchar* _CutSceneTag, const _tchar* _CutScenePath)
{
	_ulong		dwByte = { 0 };
	HANDLE		hFile = { 0 };

	hFile = CreateFile(_CutScenePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return E_FAIL;

	//� ����������
	_uint iDescSize{};

	//�޾ƿ���
	ReadFile(hFile, &iDescSize, sizeof(_uint), &dwByte, nullptr);

	vector<CUTSCENECAMERADESC> vecCutSceneCameraDesc;

	//��������(���� �Ҵ� �ּ�ȭ�ϱ� ����)
	vecCutSceneCameraDesc.resize(iDescSize);

	//ī�޶� ���� �б�
	for (size_t vecIndex = 0; vecIndex < iDescSize; vecIndex++)
	{
		ReadFile(hFile, &vecCutSceneCameraDesc[vecIndex], sizeof(CUTSCENECAMERADESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	//������ ����
	m_CutSceneCameraInfos.emplace(_CutSceneTag, vecCutSceneCameraDesc);

	return S_OK;
}

HRESULT CCamera_Manager::Add_CutScene(const _tchar* _CutSceneTag)
{
	vector<CUTSCENECAMERADESC>* pCutSceneCameraDescs = Find_CutScene(_CutSceneTag);

	size_t vecSize = (*pCutSceneCameraDescs).size();

	for (size_t vecIndex = 0; vecIndex < vecSize; vecIndex++)
	{
		m_CutSceneCameraDescs.push((*pCutSceneCameraDescs)[vecIndex]);
	}

	return S_OK;
}

HRESULT CCamera_Manager::Read_OffSetCamera(const _tchar* _OffSetTag, const _tchar* _OffSetPath)
{
	_ulong		dwByte = { 0 };
	HANDLE		hFile = { 0 };

	hFile = CreateFile(_OffSetPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return E_FAIL;

	//� ����������
	_uint iDescSize{};

	//�޾ƿ���
	ReadFile(hFile, &iDescSize, sizeof(_uint), &dwByte, nullptr);

	vector<OFFSETCAMERADESC> vecOffSetCameraDesc;

	//��������(���� �Ҵ� �ּ�ȭ�ϱ� ����)
	vecOffSetCameraDesc.resize(iDescSize);

	//ī�޶� ���� �б�
	for (size_t vecIndex = 0; vecIndex < iDescSize; vecIndex++)
	{
		ReadFile(hFile, &vecOffSetCameraDesc[vecIndex], sizeof(OFFSETCAMERADESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	//������ ����
	m_OffSetCameraInfos.emplace(_OffSetTag, vecOffSetCameraDesc);

	return S_OK;
}

HRESULT CCamera_Manager::Add_OffSetCamera(const _tchar* _OffSetTag)
{
	vector<OFFSETCAMERADESC>* pOffSetCameraDescs = Find_OffSetCamera(_OffSetTag);

	size_t vecSize = (*pOffSetCameraDescs).size();

	for (size_t vecIndex = 0; vecIndex < vecSize; vecIndex++)
	{
		m_OffSetCameraDescs.push((*pOffSetCameraDescs)[vecIndex]);
	}

	return S_OK;
}

void CCamera_Manager::Play_CutScene(_float _TimeDelta)
{
	//�ð� ����
	m_fCutSceneTimeAcc += _TimeDelta;

	//����
	if (true == m_CutSceneCameraDescs.front().isLerp)
	{
		//���� ����
		_float fRatio =
			m_fCutSceneTimeAcc /
			(m_CutSceneCameraDescs.front().fDuration -
				m_PreviousCutSceneCameraDesc.fDuration);

		_float4 vEye =
			XMVectorLerp
			(
				m_PreviousCutSceneCameraDesc.vEye,
				m_CutSceneCameraDescs.front().vEye,
				fRatio
			);

		_float4 vAt =
			XMVectorLerp
			(
				m_PreviousCutSceneCameraDesc.vAt,
				m_CutSceneCameraDescs.front().vAt,
				fRatio
			);

		_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

		//ī�޶��� �����
		m_ViewMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vEye, vAt, vUp));	
	}
	else if (false == m_CutSceneCameraDescs.front().isLerp)
	{
		_float4 vEye = m_CutSceneCameraDescs.front().vEye;

		_float4 vAt = m_CutSceneCameraDescs.front().vAt;

		_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

		//ī�޶��� �����
		m_ViewMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vEye, vAt, vUp));
	}

	//������ ���� �� ����
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_ViewMatrix);

	//ť�� �� ������ �෹�̼��� �ð� ����ġ ���� �۴ٸ�
	if ((m_PreviousCutSceneCameraDesc.fDuration + m_fCutSceneTimeAcc)
		<=
		(m_CutSceneCameraDescs.front().fDuration))
	{
		//����ġ �ʱ�ȭ
		m_fCutSceneTimeAcc = 0.0f;

		//���� �� ����
		m_PreviousCutSceneCameraDesc = m_CutSceneCameraDescs.front();

		//�� �� ���� ����
		m_CutSceneCameraDescs.pop();
	}
}

void CCamera_Manager::Play_OffSetCamera(_float _TimeDelta)
{
	////�ð� ����
	//m_fOffSetTimeAcc += _TimeDelta;

	////����
	//if (true == m_OffSetCameraDescs.front().isLerp)
	//{
	//	//���� ����
	//	_float fRatio =
	//		m_fCutSceneTimeAcc /
	//		(m_OffSetCameraDescs.front().fDuration -
	//			m_PreviousOffSetCameraDesc.fDuration);

	//	_float4 vEye =
	//		XMVectorLerp
	//		(
	//			m_PreviousOffSetCameraDesc.vEye,
	//			m_OffSetCameraDescs.front().vEye,
	//			fRatio
	//		);

	//	_float4 vAt =
	//		XMVectorLerp
	//		(
	//			m_PreviousOffSetCameraDesc.vAt,
	//			m_OffSetCameraDescs.front().vAt,
	//			fRatio
	//		);

	//	_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

	//	//ī�޶��� �����
	//	m_ViewMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vEye, vAt, vUp));
	//}
	//else if (false == m_OffSetCameraDescs.front().isLerp)
	//{
	//	_float4 vEye = m_OffSetCameraDescs.front().vEye;

	//	_float4 vAt = m_OffSetCameraDescs.front().vAt;

	//	_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

	//	//ī�޶��� �����
	//	m_ViewMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vEye, vAt, vUp));
	//}

	////������ ���� �� ����
	//m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_ViewMatrix);

	////ť�� �� ������ �෹�̼��� �ð� ����ġ ���� �۴ٸ�
	//if ((m_PreviousOffSetCameraDesc.fDuration + m_fCutSceneTimeAcc)
	//	<=
	//	(m_OffSetCameraDescs.front().fDuration))
	//{
	//	//����ġ �ʱ�ȭ
	//	m_fCutSceneTimeAcc = 0.0f;

	//	//���� �� ����
	//	m_PreviousOffSetCameraDesc = m_OffSetCameraDescs.front();

	//	//�� �� ���� ����
	//	m_OffSetCameraDescs.pop();
	//}
}

vector<CUTSCENECAMERADESC>* CCamera_Manager::Find_CutScene(const _tchar* _CutSceneTag)
{
	NULL_CHECK_RETURN(_CutSceneTag, nullptr);

	//�±� ������ �˻�
	auto CutSceneInfo = find_if(m_CutSceneCameraInfos.begin(), m_CutSceneCameraInfos.end(), CTag_Finder(_CutSceneTag));

	NULL_CHECK_RETURN(&(*CutSceneInfo).second, nullptr);

	return &(*CutSceneInfo).second;
	return nullptr;
}

vector<OFFSETCAMERADESC>* CCamera_Manager::Find_OffSetCamera(const _tchar* _OffSetTag)
{
	NULL_CHECK_RETURN(_OffSetTag, nullptr);

	//�±� ������ �˻�
	auto OffSetInfo = find_if(m_OffSetCameraInfos.begin(), m_OffSetCameraInfos.end(), CTag_Finder(_OffSetTag));

	NULL_CHECK_RETURN(&(*OffSetInfo).second, nullptr);

	return &(*OffSetInfo).second;
	return nullptr;
}

void CCamera_Manager::Free()
{
}
