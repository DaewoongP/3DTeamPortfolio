#include "CutScene_Camera_Tool.h"
#include "Camera_Point.h"
#include "GameInstance.h"
#include "Main_Camera.h"

#ifdef _DEBUG

#include "Camera_Line.h"

#endif

CCutScene_Camera_Tool::CCutScene_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;
}

HRESULT CCutScene_Camera_Tool::Initialize(void* pArg)
{
	BEGININSTANCE;

	
#ifdef _DEBUG

	//라인 준비
	Ready_Line();

#endif

	CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

	CameraPointDesc.vPosition = Point_Create_Position(_float4(), _float4());

	//생성 가이드 포인트
	m_pCreateGuidePoint =
		dynamic_cast<CCamera_Point*>(
			pGameInstance->Clone_Component(LEVEL_TOOL,
				TEXT("Prototype_GameObject_Camera_Point"),
				&CameraPointDesc));

	//생성 실패 탈출
	if (nullptr == m_pCreateGuidePoint)
	{
		MSG_BOX("Create Guide Create Failed");

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;
	return S_OK;
}

void CCutScene_Camera_Tool::Tick(_float _fTimeDelta)
{
	m_isLineUpdate = false;

	//제목
	ImGui::Text("CutScene_Tool");

	//초기화
	Clear_DoubleCheck();

	if (ImGui::Button("Clear CutScene Data"))
	{
		m_isClearDoubleCheck = true;
	}

	BEGININSTANCE;

	//포인트 생성 및 삭제 
	ImGui::Text("Point : "); ImGui::SameLine();

	ImGui::RadioButton("Select", &m_iPointRadio, 0); ImGui::SameLine();
	ImGui::RadioButton("Create", &m_iPointRadio, 1); ImGui::SameLine();
	ImGui::RadioButton("Delete", &m_iPointRadio, 2); ImGui::SameLine();
	ImGui::RadioButton("Section_Edit", &m_iPointRadio, 3);

	//Eye 또는 At 편집
	if (pGameInstance->Get_DIKeyState(DIKEYBOARD_LCONTROL))
	{
		m_iEyeOrAt = CUTSCENE_AT;
	}
	else if (pGameInstance->Get_DIKeyState(DIKEYBOARD_SPACE))
	{
		m_iEyeOrAt = CUTSCENE_EYE;
	}


	ImGui::Text("At-->Ctrl : Eye-->Space");
	ImGui::RadioButton("At_Point", &m_iEyeOrAt, CUTSCENE_AT); ImGui::SameLine();
	ImGui::RadioButton("Eye_Point", &m_iEyeOrAt, CUTSCENE_EYE);





	_float4 vRayPos{}, vRayDir{};

	//누르는 동안 갱신
	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) ||
		pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING))
	{
		Return_RayInfo(vRayPos, vRayDir);
	}

	switch (m_iPointRadio)
	{
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_SELECT:
		//누른 순간만
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		{
			Select_Tick(vRayPos, vRayDir);
		}
		Fix_CameraPointInfo(vRayPos, vRayDir);
		break;
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_CREATE:
	{
		Return_RayInfo(vRayPos, vRayDir);

		_float4 vPos = vRayPos + vRayDir * m_fDistance;

		m_pCreateGuidePoint->Set_Position(vPos);

		m_pCreateGuidePoint->Tick(pGameInstance->Get_World_Tick());

		BEGININSTANCE;
		if (pGameInstance->Get_DIKeyState(DIKEYBOARD_G, CInput_Device::KEY_DOWN))
		{
			m_isInsertBefore = !m_isInsertBefore;

		}
		ENDINSTANCE;
		ImGui::Checkbox("Insert before : \"G\" true<->false", &m_isInsertBefore);

		//생성 거리
		ImGui::DragFloat("Create Distance", &m_fDistance, 0.01f, 0.0f, 1000.0f);


		//오른쪽 누른 상태에서 왼쪽을 누른 순간만
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
			pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_PRESSING))
		{
			Create_Tick(vRayPos, vRayDir);
		}
	}
	break;
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_DELETE:
		//오른쪽 누른 상태에서 왼쪽을 누른 순간만
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
			pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_PRESSING))
		{
			Delete_Tick(vRayPos, vRayDir);
		}
		break;
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_SECTION:
		Section_Point_Update(vRayPos, vRayDir);
		break;
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_END:
		break;
	default:
		break;
	}

	_float4 vPos = *pGameInstance->Get_CamPosition();

	vector<_float> vecPos;

	vecPos.resize(3);

	vecPos[0] = vPos.x;
	vecPos[1] = vPos.y;
	vecPos[2] = vPos.z;

	ImGui::DragFloat3("PipelinePos", vecPos.data());
	
	Set_Position_CurrentPoint();

	ENDINSTANCE;

	List_Tick(_fTimeDelta);

	Save_And_Load();

	Play_CutScene();


#ifdef _DEBUG

	if (true == m_isLineUpdate)
	{
		Line_Update();
	}

	m_pEyeLine->Late_Tick(_fTimeDelta);
	m_pAtLine->Late_Tick(_fTimeDelta);
	m_pLookLine->Late_Tick(_fTimeDelta);

#endif

	Stop_CutScene();

}

HRESULT CCutScene_Camera_Tool::Render()
{
	return S_OK;
}

