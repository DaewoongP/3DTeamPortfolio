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
	//����
	ImGui::Text("CutScene_Tool");

	//�ʱ�ȭ
	Clear_DoubleCheck();

	if (ImGui::Button("Clear CutScene Data"))
	{
		m_isClearDoubleCheck = true;
	}

	//����Ʈ ���� �� ���� 
	ImGui::Text("Point : "); ImGui::SameLine();

	ImGui::RadioButton("Select", &m_iPointRadio, 0); ImGui::SameLine();
	ImGui::RadioButton("Create", &m_iPointRadio, 1); ImGui::SameLine();
	ImGui::RadioButton("Delete", &m_iPointRadio, 2);

	//Eye �Ǵ� At ����
	ImGui::RadioButton("At_Point", &m_iEyeOrAt, CUTSCENE_AT); ImGui::SameLine();
	ImGui::RadioButton("Eye_Point", &m_iEyeOrAt, CUTSCENE_EYE);

	_float4 vRayPos{}, vRayDir{};

	BEGININSTANCE;

	//������ ���� ����
	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) ||
		pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING))
	{
		Return_RayInfo(vRayPos, vRayDir);
	}

	switch (m_iPointRadio)
	{
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_SELECT:
		//���� ������
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		{
			Select_Tick(vRayPos, vRayDir);
		}
		Fix_CameraPointInfo(vRayPos, vRayDir);
		break;
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_CREATE:

		//���� �Ÿ�
		ImGui::DragFloat("Create Distance", &m_fDistance, 0.01f, 0.0f, 1000.0f);

		//������ ���� ���¿��� ������ ���� ������
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
			pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_PRESSING))
		{
			Create_Tick(vRayPos, vRayDir);
		}
		break;
	case Tool::CCutScene_Camera_Tool::CUTSCENEPOINT_DELETE:
		//������ ���� ���¿��� ������ ���� ������
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

	//ī�޶� ���� ��������...
	BEGININSTANCE;

	//ī�޶� ����
	const _float4x4* pCameraWorldMatrix = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	//ī�޶� ����Ʈ
	_float3 vCameraRight = pCameraWorldMatrix->Right();
	vCameraRight.Normalize();
	//ī�޶� ��
	_float3 vCameraUp = pCameraWorldMatrix->Up();
	vCameraUp.Normalize();
	//ī�޶� ��
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
		//x�� �̵�
		if (abs(iMouseMoveX) > abs(iMouseMoveY))
		{
			vFixPosition += vCameraRight.TransNorm() * _float(iMouseMoveX) * fSpeed;
		}
		//y�� �̵�
		else
		{
			vFixPosition += -vCameraUp.TransNorm() * _float(iMouseMoveY) * fSpeed;
		}
	}
	//z�� �̵��� �ִٸ�
	if (0 != iMouseMoveZ)
	{
		vFixPosition += vCameraLook.TransNorm() * _float(iMouseMoveZ) * fSpeed * fWheelSpeed;
	}

	m_pCurrentPoint->Set_Position(vFixPosition);
	ENDINSTANCE;

}

void CCutScene_Camera_Tool::Clear_CutSceneList()
{
	//�ʱ�ȭ 
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
	//���� ������Ʈ�� �ִٸ�
	if (nullptr != m_pAtCurrentPoint)
	{
#ifdef _DEBUG
		//���� ������Ʈ �� �����ְ�
		m_pAtCurrentPoint->Set_Collider_Color(_float4(1.0f, 0.0f, 0.0f, 1.0f));
#endif // _DEBUG
	}

	//�ٲ� ����
	m_pAtCurrentPoint = _pAtPoint;

#ifdef _DEBUG
	//���� ����
	m_pAtCurrentPoint->Set_Collider_Color(_float4(1.0f, 191.0f / 255.0f, 0.0f, 1.0f));
#endif // _DEBUG

}

void CCutScene_Camera_Tool::Create_CameraInfo(_float4 _vRayPos, _float4 _vRayDir)
{
	//CameraInfoList�� �߰�
	if (CUTSCENE_EYE == m_iEyeOrAt &&
		nullptr != m_pAtCurrentPoint &&		//���õ� At�� �־�� �Ѵ�.
		false == m_OriginAtList.empty())	//Origin�� �־�� �Ѵ�.
	{
		BEGININSTANCE;

		CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

		CameraPointDesc.vPosition = Point_Create_Position(_vRayPos, _vRayDir);

		CCamera_Point* pCameraPoint =
			dynamic_cast<CCamera_Point*>(
				pGameInstance->Clone_GameObject(
					TEXT("Prototype_GameObject_Camera_Point"),
					&CameraPointDesc));

		//���� ���� Ż��
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

		//���� ������ ���ͷ����ͷ� ����
		m_CurrentIterater = --m_CameraInfoList.end();

		//���� ������Ʈ�� �ٲ�
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

		//���� ���� Ż��
		if (nullptr == pCameraPoint)
		{
			MSG_BOX("OriginAt Create Failed");

			ENDINSTANCE;

			return;
		}

#ifdef _DEBUG
		//���� �ٲ㼭 
		pCameraPoint->Set_Collider_Color(_float4(1.0f, 0.0f, 0.0f, 1.0f));
#endif	
		//�ְڴ�.
		m_OriginAtList.push_back(pCameraPoint);
		ENDINSTANCE;
	}
}



