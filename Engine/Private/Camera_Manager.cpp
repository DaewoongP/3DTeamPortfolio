#include "Camera_Manager.h"

#include "PipeLine.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCamera_Manager);

void CCamera_Manager::Set_Shake(SHAKE_TYPE _eType, SHAKE_AXIS _eAxis, CEase::EASE _eEase, _float _fSpeed, _float _Duration, _float _fPower, SHAKE_POWER _ePower, _float3 _vAxisSet)
{
	m_fShakeTimeAcc = 0.0f;

	m_fShakeDuration = _Duration;
	m_fShakePower = _fPower;

	m_vShake_Axis_Set = _vAxisSet;

	m_eShake_Axis = _eAxis;
	m_eShake_Type = _eType;
	m_eShake_Power = _ePower;

	m_eEase = _eEase;

	//진동 주기
	m_fShakeSpeed = _fSpeed;
}

const _float4 CCamera_Manager::Get_OffSetEye(OFFSETCAMERADESC& _OffSetCameraDesc)
{
	//포지션에
	_float4 vEye = _OffSetCameraDesc.pTargetMatrix->Translation().TransNorm();
	//타겟의 룩을 기준으로
	_float4 vTargetLook = _OffSetCameraDesc.pTargetMatrix->Look().TransNorm();

	vTargetLook.Normalize();

	//오프셋한 벡터를
	_float4 vOffSet = XMVector4Transform(vTargetLook, _OffSetCameraDesc.OffsetMatrix);

	//더한다.(카메라의 이전 위치)
	vEye += vOffSet;

	return vEye;
}

void CCamera_Manager::Tick(_float _TimeDelta)
{
#ifdef _DEBUG
	if (true == m_isDebugCam)
		return;
#endif // _DEBUG

	if (nullptr == m_pCurrentCamera)
	{
		return;
	}

	m_pCurrentCamera->Tick(_TimeDelta);

	Lerp_For_Set_Camera(_TimeDelta);

	//컷씬에 재생 할 것 이 있다면.
	if (false == m_SplineData.empty() || false == m_CutSceneCameraDescs.empty())
	{
		//재생 한다.
		//Play_CutScene(_TimeDelta);
		Play_Spline_CutScene(_TimeDelta);
		Clear_Queue(m_OffSetCameraDescs);
		MainCameraOff();
	}
	//컷씬 다음 우선 순위 -> 오프셋
	else if (false == m_OffSetCameraDescs.empty())
	{
		Play_OffSetCamera(_TimeDelta);
		MainCameraOff();
	}
	//오프셋 다음 우선 순위 -> 액션
	else if (true)
	{

	}
}

void CCamera_Manager::Late_Tick(_float _TimeDelta)
{
	Shake_Update(_TimeDelta);
}

HRESULT CCamera_Manager::Initialize_CameraManager()
{
	m_pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(m_pPipeLine);

	//구면 보간 준비 
	//사용할 사이즈
	m_SplineDataIndexAccess.resize(4);
	//초기화
	for (auto& iter : m_SplineDataIndexAccess)
	{
		iter = m_SplineData.end();
	}

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

HRESULT CCamera_Manager::Add_CutScene(CUTSCENECAMERADESC& _CutSceneCameraDesc)
{
	m_CutSceneCameraDescs.push(_CutSceneCameraDesc);

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

HRESULT CCamera_Manager::Add_Camera(const _tchar* _CameraTag, CCamera* _pCamera)
{
	if (nullptr == _CameraTag || nullptr == _pCamera)
	{
		MSG_BOX("Failed Add Camera");

		return E_FAIL;
	}

	m_Cameras.emplace(_CameraTag, _pCamera);

	return S_OK;
}

HRESULT CCamera_Manager::Set_Camera(const _tchar* _CameraTag, _float _fLerpTime)
{
	if (nullptr != m_pCurrentCamera)
	{
		Safe_Release(m_pCurrentCamera);
	}

	m_pCurrentCamera = Find_Camera(_CameraTag);

	if (nullptr == m_pCurrentCamera)
	{
		return E_FAIL;
	}

	Safe_AddRef(m_pCurrentCamera);

	//시간이 있다면 러프를 하겠다.
	if (0.0f != _fLerpTime)
	{
		m_fSetCameraLerpTime = _fLerpTime;
		m_fSetCameraLerpTimeAcc = _fLerpTime;

		m_vPreviousEye = m_pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW)->Translation();
		m_vPreviousAt = m_vPreviousEye + m_pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW)->Look();
	}

	return S_OK;
}