void CCutScene_Camera_Tool::Fix_Point()
{
	if (nullptr == m_pCurrentPoint)
	{
		return;
	}

	//카메라 룩의 기준으로...
	BEGININSTANCE;

	//카메라 월드
	const _float4x4* pCameraWorldMatrix = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	//카메라 라이트
	_float3 vCameraRight = pCameraWorldMatrix->Right();
	vCameraRight.Normalize();
	//카메라 업
	_float3 vCameraUp = pCameraWorldMatrix->Up();
	vCameraUp.Normalize();
	//카메라 룩
	_float3 vCameraLook = pCameraWorldMatrix->Look();
	vCameraLook.Normalize();

	_long iMouseMoveX = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X);
	_long iMouseMoveY = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);
	_long iMouseMoveZ = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_WHEEL);

	_float4 vFixPosition = m_pCurrentPoint->Get_Position();

	_float fSpeed{ 0.01f };
	_float fWheelSpeed{ 0.01f };

	if (pGameInstance->
		Get_DIMouseState(
			CInput_Device::DIMK_LBUTTON,
			CInput_Device::KEY_PRESSING) &&
		pGameInstance->
		Get_DIMouseState(
			CInput_Device::DIMK_RBUTTON,
			CInput_Device::KEY_PRESSING))
	{
		//x축 이동
		if (abs(iMouseMoveX) > abs(iMouseMoveY))
		{
			vFixPosition += vCameraRight.TransNorm() * _float(iMouseMoveX) * fSpeed;
		}
		//y축 이동
		else
		{
			vFixPosition += -vCameraUp.TransNorm() * _float(iMouseMoveY) * fSpeed;
		}

		m_isLineUpdate = true;
	}
	//z축 이동이 있다면
	if (0 != iMouseMoveZ)
	{
		vFixPosition += vCameraLook.TransNorm() * _float(iMouseMoveZ) * fSpeed * fWheelSpeed;

		m_isLineUpdate = true;
	}

	m_pCurrentPoint->Set_Position(vFixPosition);

	ENDINSTANCE;

}

void CCutScene_Camera_Tool::Clear_CutSceneList()
{
	//초기화 
	for (auto& iter : m_CameraInfoList)
	{
		Safe_Release(iter.pEyePoint);
		Safe_Release(iter.pAtPoint);
	}
	m_CameraInfoList.clear();

	for (auto& iter : m_OriginAtList)
	{
		Safe_Release(iter);
	}
	m_OriginAtList.clear();

	m_CurrentIterater = m_CameraInfoList.end();

	m_pCurrentPoint = nullptr;

	m_pAtCurrentPoint = nullptr;

	m_isLineUpdate = true;

	m_iterStart = m_CameraInfoList.end();

	m_iterEnd = m_CameraInfoList.end(); 

	m_iterCurEyePoint = m_CameraInfoList.end();
}

void CCutScene_Camera_Tool::Change_AtPoint(CCamera_Point* _pAtPoint)
{
	//기존 오브젝트가 있다면
	if (nullptr != m_pAtCurrentPoint)
	{
#ifdef _DEBUG
		//기존 오브젝트 색 돌려주고
		m_pAtCurrentPoint->Set_Collider_Color(_float4(1.0f, 0.0f, 0.0f, 1.0f));
#endif // _DEBUG
	}

	//바꿔 끼고
	m_pAtCurrentPoint = _pAtPoint;

#ifdef _DEBUG
	//색을 입힘
	m_pAtCurrentPoint->Set_Collider_Color(_float4(1.0f, 191.0f / 255.0f, 0.0f, 1.0f));
#endif // _DEBUG

}

void CCutScene_Camera_Tool::Change_EyePoint(list<CAMERAPOINTINFODESC>::iterator _iterEye)
{
	//기존에 선택된 것이 있다면
	if (m_CameraInfoList.end() != m_iterCurEyePoint)
	{
#ifdef _DEBUG
		//기존 오브젝트 색 돌려주고
		(*m_iterCurEyePoint).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif // _DEBUG
	}

	//바꿔 끼고
	m_iterCurEyePoint = _iterEye;

#ifdef _DEBUG
	//색을 입힘
	(*m_iterCurEyePoint).pEyePoint->Set_Collider_Color(_float4(0.0f, 1.0f, 1.0f, 1.0f));
#endif // _DEBUG
}

void CCutScene_Camera_Tool::Create_CameraInfo(_float4 _vRayPos, _float4 _vRayDir)
{
	//CameraInfoList에 추가
	if (CUTSCENE_EYE == m_iEyeOrAt &&
		nullptr != m_pAtCurrentPoint &&		//선택된 At이 있어야 한다.
		false == m_OriginAtList.empty())	//Origin이 있어야 한다.
	{
		BEGININSTANCE;

		CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

		CameraPointDesc.vPosition = Point_Create_Position(_vRayPos, _vRayDir);

		CCamera_Point* pCameraPoint =
			dynamic_cast<CCamera_Point*>(
				pGameInstance->Clone_Component(LEVEL_TOOL,
					TEXT("Prototype_GameObject_Camera_Point"),
					&CameraPointDesc));

		//생성 실패 탈출
		if (nullptr == pCameraPoint)
		{
			MSG_BOX("CameraInfo Create Failed");

			ENDINSTANCE;

			return;
		}

#ifdef _DEBUG
		pCameraPoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif
		CAMERAPOINTINFODESC CameraPointInfoDesc;

		CameraPointInfoDesc.pEyePoint = pCameraPoint;
		CameraPointInfoDesc.pAtPoint = m_pAtCurrentPoint;
		Safe_AddRef(m_pAtCurrentPoint);

		//현제 선택된 포인트의 앞에 놓거나.
		if (true == m_isInsertBefore)
		{
			if (m_CameraInfoList.end() != m_iterCurEyePoint)
			{
				m_CameraInfoList.insert(m_iterCurEyePoint, CameraPointInfoDesc);
			}
			else
			{
				MSG_BOX("Failed Insert Before");

				Safe_Release(m_pAtCurrentPoint);

				Safe_Release(pCameraPoint);

				ENDINSTANCE;

				return;
			}
		}
		else
		{
			//맨뒤에 넣거나
			m_CameraInfoList.push_back(CameraPointInfoDesc);
		}
		//가장 마지막 이터레이터로 갱신
		m_CurrentIterater = --m_CameraInfoList.end();

		//현제 오브젝트로 바꿈
		m_pCurrentPoint = pCameraPoint;

		m_isLineUpdate = true;

		ENDINSTANCE;
	}
}

void CCutScene_Camera_Tool::Create_OriginAt(_float4 _vRayPos, _float4 _vRayDir)
{
	if (CUTSCENE_AT == m_iEyeOrAt)
	{
		BEGININSTANCE;

		CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

		CameraPointDesc.vPosition = Point_Create_Position(_vRayPos, _vRayDir);

		CCamera_Point* pCameraPoint =
			dynamic_cast<CCamera_Point*>(
				pGameInstance->Clone_Component(LEVEL_TOOL,
					TEXT("Prototype_GameObject_Camera_Point"),
					&CameraPointDesc));

		//생성 실패 탈출
		if (nullptr == pCameraPoint)
		{
			MSG_BOX("OriginAt Create Failed");

			ENDINSTANCE;

			return;
		}

#ifdef _DEBUG
		//색만 바꿔서 
		pCameraPoint->Set_Collider_Color(_float4(1.0f, 0.0f, 0.0f, 1.0f));
#endif	
		//넣겠다.
		m_OriginAtList.push_back(pCameraPoint);

		m_isLineUpdate = true;

		ENDINSTANCE;
	}
}



