#include "..\Public\DummyTrail.h"
#include "GameInstance.h"
#include "Effect_Window.h"
#include "Window_Manager.h"
#include "ComboBox.h"
#include "ImageFileDialog.h"

CDummyTrail::CDummyTrail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTrail(_pDevice, _pContext)
{
}
CDummyTrail::CDummyTrail(const CDummyTrail& _rhs)
	: CTrail(_rhs)
{
}
HRESULT CDummyTrail::Initialize_Prototype(const _tchar* _pDirectoryPath, _uint _iLevel)
{
	if (FAILED(__super::Initialize_Prototype(_pDirectoryPath, _iLevel)))
		return E_FAIL;

	return S_OK;
}
HRESULT CDummyTrail::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_pTextureIFD = CImageFileDialog::Create(m_pDevice, Generate_Hashtag(true).data());
	m_pTextureIFD->m_strStartPath = "../../Resources/UI/UI/Game/VFX/Textures";
	m_pTextureIFD->m_iImageButtonWidth = 32;

	vector<string> Passes = m_pShader->Get_PassList();
	m_pPassComboBox = CComboBox::Create(Generate_Hashtag(true).data(), "Pass", Passes, Passes.front().data());

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

void CDummyTrail::Tick_Imgui(_float _fTimeDelta)
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = dynamic_cast<CEffect_Window*>(pWindow);
	if (ImGui::BeginTable("CDummyTrailTable", 2))
	{
		ImGui::TableNextRow();
	
		pEffectWindow->Table_ImageButton("Texture", "cjueruwrckqq", m_pTextureIFD);
		// 메인 텍스처 교체
		if (m_pTextureIFD->IsOk())
		{
			fs::path fsFilePath = m_pTextureIFD->Get_FilePathName();
			ChangeTexture(&m_pTexture, m_wstrPath, ToRelativePath(fsFilePath.wstring().data()).c_str());
		}

		// 패스 선택
		m_strPass = m_pPassComboBox->Tick(CComboBox::TABLE, true);

		// 색깔 선택
		ImGui::Separator();
		pEffectWindow->Table_ColorEdit3("Head Color", "dpiv92490v", &m_vHeadColor);
		pEffectWindow->Table_ColorEdit3("Tail Color", "cklvklcvxjoiowe", &m_vTailColor);
		ImGui::Separator();

		// 너비 선택
		if (pEffectWindow->Table_DragFloat("Width", "xcvklje8oi", &m_fWidth))
		{
			m_HighLocalMatrix.Translation(_float3(0.f, m_fWidth, 0.f));
			m_LowLocalMatrix.Translation(_float3(0.f, -m_fWidth, 0.f));
		}

		pEffectWindow->Table_DragFloat("Duration", "cvixcviuoiwe", &m_fTailDuration, 0.01f, 0.f, FLT_MAX);

		// 시간 선택

		// 최소 거리 선택

		

		ImGui::EndTable();
	}

	// 왼쪽 아래로 고정
	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	POINT leftTop = { clientRect.left, clientRect.top };
	POINT rightBottom = { clientRect.right, clientRect.bottom };
	ClientToScreen(g_hWnd, &leftTop);
	ClientToScreen(g_hWnd, &rightBottom);
	int Left = leftTop.x;
	int Top = rightBottom.y;
	ImVec2 vWinpos = { _float(Left + 0.f), _float(Top) };
	ImGui::SetNextWindowPos(vWinpos);
	_float4x4 WorldMatirx = Get_Transform()->Get_WorldMatrix();
	pEffectWindow->MatrixNode(&WorldMatirx, "Trail_Transform", "Trail_Position", "Trail_Rotation", "Trail_Scale");
	Get_Transform()->Set_WorldMatrix(WorldMatirx);
}

void CDummyTrail::ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath)
{
	// 소유하고 있는 컴포넌트를 지운다.
	_tchar wszTag[MAX_STR];
	lstrcpy(wszTag, (*_pTexture)->Get_Tag());

	if (FAILED(CComposite::Delete_Component(wszTag)))
	{
		MSG_BOX("Failed to Delete");
		E_FAIL;
	}

	Safe_Release(*_pTexture);

	BEGININSTANCE;
	// 찾는 컴포넌트가 없으면 원본을 만들어준다.
	wstring tempTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), _pDestPath).data();
	_tchar* pTag = { nullptr };
	m_pTags.push_back(pTag = new _tchar[tempTag.length() + 1]);
	lstrcpy(pTag, tempTag.c_str());

	// 기존에 컴포넌트가 존재하는지 확인
	if (nullptr == pGameInstance->Find_Prototype(LEVEL_TOOL, pTag))
	{
		// 없다면 새로운 프로토타입 컴포넌트 생성
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, pTag, CTexture::Create(m_pDevice
			, m_pContext, _pDestPath))))
		{
			MSG_BOX("Failed to Create Prototype : Change Texture");
		}
	}

	// 새로운 텍스처 컴포넌트를 클론한다.

	_wstrOriginPath = _pDestPath;
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, pTag
		, wszTag, reinterpret_cast<CComponent**>(_pTexture))))
	{
		MSG_BOX("Failed to Add Component");
		ENDINSTANCE;
		return;
	}
	ENDINSTANCE;
}

void CDummyTrail::RemakeBuffer()
{
	TRAILDESC trailDesc;
	trailDesc.iTrailNum = m_iTrailNum;
	trailDesc.pHighLocalMatrix = &m_HighLocalMatrix;
	trailDesc.pLowLocalMatrix = &m_LowLocalMatrix;
	trailDesc.pPivotMatrix = &m_PivotMatrix;
	trailDesc.pWorldMatrix = m_pTransform->Get_WorldMatrixPtr();
	m_pBuffer->Initialize(&trailDesc);
}

CDummyTrail* CDummyTrail::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath, _uint _iLevel)
{
	CDummyTrail* pInstance = New CDummyTrail(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pDirectoryPath, _iLevel)))
	{
		MSG_BOX("Faild to Created CDummyTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CGameObject* CDummyTrail::Clone(void* _pArg)
{
	CDummyTrail* pInstance = New CDummyTrail(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Faild to Created CDummyTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CDummyTrail::Free(void)
{
	__super::Free();

	Safe_Release(m_pTextureIFD);
	Safe_Release(m_pPassComboBox);

	for (auto& pTag : m_pTags)
		Safe_Delete_Array(pTag);
}