#include "Light_Window.h"
#include"GameInstance.h"

CLight_Window::CLight_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CImWindow(pDevice,pContext)
{
}

HRESULT CLight_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{

	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;


	

	m_WindowFlag = ImGuiWindowFlags_NoResize;
	//StrInput = "Default_Dir_Light";
	BEGININSTANCE
		ZEROMEM(&LightDesc);
	LightDesc.vPos = _float4(0.f, 2.f, 0.f, 1.f);
	LightDesc.vDir = _float4(0.3f,-1.f,0.3f,0.f);

	LightDesc.vDiffuse = _float4(1.f,1.f,1.f,1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc);
	//m_vecLightDesc.push_back(LightDesc);
	//m_vecLightList.push_back(StrInput);
	ENDINSTANCE
	return S_OK;
}

void CLight_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Light", nullptr, m_WindowFlag);


	const char* Types[] = { "Directional","Point","Spot"};

	ImGui::Combo("LightType", &m_iLightType, Types, IM_ARRAYSIZE(Types));
	if (ImGui::InputTextWithHint("input Name", "enter LightName", AddLightName, IM_ARRAYSIZE(AddLightName)))
	{
		StrInput = AddLightName;
	}
	
	m_iCurrent_LightIndex = m_iLightIndex;//list 박스에서 다른 빛으로변경할때 변경된것판단.

	Light_ComboBox();
	
	if (m_iLightIndex != m_iCurrent_LightIndex)
	{
		BEGININSTANCE
		LightDesc = *pGameInstance->Get_Light(m_iLightIndex);

		FloatToFloat4(vPos, LightDesc.vPos);
		*fRange = LightDesc.fRange;
		*fSpotPower = LightDesc.fSpotPower;
		FloatToFloat4(vDir, LightDesc.vDir);
		FloatToFloat4(vDiffuse, LightDesc.vDiffuse);
		FloatToFloat4(vAmbient, LightDesc.vAmbient);
		FloatToFloat4(vSpecular, LightDesc.vSpecular);
		m_iLightType = LightDesc.iLightType;
		ENDINSTANCE
	}
	ImGui::DragFloat3("Position", vPos, 0.1f);

	ImGui::DragFloat3("Direction", vDir, 0.01f, -1.f, 1.f);

	ImGui::DragFloat("Range", fRange, 0.1f);

	ImGui::DragFloat("SpotPower", fSpotPower, 0.1f);

	ImGui::DragFloat4("vDiffuse", vDiffuse, 0.01f, 0.f, 1.f);

	ImGui::DragFloat4("Ambient", vAmbient, 0.01f, 0.f, 1.f);

	ImGui::DragFloat4("vSpecular", vSpecular, 0.01f, 0.f, 1.f);
	ImGui::Separator();

	Create_Light();
	if (m_iLightIndex >= 0)
		ImGui::Checkbox("Setting", &m_isSetting);
	Save_Light(); ImGui::SameLine();
	Load_Light();
	Delete_Light(m_iLightIndex,LightDesc.szName);
	Clear_Light();

	ImGui::End();
}

