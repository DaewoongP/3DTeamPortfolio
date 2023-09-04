#include "CutScene_Camera_Tool.h"
#include "Camera_Point.h"
#include "GameInstance.h"
#include "Main_Camera.h"

CCutScene_Camera_Tool::CCutScene_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;
}

HRESULT CCutScene_Camera_Tool::Initialize(void* pArg)
{
	BEGININSTANCE;

	CCamera::CAMERADESC CameraDesc;

	CameraDesc.m_fAspect = _float(g_iWinSizeX) / _float(g_iWinSizeY);
	CameraDesc.m_fFovY = XMConvertToRadians(90.f);
	CameraDesc.m_fNear = 0.1f;
	CameraDesc.m_fFar = 1000.f;

	CMain_Camera* pMain_Camera = CMain_Camera::Create(m_pDevice, m_pContext, &CameraDesc);
	
	pMain_Camera->Set_MoveSpeed(5.0f);

	pGameInstance->Add_MainCamera((CCamera*)pMain_Camera);

	ENDINSTANCE;
	return S_OK;
}

void CCutScene_Camera_Tool::Tick(_float _fTimeDelta)
{
	//제목
	ImGui::Text("CutScene_Tool");

	//초기화
	Clear_DoubleCheck();

	if (ImGui::Button("Clear CutScene Data"))
	{
		m_isClearDoubleCheck = true;
	}

	//포인트 생성 및 삭제 
	ImGui::Text("Point : "); ImGui::SameLine();

	ImGui::RadioButton("Select", &m_iPointRadio, 0); ImGui::SameLine();
	ImGui::RadioButton("Create", &m_iPointRadio, 1); ImGui::SameLine();
	ImGui::RadioButton("Delete", &m_iPointRadio, 2);

	//Eye 또는 At 편집
	ImGui::RadioButton("At_Point", &m_iEyeOrAt, CUTSCENE_AT); ImGui::SameLine();
	ImGui::RadioButton("Eye_Point", &m_iEyeOrAt, CUTSCENE_EYE);

	_float4 vRayPos{}, vRayDir{};

	BEGININSTANCE;

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

		//생성 거리
		ImGui::DragFloat("Create Distance", &m_fDistance, 0.01f, 0.0f, 1000.0f);

		//오른쪽 누른 상태에서 왼쪽을 누른 순간만
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
			pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_PRESSING))
		{
			Create_Tick(vRayPos, vRayDir);
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
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_END:
		break;
	default:
		break;
	}

	ENDINSTANCE;

	List_Tick(_fTimeDelta);

	Save_And_Load();

	Play_CutScene();
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
	}
	//z축 이동이 있다면
	if (0 != iMouseMoveZ)
	{
		vFixPosition += vCameraLook.TransNorm() * _float(iMouseMoveZ) * fSpeed * fWheelSpeed;
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
				pGameInstance->Clone_GameObject(
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

		m_CameraInfoList.push_back(CameraPointInfoDesc);

		//가장 마지막 이터레이터로 갱신
		m_CurrentIterater = --m_CameraInfoList.end();

		//현제 오브젝트로 바꿈
		m_pCurrentPoint = pCameraPoint;
		
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
				pGameInstance->Clone_GameObject(
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
			iter!= m_CameraInfoList.end(); iter++)
		{
			if ((*iter).pEyePoint->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
			{
				m_pCurrentPoint = (*iter).pEyePoint;
				
				//이터레이터 갱신
				m_CurrentIterater = iter;

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

	for (auto &iter : m_CameraInfoList)
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
				pGameInstance->Clone_GameObject(
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
				pGameInstance->Clone_GameObject(
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

			//불로오기 함수 실행
			Load_CutSceneInfo(wszFilePath);
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
