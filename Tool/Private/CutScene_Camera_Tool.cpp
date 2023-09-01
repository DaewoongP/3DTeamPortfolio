#include "CutScene_Camera_Tool.h"
#include "Camera_Point.h"
#include "GameInstance.h"

CCutScene_Camera_Tool::CCutScene_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;
}

HRESULT CCutScene_Camera_Tool::Initialize(void* pArg)
{
	return S_OK;
}

void CCutScene_Camera_Tool::Tick(_float _fTimeDelta)
{
	//제목
	ImGui::Text("CutScene_Tool");

	//포인트 생성 및 삭제 
	ImGui::Text("Point : "); ImGui::SameLine();
	
	ImGui::RadioButton("Select", &m_iPointRadio, 0); ImGui::SameLine();
	ImGui::RadioButton("Create", &m_iPointRadio, 1); ImGui::SameLine();
	ImGui::RadioButton("Delete", &m_iPointRadio, 2);

	//생성 거리
	ImGui::DragFloat("Create Distance", &m_fDistance, 0.01f, 0.0f, 1000.0f);

	//eye와 at중 현제 인덱스보다 작은 것이 있을 수 있기 때문에,
	//인덱스와 포인터를 바꿔준다.
	if (ImGui::Checkbox("isEye", &m_isEye))
	{
		//eye의 경우, 세트 사이즈 보다 eye리스트가 작다면
		if (true ==  m_isEye && m_iSetSize > m_EyeList.size())
		{
			//현제 인덱스 변경 -> 리스트 사이즈 -1
			m_iCurrentIndex = m_EyeList.size() - 1;

			//현제 오브젝트 변경 -> 리스트 맨 뒤 원소
			m_pCurrentPoint = m_EyeList.back();
		}
		//작지 않다면
		else if (true == m_isEye)
		{
			//오브젝트만 바꿔준다. 
			LIST_INDEX_ACCESS_POINTER(m_EyeList, m_iCurrentIndex, m_pCurrentPoint);
		}
	}

	//미리보기
	static _float vCameraPointPosition[4];
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

	_float* fDuration{};

	LIST_INDEX_ACCESS(m_DurationList, m_iCurrentIndex, fDuration)
	
	if (nullptr != fDuration)
	{
		ImGui::InputFloat("Duration", fDuration, 0.1f, 0.1f);
	}

	_bool* isLerp{};

	LIST_INDEX_ACCESS(m_Lerplist, m_iCurrentIndex, isLerp)
	
	if (nullptr != isLerp)
	{
		ImGui::Checkbox("isLerp", isLerp);
	}

	//생성 삭제
	Point_Select_Create_Delete();
	
	for (auto& iter : m_EyeList)
	{
		iter->Tick(_fTimeDelta);
	}

	for (auto& iter : m_AtList)
	{
		iter->Tick(_fTimeDelta);
	}

}

HRESULT CCutScene_Camera_Tool::Render()
{
	return S_OK;
}

void CCutScene_Camera_Tool::Create_Point(const _float4& _vPosition)
{
	BEGININSTANCE;

	//클릭 시
	if (pGameInstance->Get_DIMouseState(
		CInput_Device::DIMK_LBUTTON,
		CInput_Device::KEY_DOWN) &&
		pGameInstance->Get_DIMouseState(
			CInput_Device::DIMK_RBUTTON,
			CInput_Device::KEY_PRESSING))
	{
		CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

		CameraPointDesc.vPosition = _vPosition;

		CCamera_Point* pCameraPoint = 
			dynamic_cast<CCamera_Point*>(
			pGameInstance->Clone_GameObject(
				TEXT("Prototype_GameObject_Camera_Point"),
				&CameraPointDesc));

		//생성 실패 탈출
		if (nullptr == pCameraPoint)
		{
			MSG_BOX("Camera_Point Create Failed");
			
			ENDINSTANCE
			
			return;
		}
		
		//EyeList에 추가
		if (true == m_isEye)
		{
			pCameraPoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
			m_EyeList.push_back(pCameraPoint);
		}
		//AtList에 추가
		else if (false == m_isEye)
		{
			pCameraPoint->Set_Collider_Color(_float4(1.0f, 0.0f, 0.0f, 1.0f));
			m_AtList.push_back(pCameraPoint);
		}

		//작은 값을 세트 값으로 정한다.
		m_iSetSize =
			m_EyeList.size() > m_AtList.size()
			?
			m_AtList.size() : m_EyeList.size();

		//큰 값을 맥스 값으로 정한다.
		m_iMaxSize =
			m_EyeList.size() > m_AtList.size()
			?
			m_EyeList.size() : m_AtList.size();

		//맥스 값 보다 작은리스트가 있다면 추가
		while (m_iMaxSize > m_Lerplist.size())
		{
			m_Lerplist.push_back(true);
		}
		while (m_iMaxSize > m_DurationList.size())
		{
			m_DurationList.push_back(1.0f);
		}
	}

	ENDINSTANCE
}