CCamera* CCamera_Manager::Find_Camera(const _tchar* _CameraTag)
{
	unordered_map<const _tchar*, class CCamera*>::iterator iter = find_if(m_Cameras.begin(), m_Cameras.end(), CTag_Finder(_CameraTag));

	if (m_Cameras.end() == iter)
	{
		MSG_BOX("Failed Fine Camera");

		return nullptr;
	}

	return iter->second;
}

void CCamera_Manager::Stop_CutScene()
{
	Clear_Queue(m_CutSceneCameraDescs);

	m_SplineData.clear();

	m_fCutSceneTimeAcc = 0.0f;
}

HRESULT CCamera_Manager::Clear()
{
	Safe_Release(m_pCurrentCamera);

	for (auto& iter : m_Cameras)
	{
		Safe_Release(iter.second);
	}

	m_Cameras.clear();

	m_SplineData.clear();

	m_SplineDataIndexAccess.clear();

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
			m_CutSceneCameraDescs.front().fDuration;

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
		m_ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);
	}
	else if (false == m_CutSceneCameraDescs.front().isLerp)
	{
		_float4 vEye = m_CutSceneCameraDescs.front().vEye;

		_float4 vAt = m_CutSceneCameraDescs.front().vAt;

		_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

		//카메라의 역행렬
		m_ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);
	}

	//파이프 라인 값 변경
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_ViewMatrix);

	//큐의 앞 원소의 듀레이션이 시간 누적치 보다 작다면
	if (m_fCutSceneTimeAcc
		>=
		m_CutSceneCameraDescs.front().fDuration)
	{
		//누적치 초기화
		m_fCutSceneTimeAcc = 0.0f;

		//이전 값 변경
		m_PreviousCutSceneCameraDesc = m_CutSceneCameraDescs.front();

		//맨 앞 원소 삭제
		m_CutSceneCameraDescs.pop();
	}
}