_bool CCutScene_Camera_Tool::Select_Eye_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	_bool isSelect{ false };

	if (CUTSCENE_EYE == m_iEyeOrAt)
	{
		//아무의미 없음 매개변수 채우기 용
		_float fDistanceTemp{};

		for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter != m_CameraInfoList.end(); iter++)
		{
			if ((*iter).pEyePoint->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
			{
				m_pCurrentPoint = (*iter).pEyePoint;

				//이터레이터 갱신
				m_CurrentIterater = iter;

				//색 변경
				Change_EyePoint(m_CurrentIterater);

				isSelect = true;
			}
		}
	}

	return isSelect;
}

_bool CCutScene_Camera_Tool::Select_OriginAt_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	_bool isSelect{ false };

	if (CUTSCENE_AT == m_iEyeOrAt)
	{
		//아무의미 없음 매개변수 채우기 용
		_float fDistanceTemp{};

		for (auto& iter : m_OriginAtList)
		{
			if (iter->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
			{
				m_pCurrentPoint = iter;

				//반복자 초기화로 값 수정 막기
				m_CurrentIterater = m_CameraInfoList.end();

				//변경
				Change_AtPoint(m_pCurrentPoint);

				isSelect = true;
			}
		}
	}

	return isSelect;
}

void CCutScene_Camera_Tool::Create_Tick(_float4 _vRayPos, _float4 _vRayDir)
{
	Create_CameraInfo(_vRayPos, _vRayDir);
	Create_OriginAt(_vRayPos, _vRayDir);
}

void CCutScene_Camera_Tool::Delete_Eye_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	//선택 하고
	if (CUTSCENE_EYE == m_iEyeOrAt && Select_Eye_Point(_vRayPos, _vRayDir))
	{
		//선택된 반복자의 원소 카운트 감소 후
		Safe_Release((*m_CurrentIterater).pEyePoint);
		Safe_Release((*m_CurrentIterater).pAtPoint);
		//삭제
		m_CameraInfoList.erase(m_CurrentIterater);

		//반복자 초기화
		m_CurrentIterater = m_CameraInfoList.end();

		//선택된 객체 초기화
		m_pCurrentPoint = nullptr;

		m_isLineUpdate = true;

		m_iterCurEyePoint = m_CameraInfoList.end();;
	}
}

void CCutScene_Camera_Tool::Delete_OriginAt_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	//선택 하고
	if (CUTSCENE_AT == m_iEyeOrAt && Select_OriginAt_Point(_vRayPos, _vRayDir))
	{
		//인포 리스트 전체 순회
		for (
			list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter != m_CameraInfoList.end();
			)
		{
			//같은 At을 가지고 있다면
			if ((*iter).pAtPoint == m_pCurrentPoint)
			{
				//선택된 반복자의 원소 카운트 감소 후
				Safe_Release((*iter).pEyePoint);
				Safe_Release((*iter).pAtPoint);
				//삭제
				iter = m_CameraInfoList.erase(iter);
			}
			//같은게 없다면 반복자 증가
			else
			{
				++iter;
			}
		}
		//오리진 리스트 전체 순회
		for (list<CCamera_Point*>::iterator iter = m_OriginAtList.begin(); iter != m_OriginAtList.end();)
		{
			//같다면
			if ((*iter) == m_pCurrentPoint)
			{
				//레퍼런스 카운트 감소 후
				Safe_Release(*iter);
				//삭제
				iter = m_OriginAtList.erase(iter);
			}
			//같은게 없다면 반복자 증가
			else
			{
				++iter;
			}
		}

		//반복자 초기화
		m_CurrentIterater = m_CameraInfoList.end();

		//선택된 객체 초기화
		m_pCurrentPoint = nullptr;
		m_pAtCurrentPoint = nullptr;

		m_isLineUpdate = true;
	}
}

void CCutScene_Camera_Tool::Select_Tick(_float4 _vRayPos, _float4 _vRayDir)
{
	//CUTSCENE_EYE == m_iEyeOrAt
	Select_Eye_Point(_vRayPos, _vRayDir);
	//CUTSCENE_AT == m_iEyeOrAt
	Select_OriginAt_Point(_vRayPos, _vRayDir);
}

void CCutScene_Camera_Tool::Fix_CameraPointInfo(_float4 _vRayPos, _float4 _vRayDir)
{
	//포인트 위치 수정
	Fix_Point();

	//포지션 표시
	{
		_float vCameraPointPosition[4]{};
		if (nullptr != m_pCurrentPoint)
		{
			vCameraPointPosition[0] = m_pCurrentPoint->Get_Position().x;
			vCameraPointPosition[1] = m_pCurrentPoint->Get_Position().y;
			vCameraPointPosition[2] = m_pCurrentPoint->Get_Position().z;
			vCameraPointPosition[3] = m_pCurrentPoint->Get_Position().w;
		}
		if (ImGui::DragFloat3("Point Position", vCameraPointPosition, 0.01f, 0.0f, 1000.0f))
		{
			if (nullptr != m_pCurrentPoint)
			{
				_float4 vPosition{};

				vPosition.x = vCameraPointPosition[0];
				vPosition.y = vCameraPointPosition[1];
				vPosition.z = vCameraPointPosition[2];
				vPosition.w = vCameraPointPosition[3];

				m_pCurrentPoint->Set_Position(vPosition);
			}
		}
	}

	//나머지 원소들 수정 Eye일 경우에만
	if (CUTSCENE_EYE == m_iEyeOrAt && m_CameraInfoList.end() != m_CurrentIterater)
	{
		ImGui::InputFloat("Duration", &(*m_CurrentIterater).fDuration, 0.1f, 0.1f);

		ImGui::Checkbox("isLerp", &(*m_CurrentIterater).isLerp);
	}
}

void CCutScene_Camera_Tool::Delete_Tick(_float4 _vRayPos, _float4 _vRayDir)
{
	//CUTSCENE_EYE == m_iEyeOrAt
	Delete_Eye_Point(_vRayPos, _vRayDir);
	//CUTSCENE_AT == m_iEyeOrAt
	Delete_OriginAt_Point(_vRayPos, _vRayDir);
}

