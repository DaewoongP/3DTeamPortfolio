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
	


	return S_OK;
}

void CLight_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Light", nullptr, m_WindowFlag);

	const char* Types[] = { "Directional","Point","Spot"};

	char pInput[128];//= StrInput.data();
	ImGui::Combo("LightType", &m_iLightType, Types, IM_ARRAYSIZE(Types));
	if (ImGui::InputTextWithHint("input Name", "enter LightName", pInput, IM_ARRAYSIZE(pInput)))
	{
		StrInput = pInput;
	}

	

	Light_ComboBox();

	Create_Light();
	Clear_Light();
	
	

	ImGui::End();

	//__super::MatrixNode(&pMatrix, "Light_Transform", "Light_Position", "Light_Rotation", "Light_Scale");
}

HRESULT CLight_Window::Render()
{
	return S_OK;
}

void CLight_Window::Save_Light()
{
}

void CLight_Window::Load_Light()
{
}

HRESULT CLight_Window::Create_Light()
{
	static float vPos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ImGui::DragFloat3("Position", vPos, 0.1f);

	static float vDir[4] = { 1.f, -1.f, 1.f, 0.0f };
	ImGui::DragFloat3("Direction", vDir, 0.01f, -1.f, 1.f);

	static float fRange[1] = { 0.f };
	ImGui::DragFloat("Range", fRange, 0.1f);

	static float fSpotPower[1] = { 0.f };
	ImGui::DragFloat("SpotPower", fSpotPower, 0.1f);

	static float vDiffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ImGui::DragFloat4("vDiffuse", vDiffuse, 0.01f, 0.f, 1.f);

	static float vAmbient[4] = { 1.f, 1.f, 1.f, 1.0f };
	ImGui::DragFloat4("Ambient", vAmbient, 0.01f, 0.f, 1.f);

	static float vSpecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ImGui::DragFloat4("vSpecular", vSpecular, 0.01f, 0.f, 1.f);
	


		BEGININSTANCE
		CLight::LIGHTDESC LightDesc;
		ZEROMEM(&LightDesc);
		if (ImGui::Button("Create"))
		{
			if (m_iLightIndex > 0)
			{
				m_vecLightDesc.push_back(LightDesc);
				m_vecLightList.push_back(StrInput);
			}
			if (nullptr==pGameInstance->Add_Lights(LightDesc))
			{
				MSG_BOX("Failed to create Light");
				return E_FAIL;
			}
		}

		switch (m_iLightType + 1)
		{
		case 1:
			ZEROMEM(&LightDesc);

			m_szName = "DirectionLight";
			LightDesc.eType = CLight::TYPE_DIRECTIONAL;
			LightDesc.vDir = _float4(vDir);

			LightDesc.vDiffuse = _float4(vDiffuse);
			LightDesc.vAmbient = _float4(vAmbient);
			LightDesc.vSpecular = _float4(vSpecular);

			break;
		case 2:
			ZEROMEM(&LightDesc);	
			m_szName = "PointLight";

			LightDesc.eType = CLight::TYPE_POINT;
			LightDesc.vPos = _float4(vPos);
			LightDesc.fRange = fRange[0];

			LightDesc.vDiffuse = _float4(vDiffuse);
			LightDesc.vAmbient = _float4(vAmbient);
			LightDesc.vSpecular = _float4(vSpecular);
			

			break;
		case 3:
			ZEROMEM(&LightDesc);
			m_szName = "SpotLight";

			LightDesc.eType = CLight::TYPE_SPOTLIGHT;
			LightDesc.vPos = _float4(vPos);
			LightDesc.vDir = _float4(vDir);
			LightDesc.fSpotPower = fSpotPower[0];

			LightDesc.vDiffuse = _float4(vDiffuse);
			LightDesc.vAmbient = _float4(vAmbient);
			LightDesc.vSpecular = _float4(vSpecular);
			break;

		default :
			break;
		}


		ImGui::Checkbox("Setting", &m_isSetting);

		if (m_isSetting == true)
		{
			pGameInstance->Set_Light(m_iLightIndex, LightDesc);
		}
		if (ImGui::Button("Input List"))
		{
			m_vecLightDesc.push_back(LightDesc);
			m_vecLightList.push_back(StrInput);

		}
		ENDINSTANCE
	
		return S_OK;
}

void CLight_Window::Clear_Light()
{
	BEGININSTANCE
		pGameInstance;
	if (ImGui::Button("Clear"))
	{
		m_isSetting = false;
		if (pGameInstance != nullptr)
			pGameInstance->Clear_Lights();
	}
	ENDINSTANCE
	
}

void CLight_Window::Light_ComboBox()
{
	ImGui::ListBox("Light List", &m_iLightIndex, VectorGetter,static_cast<void*>(&m_vecLightList), (_uint)m_vecLightList.size(), 10);
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
	


}