void CCamera_Manager::Play_Spline_CutScene(_float _TimeDelta)
{
	//시간 누적
	m_fCutSceneTimeAcc += _TimeDelta;

	//list 크기 2보다 작은 경우(초입) 이전과 같다.
	if (2 > m_SplineData.size())
	{
		//러프
		if (true == m_CutSceneCameraDescs.front().isLerp)
		{
			CutScene_Lerp_Update(m_PreviousCutSceneCameraDesc, m_CutSceneCameraDescs.front());
		}
		else if (false == m_CutSceneCameraDescs.front().isLerp)
		{
			CutScene_Do_Not_Lerp_Update(m_CutSceneCameraDescs.front());
		}

		//큐의 앞 원소의 듀레이션이 시간 누적치 보다 작다면
		if (m_fCutSceneTimeAcc
			>=
			m_CutSceneCameraDescs.front().fDuration)
		{
			//누적치 초기화
			m_fCutSceneTimeAcc = 0.0f;

			//이전 값 변경
			m_PreviousCutSceneCameraDesc = m_CutSceneCameraDescs.front();

			//list에 넣고
			m_SplineData.push_back(m_CutSceneCameraDescs.front());

			//맨 앞 원소 삭제
			m_CutSceneCameraDescs.pop();

			//만약 list 사이즈가 2라면
			if (2 == m_SplineData.size())
			{
				//사이즈 4까지 채워준다.
				while (4 > m_SplineData.size())
				{
					//list에 넣고
					m_SplineData.push_back(m_CutSceneCameraDescs.front());

					//맨 앞 원소 삭제
					m_CutSceneCameraDescs.pop();
				}
			}

			//벡터 초기화
			Connect_List_To_Vector();
		}
	}

	//list 크기 4인 경우 
	else if (4 == m_SplineData.size())
	{
		//이전 값을 바꾼다.(마지막 보간을 위해)
		//m_PreviousCutSceneCameraDesc = *m_SplineDataIndexAccess[1];
		
		//러프
		if (true == (*m_SplineDataIndexAccess[2]).isLerp)
		{
			//러프 비율
			_float fRatio =
				m_fCutSceneTimeAcc /
				(*m_SplineDataIndexAccess[2]).fDuration;

			_float4 vEye = XMVectorCatmullRom(
				(*m_SplineDataIndexAccess[0]).vEye,
				(*m_SplineDataIndexAccess[1]).vEye,
				(*m_SplineDataIndexAccess[2]).vEye,
				(*m_SplineDataIndexAccess[3]).vEye,
				fRatio);

			_float4 vAt = XMVectorCatmullRom(
				(*m_SplineDataIndexAccess[0]).vAt,
				(*m_SplineDataIndexAccess[1]).vAt,
				(*m_SplineDataIndexAccess[2]).vAt,
				(*m_SplineDataIndexAccess[3]).vAt,
				fRatio);

			_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

			//카메라의 역행렬
			m_ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);
		}
		//러프 안함
		else if (false == (*m_SplineDataIndexAccess[2]).isLerp)
		{
			CutScene_Do_Not_Lerp_Update(*m_SplineDataIndexAccess[2]);
		}

		//시간 누적치가 목적지의 듀레이션보다 작다면
		if (m_fCutSceneTimeAcc
			>=
			(*m_SplineDataIndexAccess[2]).fDuration)
		{
			//누적치 초기화
			m_fCutSceneTimeAcc = 0.0f;

			//이전 값 변경
			m_PreviousCutSceneCameraDesc = *m_SplineDataIndexAccess[2];

			//첫 원소 삭제 3
			m_SplineData.erase(m_SplineData.begin());
			
			//큐가 있다면
			if (false == m_CutSceneCameraDescs.empty())
			{
				//list에 넣고 4
				m_SplineData.push_back(m_CutSceneCameraDescs.front());

				//맨 앞 원소 삭제
				m_CutSceneCameraDescs.pop();
			}

			Connect_List_To_Vector();
		}
	}

	//list 크기 3인 경우
	else if (3 == m_SplineData.size())
	{
		//러프
		if (true == (*m_SplineDataIndexAccess[2]).isLerp)
		{
			CutScene_Lerp_Update(*m_SplineDataIndexAccess[1], *m_SplineDataIndexAccess[2]);
		}
		//러프 안함
		else if (false == (*m_SplineDataIndexAccess[2]).isLerp)
		{
			CutScene_Do_Not_Lerp_Update(*m_SplineDataIndexAccess[2]);
		}

		//시간 누적치가 목적지의 듀레이션보다 작다면
		if (m_fCutSceneTimeAcc
			>=
			(*m_SplineDataIndexAccess[2]).fDuration)
		{
			//누적치 초기화
			m_fCutSceneTimeAcc = 0.0f;

			//이전 값 변경
			m_PreviousCutSceneCameraDesc = *m_SplineDataIndexAccess[2];

			//첫 원소 삭제 2
			m_SplineData.erase(m_SplineData.begin());
			
			//큐가 있고, 리스트의 사이즈가 4보다 작다면
			while (false == m_CutSceneCameraDescs.empty() && 4 > m_SplineData.size())
			{
				//list에 넣고 4
				m_SplineData.push_back(m_CutSceneCameraDescs.front());

				//맨 앞 원소 삭제
				m_CutSceneCameraDescs.pop();
			}
			
			//큐가 없다면
			if (true == m_CutSceneCameraDescs.empty())
			{
				//리스트 클리어
				m_SplineData.clear();
			}

			Connect_List_To_Vector();
		}
	}
	
	//파이프 라인 값 변경
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_ViewMatrix);
}

void CCamera_Manager::Play_OffSetCamera(_float _TimeDelta)
{
	//시간 누적
	m_fOffSetTimeAcc += _TimeDelta;

	//러프
	if (true == m_OffSetCameraDescs.front().isLerp)
	{
		//이전 프레임 위치
		_float4 vPriviousEye = Get_OffSetEye(m_PreviousOffSetCameraDesc);

		//가야하는 프레임 위치
		_float4 vCurrentEye = Get_OffSetEye(m_OffSetCameraDescs.front());

		//러프 비율
		_float fRatio =
			m_fCutSceneTimeAcc /
			m_OffSetCameraDescs.front().fDuration;

		_float4 vEye =
			XMVectorLerp
			(
				vPriviousEye,
				vCurrentEye,
				fRatio
			);

		_float4 vAt = m_OffSetCameraDescs.front().pTargetMatrix->Translation().TransNorm();
				
		_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

		//카메라의 역행렬
		m_ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);
	}
	else if (false == m_OffSetCameraDescs.front().isLerp)
	{
		_float4 vEye = Get_OffSetEye(m_OffSetCameraDescs.front());

		_float4 vAt = m_OffSetCameraDescs.front().pTargetMatrix->Translation().TransNorm();

		_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

		//카메라의 역행렬
		m_ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);
	}

	//파이프 라인 값 변경
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_ViewMatrix);

	//큐의 앞 원소의 듀레이션이 시간 누적치 보다 작다면
	if (m_fOffSetTimeAcc
		<=
		m_OffSetCameraDescs.front().fDuration)
	{
		//누적치 초기화
		m_fOffSetTimeAcc = 0.0f;

		//이전 값 변경
		m_PreviousOffSetCameraDesc = m_OffSetCameraDescs.front();

		//맨 앞 원소 삭제
		m_OffSetCameraDescs.pop();
	}
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

