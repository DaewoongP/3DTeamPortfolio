#include "CCutScene_Camera_Tool.h"
#include "CCamera_Point.h"
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
	
	ImGui::RadioButton("None", &m_iPointRadio, 0); ImGui::SameLine();
	ImGui::RadioButton("Create", &m_iPointRadio, 1); ImGui::SameLine();
	ImGui::RadioButton("Delete", &m_iPointRadio, 2);

	//생성 거리
	ImGui::DragFloat("Create Distance", &m_fDistance, 0.01f, 0.0f, 1000.0f);

	ImGui::Checkbox("isEye", &m_isEye);

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
	BEGININSTANCE

	//클릭 시
	if (pGameInstance->Get_DIMouseState(
		CInput_Device::DIMK_LBUTTON,
		CInput_Device::KEY_DOWN))
	{
		CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

		CameraPointDesc.vPosition = _vPosition;

		CCamera_Point* pCameraPoint =
			CCamera_Point::Create(m_pDevice, m_pContext, &CameraPointDesc);

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
			m_EyeList.push_back(pCameraPoint);
		}
		//AtList에 추가
		else if (false == m_isEye)
		{
			m_AtList.push_back(pCameraPoint);
		}

		//작은 값을 최대 값으로 정한다.
		m_iMaxIndex =
			m_EyeList.size() > m_AtList.size()
			?
			m_AtList.size() : m_EyeList.size();

		//이보다 작은리스트가 있다면 추가
		while (m_iMaxIndex > m_Lerplist.size())
		{
			m_Lerplist.push_back(true);
		}
		while (m_iMaxIndex > m_DurationList.size())
		{
			m_DurationList.push_back(1.0f);
		}
	}

	ENDINSTANCE
}

void CCutScene_Camera_Tool::Delete_Point(const _uint& _iIndex)
{
	auto EyeIter = m_EyeList.begin();
	auto AtIter = m_AtList.begin();
	auto LerpIter = m_Lerplist.begin();
	auto DurationIter = m_DurationList.begin();
	
	for (size_t DeleteIndex = 0; DeleteIndex < _iIndex; DeleteIndex++)
	{
		++EyeIter;
		++AtIter;
		++LerpIter;
		++DurationIter;
	}

	m_EyeList.erase(EyeIter);
	m_AtList.erase(AtIter);
	m_Lerplist.erase(LerpIter);
	m_DurationList.erase(DurationIter);
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
	WriteFile(hFile, &m_iMaxIndex, sizeof(_uint), &dwByte, nullptr);

	//인덱스 만큼 반복
	CUTSCENECAMERADESC CutSceneCameraDesc;

	for (size_t i = 0; i < m_iMaxIndex; i++)
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

void CCutScene_Camera_Tool::Point_Create_Delete()
{
	switch (m_iPointRadio)
	{
	case CUTSCENEPOINT_NONE:
	{

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
		_float4 vRayPos;
		_float4 vRayDir;

		BEGININSTANCE

		pGameInstance->
			Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

		//레이를 순회해서 트루를 반환하는 인덱스를 저장 한다.



		if (true == m_isEye)
		{

		}
		else if (false == m_isEye)
		{

		}

		ENDINSTANCE
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