void CCutScene_Camera_Tool::Delete_Point()
{
	//선택된 객체가 있을 때
	if (Select_Point())
	{
		//세트 크기보다 크다면 (포인트 리스트 전체 순회 해서 하나만 삭제한다.)
		if (m_iSetSize - 1 < m_iCurrentIndex || m_EyeList.empty() || m_AtList.empty())
		{
			_bool isThisList{ false };

			list<CCamera_Point*>::iterator PointListIter{};

			//Eye순회
			for (list<CCamera_Point*>::iterator iter = m_EyeList.begin(); iter != m_EyeList.end(); ++iter)
			{
				//선택된 객체랑 똑같은 주소를 담은 반복자가 있다면
				if (*iter == m_pCurrentPoint)
				{
					//반복자 받고
					PointListIter = iter;
					//이 리스트다
					isThisList = true;
				}
			}
			//이 리스트임 여기서 지우셈
			if (isThisList)
			{
				Safe_Release(*PointListIter);
				m_EyeList.erase(PointListIter);

				if (true == m_EyeList.empty())
				{
					m_pCurrentPoint = nullptr;
				}
				else
				{
					m_pCurrentPoint = m_AtList.back();
				}
			}

			//재활용
			isThisList = false;

			//At순회
			for (list<CCamera_Point*>::iterator iter = m_AtList.begin(); iter != m_AtList.end(); ++iter)
			{
				//선택된 객체랑 똑같은 주소를 담은 반복자가 있다면
				if (*iter == m_pCurrentPoint)
				{
					//반복자 받고
					PointListIter = iter;
					//이 리스트다
					isThisList = true;
				}
			}
			//이 리스트임 여기서 지우셈
			if (isThisList)
			{
				Safe_Release(*PointListIter);
				m_AtList.erase(PointListIter);

				if (true == m_AtList.empty())
				{
					m_pCurrentPoint = nullptr;
				}
				else
				{
					m_pCurrentPoint = m_AtList.back();
				}
			}

			//탈출
			return;
		}
		
		//세트크기 안에 있다면 
		//세트로 지우겠다.
		else
		{
			auto EyeIter = m_EyeList.begin();
			auto AtIter = m_AtList.begin();
			auto LerpIter = m_Lerplist.begin();
			auto DurationIter = m_DurationList.begin();

			for (size_t DeleteIndex = 0; DeleteIndex < m_iCurrentIndex; DeleteIndex++)
			{
				++EyeIter;
				++AtIter;
				++LerpIter;
				++DurationIter;
			}
			
			Safe_Release(*EyeIter);
			Safe_Release(*AtIter);

			m_EyeList.erase(EyeIter);
			m_AtList.erase(AtIter);
			m_Lerplist.erase(LerpIter);
			m_DurationList.erase(DurationIter);

			//크기 감소
			if (0 != m_iMaxSize)
			{
				--m_iMaxSize;
			}
			if (0 != m_iMaxSize)
			{
				--m_iSetSize;
			}

			//현제 오브젝트 가장 맨 뒤 원소로 변경
			if (true == m_isEye)
			{
				if (true == m_EyeList.empty())
				{
					m_pCurrentPoint = nullptr;

					return;
				}

				EyeIter = m_EyeList.begin();

				//인덱스 변경
				m_iCurrentIndex = m_EyeList.size() - 1;

				for (size_t i = 0; i < m_iCurrentIndex; i++)
				{
					++EyeIter;
				}

				m_pCurrentPoint = *EyeIter;
			}
			else if (false == m_isEye)
			{
				if (true == m_AtList.empty())
				{
					m_pCurrentPoint = nullptr;

					return;
				}

				AtIter = m_AtList.begin();

				//인덱스 변경
				m_iCurrentIndex = m_AtList.size() - 1;

				for (size_t i = 0; i < m_iCurrentIndex; i++)
				{
					++AtIter;
				}

				m_pCurrentPoint = *AtIter;
			}
		}
	}
}