void CCamera_Manager::Connect_List_To_Vector()
{
	list<CUTSCENECAMERADESC>::iterator iter = m_SplineData.begin();

	for (size_t i = 0; i < 4; i++)
	{
		//end여도 들어가라
		m_SplineDataIndexAccess[i] = iter;
		//end가 아닐경우만 ++
		if (m_SplineData.end() != iter)
		{
			++iter;
		}
	}
}

void CCamera_Manager::CutScene_Do_Not_Lerp_Update(CUTSCENECAMERADESC _CutSceneCameraDesc)
{
	_float4 vEye = _CutSceneCameraDesc.vEye;

	_float4 vAt = _CutSceneCameraDesc.vAt;

	_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

	//카메라의 역행렬
	m_ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);
}

void CCamera_Manager::CutScene_Lerp_Update(CUTSCENECAMERADESC _CutSceneCameraDescStart, CUTSCENECAMERADESC _CutSceneCameraDescEnd)
{
	_float fRatio =
		m_fCutSceneTimeAcc /
		_CutSceneCameraDescEnd.fDuration;

	_float4 vEye =
		XMVectorLerp
		(
			_CutSceneCameraDescStart.vEye,
			_CutSceneCameraDescEnd.vEye,
			fRatio
		);

	_float4 vAt =
		XMVectorLerp
		(
			_CutSceneCameraDescStart.vAt,
			_CutSceneCameraDescEnd.vAt,
			fRatio
		);

	_float4 vUp = _float4(0.0f, 1.0f, 0.0f, 0.0f);

	//카메라의 역행렬
	m_ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);
}

