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

	//���� �غ�
	Ready_Line();

#endif

	CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

	CameraPointDesc.vPosition = Point_Create_Position(_float4(), _float4());

	//���� ���̵� ����Ʈ
	m_pCreateGuidePoint =
		dynamic_cast<CCamera_Point*>(
			pGameInstance->Clone_Component(LEVEL_TOOL,
				TEXT("Prototype_GameObject_Camera_Point"),
				&CameraPointDesc));

	//���� ���� Ż��
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

	//����
	ImGui::Text("CutScene_Tool");

	//�ʱ�ȭ
	Clear_DoubleCheck();

	if (ImGui::Button("Clear CutScene Data"))
	{
		m_isClearDoubleCheck = true;
	}

	BEGININSTANCE;

	//����Ʈ ���� �� ���� 
	ImGui::Text("Point : "); ImGui::SameLine();

	ImGui::RadioButton("Select", &m_iPointRadio, 0); ImGui::SameLine();
	ImGui::RadioButton("Create", &m_iPointRadio, 1); ImGui::SameLine();
	ImGui::RadioButton("Delete", &m_iPointRadio, 2); ImGui::SameLine();
	ImGui::RadioButton("Section_Edit", &m_iPointRadio, 3);

	//Eye �Ǵ� At ����
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

		//���� �Ÿ�
		ImGui::DragFloat("Create Distance", &m_fDistance, 0.01f, 0.0f, 1000.0f);


		//������ ���� ���¿��� ������ ���� ������
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
			pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_PRESSING))
		{
			Create_Tick(vRayPos, vRayDir);
		}
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
			CInput_Device::KEY_PRESSING) &&
		pGameInstance->
		Get_DIMouseState(
			CInput_Device::DIMK_RBUTTON,
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

		m_isLineUpdate = true;
	}
	//z�� �̵��� �ִٸ�
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

	m_isLineUpdate = true;

	m_iterStart = m_CameraInfoList.end();

	m_iterEnd = m_CameraInfoList.end(); 

	m_iterCurEyePoint = m_CameraInfoList.end();
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