HRESULT CLight_Window::Render()
{
	return S_OK;
}
HRESULT CLight_Window::Create_Light()
{
	//m_iLightType = LightDesc.iLightType;

	

	if (ImGui::Button("Same as Diffuse"))
	{
		for (_uint i = 0; i < 4; ++i)
		{
			vAmbient[i] = vDiffuse[i];
			vSpecular[i] = vDiffuse[i];
		}

	}
	ImGui::Separator();
	BEGININSTANCE



		switch (m_iLightType + 1)
		{
		case 1:
			ZEROMEM(&LightDesc);

			m_szName = "DirectionLight";
			//LightDesc.vPos = _float4(vPos);
			LightDesc.vDir = _float4(vDir);

			LightDesc.vDiffuse = _float4(vDiffuse);
			LightDesc.vAmbient = _float4(vAmbient);
			LightDesc.vSpecular = _float4(vSpecular);
			LightDesc.eType = CLight::TYPE_DIRECTIONAL;
			LightDesc.iLightType = (_uint)CLight::TYPE_DIRECTIONAL;
			strcpy_s(LightDesc.szName,sizeof(LightDesc.szName), StrInput.c_str());
			break;
		case 2:
			ZEROMEM(&LightDesc);
			m_szName = "PointLight";

			LightDesc.vPos = _float4(vPos);
			LightDesc.fRange = fRange[0];

			LightDesc.vDiffuse = _float4(vDiffuse);
			LightDesc.vAmbient = _float4(vAmbient);
			LightDesc.vSpecular = _float4(vSpecular);
			LightDesc.eType = CLight::TYPE_POINT;
			LightDesc.iLightType = (_uint)CLight::TYPE_POINT;
			strcpy_s(LightDesc.szName, sizeof(LightDesc.szName), StrInput.c_str());

			break;
		case 3:
			ZEROMEM(&LightDesc);
			m_szName = "SpotLight";
			LightDesc.vPos = _float4(vPos);
			LightDesc.vDir = _float4(vDir);
			LightDesc.fSpotPower = fSpotPower[0];

			LightDesc.vDiffuse = _float4(vDiffuse);
			LightDesc.vAmbient = _float4(vAmbient);
			LightDesc.vSpecular = _float4(vSpecular);
			LightDesc.eType = CLight::TYPE_SPOTLIGHT;
			LightDesc.iLightType = (_uint)CLight::TYPE_SPOTLIGHT;
			strcpy_s(LightDesc.szName, sizeof(LightDesc.szName), StrInput.c_str());
			break;

		default:
			break;
		}

	if (ImGui::Button("Create"))
	{
		m_isSetting = false;
		if (nullptr == pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc))
		{
			MSG_BOX("Failed to create Light");
			return E_FAIL;
		}
		m_vecLightDesc.push_back(LightDesc);
		m_vecLightList.push_back(StrInput);
		m_iLightIndex++;
		ResetValue();
	}


	

	if (m_isSetting == true)
	{

		if (m_vecLightDesc.empty())
		{
			MSG_BOX("Please Create Light first.");
			m_isSetting = false;
		}
		else if (m_iLightIndex != m_iCurrent_LightIndex)
		{
			LightDesc = *pGameInstance->Get_Light(m_iLightIndex);

			FloatToFloat4(vPos, LightDesc.vPos);
			*fRange = LightDesc.fRange;
			*fSpotPower = LightDesc.fSpotPower;
			FloatToFloat4(vDir, LightDesc.vDir);
			FloatToFloat4(vDiffuse, LightDesc.vDiffuse);
			FloatToFloat4(vAmbient, LightDesc.vAmbient);
			FloatToFloat4(vSpecular, LightDesc.vSpecular);
		}
		else
		{
			pGameInstance->Set_Light(m_iLightIndex, LightDesc);
		}

	}
	ImGui::SameLine();
	if (ImGui::Button("Input List"))
	{
		m_isSetting = false;
		if (m_iLightIndex >= 0)
		{
			pGameInstance->Set_Light(m_iLightIndex, LightDesc);
			Set_Tool_Light(m_iLightIndex,LightDesc);
		}

	}
	
	ENDINSTANCE
		m_iLightType = LightDesc.iLightType;
		return S_OK;
}
HRESULT CLight_Window::Save_Light()
{
	if(ImGui::Button("Save"))
	{
		_tchar dataFile[MAX_PATH] = TEXT("../../Resources/LightData/Light.dat");

		HANDLE hFile = CreateFile(dataFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			MSG_BOX("Failed to Save Light Data");
		}

		DWORD	dwByte = 0;

		for (auto& iter : m_vecLightDesc)
		{
			if (!WriteFile(hFile, &iter.eType, sizeof(CLight::LIGHTDESC), &dwByte, nullptr))
				MSG_BOX("Failed to Write eType");

			if (!WriteFile(hFile, &iter.vPos, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Write vPos");

			if (!WriteFile(hFile, &iter.vDir, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Write vDir");

			if (!WriteFile(hFile, &iter.fRange, sizeof(_float), &dwByte, nullptr))
				MSG_BOX("Failed to Write fRange");

			if (!WriteFile(hFile, &iter.fSpotPower, sizeof(_float), &dwByte, nullptr))
				MSG_BOX("Failed to Write fSpotPower");

			if (!WriteFile(hFile, &iter.vDiffuse, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Write vDiffuse");

			if (!WriteFile(hFile, &iter.vAmbient, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Write vAmbient");

			if (!WriteFile(hFile, &iter.vSpecular, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Write vSpecular");

			if (!WriteFile(hFile, &iter.iLightType, sizeof(_uint), &dwByte, nullptr))
				MSG_BOX("Failed to Write Type");

			if (!WriteFile(hFile, &iter.szName, sizeof(_tchar), &dwByte, nullptr))
				MSG_BOX("Failed to Write Name");
		}



		MSG_BOX("Save Success");

		CloseHandle(hFile);
	}
	return S_OK;

}



HRESULT CLight_Window::Load_Light()
{
	if (ImGui::Button("Load"))
	{
		Clear_Light();
		m_vecLightDesc.clear();

		_tchar dataFile[MAX_PATH] = TEXT("../../Resources/LightData/Light.dat");

		HANDLE hFile = CreateFile(dataFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			MSG_BOX("Failed to Load Light Data");
		}

		DWORD	dwByte = 0;
		CLight::LIGHTDESC SaveDesc;
		ZEROMEM(&SaveDesc);

		while (true)
		{
			if (!ReadFile(hFile, &SaveDesc.eType, sizeof(CLight::LIGHTDESC), &dwByte, nullptr))
				MSG_BOX("Failed to Load eType");

			if (!ReadFile(hFile, &SaveDesc.vPos, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Load vPos");

			if (!ReadFile(hFile, &SaveDesc.vDir, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Load vDir");

			if (!ReadFile(hFile, &SaveDesc.fRange, sizeof(_float), &dwByte, nullptr))
				MSG_BOX("Failed to Load fRange");

			if (!ReadFile(hFile, &SaveDesc.fSpotPower, sizeof(_float), &dwByte, nullptr))
				MSG_BOX("Failed to Load fSpotPower");

			if (!ReadFile(hFile, &SaveDesc.vDiffuse, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Load vDiffuse");

			if (!ReadFile(hFile, &SaveDesc.vAmbient, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Load vAmbient");

			if (!ReadFile(hFile, &SaveDesc.vSpecular, sizeof(_float4), &dwByte, nullptr))
				MSG_BOX("Failed to Load vSpecular");

			if (!ReadFile(hFile, &SaveDesc.iLightType, sizeof(_uint), &dwByte, nullptr))
				MSG_BOX("Failed to Load Type");

			if (!ReadFile(hFile, &SaveDesc.szName, sizeof(_tchar), &dwByte, nullptr))
				MSG_BOX("Failed to Load Name");

			if (dwByte == 0)
			{
				break;
			}

			m_vecLightDesc.push_back(SaveDesc);
			m_vecLightList.push_back(SaveDesc.szName);

		}

		MSG_BOX("Load Successed");

		CloseHandle(hFile);
	

		BEGININSTANCE
			for (size_t i = 0; i < m_vecLightDesc.size(); ++i)
			{
				LightDesc = m_vecLightDesc[i];
				pGameInstance->Add_Lights( m_pDevice,m_pContext, LightDesc);

			}

		ENDINSTANCE
	}
	


	return S_OK;
}

void CLight_Window::Set_Tool_Light(_uint iIndex, CLight::LIGHTDESC LightDesc)
{
	auto iter = m_vecLightDesc.begin();
	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	*iter = LightDesc;

}

void CLight_Window::Delete_Light(_uint iIndex , const _char* pName)
{
	if (ImGui::Button("Delete"))
	{
		BEGININSTANCE

			if (pGameInstance != nullptr)
				pGameInstance->Delete_Lights(iIndex,pName);
		ENDINSTANCE
		
		auto iter = m_vecLightDesc.begin();
			
		for (size_t i = 0; i < iIndex; ++i)
			++iter;
		m_vecLightDesc.erase(iter);

		auto iterList = m_vecLightList.begin();

		for (size_t i = 0; i < iIndex; ++i)
			++iterList;
		m_vecLightList.erase(iterList);

		
	}
}


void CLight_Window::Clear_Light()
{
	if (ImGui::Button("Delete All"))
	{
		/*m_isCheck = true;
		if (m_isCheck)
		{
			ImGui::Begin("Warning", &m_isCheck);
			ImGui::Text("Are you want to Delete All LIghts?");
			if(ImGui::Button("Yes"))
			{
				m_isSetting = false;
				BEGININSTANCE

				if (pGameInstance != nullptr)
					pGameInstance->Clear_Lights();
				ENDINSTANCE
				m_vecLightDesc.clear();
				m_vecLightList.clear();
				m_iLightIndex = 0;
				m_isCheck = false;
			}
			if (ImGui::Button("No"))
			{
				m_isCheck = false;
			}
			m_isCheck;
			ImGui::End();

		}*/
		m_isSetting = false;
		BEGININSTANCE

			if (pGameInstance != nullptr)
				pGameInstance->Clear_Lights();
		ENDINSTANCE
			m_vecLightDesc.clear();
		m_vecLightList.clear();
		m_iLightIndex = 0;
	}
	
	
}

void CLight_Window::ResetValue()
{
	vPos[0] = { 0.0f }; vPos[1] = { 0.0f }; vPos[2] = { 0.0f }; vPos[3] = { 1.0f };

	vDir[0] = { 1.0f }; vDir[1] = { -1.0f }; vDir[2] = { 1.0f }; vDir[3] = { 0.0f };

	fRange[0] = { 0.f };

	fSpotPower[0] = { 0.f };

	vDiffuse[0] = { 0.f }; vDiffuse[1] = { -0.f }; vDiffuse[2] = { 0.f }; vDiffuse[3] = { 1.f };

	vAmbient[0] = { 1.f }; vAmbient[1] = { 1.f }; vAmbient[2] = { 1.f }; vAmbient[3] = { 1.0f };

	vSpecular[0] = { 0.f }; vSpecular[1] = { 0.f }; vSpecular[2] = { 0.f }; vSpecular[3] = { 1.0f };

}

void CLight_Window::Light_ComboBox()
{
	ImGui::ListBox("Light List", &m_iLightIndex, VectorGetter,static_cast<void*>(&m_vecLightList), (_uint)m_vecLightList.size(), 10);
}

void CLight_Window::FloatToFloat4(_float* Input, _float4 Out)
{
	  Input[0] = Out.x;
	  Input[1] = Out.y;
	  Input[2] = Out.z;
	  Input[3] = Out.w;
}

HRESULT CLight_Window::Add_Component()
{


	return S_OK;

	//void CLight_Window::Picking()
	//{
	//	_float3 vPos = Find_PickingPos();
	//
	//	// 현재 피킹 위치 표시
	//	ImGui::Text("Picking Position");
	//	ImGui::Text("Pressing LShift : Rounding the value");
	//	ImGui::Text("%.1f /", vPos.x);
	//	ImGui::SameLine();
	//	ImGui::Text("%.1f /", vPos.y);
	//	ImGui::SameLine();
	//	ImGui::Text("%.1f", vPos.z);
	//	ImGui::Text("----------------------------------------");
	//
	//	// 마우스 좌클릭을 하면 해당 위치로 더미 이동
	//	BEGININSTANCE; if (true == pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
	//		-1.f != vPos.x)
	//	{
	//		// shift키를 누르고 있으면 격자에 딱 맞게 위치가 반올림됨
	//		if (true == pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_PRESSING))
	//		{
	//			vPos.x = round(vPos.x);
	//			vPos.y = round(vPos.y);
	//			vPos.z = round(vPos.z);
	//
	//			m_pDummy->Set_Pos(vPos);
	//		}
	//
	//		else
	//		{
	//			m_pDummy->Set_Pos(vPos);
	//		}
	//	} ENDINSTANCE;
	//}
	//
	//_float3 CLight_Window::Find_PickingPos()
	//{
	//	
	//		_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	//		_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };
	//
	//		BEGININSTANCE; pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);
	//
	//		CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
	//			static_cast<CTerrain*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
	//				TEXT("GameObject_Terrain")))->Get_Buffer()); ENDINSTANCE;
	//
	//		float fDist = FLT_MAX; // 피킹 연산 후 최종 거리값
	//
	//		_bool bResult = pTerrain->IsPicked(vRayPos, vRayDir, fDist);
	//
	//		// 결과가 나올 경우 RayDir에 거리값을 곱해 최종 위치 산출
	//		if (true == bResult)
	//		{
	//			if (FLT_MAX > fDist)
	//			{
	//				_float4 vFinalPos;
	//
	//				vRayDir *= fDist;
	//				vFinalPos = vRayPos + vRayDir;
	//
	//				return _float3(vFinalPos.x, vFinalPos.y, vFinalPos.z);
	//			}
	//		}
	//
	//		return _float3(-1.f, -1.f, -1.f);
	//	
	//}
}
CLight_Window* CLight_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CLight_Window* pInstance = New CLight_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CLight_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLight_Window::Free()
{
	__super::Free();
	
	m_vecLightDesc.clear();
	m_vecLightList.clear();


}