_bool CCutScene_Camera_Tool::Select_Eye_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	_bool isSelect{ false };

	if (CUTSCENE_EYE == m_iEyeOrAt)
	{
		//�ƹ��ǹ� ���� �Ű����� ä��� ��
		_float fDistanceTemp{};

		for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter!= m_CameraInfoList.end(); iter++)
		{
			if ((*iter).pEyePoint->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
			{
				m_pCurrentPoint = (*iter).pEyePoint;
				
				//���ͷ����� ����
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
		//�ƹ��ǹ� ���� �Ű����� ä��� ��
		_float fDistanceTemp{};

		for (auto& iter : m_OriginAtList)
		{
			if (iter->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
			{
				m_pCurrentPoint = iter;

				//�ݺ��� �ʱ�ȭ�� �� ���� ����
				m_CurrentIterater = m_CameraInfoList.end();

				//����
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
	//���� �ϰ�
	if (CUTSCENE_EYE == m_iEyeOrAt && Select_Eye_Point(_vRayPos, _vRayDir))
	{
		//���õ� �ݺ����� ���� ī��Ʈ ���� ��
		Safe_Release((*m_CurrentIterater).pEyePoint);
		Safe_Release((*m_CurrentIterater).pAtPoint);
		//����
		m_CameraInfoList.erase(m_CurrentIterater);

		//�ݺ��� �ʱ�ȭ
		m_CurrentIterater = m_CameraInfoList.end();

		//���õ� ��ü �ʱ�ȭ
		m_pCurrentPoint = nullptr;
	}
}

void CCutScene_Camera_Tool::Delete_OriginAt_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	//���� �ϰ�
	if (CUTSCENE_AT == m_iEyeOrAt && Select_OriginAt_Point(_vRayPos, _vRayDir))
	{
		//���� ����Ʈ ��ü ��ȸ
		for (
			list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin(); 
			iter != m_CameraInfoList.end();
			) 
		{
			//���� At�� ������ �ִٸ�
			if ((*iter).pAtPoint == m_pCurrentPoint) 
			{
				//���õ� �ݺ����� ���� ī��Ʈ ���� ��
				Safe_Release((*iter).pEyePoint);
				Safe_Release((*iter).pAtPoint);
				//����
				iter = m_CameraInfoList.erase(iter);
			}
			//������ ���ٸ� �ݺ��� ����
			else
			{
				++iter;
			}
		}
		//������ ����Ʈ ��ü ��ȸ
		for (list<CCamera_Point*>::iterator iter = m_OriginAtList.begin(); iter != m_OriginAtList.end();)
		{
			//���ٸ�
			if ((*iter) == m_pCurrentPoint)
			{
				//���۷��� ī��Ʈ ���� ��
				Safe_Release(*iter);
				//����
				iter = m_OriginAtList.erase(iter);
			}
			//������ ���ٸ� �ݺ��� ����
			else
			{
				++iter;
			}
		}

		//�ݺ��� �ʱ�ȭ
		m_CurrentIterater = m_CameraInfoList.end();

		//���õ� ��ü �ʱ�ȭ
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
	//����Ʈ ��ġ ����
	Fix_Point();

	//������ ǥ��
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

	//������ ���ҵ� ���� Eye�� ��쿡��
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
	
	//������ ����
	WriteFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

	//������ ��ŭ �ݺ�
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

	//�ʱ�ȭ
	Clear_CutSceneList();

	DWORD dwByte = { 0 };

	//������ �޾��ְ�
	_uint iListSize{};

	//������
	ReadFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

	//���� ����ü
	CUTSCENECAMERADESC CutSceneCameraDesc;

	//���� ����ü 
	CAMERAPOINTINFODESC CameraPointInfoDesc;
	BEGININSTANCE;

	//������ ��ŭ �а� ����
	for (size_t i = 0; i < iListSize; i++)
	{
		ReadFile(hFile, &CutSceneCameraDesc, sizeof(CutSceneCameraDesc), &dwByte, nullptr);

		CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

		//Eye�� �׳� ����
		CameraPointDesc.vPosition = CutSceneCameraDesc.vEye;

		CCamera_Point* pCameraPoint =
			dynamic_cast<CCamera_Point*>(
				pGameInstance->Clone_GameObject(
					TEXT("Prototype_GameObject_Camera_Point"),
					&CameraPointDesc));

		//���� ���� Ż��
		if (nullptr == pCameraPoint)
		{
			MSG_BOX("Camera_Point Create Failed");

			ENDINSTANCE;

			CloseHandle(hFile);

			return E_FAIL;
		}

		//Eye�� �߰�
#ifdef _DEBUG
		pCameraPoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif
		CameraPointInfoDesc.pEyePoint = pCameraPoint;


		//At�� ���ζ� ��ġ ���ض�
		CameraPointDesc.vPosition = CutSceneCameraDesc.vAt;

		pCameraPoint =
			dynamic_cast<CCamera_Point*>(
				pGameInstance->Clone_GameObject(
					TEXT("Prototype_GameObject_Camera_Point"),
					&CameraPointDesc));

		//���� ���� Ż��
		if (nullptr == pCameraPoint)
		{
			MSG_BOX("Camera_Point Create Failed");

			ENDINSTANCE;

			CloseHandle(hFile);

			return E_FAIL;
		}
		//��
		_bool isInOrigin{ false };
		for (auto& iter : m_OriginAtList)
		{
			//������ �ִٸ� 
			if (iter->Get_Position() == pCameraPoint->Get_Position())
			{
				//���� ������ ��ü�� �����ְ�
				Safe_Release(pCameraPoint);
				//Origin�� ���Ҹ� �־����
				CameraPointInfoDesc.pAtPoint = iter;
				//���۷��� ī��Ʈ ����
				Safe_AddRef(iter);
				//Origin�� �ִ���
				isInOrigin = true;
			}
		}
		//Origin�� ������?
		if (false == isInOrigin)
		{
#ifdef _DEBUG
			//�� �ٲٰ�
			pCameraPoint->Set_Collider_Color(_float4(1.0f, 0.0f, 0.0f, 1.0f));
#endif
			//Origin���� �־����
			CameraPointInfoDesc.pAtPoint = pCameraPoint;
			m_OriginAtList.push_back(pCameraPoint);
		}

		//Lerp
		CameraPointInfoDesc.isLerp = CutSceneCameraDesc.isLerp;

		//Duration
		CameraPointInfoDesc.fDuration = CutSceneCameraDesc.fDuration;

		//����Ʈ�� �־��ش�.
		m_CameraInfoList.push_back(CameraPointInfoDesc);
	}

	MSG_BOX("CutScene Load Success");
	CloseHandle(hFile);

	ENDINSTANCE;

	return S_OK;
}

void CCutScene_Camera_Tool::Save_And_Load()
{
	//ȭ�� ����
	if (ImGui::Button("Save CutScene"))
		ImGuiFileDialog::Instance()->
		//			�� â�� ���� Ű��		â �̸�		�⺻ ������	�⺻ ���
		OpenDialog("CutSceneSaveDialog", "Save File", ".cut", "../../Resources/GameData/CutScene/");

	// ȭ�� �������
	if (ImGuiFileDialog::Instance()->Display("CutSceneSaveDialog"))
	{
		// OK������
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			//��Ʈ ����� 0�� �ƴ϶��
			if (false == m_CameraInfoList.empty())
			{
				//���� ���
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

				//_tchar�� ����
				_tchar wszFilePath[MAX_PATH]{};
				CharToWChar(filePathName.c_str(), wszFilePath);

				//���� �Լ� ����
				Save_CutSceneInfo(wszFilePath);
			}
		}
		// �ݴ´�.
		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::SameLine();

	//ȭ�� ����
	if (ImGui::Button("Load CutScene"))
		ImGuiFileDialog::Instance()->
		//			�� â�� ���� Ű��	  â �̸�	  �⺻ ������	�⺻ ���
		OpenDialog("CutSceneLoadDialog", "Load File", ".cut", "../../Resources/GameData/CutScene/");

	// ȭ�� �������
	if (ImGuiFileDialog::Instance()->Display("CutSceneLoadDialog"))
	{
		// OK������
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			//���� ���
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			//_tchar�� ����
			_tchar wszFilePath[MAX_PATH]{};
			CharToWChar(filePathName.c_str(), wszFilePath);

			//�ҷο��� �Լ� ����
			Load_CutSceneInfo(wszFilePath);
		}
		// �ݴ´�.
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
