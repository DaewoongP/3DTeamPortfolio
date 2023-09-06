#include "Light_Window.h"
#include"GameInstance.h"
#include"LightDot.h"
#include"MapObject.h"
#include"Terrain.h"
#include"RenderTarget.h"

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
	LightDesc.vPos = _float4(5.f, 15.f, 5.f, 1.f);
	//LightDesc.vDir = _float4(0.3f,-0.9f,0.3f,0.f);
	LightDesc.vDir = _float4(0.34f, -0.98f, 0.34f, 0.f);
	
	LightDesc.vDiffuse = _float4(1.f,1.f,1.f,1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc);
	
	CLightDot::LIGHTPOS LightDot;
	ZEROMEM(&LightDot);
	FloatToFloat4(vPos, LightDot.vPosition);
	FloatToFloat4(vDir, LightDot.vDir);
	m_pLightDot =dynamic_cast<CLightDot*>(pGameInstance->Clone_Component(LEVEL_TOOL,TEXT("Prototype_GameObject_LightDot"),&LightDot));


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
	Set_LightInfo();
	ImGui::Separator();

	ImGui::Checkbox("Picking", &m_isCheckPicking);
		if (m_isCheckPicking)
	{
		ImGui::Begin("Picking Info", &m_isCheckPicking);
		Picking_Menu(LightDesc);
		ImGui::End();
	}

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
			LightDesc.vPos = _float4(5.f,15.f,5.f,1.f);
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
		if(!m_vecLightList.empty())
			m_iLightIndex++;
		m_isSetting = false;
		if (nullptr == pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc))
		{
			MSG_BOX("Failed to create Light");
			return E_FAIL;
		}
		m_vecLightDesc.push_back(LightDesc);
		m_vecLightList.push_back(StrInput);
		
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
		_tchar dataFile[MAX_PATH] = TEXT("../../Resources/LightData/Light.ljh");

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

		_tchar dataFile[MAX_PATH] = TEXT("../../Resources/LightData/Light.ljh");

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
		if(m_iLightIndex>0)
			m_iLightIndex--;

		
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

void CLight_Window::Set_LightInfo()
{
	if (!m_isCheckPicking)
	{
		ImGui::DragFloat3("Position", vPos, 0.1f);
	}
	else if (m_isCheckPicking)
	{	if(m_isCursorIn)
		FloatToFloat4(vPos, _float4(m_vPickPos.x, m_vPickPos.y, m_vPickPos.z,1.f));
		ImGui::DragFloat3("Position",vPos, 0.1f);

	}
	ImGui::DragFloat3("Direction", vDir, 0.01f, -1.f, 1.f);
	ImGui::DragFloat("Range", fRange, 0.1f);
	ImGui::DragFloat("SpotPower", fSpotPower, 0.1f);
	ImGui::DragFloat4("vDiffuse", vDiffuse, 0.01f, 0.f, 1.f);
	ImGui::DragFloat4("Ambient", vAmbient, 0.01f, 0.f, 1.f);
	ImGui::DragFloat4("vSpecular", vSpecular, 0.01f, 0.f, 1.f);

}