void CCutScene_Camera_Tool::Clear_DoubleCheck()
{
	if (true == m_isClearDoubleCheck)
	{
		ImGui::Begin("Are you sure clear list");
		if (ImGui::Button("YES"))
		{
			Clear_CutSceneList();
			m_isClearDoubleCheck = false;

			m_isLineUpdate = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("NO"))
		{
			m_isClearDoubleCheck = false;
		}
		ImGui::End();
	}
}

HRESULT CCutScene_Camera_Tool::Save_CutSceneInfo(const _tchar* _wszFilePath)
{
	HANDLE hFile = CreateFile(_wszFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		MSG_BOX("CutScene Save Fail");
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD dwByte = { 0 };

	_uint iListSize = m_CameraInfoList.size();

	//사이즈 저장
	WriteFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

	//사이즈 만큼 반복
	CUTSCENECAMERADESC CutSceneCameraDesc;

	for (auto& iter : m_CameraInfoList)
	{
		CutSceneCameraDesc.vEye = iter.pEyePoint->Get_Position();
		CutSceneCameraDesc.vAt = iter.pAtPoint->Get_Position();
		CutSceneCameraDesc.isLerp = iter.isLerp;
		CutSceneCameraDesc.fDuration = iter.fDuration;

		WriteFile(hFile, &CutSceneCameraDesc, sizeof(CutSceneCameraDesc), &dwByte, nullptr);
	}

	MSG_BOX("CutScene Save Success");
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CCutScene_Camera_Tool::Load_CutSceneInfo(const _tchar* _wszFilePath)
{
	HANDLE hFile = CreateFile(_wszFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
	{
		MSG_BOX("CutScene Load Fail");
		CloseHandle(hFile);
		return E_FAIL;
	}

	//초기화
	Clear_CutSceneList();

	DWORD dwByte = { 0 };

	//사이즈 받아주고
	_uint iListSize{};

	//사이즈
	ReadFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

	//읽을 구조체
	CUTSCENECAMERADESC CutSceneCameraDesc;

	//담을 구조체 
	CAMERAPOINTINFODESC CameraPointInfoDesc;
	BEGININSTANCE;

	//사이즈 만큼 읽고 생성
	for (size_t i = 0; i < iListSize; i++)
	{
		ReadFile(hFile, &CutSceneCameraDesc, sizeof(CutSceneCameraDesc), &dwByte, nullptr);

		CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

		//Eye는 그냥 생성
		CameraPointDesc.vPosition = CutSceneCameraDesc.vEye;

		CCamera_Point* pCameraPoint =
			dynamic_cast<CCamera_Point*>(
				pGameInstance->Clone_Component(LEVEL_TOOL,
					TEXT("Prototype_GameObject_Camera_Point"),
					&CameraPointDesc));

		//생성 실패 탈출
		if (nullptr == pCameraPoint)
		{
			MSG_BOX("Camera_Point Create Failed");

			ENDINSTANCE;

			CloseHandle(hFile);

			return E_FAIL;
		}

		//Eye에 추가
#ifdef _DEBUG
		pCameraPoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif
		CameraPointInfoDesc.pEyePoint = pCameraPoint;


		//At은 전부랑 위치 비교해라
		CameraPointDesc.vPosition = CutSceneCameraDesc.vAt;

		pCameraPoint =
			dynamic_cast<CCamera_Point*>(
				pGameInstance->Clone_Component(LEVEL_TOOL,
					TEXT("Prototype_GameObject_Camera_Point"),
					&CameraPointDesc));

		//생성 실패 탈출
		if (nullptr == pCameraPoint)
		{
			MSG_BOX("Camera_Point Create Failed");

			ENDINSTANCE;

			CloseHandle(hFile);

			return E_FAIL;
		}
		//비교
		_bool isInOrigin{ false };
		for (auto& iter : m_OriginAtList)
		{
			//같은게 있다면 
			if (iter->Get_Position() == pCameraPoint->Get_Position())
			{
				//새로 생성된 객체는 지워주고
				Safe_Release(pCameraPoint);
				//Origin의 원소를 넣어줘라
				CameraPointInfoDesc.pAtPoint = iter;
				//레퍼런스 카운트 증가
				Safe_AddRef(iter);
				//Origin에 있더라
				isInOrigin = true;
			}
		}
		//Origin에 없던데?
		if (false == isInOrigin)
		{
#ifdef _DEBUG
			//색 바꾸고
			pCameraPoint->Set_Collider_Color(_float4(1.0f, 0.0f, 0.0f, 1.0f));
#endif
			//Origin에도 넣어줘라
			CameraPointInfoDesc.pAtPoint = pCameraPoint;
			m_OriginAtList.push_back(pCameraPoint);
		}

		//Lerp
		CameraPointInfoDesc.isLerp = CutSceneCameraDesc.isLerp;

		//Duration
		CameraPointInfoDesc.fDuration = CutSceneCameraDesc.fDuration;

		//리스트에 넣어준다.
		m_CameraInfoList.push_back(CameraPointInfoDesc);
	}

	MSG_BOX("CutScene Load Success");
	CloseHandle(hFile);

	ENDINSTANCE;

	return S_OK;
}

void CCutScene_Camera_Tool::Save_And_Load()
{
	//스피드 조정

	//스피드 러프 할지 말지




	//화면 띄우기
	if (ImGui::Button("Save CutScene"))
		ImGuiFileDialog::Instance()->
		//			이 창의 고유 키값		창 이름		기본 형식자	기본 경로
		OpenDialog("CutSceneSaveDialog", "Save File", ".cut", "../../Resources/GameData/CutScene/");

	// 화면 띄워지면
	if (ImGuiFileDialog::Instance()->Display("CutSceneSaveDialog"))
	{
		// OK누르면
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			//세트 사이즈가 0이 아니라면
			if (false == m_CameraInfoList.empty())
			{
				//파일 경로
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

				//_tchar로 가공
				_tchar wszFilePath[MAX_PATH]{};
				CharToWChar(filePathName.c_str(), wszFilePath);

				//저장 함수 실행
				Save_CutSceneInfo(wszFilePath);

				m_isLineUpdate = true;

#ifdef _DEBUG

				Line_Update();

#endif
			}
		}
		// 닫는다.
		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::SameLine();

	//화면 띄우기
	if (ImGui::Button("Load CutScene"))
		ImGuiFileDialog::Instance()->
		//			이 창의 고유 키값	  창 이름	  기본 형식자	기본 경로
		OpenDialog("CutSceneLoadDialog", "Load File", ".cut", "../../Resources/GameData/CutScene/");

	// 화면 띄워지면
	if (ImGuiFileDialog::Instance()->Display("CutSceneLoadDialog"))
	{
		// OK누르면
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			//파일 경로
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			//_tchar로 가공
			_tchar wszFilePath[MAX_PATH]{};
			CharToWChar(filePathName.c_str(), wszFilePath);

			//파일이 없다면
			if (false == fs::exists(filePathName))
			{
				// 닫는다.
				ImGuiFileDialog::Instance()->Close();
				return;
			}

			//불로오기 함수 실행
			Load_CutSceneInfo(wszFilePath);

			m_isLineUpdate = true;
		}
		// 닫는다.
		ImGuiFileDialog::Instance()->Close();
	}
}

void CCutScene_Camera_Tool::Play_CutScene()
{
	if (ImGui::Button("Play_CutScene"))
	{
		for (auto& iter : m_CameraInfoList)
		{
			Add_CutScene(iter);
		}
	}
}

void CCutScene_Camera_Tool::Add_CutScene(const CAMERAPOINTINFODESC& _CameraPointInfoDesc)
{
	BEGININSTANCE;

	CUTSCENECAMERADESC CutSceneCameraDesc{};

	CutSceneCameraDesc.vEye = _CameraPointInfoDesc.pEyePoint->Get_Position();
	CutSceneCameraDesc.vAt = _CameraPointInfoDesc.pAtPoint->Get_Position();
	CutSceneCameraDesc.isLerp = _CameraPointInfoDesc.isLerp;
	CutSceneCameraDesc.fDuration = _CameraPointInfoDesc.fDuration;

	pGameInstance->Add_CutScene(CutSceneCameraDesc);

	ENDINSTANCE;
}

#ifdef _DEBUG

HRESULT CCutScene_Camera_Tool::Ready_Line()
{
	BEGININSTANCE;

	m_pEyeLine =
		dynamic_cast<CCamera_Line*>(
			pGameInstance->Clone_Component(LEVEL_TOOL,
				TEXT("Prototype_GameObject_Camera_Line")));

	m_pAtLine =
		dynamic_cast<CCamera_Line*>(
			pGameInstance->Clone_Component(LEVEL_TOOL,
				TEXT("Prototype_GameObject_Camera_Line")));

	m_pLookLine =
		dynamic_cast<CCamera_Line*>(
			pGameInstance->Clone_Component(LEVEL_TOOL,
				TEXT("Prototype_GameObject_Camera_Line")));

	//생성 실패 탈출
	if (nullptr == m_pEyeLine)
	{
		MSG_BOX("Camera Eye Line Create Failed");

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CCutScene_Camera_Tool::Line_Update()
{
	//그냥 지우고
	m_pEyeLine->LineBufferClear();
	m_pAtLine->LineBufferClear();
	m_pLookLine->LineBufferClear();


	//사이즈가 있다면
	if (1 < m_CameraInfoList.size())
	{
		EyeLine_Update();
		AtLine_Update();
		LookLine_Update();
	}

	return S_OK;
}

HRESULT CCutScene_Camera_Tool::EyeLine_Update()
{
	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	LineDesc.iNum = ((m_CameraInfoList.size() - 1) * 10);

	//라인 포인트
	vector<_float3> Lines;

	Lines.resize(LineDesc.iNum * 2);

	list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();

	//인덱스
	_uint iIndexCount{ 0 };

	for (size_t i = 0; i < m_CameraInfoList.size(); i++)
	{
		//첫번째와 마지막 이터레이터는 선형
		//첫번째		//두번째 거리 저장
		if (iter == m_CameraInfoList.begin())
		{
			//두번째 이터레이터와
			list<CAMERAPOINTINFODESC>::iterator nextIter = ++m_CameraInfoList.begin();

			//첫번째 거리 0 및 러프 끔
			(*iter).isLerp = false;
			(*iter).fSplineLength = 0.0f;
			//두번째 거리 = 두번째 - 첫번째의 거리
			(*nextIter).fSplineLength =
				XMVectorGetX(
					XMVector3Length(
						(*iter).pEyePoint->Get_Position().xyz() -
						(*nextIter).pEyePoint->Get_Position().xyz()));


			//선형으로 추가
			for (size_t i = 0; i < 10; i++)
			{
				Lines[iIndexCount] = (
					XMVectorLerp(
						(*iter).pEyePoint->Get_Position(),
						(*nextIter).pEyePoint->Get_Position(),
						i * 0.1f));

				++iIndexCount;

				Lines[iIndexCount] = (
					XMVectorLerp(
						(*iter).pEyePoint->Get_Position(),
						(*nextIter).pEyePoint->Get_Position(),
						(i + 1) * 0.1f));

				++iIndexCount;
			}
		}
		//끝에서 2번째 오면 마지막 까지 선형
		else if (iter == ----m_CameraInfoList.end())
		{
			//마지막 이터레이터와
			list<CAMERAPOINTINFODESC>::iterator NextIter = --m_CameraInfoList.end();

			//마지막 거리
			(*NextIter).fSplineLength =
				XMVectorGetX(
					XMVector3Length(
						(*iter).pEyePoint->Get_Position().xyz() -
						(*NextIter).pEyePoint->Get_Position().xyz()));


			//선형으로 추가
			for (size_t i = 0; i < 10; i++)
			{
				Lines[iIndexCount] = (
					XMVectorLerp(
						(*iter).pEyePoint->Get_Position(),
						(*NextIter).pEyePoint->Get_Position(),
						i * 0.1f));

				++iIndexCount;

				Lines[iIndexCount] = (
					XMVectorLerp(
						(*iter).pEyePoint->Get_Position(),
						(*NextIter).pEyePoint->Get_Position(),
						(i + 1) * 0.1f));

				++iIndexCount;
			}
		}
		//마지막 무시
		else if (iter == --m_CameraInfoList.end())
		{
		}
		//나머지
		else
		{
			//1
			list<CAMERAPOINTINFODESC>::iterator Ite1 = --iter;
			//2
			++iter;
			//3
			list<CAMERAPOINTINFODESC>::iterator Ite3 = ++iter;
			//4
			list<CAMERAPOINTINFODESC>::iterator Ite4 = ++iter;

			//2
			----iter;

			(*Ite3).fSplineLength = 0.0f;

			for (size_t i = 0; i < 10; i++)
			{
				_float3 vStartPos{}, vEndPos{};

				vStartPos = Lines[iIndexCount] = (
					XMVectorCatmullRom(
						(*Ite1).pEyePoint->Get_Position(),
						(*iter).pEyePoint->Get_Position(),
						(*Ite3).pEyePoint->Get_Position(),
						(*Ite4).pEyePoint->Get_Position(),
						i * 0.1f));

				++iIndexCount;

				vEndPos = Lines[iIndexCount] = (
					XMVectorCatmullRom(
						(*Ite1).pEyePoint->Get_Position(),
						(*iter).pEyePoint->Get_Position(),
						(*Ite3).pEyePoint->Get_Position(),
						(*Ite4).pEyePoint->Get_Position(),
						(i + 1) * 0.1f));

				++iIndexCount;

				//스플라인 보간 누적 10번
				(*Ite3).fSplineLength += XMVectorGetX(
					XMVector3Length(vStartPos - vEndPos));
			}
		}

		//증가
		++iter;
	}

	LineDesc.pLines = Lines.data();

	if (0 < LineDesc.iNum &&
		nullptr != LineDesc.pLines)
	{
		if (FAILED(m_pEyeLine->LineBufferInitialize(&LineDesc)))
		{
			MSG_BOX("CameraLine Initailize Failed");
			return E_FAIL;
		}
		m_pEyeLine->Set_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
	}

	return S_OK;
}

HRESULT CCutScene_Camera_Tool::AtLine_Update()
{
	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	LineDesc.iNum = ((m_CameraInfoList.size() - 1) * 10);

	//라인 포인트
	vector<_float3> Lines;

	Lines.resize(LineDesc.iNum * 2);

	list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();

	//인덱스
	_uint iIndexCount{ 0 };

	for (size_t i = 0; i < m_CameraInfoList.size(); i++)
	{
		//첫번째와 마지막 이터레이터는 선형
		//첫번째
		if (iter == m_CameraInfoList.begin())
		{
			//두번째 이터레이터와
			list<CAMERAPOINTINFODESC>::iterator nextIter = ++m_CameraInfoList.begin();

			//선형으로 추가
			for (size_t i = 0; i < 10; i++)
			{
				Lines[iIndexCount] = (
					XMVectorLerp(
						(*iter).pAtPoint->Get_Position(),
						(*nextIter).pAtPoint->Get_Position(),
						i * 0.1f));

				++iIndexCount;

				Lines[iIndexCount] = (
					XMVectorLerp(
						(*iter).pAtPoint->Get_Position(),
						(*nextIter).pAtPoint->Get_Position(),
						(i + 1) * 0.1f));

				++iIndexCount;
			}
		}
		//끝에서 2번째 오면 마지막 까지 선형
		else if (iter == ----m_CameraInfoList.end())
		{
			//마지막 이터레이터와
			list<CAMERAPOINTINFODESC>::iterator NextIter = --m_CameraInfoList.end();

			//선형으로 추가
			for (size_t i = 0; i < 10; i++)
			{
				Lines[iIndexCount] = (
					XMVectorLerp(
						(*iter).pAtPoint->Get_Position(),
						(*NextIter).pAtPoint->Get_Position(),
						i * 0.1f));

				++iIndexCount;

				Lines[iIndexCount] = (
					XMVectorLerp(
						(*iter).pAtPoint->Get_Position(),
						(*NextIter).pAtPoint->Get_Position(),
						(i + 1) * 0.1f));

				++iIndexCount;
			}
		}
		//마지막 무시
		else if (iter == --m_CameraInfoList.end())
		{
		}
		//나머지
		else
		{
			//1
			list<CAMERAPOINTINFODESC>::iterator Ite1 = --iter;
			//2
			++iter;
			//3
			list<CAMERAPOINTINFODESC>::iterator Ite3 = ++iter;
			//4
			list<CAMERAPOINTINFODESC>::iterator Ite4 = ++iter;

			//2
			----iter;

			for (size_t i = 0; i < 10; i++)
			{
				Lines[iIndexCount] = (
					XMVectorCatmullRom(
						(*Ite1).pAtPoint->Get_Position(),
						(*iter).pAtPoint->Get_Position(),
						(*Ite3).pAtPoint->Get_Position(),
						(*Ite4).pAtPoint->Get_Position(),
						i * 0.1f));

				++iIndexCount;

				Lines[iIndexCount] = (
					XMVectorCatmullRom(
						(*Ite1).pAtPoint->Get_Position(),
						(*iter).pAtPoint->Get_Position(),
						(*Ite3).pAtPoint->Get_Position(),
						(*Ite4).pAtPoint->Get_Position(),
						(i + 1) * 0.1f));

				++iIndexCount;
			}
		}

		//증가
		++iter;
	}

	LineDesc.pLines = Lines.data();

	if (0 < LineDesc.iNum &&
		nullptr != LineDesc.pLines)
	{
		if (FAILED(m_pAtLine->LineBufferInitialize(&LineDesc)))
		{
			MSG_BOX("CameraLine Initailize Failed");
			return E_FAIL;
		}
		m_pAtLine->Set_Color(_float4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	return S_OK;
}

HRESULT CCutScene_Camera_Tool::LookLine_Update()
{
	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	LineDesc.iNum = ((m_CameraInfoList.size()));

	//라인 포인트
	vector<_float3> Lines;

	Lines.resize(LineDesc.iNum * 2);

	list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();

	//인덱스
	_uint iIndexCount{ 0 };

	for (size_t i = 0; i < m_CameraInfoList.size(); i++)
	{
		//두번째 이터레이터와
		list<CAMERAPOINTINFODESC>::iterator nextIter = ++m_CameraInfoList.begin();

		//Eye부터
		Lines[iIndexCount] = (*iter).pEyePoint->Get_Position().xyz();

		++iIndexCount;

		//At까지
		Lines[iIndexCount] = (*iter).pAtPoint->Get_Position().xyz();

		++iIndexCount;

		//증가
		++iter;
	}

	LineDesc.pLines = Lines.data();

	if (0 < LineDesc.iNum &&
		nullptr != LineDesc.pLines)
	{
		if (FAILED(m_pLookLine->LineBufferInitialize(&LineDesc)))
		{
			MSG_BOX("CameraLine Initailize Failed");
			return E_FAIL;
		}
		m_pLookLine->Set_Color(_float4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	return S_OK;
}
#endif

void CCutScene_Camera_Tool::Select_Start_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	//충돌 한것을 시작 포인트로 만들고 색을 채운다.
		
	//아무의미 없음 매개변수 채우기 용
	_float fDistanceTemp{};

	for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
		iter != m_CameraInfoList.end(); iter++)
	{
		//충돌 시 
		if ((*iter).pEyePoint->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
		{
			//만약에 시작 포인트가 있었다면 
			if (m_iterStart != m_CameraInfoList.end())
			{
#ifdef _DEBUG
				//기존 시작 포인트의 색을 돌려준다. 
				(*m_iterStart).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif // _DEBUG
			}

			//시작 점에 대입 한다. 
			m_iterStart = iter;
		}
	}
}

void CCutScene_Camera_Tool::Select_End_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	//충돌 한것을 끝 포인트로 만들고 색을 채운다.
	
	//아무의미 없음 매개변수 채우기 용
	_float fDistanceTemp{};

	for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
		iter != m_CameraInfoList.end(); iter++)
	{
		//충돌 시 
		if ((*iter).pEyePoint->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
		{
			//만약에 끝 포인트가 있었다면 
			if (m_iterEnd != m_CameraInfoList.end())
			{
#ifdef _DEBUG
				//기존 끝 포인트의 색을 돌려준다. 
				(*m_iterEnd).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif
			}

			//끝 점에 대입 한다. 
			m_iterEnd = iter;
		}
	}
	
}

void CCutScene_Camera_Tool::Swap_Section_Point()
{
	//둘 중 하나라도 비었다면 탈출
	if (m_CameraInfoList.end() == m_iterStart || m_CameraInfoList.end() == m_iterEnd)
	{
		return;
	}
	//둘이 같다면 둘다 취소 한다. 
	if (m_iterEnd == m_iterStart)
	{
		MSG_BOX("Same Point; Select Point Again;");
#ifdef _DEBUG
		//둘다 색 돌려주고
		(*m_iterStart).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
		(*m_iterEnd).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif
		//초기화
		m_iterStart = m_iterEnd = m_CameraInfoList.end();

		return;
	}

	//이전에 start가 있었니?
	_bool isStart{ false };

	//전체 순회(둘중 하나만 만나도 탈출)
	for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
		iter != m_CameraInfoList.end(); ++iter)
	{
		if (iter == m_iterStart)
		{
			isStart = true;
			break;
		}
		if (iter == m_iterEnd)
		{
			isStart = false;
			break;
		}
	}

	//시작점이 먼저라면 
	if (true == isStart)
	{
		//아무것도 하지 않는다.
	}
	//끝점이 먼저라면
	else if (false == isStart)
	{
		//스왑한다.
		list<CAMERAPOINTINFODESC>::iterator iterTemp = m_CameraInfoList.end();
		iterTemp = m_iterStart;
		m_iterStart = m_iterEnd;
		m_iterEnd = iterTemp;
	}
#ifdef _DEBUG
	//이후 색상 입힌다.
	(*m_iterStart).pEyePoint->Set_Collider_Color(_float4(1.0f, 1.0f, 1.0f, 1.0f));
	(*m_iterEnd).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 0.0f, 1.0f));
#endif // _DEBUG
}

void CCutScene_Camera_Tool::Section_Point_Update(_float4 _vRayPos, _float4 _vRayDir)
{
	//스타트 앤드 선택
	ImGui::Text("Section Select : "); ImGui::SameLine();

	ImGui::RadioButton("Start", &m_iSectionRadio, 0); ImGui::SameLine();
	ImGui::RadioButton("End", &m_iSectionRadio, 1);

	BEGININSTANCE;

	//클릭시 라디오로 나눔
	if (pGameInstance->Get_DIMouseState(
		CInput_Device::DIMK_LBUTTON,
		CInput_Device::KEY_DOWN) &&
		pGameInstance->Get_DIMouseState(
			CInput_Device::DIMK_RBUTTON,
			CInput_Device::KEY_PRESSING))
	{
		//라디오로 나눔 선택 후
		switch (m_iSectionRadio)
		{
		case Tool::CCutScene_Camera_Tool::SECTION_START:
		{
			Select_Start_Point(_vRayPos, _vRayDir);
		}
			break;
		case Tool::CCutScene_Camera_Tool::SECTION_END:
		{
			Select_End_Point(_vRayPos, _vRayDir);
		}
			break;
		default:
			break;
		}

		//순서 정리 색 입힘
		Swap_Section_Point();
	}

	//구간 재생 시간
	ImGui::DragFloat("Section_Play_Time", &m_fSectionPlayTime, 0.1f, 0.1f, 1000.0f);
	//시간 적용 (버튼 포함)
	Set_Section_Point_Duration();

	//재생
	Play_Section();

	ENDINSTANCE;
}

void CCutScene_Camera_Tool::Set_Section_Point_Duration()
{
	if (ImGui::Button("Set_Duration"))
	{
		if (0.0f >= m_fSectionPlayTime)
		{
			MSG_BOX("Section Time error");
			return;
		}

		if (m_CameraInfoList.end() == m_iterStart ||
			m_CameraInfoList.end() == m_iterEnd)
		{
			MSG_BOX("you do not select start soint and end Point");
			return;
		}


		//시작임?
		_bool isStart{ false };

		//이구간의 총길이를 알아야 한다.
		_float fSectionSTotalLength{ 0.0f };

		for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter != m_CameraInfoList.end(); ++iter)
		{
			//시작점이후의 점의 듀레이션 부터
			if (iter == m_iterStart)
			{
				isStart = true;
			}
			//끝점 까지 작업 
			else if (iter == m_iterEnd)
			{
				fSectionSTotalLength += (*iter).fSplineLength;

				isStart = false;

				break;
			}
			else if (true == isStart)
			{
				fSectionSTotalLength += (*iter).fSplineLength;
			}
		}

		if (0.0f == fSectionSTotalLength)
		{
			MSG_BOX("Section Total Length is Zero");
			return;
		}


		//구해진 총길이로 듀레이션 결정
		for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter != m_CameraInfoList.end(); ++iter)
		{
			//시작점이후의 점의 듀레이션 부터
			if (iter == m_iterStart)
			{
				isStart = true;
			}
			//끝점 까지 작업 
			else if (iter == m_iterEnd)
			{
				(*iter).fDuration = (*iter).fSplineLength / fSectionSTotalLength * m_fSectionPlayTime;

				isStart = false;

				break;
			}
			else if (true == isStart)
			{
				(*iter).fDuration = (*iter).fSplineLength / fSectionSTotalLength * m_fSectionPlayTime;
			}
		}
	}
}



void CCutScene_Camera_Tool::List_Tick(_float _TimeDelta)
{
	for (auto& iter : m_CameraInfoList)
	{
		iter.pEyePoint->Tick(_TimeDelta);
	}
	for (auto& iter : m_OriginAtList)
	{
		iter->Tick(_TimeDelta);
	}
}

void CCutScene_Camera_Tool::Play_Section()
{

	if (ImGui::Button("Play_Section"))
	{
		//둘다 있어야 함
		if (m_CameraInfoList.end() == m_iterStart ||
			m_CameraInfoList.end() == m_iterEnd)
		{
			MSG_BOX("you do not select start soint and end Point");
			return;
		}
		if (m_iterStart == m_iterEnd)
		{
			MSG_BOX("Section Point Same Error");
			return;
		}

		//시작임?
		_bool isStart{ false };

		for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter != m_CameraInfoList.end(); ++iter)
		{
			//시작점이후 부터 시작 
			if (iter == m_iterStart)
			{
				isStart = true;
				//이전 몇개를 더 넣어야 자연스러울듯...처음이면 안넣어도 되고...
				BEGININSTANCE;

				CUTSCENECAMERADESC CutSceneCameraDesc{};

				//시작이 아니라면
				if (iter != m_CameraInfoList.begin())
				{
					//이전 것 1개
					CutSceneCameraDesc.vEye = (*--iter).pEyePoint->Get_Position();
					CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
					CutSceneCameraDesc.isLerp = false;
					CutSceneCameraDesc.fDuration = 0.0f;

					pGameInstance->Add_CutScene(CutSceneCameraDesc);

					//시작 것 1개
					CutSceneCameraDesc.vEye = (*++iter).pEyePoint->Get_Position();
					CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
					CutSceneCameraDesc.isLerp = false;
					CutSceneCameraDesc.fDuration = 0.0f;

					pGameInstance->Add_CutScene(CutSceneCameraDesc);
				}
				//시작이라면
				else if (iter == m_CameraInfoList.begin())
				{
					//지금 것 1개
					CutSceneCameraDesc.vEye = (*iter).pEyePoint->Get_Position();
					CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
					CutSceneCameraDesc.isLerp = (*iter).isLerp;
					CutSceneCameraDesc.fDuration = (*iter).fDuration;

					pGameInstance->Add_CutScene(CutSceneCameraDesc);
				}

				ENDINSTANCE;

			}
			//끝점 까지 작업 
			else if (iter == m_iterEnd)
			{
				//이후 몇개를 더 넣어야 자연스러움...마지막이면 안넣어도 되고...

				BEGININSTANCE;

				CUTSCENECAMERADESC CutSceneCameraDesc{};

				//지금 것 1개
				CutSceneCameraDesc.vEye = (*iter).pEyePoint->Get_Position();
				CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
				CutSceneCameraDesc.isLerp = (*iter).isLerp;
				CutSceneCameraDesc.fDuration = (*iter).fDuration;

				pGameInstance->Add_CutScene(CutSceneCameraDesc);

				//마지막이 아니라면
				if (iter != --m_CameraInfoList.end())
				{
					//마지막 것 1개
					CutSceneCameraDesc.vEye = (*++iter).pEyePoint->Get_Position();
					CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
					CutSceneCameraDesc.isLerp = false;
					CutSceneCameraDesc.fDuration = 0.0f;

					pGameInstance->Add_CutScene(CutSceneCameraDesc);
				}

				ENDINSTANCE;

				break;
			}
			else if (true == isStart)
			{
				Add_CutScene(*iter);
			}
		}
	}
}