void CCutScene_Camera_Tool::Change_EyePoint(list<CAMERAPOINTINFODESC>::iterator _iterEye)
{
	//������ ���õ� ���� �ִٸ�
	if (m_CameraInfoList.end() != m_iterCurEyePoint)
	{
#ifdef _DEBUG
		//���� ������Ʈ �� �����ְ�
		(*m_iterCurEyePoint).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif // _DEBUG
	}

	//�ٲ� ����
	m_iterCurEyePoint = _iterEye;

#ifdef _DEBUG
	//���� ����
	(*m_iterCurEyePoint).pEyePoint->Set_Collider_Color(_float4(0.0f, 1.0f, 1.0f, 1.0f));
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
				pGameInstance->Clone_Component(LEVEL_TOOL,
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

		//���� ���õ� ����Ʈ�� �տ� ���ų�.
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
			//�ǵڿ� �ְų�
			m_CameraInfoList.push_back(CameraPointInfoDesc);
		}
		//���� ������ ���ͷ����ͷ� ����
		m_CurrentIterater = --m_CameraInfoList.end();

		//���� ������Ʈ�� �ٲ�
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

		m_isLineUpdate = true;

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
			iter != m_CameraInfoList.end(); iter++)
		{
			if ((*iter).pEyePoint->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
			{
				m_pCurrentPoint = (*iter).pEyePoint;

				//���ͷ����� ����
				m_CurrentIterater = iter;

				//�� ����
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

		m_isLineUpdate = true;

		m_iterCurEyePoint = m_CameraInfoList.end();;
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

	//������ ����
	WriteFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

	//������ ��ŭ �ݺ�
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
				pGameInstance->Clone_Component(LEVEL_TOOL,
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
				pGameInstance->Clone_Component(LEVEL_TOOL,
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
	//���ǵ� ����

	//���ǵ� ���� ���� ����




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

				m_isLineUpdate = true;

#ifdef _DEBUG

				Line_Update();

#endif
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

			//������ ���ٸ�
			if (false == fs::exists(filePathName))
			{
				// �ݴ´�.
				ImGuiFileDialog::Instance()->Close();
				return;
			}

			//�ҷο��� �Լ� ����
			Load_CutSceneInfo(wszFilePath);

			m_isLineUpdate = true;
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

	//���� ���� Ż��
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
	//�׳� �����
	m_pEyeLine->LineBufferClear();
	m_pAtLine->LineBufferClear();
	m_pLookLine->LineBufferClear();


	//����� �ִٸ�
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

	//���� ����Ʈ
	vector<_float3> Lines;

	Lines.resize(LineDesc.iNum * 2);

	list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();

	//�ε���
	_uint iIndexCount{ 0 };

	for (size_t i = 0; i < m_CameraInfoList.size(); i++)
	{
		//ù��°�� ������ ���ͷ����ʹ� ����
		//ù��°		//�ι�° �Ÿ� ����
		if (iter == m_CameraInfoList.begin())
		{
			//�ι�° ���ͷ����Ϳ�
			list<CAMERAPOINTINFODESC>::iterator nextIter = ++m_CameraInfoList.begin();

			//ù��° �Ÿ� 0 �� ���� ��
			(*iter).isLerp = false;
			(*iter).fSplineLength = 0.0f;
			//�ι�° �Ÿ� = �ι�° - ù��°�� �Ÿ�
			(*nextIter).fSplineLength =
				XMVectorGetX(
					XMVector3Length(
						(*iter).pEyePoint->Get_Position().xyz() -
						(*nextIter).pEyePoint->Get_Position().xyz()));


			//�������� �߰�
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
		//������ 2��° ���� ������ ���� ����
		else if (iter == ----m_CameraInfoList.end())
		{
			//������ ���ͷ����Ϳ�
			list<CAMERAPOINTINFODESC>::iterator NextIter = --m_CameraInfoList.end();

			//������ �Ÿ�
			(*NextIter).fSplineLength =
				XMVectorGetX(
					XMVector3Length(
						(*iter).pEyePoint->Get_Position().xyz() -
						(*NextIter).pEyePoint->Get_Position().xyz()));


			//�������� �߰�
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
		//������ ����
		else if (iter == --m_CameraInfoList.end())
		{
		}
		//������
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

				//���ö��� ���� ���� 10��
				(*Ite3).fSplineLength += XMVectorGetX(
					XMVector3Length(vStartPos - vEndPos));
			}
		}

		//����
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

	//���� ����Ʈ
	vector<_float3> Lines;

	Lines.resize(LineDesc.iNum * 2);

	list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();

	//�ε���
	_uint iIndexCount{ 0 };

	for (size_t i = 0; i < m_CameraInfoList.size(); i++)
	{
		//ù��°�� ������ ���ͷ����ʹ� ����
		//ù��°
		if (iter == m_CameraInfoList.begin())
		{
			//�ι�° ���ͷ����Ϳ�
			list<CAMERAPOINTINFODESC>::iterator nextIter = ++m_CameraInfoList.begin();

			//�������� �߰�
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
		//������ 2��° ���� ������ ���� ����
		else if (iter == ----m_CameraInfoList.end())
		{
			//������ ���ͷ����Ϳ�
			list<CAMERAPOINTINFODESC>::iterator NextIter = --m_CameraInfoList.end();

			//�������� �߰�
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
		//������ ����
		else if (iter == --m_CameraInfoList.end())
		{
		}
		//������
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

		//����
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

	//���� ����Ʈ
	vector<_float3> Lines;

	Lines.resize(LineDesc.iNum * 2);

	list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();

	//�ε���
	_uint iIndexCount{ 0 };

	for (size_t i = 0; i < m_CameraInfoList.size(); i++)
	{
		//�ι�° ���ͷ����Ϳ�
		list<CAMERAPOINTINFODESC>::iterator nextIter = ++m_CameraInfoList.begin();

		//Eye����
		Lines[iIndexCount] = (*iter).pEyePoint->Get_Position().xyz();

		++iIndexCount;

		//At����
		Lines[iIndexCount] = (*iter).pAtPoint->Get_Position().xyz();

		++iIndexCount;

		//����
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
	//�浹 �Ѱ��� ���� ����Ʈ�� ����� ���� ä���.
		
	//�ƹ��ǹ� ���� �Ű����� ä��� ��
	_float fDistanceTemp{};

	for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
		iter != m_CameraInfoList.end(); iter++)
	{
		//�浹 �� 
		if ((*iter).pEyePoint->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
		{
			//���࿡ ���� ����Ʈ�� �־��ٸ� 
			if (m_iterStart != m_CameraInfoList.end())
			{
#ifdef _DEBUG
				//���� ���� ����Ʈ�� ���� �����ش�. 
				(*m_iterStart).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif // _DEBUG
			}

			//���� ���� ���� �Ѵ�. 
			m_iterStart = iter;
		}
	}
}

void CCutScene_Camera_Tool::Select_End_Point(_float4 _vRayPos, _float4 _vRayDir)
{
	//�浹 �Ѱ��� �� ����Ʈ�� ����� ���� ä���.
	
	//�ƹ��ǹ� ���� �Ű����� ä��� ��
	_float fDistanceTemp{};

	for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
		iter != m_CameraInfoList.end(); iter++)
	{
		//�浹 �� 
		if ((*iter).pEyePoint->RayIntersects(_vRayPos, _vRayDir, fDistanceTemp))
		{
			//���࿡ �� ����Ʈ�� �־��ٸ� 
			if (m_iterEnd != m_CameraInfoList.end())
			{
#ifdef _DEBUG
				//���� �� ����Ʈ�� ���� �����ش�. 
				(*m_iterEnd).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif
			}

			//�� ���� ���� �Ѵ�. 
			m_iterEnd = iter;
		}
	}
	
}

void CCutScene_Camera_Tool::Swap_Section_Point()
{
	//�� �� �ϳ��� ����ٸ� Ż��
	if (m_CameraInfoList.end() == m_iterStart || m_CameraInfoList.end() == m_iterEnd)
	{
		return;
	}
	//���� ���ٸ� �Ѵ� ��� �Ѵ�. 
	if (m_iterEnd == m_iterStart)
	{
		MSG_BOX("Same Point; Select Point Again;");
#ifdef _DEBUG
		//�Ѵ� �� �����ְ�
		(*m_iterStart).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
		(*m_iterEnd).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 1.0f, 1.0f));
#endif
		//�ʱ�ȭ
		m_iterStart = m_iterEnd = m_CameraInfoList.end();

		return;
	}

	//������ start�� �־���?
	_bool isStart{ false };

	//��ü ��ȸ(���� �ϳ��� ������ Ż��)
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

	//�������� ������� 
	if (true == isStart)
	{
		//�ƹ��͵� ���� �ʴ´�.
	}
	//������ �������
	else if (false == isStart)
	{
		//�����Ѵ�.
		list<CAMERAPOINTINFODESC>::iterator iterTemp = m_CameraInfoList.end();
		iterTemp = m_iterStart;
		m_iterStart = m_iterEnd;
		m_iterEnd = iterTemp;
	}
#ifdef _DEBUG
	//���� ���� ������.
	(*m_iterStart).pEyePoint->Set_Collider_Color(_float4(1.0f, 1.0f, 1.0f, 1.0f));
	(*m_iterEnd).pEyePoint->Set_Collider_Color(_float4(0.0f, 0.0f, 0.0f, 1.0f));
#endif // _DEBUG
}

void CCutScene_Camera_Tool::Section_Point_Update(_float4 _vRayPos, _float4 _vRayDir)
{
	//��ŸƮ �ص� ����
	ImGui::Text("Section Select : "); ImGui::SameLine();

	ImGui::RadioButton("Start", &m_iSectionRadio, 0); ImGui::SameLine();
	ImGui::RadioButton("End", &m_iSectionRadio, 1);

	BEGININSTANCE;

	//Ŭ���� ������ ����
	if (pGameInstance->Get_DIMouseState(
		CInput_Device::DIMK_LBUTTON,
		CInput_Device::KEY_DOWN) &&
		pGameInstance->Get_DIMouseState(
			CInput_Device::DIMK_RBUTTON,
			CInput_Device::KEY_PRESSING))
	{
		//������ ���� ���� ��
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

		//���� ���� �� ����
		Swap_Section_Point();
	}

	//���� ��� �ð�
	ImGui::DragFloat("Section_Play_Time", &m_fSectionPlayTime, 0.1f, 0.1f, 1000.0f);
	//�ð� ���� (��ư ����)
	Set_Section_Point_Duration();

	//���
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


		//������?
		_bool isStart{ false };

		//�̱����� �ѱ��̸� �˾ƾ� �Ѵ�.
		_float fSectionSTotalLength{ 0.0f };

		for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter != m_CameraInfoList.end(); ++iter)
		{
			//������������ ���� �෹�̼� ����
			if (iter == m_iterStart)
			{
				isStart = true;
			}
			//���� ���� �۾� 
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


		//������ �ѱ��̷� �෹�̼� ����
		for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter != m_CameraInfoList.end(); ++iter)
		{
			//������������ ���� �෹�̼� ����
			if (iter == m_iterStart)
			{
				isStart = true;
			}
			//���� ���� �۾� 
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
		//�Ѵ� �־�� ��
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

		//������?
		_bool isStart{ false };

		for (list<CAMERAPOINTINFODESC>::iterator iter = m_CameraInfoList.begin();
			iter != m_CameraInfoList.end(); ++iter)
		{
			//���������� ���� ���� 
			if (iter == m_iterStart)
			{
				isStart = true;
				//���� ��� �� �־�� �ڿ��������...ó���̸� �ȳ־ �ǰ�...
				BEGININSTANCE;

				CUTSCENECAMERADESC CutSceneCameraDesc{};

				//������ �ƴ϶��
				if (iter != m_CameraInfoList.begin())
				{
					//���� �� 1��
					CutSceneCameraDesc.vEye = (*--iter).pEyePoint->Get_Position();
					CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
					CutSceneCameraDesc.isLerp = false;
					CutSceneCameraDesc.fDuration = 0.0f;

					pGameInstance->Add_CutScene(CutSceneCameraDesc);

					//���� �� 1��
					CutSceneCameraDesc.vEye = (*++iter).pEyePoint->Get_Position();
					CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
					CutSceneCameraDesc.isLerp = false;
					CutSceneCameraDesc.fDuration = 0.0f;

					pGameInstance->Add_CutScene(CutSceneCameraDesc);
				}
				//�����̶��
				else if (iter == m_CameraInfoList.begin())
				{
					//���� �� 1��
					CutSceneCameraDesc.vEye = (*iter).pEyePoint->Get_Position();
					CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
					CutSceneCameraDesc.isLerp = (*iter).isLerp;
					CutSceneCameraDesc.fDuration = (*iter).fDuration;

					pGameInstance->Add_CutScene(CutSceneCameraDesc);
				}

				ENDINSTANCE;

			}
			//���� ���� �۾� 
			else if (iter == m_iterEnd)
			{
				//���� ��� �� �־�� �ڿ�������...�������̸� �ȳ־ �ǰ�...

				BEGININSTANCE;

				CUTSCENECAMERADESC CutSceneCameraDesc{};

				//���� �� 1��
				CutSceneCameraDesc.vEye = (*iter).pEyePoint->Get_Position();
				CutSceneCameraDesc.vAt = (*iter).pAtPoint->Get_Position();
				CutSceneCameraDesc.isLerp = (*iter).isLerp;
				CutSceneCameraDesc.fDuration = (*iter).fDuration;

				pGameInstance->Add_CutScene(CutSceneCameraDesc);

				//�������� �ƴ϶��
				if (iter != --m_CameraInfoList.end())
				{
					//������ �� 1��
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