void CLight_Window::FloatToFloat4(_float* Input, _float4 Out)
{
	  Input[0] = Out.x;
	  Input[1] = Out.y;
	  Input[2] = Out.z;
	  Input[3] = Out.w;
}
void CLight_Window::Float4ToFloat(_float* Input, _float4 Out)
{
		 Out.x = Out.x; Input[0];
		 Out.y = Out.y; Input[1];
		 Out.z = Out.z; Input[2];
		 Out.w = Out.w; Input[3];
}
_float3 CLight_Window::Find_PickPos()
{
	

	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	BEGININSTANCE; pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

	CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
		static_cast<CTerrain*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
			TEXT("GameObject_Terrain")))->Get_Buffer()); ENDINSTANCE;

	float fDist = FLT_MAX; // 피킹 연산 후 최종 거리값

	_bool bResult = pTerrain->IsPicked(vRayPos, vRayDir, fDist);

	// 결과가 나올 경우 RayDir에 거리값을 곱해 최종 위치 산출
	if (true == bResult)
	{
		if (FLT_MAX > fDist)
		{
			_float4 vFinalPos;

			vRayDir *= fDist;
			vFinalPos = vRayPos + vRayDir;

			return _float3(vFinalPos.x, vFinalPos.y, vFinalPos.z);
		}
	}

	return _float3(-1.f, -1.f, -1.f);
}
void CLight_Window::Picking_Menu(CLight::LIGHTDESC LightDesc)
{
	POINT pt;
	GetCursorPos(&pt);

	// 화면의 크기를 얻어옵니다.
	
	ScreenToClient(g_hWnd, &pt);

	// 마우스 커서의 위치가 화면 밖으로 나갔는지 확인합니다.
	if (pt.x > 0 && pt.x < 1280 &&
		pt.y >0 && pt.y < 720)
	{
		m_isCursorIn = true;

	}
	else
		m_isCursorIn = false;

		m_vPickPos = Find_PickPos();
		// 현재 피킹 위치 표시
		ImGui::Text("Picking Position");
		ImGui::Text("Pressing LShift : Rounding the value");
		ImGui::Text("%.1f /", m_vPickPos.x);
		ImGui::SameLine();
		ImGui::Text("%.1f /", m_vPickPos.y);
		ImGui::SameLine();
		ImGui::Text("%.1f", m_vPickPos.z);
		ImGui::Separator();
		//LightDesc.vPos = _float4(m_vPickPos.x, m_vPickPos.y, m_vPickPos.z, 1.f);
		if (m_isCursorIn)
		{
			BEGININSTANCE
				if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
				{
					m_isSetting = false;

					pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc);
					m_vecLightDesc.push_back(LightDesc);
					m_vecLightList.push_back(StrInput);
					m_iLightIndex++;
					ResetValue();
				}
			ENDINSTANCE
		}

}
void CLight_Window::Mesh_Picking()
{
#ifdef _DEBUG
	BEGININSTANCE
	
		//Target_Picking의 ID3D11Texture2D를 가져옴
		ID3D11Texture2D* pTexture = pGameInstance->Find_RenderTarget(TEXT("Target_Picking"))->Get_Texture2D();

		// 텍스처의 너비와 높이
		D3D11_TEXTURE2D_DESC  TextureDesc;
		pTexture->GetDesc(&TextureDesc);

		_uint iWidth = TextureDesc.Width;
		_uint iHeight = TextureDesc.Height;

		// 마우스 위치
		D3D11_VIEWPORT ViewPort;
		_uint iNumViewPorts = 1;

		ZEROMEM(&ViewPort);
		if (nullptr == m_pContext)
			return;
		m_pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

		POINT	pt{};
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		D3D11_BOX findDesc;
		ZEROMEM(&findDesc);

		// 이 구조체의 정보를 토대로 단 하나의 픽셀을 가져온다
		findDesc.left = pt.x;
		findDesc.right = pt.x + 1;
		findDesc.top = pt.y;
		findDesc.bottom = pt.y + 1;
		findDesc.front = 0;
		findDesc.back = 1;

		// Usage 버퍼 생성, 읽기 전용이다.
		ID3D11Texture2D* pCopyTexture2D = { nullptr };
		D3D11_TEXTURE2D_DESC	TextureDescCopy;
		ZEROMEMSIZE(&TextureDescCopy, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDescCopy.Width = 1;
		TextureDescCopy.Height = 1;
		TextureDescCopy.MipLevels = 1;
		TextureDescCopy.ArraySize = 1;
		TextureDescCopy.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

		TextureDescCopy.SampleDesc.Quality = 0;
		TextureDescCopy.SampleDesc.Count = 1;

		TextureDescCopy.Usage = D3D11_USAGE_STAGING;
		TextureDescCopy.BindFlags = 0;
		TextureDescCopy.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		TextureDescCopy.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDescCopy, nullptr, &pCopyTexture2D)))
			return;

		// 새로 만든 1칸짜리 텍스처에 가져온 렌더 타겟 텍스처에서 원하는 1칸을 넣어준다.
		m_pContext->CopySubresourceRegion(pCopyTexture2D, 0, 0, 0, 0, pTexture, 0, &findDesc);

		D3D11_MAPPED_SUBRESOURCE MappedDesc;
		ZEROMEM(&MappedDesc);

		if (FAILED(m_pContext->Map(pCopyTexture2D, 0, D3D11_MAP_READ, 0, &MappedDesc)))
		{
			m_pContext->Unmap(pCopyTexture2D, 0);
			MSG_BOX("Failed to Map Picking Texture");
			return;
		}

		// 해당 Pixel Copy가 잘못 되었을 경우..
		if (MappedDesc.pData == nullptr)
		{
			m_pContext->Unmap(pCopyTexture2D, 0);
			MSG_BOX("Copy Data is nullptr");
			return;
		}

		_uint pickID = { 0 };
		pickID = ((_uint*)MappedDesc.pData)[0];

		pickID -= 4278190080;

		string s = ("GameObject_MapObject_");
		s += std::to_string(pickID);

	
		m_pContext->Unmap(pCopyTexture2D, 0);

		Safe_Release(pCopyTexture2D);
	ENDINSTANCE;
#endif
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
	Safe_Release(m_pLightDot);
	m_vecLightDesc.clear();
	m_vecLightList.clear();


}
