#include "..\Public\ImGui_Manager.h"
#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Test_Player.h"

// 릴리즈 빌드에서 제외된 파일입니다.
// 코드 작성시 유의 해주세요

/*
디버그 용도 클래스가 아니면
절대 Get, Set 남발하지말고,
여기 윈도우에 굳이 필요없는 변수 넣지말고
#ifdef 꼭 걸어주세요
*/

IMPLEMENT_SINGLETON(CImGui_Manager)

HRESULT CImGui_Manager::Initialize(LEVELID iLevelID)
{

	


	return S_OK;
}

void CImGui_Manager::Tick(_float fTimeDelta)
{
	RECT rc;
	GetWindowRect(g_hWnd, &rc);

	ImGui::SetNextWindowPos(ImVec2(_float(rc.right), _float(rc.top)));
	ImGui::SetNextWindowSize(ImVec2(600.f, g_iWinSizeY + 50.f));

	ImGui::Begin("Client Debug", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	Debug_Camera();

	ImGui::End();
}

void CImGui_Manager::Debug_Camera()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Debug* pCam = static_cast<CCamera_Debug*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Debug"), TEXT("GameObject_Camera_Debug")));

	_float fSpeed = pCam->Get_Speed();

	ImGui::Checkbox("Fix Mouse : ~ ", pCam->Get_FixMouse());

	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.f);
	if (ImGui::SliderFloat("Camera Speed", &fSpeed, 0.f, 250.f, "%.1f"))
	{
		pCam->Set_Speed(fSpeed);
	}

	CTest_Player* pTest_Player = static_cast<CTest_Player*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Debug"), TEXT("GameObject_Test_Player")));

	if (ImGui::Checkbox("3rd Camera", &m_is3rdCam))
	{
		CTransform* pTransform = pTest_Player->Get_Transform();
		m_v3rdCamOffset -= pTransform->Get_Look() * 5.f;
		m_v3rdCamOffset += pTransform->Get_Up() * 5.f;
	}
	if (true == m_is3rdCam)
	{
		ImGui::SameLine();
		ImGui::Checkbox("Fix Camera Move", &m_isFix3rdCam);
		_float3 vPlayerPos = pTest_Player->Get_Transform()->Get_Position();
		if (true == m_isFix3rdCam)
		{
			pCam->LookAt(vPlayerPos);
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(150.f);
		ImGui::InputFloat3("3rd Cam Offset", reinterpret_cast<_float*>(&m_v3rdCamOffset), "%.1f");
		pCam->Set_Position(vPlayerPos + m_v3rdCamOffset);
	}

	Safe_Release(pGameInstance);
}

void CImGui_Manager::Free()
{
}
