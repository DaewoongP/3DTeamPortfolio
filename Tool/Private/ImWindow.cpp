#include "..\Public\ImWindow.h"

CImWindow::CImWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImWindow::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	m_vWindowPos = vWindowPos;
	m_vWindowSize = vWindowSize;
	
	return S_OK;
}

void CImWindow::Tick(_float fTimeDelta)
{
	RECT rc;
	GetWindowRect(g_hWnd, &rc);
	
	ImGui::SetNextWindowPos(ImVec2(_float(rc.left), _float(rc.top)) + m_vWindowPos);
	ImGui::SetNextWindowSize(m_vWindowSize);
}

HRESULT CImWindow::Render()
{
	return S_OK;
}

void CImWindow::MatrixNode(_float4x4* pMatrix, const _char* pNodeName, const _char* pPosTag, const _char* pRotTag, const _char* pScaleTag, _float fSpeed)
{
	if (nullptr == pMatrix || nullptr == pNodeName 
		|| nullptr == pPosTag || nullptr == pScaleTag || nullptr == pRotTag)
		return;

	ImGui::Begin(pNodeName);
	// ���ڸ� ���ؼ� ������ �±׸� �������.
	string strPositionTag = pPosTag;
	string strRotationTag = pRotTag;
	string strScaleTag = pScaleTag;
	string strPositionName = "##" + strPositionTag;
	string strRotationName = "##" + strRotationTag;
	string strScaleName = "##" + strScaleTag;
	
	// ���ڷ� ���� ��Ʈ�����κ��� �̵�, �Ÿ�, ũ�⸦ ����.
	_float3 vTranslations = pMatrix->Translation();	
	_float3 vAngles = pMatrix->ToEuler();
	_float3 vScales = _float3(pMatrix->Right().Length(), pMatrix->Up().Length(), pMatrix->Look().Length());

	vAngles.x = XMConvertToDegrees(vAngles.x);
	vAngles.y = XMConvertToDegrees(vAngles.y);
	vAngles.z = XMConvertToDegrees(vAngles.z);

	// �¿찡 ���� �Ǿ� �־����� Ȯ��.
	_float3 vXAxis = pMatrix->Right();
	_float3 vYAxis = pMatrix->Up();
	_float3 vZAxis = pMatrix->Look();
	if (vXAxis.x < 0.f)
	{
		vScales.x = -vScales.x;
	}
	if (vYAxis.y < 0.f)
	{
		vScales.y = -vScales.y;
	}
	if (vZAxis.z < 0.f)
	{
		vScales.z = -vScales.z;
	}

	// ������ �� ����
	ImGui::Text("Position"); ImGui::SameLine();
	ImGui::DragFloat3(strPositionName.data(), reinterpret_cast<_float*>(&vTranslations), fSpeed);
	ImGui::Text("Rotation"); ImGui::SameLine();
	ImGui::DragFloat3(strRotationName.data(), reinterpret_cast<_float*>(&vAngles), fSpeed);
	ImGui::Text("   Scale"); ImGui::SameLine();
	ImGui::DragFloat3(strScaleName.data(), reinterpret_cast<_float*>(&vScales), fSpeed);

	// ȸ������� ����� ���� ���� ��ȯ
	vAngles.x = XMConvertToRadians(vAngles.x);
	vAngles.y = XMConvertToRadians(vAngles.y);
	vAngles.z = XMConvertToRadians(vAngles.z);

	// SRT����
	_float4x4 ScaleMatrix = _float4x4::MatrixScale(vScales);
	_float4x4 RotMatrix = _float4x4::MatrixRotationZ(vAngles.z) * _float4x4::MatrixRotationX(vAngles.x) *_float4x4::MatrixRotationY(vAngles.y);
	_float4x4 TransMatrix = _float4x4::MatrixTranslation(vTranslations);

	// ���ο� ������� ����.
	*pMatrix = ScaleMatrix * RotMatrix * TransMatrix;

	ImGui::End();
}

void CImWindow::Free(void)
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