void CCamera_Manager::Shake_Update(_float _TimeDelta)
{
	//예외 조건
	//누적 시간 >= 총 시간
	if (m_fShakeTimeAcc >= m_fShakeDuration)
	{
		return;
	}

	m_fShakeTimeAcc += _TimeDelta;

	if (m_fShakeTimeAcc >= m_fShakeDuration)
	{
		m_fShakeTimeAcc = m_fShakeDuration;
	}

	_float4x4 view_Matrix = *m_pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	_float fShakePower = { 0.0f };
	fShakePower = m_fShakePower - CEase::Ease(m_eEase, m_fShakeTimeAcc, 0.0f, m_fShakePower, m_fShakeDuration);
	
	switch (m_eShake_Power)
	{
	case Engine::CCamera_Manager::SHAKE_POWER_CRECENDO:
	{
		fShakePower = CEase::Ease(m_eEase, m_fShakeTimeAcc, 0.0f, m_fShakePower, m_fShakeDuration);
	}
		break;
	case Engine::CCamera_Manager::SHAKE_POWER_DECRECENDO:
	{
		fShakePower = m_fShakePower - CEase::Ease(m_eEase, m_fShakeTimeAcc, 0.0f, m_fShakePower, m_fShakeDuration);
	}
	break;
	case Engine::CCamera_Manager::SHAKE_POWER_CRECENDO_DECRECENDO:
	{
		//지속시간의 반을 넘어갔다면, 점점 약해진다.
		if (m_fShakeTimeAcc >= m_fShakeDuration * 0.5f)
		{
			fShakePower = m_fShakePower - CEase::Ease(m_eEase, m_fShakeTimeAcc, 0.0f, m_fShakePower, m_fShakeDuration);
		}
		else
		{
			fShakePower = CEase::Ease(m_eEase, m_fShakeTimeAcc, 0.0f, m_fShakePower, m_fShakeDuration);
		}
	}
		break;
	case Engine::CCamera_Manager::SHAKE_POWER_END:
		break;
	default:
		break;
	}



	_float fSin = sinf(m_fShakeTimeAcc * m_fShakeDuration * XMConvertToRadians(360.0f) * m_fShakeSpeed);

	_float fShakeResult = fSin * fShakePower;

	//쉐이크
	switch (m_eShake_Type)
	{
	case Engine::CCamera_Manager::SHAKE_TYPE_TRANSLATION:
	{
		switch (m_eShake_Axis)
		{
		case Engine::CCamera_Manager::SHAKE_AXIS_RIGHT:
		{
			_float3 vRight = view_Matrix.Right();
			vRight.Normalize();
			vRight *= fShakeResult;
			view_Matrix = view_Matrix * XMMatrixTranslation(vRight.x, vRight.y, vRight.z);
		}
			break;
		case Engine::CCamera_Manager::SHAKE_AXIS_UP:
		{
			_float3 vUp = view_Matrix.Up();
			vUp.Normalize();
			vUp *= fShakeResult;
			view_Matrix = view_Matrix * XMMatrixTranslation(vUp.x, vUp.y, vUp.z);
		}
			break;
		case Engine::CCamera_Manager::SHAKE_AXIS_LOOK:
		{
			_float3 vLook = view_Matrix.Look();
			vLook.Normalize();
			vLook *= fShakeResult;
			view_Matrix = view_Matrix * XMMatrixTranslation(vLook.x, vLook.y, vLook.z);
		}
			break;
		case Engine::CCamera_Manager::SHAKE_AXIS_SET:
		{
			_float3 vAxis = m_vShake_Axis_Set;
			vAxis.Normalize();
			vAxis *= fShakeResult;
			view_Matrix = view_Matrix * XMMatrixTranslation(vAxis.x, vAxis.y, vAxis.z);
		}
			break;
		case Engine::CCamera_Manager::SHAKE_AXIS_END:
			break;
		default:
			break;
		}
	}
		break;
	case Engine::CCamera_Manager::SHAKE_TYPE_ROTATION:
	{
		_float3 vRight = view_Matrix.Right();
		_float3 vUp = view_Matrix.Up();
		_float3 vLook = view_Matrix.Look();

		_float4x4 RotationMatrix = _float4x4();

		switch (m_eShake_Axis)
		{
		case Engine::CCamera_Manager::SHAKE_AXIS_RIGHT:
		{
			RotationMatrix = XMMatrixRotationQuaternion(
				XMQuaternionRotationAxis(XMVector3Normalize(vRight), fShakeResult));
		}
			break;
		case Engine::CCamera_Manager::SHAKE_AXIS_UP:
		{
			RotationMatrix = XMMatrixRotationQuaternion(
				XMQuaternionRotationAxis(XMVector3Normalize(vUp), fShakeResult));
		}
			break;
		case Engine::CCamera_Manager::SHAKE_AXIS_LOOK:
		{
			RotationMatrix = XMMatrixRotationQuaternion(
				XMQuaternionRotationAxis(XMVector3Normalize(vLook), fShakeResult));
		}
			break;
		case Engine::CCamera_Manager::SHAKE_AXIS_SET:
		{
			RotationMatrix = XMMatrixRotationQuaternion(
				XMQuaternionRotationAxis(XMVector3Normalize(m_vShake_Axis_Set), fShakeResult));
		}
			break;
		case Engine::CCamera_Manager::SHAKE_AXIS_END:
			break;
		default:
			break;
		}

		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);

		memcpy(&view_Matrix.m[0][0], &vRight, sizeof(_float3));
		memcpy(&view_Matrix.m[1][0], &vUp, sizeof(_float3));
		memcpy(&view_Matrix.m[2][0], &vLook, sizeof(_float3));
	}
		break;
	case Engine::CCamera_Manager::SHAKE_TYPE_END:
		break;
	default:
		break;
	}

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, view_Matrix);
}

void CCamera_Manager::Lerp_For_Set_Camera(_float _TimeDelta)
{
	//탈출
	if (0.0f == m_fSetCameraLerpTimeAcc)
	{
		return;
	}

	m_pPipeLine->Tick();

	//감소
	m_fSetCameraLerpTimeAcc -= _TimeDelta;

	//초기화
	if (0.0f > m_fSetCameraLerpTimeAcc)
	{
		m_fSetCameraLerpTimeAcc = 0.0f;
	}

	// 1 ~ 0
	_float fRatio = m_fSetCameraLerpTimeAcc / m_fSetCameraLerpTime;

	_float3 vEye = m_pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW)->Translation();
	_float3 vAt  = vEye + m_pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW)->Look();

	vEye = XMVectorLerp(vEye, m_vPreviousEye, fRatio);
	vAt = XMVectorLerp(vAt, m_vPreviousAt, fRatio);

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixLookAtLH(vEye, vAt, _float3(0.0f, 1.0f, 0.0f)));
}

void CCamera_Manager::Free()
{
	Safe_Release(m_pPipeLine);
	
	Clear();
}