HRESULT CCutScene_Camera_Tool::Save_CutScene(const _tchar* _wszFileName)
{
	const _tchar wszFilePath[MAX_PATH]{TEXT("../../Resources/GameData/CutScene/")};

	lstrcmp(wszFilePath, _wszFileName);
	lstrcmp(wszFilePath, TEXT(".dat"));

	HANDLE hFile = CreateFile(_wszFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		MSG_BOX("CutScene Save Fail");
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD dwByte = { 0 };

	//인덱스 저장
	WriteFile(hFile, &m_iSetSize, sizeof(_uint), &dwByte, nullptr);

	//인덱스 만큼 반복
	CUTSCENECAMERADESC CutSceneCameraDesc;

	for (size_t i = 0; i < m_iSetSize; i++)
	{
		auto EyeIter = m_EyeList.begin();
		auto AtIter = m_AtList.begin();
		auto LerpIter = m_Lerplist.begin();
		auto DurationIter = m_DurationList.begin();
		for (size_t CurrentSize = 0; CurrentSize < i; CurrentSize++)
		{
			++EyeIter;
			++AtIter;
			++LerpIter;
			++DurationIter;
		}
		CutSceneCameraDesc.vEye = (*EyeIter)->Get_Position();
		CutSceneCameraDesc.vAt = (*AtIter)->Get_Position();
		CutSceneCameraDesc.isLerp = *LerpIter;
		CutSceneCameraDesc.fDuration = *DurationIter;

		WriteFile(hFile, &CutSceneCameraDesc, sizeof(CutSceneCameraDesc), &dwByte, nullptr);
	}

	MSG_BOX("CutScene Save Success");
	CloseHandle(hFile);

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
			vFixPosition += vCameraRight.TransNorm() * iMouseMoveX * fSpeed;
		}
		//y축 이동
		else
		{
			vFixPosition += -vCameraUp.TransNorm() * iMouseMoveY * fSpeed;
		}
	}
	//z축 이동이 있다면
	if (0 != iMouseMoveZ)
	{
		vFixPosition += vCameraLook.TransNorm() * iMouseMoveZ * fSpeed * fWheelSpeed;
	}

	m_pCurrentPoint->Set_Position(vFixPosition);
	ENDINSTANCE;

}


void CCutScene_Camera_Tool::Point_Select_Create_Delete()
{
	switch (m_iPointRadio)
	{
	case CUTSCENEPOINT_NONE:
	{
		BEGININSTANCE

		//선택
		if (pGameInstance->
			Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON, 
				CInput_Device::KEY_DOWN))
		{
			Select_Point();
		}
		//위치 수정
		else 
		{
			Fix_Point();
		}

		ENDINSTANCE
	}
	break;

	case CUTSCENEPOINT_CREATE:
	{
		_float4 vRayPos;
		_float4 vRayDir;

		BEGININSTANCE

		pGameInstance->
			Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

		//생성 위치
		_float4 vCreatePos = vRayPos + vRayDir * m_fDistance;

		Create_Point(vCreatePos);

		ENDINSTANCE
	}
	break;

	case CUTSCENEPOINT_DELETE:
	{
		BEGININSTANCE;

		if (pGameInstance->
			Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			Delete_Point();
		}

		ENDINSTANCE;
	}
	break;

	case CUTSCENEPOINT_END:
	{

	}
	break;

	default:
		break;
	}
}

_bool CCutScene_Camera_Tool::Select_Point()
{
	if (true == m_AtList.empty() && true == m_EyeList.empty())
	{
		return false;
	}

	//충돌을 했는지 체크
	_bool isIntersect{ false };

	_int iIndexCount{ -1 };

	_float4 vRayPos{};

	_float4 vRayDir{};

	BEGININSTANCE;

	pGameInstance->
		Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

	// 카메라 위치 포인트
	if (true == m_isEye)
	{
		for (auto& iter : m_EyeList)
		{
			//인덱스 증가
			++iIndexCount;

			//필요는 없는데 일단 채우기 위해
			_float pDistance{};
			//(혹시 소팅 필요하게 되면 사용하긴 할듯)

			//충돌을 했다면
			if (iter->RayIntersects(vRayPos, vRayDir, pDistance))
			{
				//수정할 오브젝트 변경
				m_pCurrentPoint = iter;

				//인덱스도 변경
				m_iCurrentIndex = iIndexCount;

				//충돌 체크
				isIntersect = true;
			}
		}
	}
	// 타겟 위치 포인트
	else if (false == m_isEye)
	{
		for (auto& iter : m_AtList)
		{
			//인덱스 증가
			++iIndexCount;

			//필요는 없는데 일단 채우기 위해
			_float pDistance{};
			//(혹시 소팅 필요하게 되면 사용하긴 할듯)

			//충돌을 했다면
			if (iter->RayIntersects(vRayPos, vRayDir, pDistance))
			{
				//수정할 오브젝트 변경
				m_pCurrentPoint = iter;

				//인덱스도 변경
				m_iCurrentIndex = iIndexCount;

				//충돌 체크
				isIntersect = true;
			}
		}
	}
	ENDINSTANCE;
	
	return isIntersect;
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

	for (auto& iter : m_EyeList)
	{
		Safe_Release(iter);
	}
	m_EyeList.clear();

	for (auto& iter : m_AtList)
	{
		Safe_Release(iter);
	}
	m_AtList.clear();

	m_Lerplist.clear();
	m_DurationList.clear();

}
