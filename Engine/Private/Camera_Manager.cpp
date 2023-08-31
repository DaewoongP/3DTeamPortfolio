#include "Camera_Manager.h"

#include "PipeLine.h"

IMPLEMENT_SINGLETON(CCamera_Manager);

void CCamera_Manager::Tick(_float _TimeDelta)
{
	NULL_CHECK_RETURN(m_pMainCamera, );

	m_pMainCamera->Tick(_TimeDelta);

	//컷씬에 재생 할 것 이 있다면.
	if (false == m_CutSceneCameraDescs.empty())
	{
		//재생 한다.
		Play_CutScene(_TimeDelta);
	}
	//컷씬 다음 우선 순위 -> 오프셋
	else if (false == m_OffSetCameraDescs.empty())
	{
		Play_OffSetCamera(_TimeDelta);
	}
	//오프셋 다음 우선 순위 -> 액션
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
	Safe_AddRef(m_pPipeLine);

	return S_OK;
}

HRESULT CCamera_Manager::Add_MainCamera(CCamera* _pMainCamera)
{
	NULL_CHECK_RETURN(_pMainCamera, E_FAIL);

	//혹시있다면 변경이므로 레퍼런스 카운트 여기서 제거
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

	//몇개 받을것인지
	_uint iDescSize{};

	//받아오고
	ReadFile(hFile, &iDescSize, sizeof(_uint), &dwByte, nullptr);

	vector<CUTSCENECAMERADESC> vecCutSceneCameraDesc;

	//리사이즈(동적 할당 최소화하기 위해)
	vecCutSceneCameraDesc.resize(iDescSize);

	//카메라 정보 읽기
	for (size_t vecIndex = 0; vecIndex < iDescSize; vecIndex++)
	{
		ReadFile(hFile, &vecCutSceneCameraDesc[vecIndex], sizeof(CUTSCENECAMERADESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	//데이터 저장
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

	//몇개 받을것인지
	_uint iDescSize{};

	//받아오고
	ReadFile(hFile, &iDescSize, sizeof(_uint), &dwByte, nullptr);

	vector<OFFSETCAMERADESC> vecOffSetCameraDesc;

	//리사이즈(동적 할당 최소화하기 위해)
	vecOffSetCameraDesc.resize(iDescSize);

	//카메라 정보 읽기
	for (size_t vecIndex = 0; vecIndex < iDescSize; vecIndex++)
	{
		ReadFile(hFile, &vecOffSetCameraDesc[vecIndex], sizeof(OFFSETCAMERADESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	//데이터 저장
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
	//시간 누적
	m_fCutSceneTimeAcc += _TimeDelta;

	//러프
	if (true == m_CutSceneCameraDescs.front().isLerp)
	{
		//러프 비율
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

		//카메라의 역행렬
		m_ViewMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vEye, vAt, vUp));
	}
	else if (false == m_CutSceneCameraDescs.front().isLerp)
	{
		_float4 vEye = m_CutSceneCameraDescs.front().vEye;

		_float4 vAt = m_CutSceneCameraDescs.front().vAt;

		_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

		//카메라의 역행렬
		m_ViewMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vEye, vAt, vUp));
	}

	//파이프 라인 값 변경
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_ViewMatrix);

	//큐의 앞 원소의 듀레이션이 시간 누적치 보다 작다면
	if ((m_PreviousCutSceneCameraDesc.fDuration + m_fCutSceneTimeAcc)
		<=
		(m_CutSceneCameraDescs.front().fDuration))
	{
		//누적치 초기화
		m_fCutSceneTimeAcc = 0.0f;

		//이전 값 변경
		m_PreviousCutSceneCameraDesc = m_CutSceneCameraDescs.front();

		//맨 앞 원소 삭제
		m_CutSceneCameraDescs.pop();
	}
}

void CCamera_Manager::Play_OffSetCamera(_float _TimeDelta)
{
	////시간 누적
	//m_fOffSetTimeAcc += _TimeDelta;

	////러프
	//if (true == m_OffSetCameraDescs.front().isLerp)
	//{
	//	//러프 비율
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

	//	//카메라의 역행렬
	//	m_ViewMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vEye, vAt, vUp));
	//}
	//else if (false == m_OffSetCameraDescs.front().isLerp)
	//{
	//	_float4 vEye = m_OffSetCameraDescs.front().vEye;

	//	_float4 vAt = m_OffSetCameraDescs.front().vAt;

	//	_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

	//	//카메라의 역행렬
	//	m_ViewMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vEye, vAt, vUp));
	//}

	////파이프 라인 값 변경
	//m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_ViewMatrix);

	////큐의 앞 원소의 듀레이션이 시간 누적치 보다 작다면
	//if ((m_PreviousOffSetCameraDesc.fDuration + m_fCutSceneTimeAcc)
	//	<=
	//	(m_OffSetCameraDescs.front().fDuration))
	//{
	//	//누적치 초기화
	//	m_fCutSceneTimeAcc = 0.0f;

	//	//이전 값 변경
	//	m_PreviousOffSetCameraDesc = m_OffSetCameraDescs.front();

	//	//맨 앞 원소 삭제
	//	m_OffSetCameraDescs.pop();
	//}
}

vector<CUTSCENECAMERADESC>* CCamera_Manager::Find_CutScene(const _tchar* _CutSceneTag)
{
	NULL_CHECK_RETURN(_CutSceneTag, nullptr);

	//태그 값으로 검색
	auto CutSceneInfo = find_if(m_CutSceneCameraInfos.begin(), m_CutSceneCameraInfos.end(), CTag_Finder(_CutSceneTag));

	NULL_CHECK_RETURN(&(*CutSceneInfo).second, nullptr);

	return &(*CutSceneInfo).second;
}

vector<OFFSETCAMERADESC>* CCamera_Manager::Find_OffSetCamera(const _tchar* _OffSetTag)
{
	NULL_CHECK_RETURN(_OffSetTag, nullptr);

	//태그 값으로 검색
	auto OffSetInfo = find_if(m_OffSetCameraInfos.begin(), m_OffSetCameraInfos.end(), CTag_Finder(_OffSetTag));

	NULL_CHECK_RETURN(&(*OffSetInfo).second, nullptr);

	return &(*OffSetInfo).second;
}

void CCamera_Manager::Free()
{
	Safe_Release(m_pPipeLine);
}