void CCutScene_Camera_Tool::Set_Position_CurrentPoint()
{
	ImGui::Text("\"T\" : Set position to selected point");
	BEGININSTANCE;

	if (nullptr != m_pCurrentPoint && pGameInstance->Get_DIKeyState(DIKEYBOARD_T, CInput_Device::KEY_DOWN))
	{
		dynamic_cast<CMain_Camera*>(pGameInstance->Find_Camera(TEXT("Main_Camera")))->Set_Position(m_pCurrentPoint->Get_Position().xyz());
	}

	ENDINSTANCE;
}

void CCutScene_Camera_Tool::Stop_CutScene()
{
	BEGININSTANCE;

	if (ImGui::Button("Stop_CurScene"))
	{
		pGameInstance->Stop_CutScene();
	}

	ENDINSTANCE;
}

CCutScene_Camera_Tool* CCutScene_Camera_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
	CCutScene_Camera_Tool* pInstance = New CCutScene_Camera_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed Create CutScene_Camera_Tool");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CCutScene_Camera_Tool::Free()
{
	Clear_CutSceneList();

#ifdef _DEBUG

	Safe_Release(m_pEyeLine);
	Safe_Release(m_pAtLine);
	Safe_Release(m_pLookLine);

#endif

	Safe_Release(m_pCreateGuidePoint);
}

void CCutScene_Camera_Tool::Return_RayInfo(_float4& _vRayPos, _float4& _vRayDir)
{
	BEGININSTANCE;

	pGameInstance->
		Get_WorldMouseRay(m_pContext, g_hWnd, &_vRayPos, &_vRayDir);

	ENDINSTANCE;
}

_float4 CCutScene_Camera_Tool::Point_Create_Position(_float4 _vRayPos, _float4 _vRayDir)
{
	return _vRayPos + _vRayDir * m_fDistance;
}
