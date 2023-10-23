#include "..\Public\Effect_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"
#include "ParticleSystem.h"
#include "DummyParticle.h"
#include "ComboBox.h"
#include "ImageFileDialog.h"
#include "DummyMeshEffect.h"
#include "DummyTrail.h"
#include "DummyFlipBook.h"
#include "Dummy_Effect.h"
CEffect_Window::CEffect_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImWindow(_pDevice, _pContext)
{
}
HRESULT CEffect_Window::Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	if (FAILED(__super::Initialize(_vWindowPos, _vWindowSize)))
		return E_FAIL;
	BEGININSTANCE;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	// 레이어에서 돌리기
	//pGameInstance->Add_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyParticle")
	//	, TEXT("Test"), TEXT("TestParticle"));
	//m_pDummyParticle = dynamic_cast<CDummyParticle*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Test"), TEXT("TestParticle")));

	//pGameInstance->Add_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyMeshEffect")
	//	, TEXT("Test"), TEXT("TestMeshEffect"));
	//m_pDummyMeshEffect = dynamic_cast<CDummyMeshEffect*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Test"), TEXT("TestMeshEffect")));

	// 직접 돌리기.
	m_pDummyParticle = dynamic_cast<CDummyParticle*>(pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyParticle")));
	m_pDummyMeshEffect = dynamic_cast<CDummyMeshEffect*>(pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyMeshEffect")));
	m_pDummyTrail = dynamic_cast<CDummyTrail*>(pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyTrail")));
	m_pDummyFlipbook = dynamic_cast<CDummyFlipBook*>(pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyFlipbook")));
	m_pDummyEffect = dynamic_cast<CDummy_Effect*>(pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyEffect")));

	ENDINSTANCE;
	return S_OK;
}

void CEffect_Window::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);
	// 왼쪽 아래로 고정

	if (true == m_pDummyParticle->m_isHardReset)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		Safe_Release(m_pDummyParticle);
		m_pDummyParticle = dynamic_cast<CDummyParticle*>(pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyParticle")));
		Safe_Release(pGameInstance);
	}

	if (true == m_pDummyMeshEffect->m_isHardReset)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		Safe_Release(m_pDummyMeshEffect);
		m_pDummyMeshEffect = dynamic_cast<CDummyMeshEffect*>(pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyMeshEffect")));
		Safe_Release(pGameInstance);
	}

	ImGui::RadioButton("Particle", &m_iChooseRadio, 0);
	ImGui::RadioButton("MeshEffect", &m_iChooseRadio, 1);
	ImGui::RadioButton("Trail", &m_iChooseRadio, 2);
	ImGui::RadioButton("Flipbook", &m_iChooseRadio, 3);
	ImGui::RadioButton("DummyEffect", &m_iChooseRadio, 4);

	switch (m_iChooseRadio)
	{
	case 0:
	{
		ImGui::Begin("Particle", nullptr, m_WindowFlag);
		if (nullptr != m_pDummyParticle)
		{
			m_pDummyParticle->Tick_Imgui(_fTimeDelta);
			m_pDummyParticle->Tick(_fTimeDelta);
			m_pDummyParticle->Late_Tick(_fTimeDelta);
		}
		ImGui::End();
	}
	break;
	case 1:
	{
		ImGui::Begin("MeshEffect", nullptr, m_WindowFlag);
		if (nullptr != m_pDummyMeshEffect)
		{
			m_pDummyMeshEffect->Tick_Imgui(_fTimeDelta);
			m_pDummyMeshEffect->Tick(_fTimeDelta);
			m_pDummyMeshEffect->Late_Tick(_fTimeDelta);
		}
		ImGui::End();
	}
	break;
	case 2:
	{
		ImGui::Begin("Trail", nullptr, m_WindowFlag);
		if (nullptr != m_pDummyTrail)
		{
			m_pDummyTrail->Tick_Imgui(_fTimeDelta);
			m_pDummyTrail->Tick(_fTimeDelta);
			m_pDummyTrail->Late_Tick(_fTimeDelta);
		}
		ImGui::End();
	}
	break;
	case 3:
	{
		ImGui::Begin("Flipbook", nullptr, m_WindowFlag);
		if (nullptr != m_pDummyFlipbook)
		{
			m_pDummyFlipbook->Tick_Imgui(_fTimeDelta);
			m_pDummyFlipbook->Tick(_fTimeDelta);
			m_pDummyFlipbook->Late_Tick(_fTimeDelta);
		}
		ImGui::End();
	}
	break;
	case 4:
	{
		ImGui::Begin("DummyEffect", nullptr, m_WindowFlag);
		if (nullptr != m_pDummyEffect)
		{
			m_pDummyEffect->Tick_Imgui(_fTimeDelta);
			m_pDummyEffect->Tick(_fTimeDelta);
			m_pDummyEffect->Late_Tick(_fTimeDelta);
		}
		ImGui::End();
	}
	break;
	}
	//MeshEffect(_fTimeDelta);
	//ImGui::ShowDemoWindow();
}
void CEffect_Window::MeshEffect(_float _fTimeDelta)
{
	//RECT clientRect;
	//GetClientRect(g_hWnd, &clientRect);
	//POINT leftTop = { clientRect.left, clientRect.top };
	//POINT rightBottom = { clientRect.right, clientRect.bottom };
	//ClientToScreen(g_hWnd, &leftTop);
	//ClientToScreen(g_hWnd, &rightBottom);
	//int Left = leftTop.x;
	//int Top = rightBottom.y;
	//ImVec2 vWinpos = { _float(Left + 500.f), _float(Top) };
	//ImGui::SetNextWindowPos(vWinpos);
	//
	//m_pDummyMeshEffect->Tick(_fTimeDelta);
	//m_pDummyMeshEffect->Late_Tick(_fTimeDelta);

	//ImGui::Begin("MeshEffect");

	//m_pDummyMeshEffect->ImGui(_fTimeDelta);
	// 
	//ImGui::End();
}
void CEffect_Window::Create_Button()
{
	if (ImGui::Button("New Particle"))
	{
		Safe_Release(m_pDummyParticle);
		m_pDummyParticle = CDummyParticle::Create(m_pDevice, m_pContext, TEXT(""));
	}
}
_bool CEffect_Window::Table_DragFloat(string strName, string _strTag, _float* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	//const static string strTag = Generate_Hashtag(true);
	isResult = ImGui::DragFloat(strTag.data(), _pValue, _fDragSpeed, _fMin, _fMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
	return isResult;
}
_bool CEffect_Window::Table_DragFloat2(string _strName, string _strTag, _float2* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::DragFloat2(strTag.data(), (_float*)_pValue, _fDragSpeed, _fMin, _fMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
	return isResult;
}
_bool CEffect_Window::Table_DragFloat2Range(string _strName, string _strTag, _float2* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::DragFloatRange2(strTag.data(), &_pValue->x, &_pValue->y, _fDragSpeed, _fMin, _fMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
	return isResult;
}
_bool CEffect_Window::Table_CheckBox(string _strName, string _strTag, _bool* _pValue, _bool isImplement)
{
	_bool isResult = false;
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::Checkbox(strTag.data(), _pValue);
	if (false == isImplement)
	{
		ImGui::SameLine(); ImGui::Text("Not Imple");
	}
	ImGui::TableNextRow();
	return isResult;
}
_bool CEffect_Window::Table_ColorEdit4(string _strName, string _strTag, _float4* pValue)
{
	_bool isResult = false;
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::ColorEdit4(strTag.data(), reinterpret_cast<_float*>(pValue)); ImGui::TableNextRow();
	return isResult;
}
_bool CEffect_Window::Table_ColorEdit3(string _strName, string _strTag, _float3* pValue)
{
	_bool isResult = false;
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::ColorEdit3(strTag.data(), reinterpret_cast<_float*>(pValue)); ImGui::TableNextRow();
	return isResult;
}
_bool CEffect_Window::Table_ImageButton(string _strName, string _strTag, CImageFileDialog* _pImageFileDialog)
{
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	_pImageFileDialog->Tick(); ImGui::TableNextRow();
	return true;
}
_bool CEffect_Window::Table_Void()
{
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(" ");
	ImGui::TableSetColumnIndex(1);
	ImGui::Text(" "); ImGui::TableNextRow();
	return true;
}
_bool CEffect_Window::Table_DragFloatWithOption(string _strName, string _strTag, _float* _pValue, _float2* _pRangeValue, _bool* _pCheckBox, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	_bool isResult = false;
	string strTag = "##" + _strTag;
	string strCheckBoxTag = strTag + "CheckBox";
	ImGui::PushItemWidth(m_fWidgetSize);
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	if (true == *_pCheckBox)
	{
		string strRangeTag = strTag + "Range";
		isResult = ImGui::DragFloatRange2(strTag.data(), &_pRangeValue->x, &_pRangeValue->y, _fDragSpeed, _fMin, _fMax);
	}
	else
	{
		string strFloatTag = strTag + "Float";
		isResult = ImGui::DragFloat(strTag.data(), _pValue, _fDragSpeed, _fMin, _fMax);
	}

	ImGui::SameLine(); ImGui::Checkbox(strCheckBoxTag.data(), _pCheckBox);
	ImGui::PopItemWidth();
	ImGui::TableNextRow();
	return isResult;
}

_bool CEffect_Window::Table_DragXYZ(string _strName, string _strTag, _float3* pValue, _float _fDragSpeed, _float _fMin, _float _fMax, _bool isImplement)
{
	string strTagX = "##" + _strTag + "X"; // X, Y, Z를 구분하도록 수정
	string strTagY = "##" + _strTag + "Y";
	string strTagZ = "##" + _strTag + "Z";
	_bool isResult[3] = {};
	ImVec2 textSize = ImGui::CalcTextSize("X");
	_float textWidth = textSize.x;

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data());
	ImGui::TableSetColumnIndex(1);

	ImGui::PushItemWidth(40.0f);  // DragFloat의 길이를 60으로 설정

	ImGui::Text("X"); ImGui::SameLine();
	isResult[0] = ImGui::DragFloat(strTagX.data(), &pValue->x, _fDragSpeed, _fMin, _fMax); ImGui::SameLine();

	ImGui::Text("Y"); ImGui::SameLine();
	isResult[1] = ImGui::DragFloat(strTagY.data(), &pValue->y, _fDragSpeed, _fMin, _fMax); ImGui::SameLine();

	ImGui::Text("Z"); ImGui::SameLine();
	isResult[2] = ImGui::DragFloat(strTagZ.data(), &pValue->z, _fDragSpeed, _fMin, _fMax); ImGui::SameLine();

	ImGui::PopItemWidth();  // 설정을 원래대로 돌려놓습니다

	if (false == isImplement)
	{
		ImGui::SameLine(); ImGui::Text("Not Imple");
	}

	ImGui::TableNextRow();

	if (isResult[0] || isResult[1] || isResult[2])
		return true;

	return false;
}
_bool CEffect_Window::Table_DragXYZRange(string _strName, string _strTag, _float3* pValueMin, _float3* pValueMax, _float _fDragSpeed, _float _fMin, _float _fMax, _bool isImplement)
{
	_bool isMinResult = { false };
	_bool isMaxResult = { false };
	string strMinName = _strName + " Min";
	string strMaxName = _strName + " Max";

	//isMinResult = Table_DragXYZ(_strName.data());
	//isMaxResult = Table_DragXYZ(_strName.data());
	return _bool();
}
_bool CEffect_Window::Table_DragInt(string _strName, string _strTag, _int* _pValue, _float _fDragSpeed, _int _iMin, _int _iMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::DragInt(strTag.data(), _pValue, _fDragSpeed, _iMin, _iMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();

	return isResult;
}
_bool CEffect_Window::Table_DragInt2(string _strName, string _strTag, _int2* _pValue, _float _fDragSpeed, _int _iMin, _int _iMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::DragInt2(strTag.data(), (_int*)_pValue, _fDragSpeed, _iMin, _iMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
	return isResult;
}
_bool CEffect_Window::Table_DragInt2Range(string _strName, string _strTag, _int2* _pValue, _float _fDragSpeed, _int _iMin, _int _iMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	if(0 == _iMax)
		_iMax = 1;
	isResult = ImGui::DragIntRange2(strTag.data(), &_pValue->x, &_pValue->y, _fDragSpeed, _iMin, _iMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
	return isResult;
}
CEffect_Window* CEffect_Window::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	CEffect_Window* pInstance = New CEffect_Window(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_vWindowPos, _vWindowSize)))
	{
		MSG_BOX("Failed Create CEffect_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
void CEffect_Window::Free(void)
{
	__super::Free();

	Safe_Release(m_pDummyParticle);
	Safe_Release(m_pDummyMeshEffect);
	Safe_Release(m_pDummyTrail);
	Safe_Release(m_pDummyFlipbook);
	Safe_Release(m_pDummyEffect);
}