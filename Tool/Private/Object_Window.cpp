#include "..\Public\Object_Window.h"

#include "Terrain.h"
#include "VIBuffer_Terrain.h"

CObject_Window::CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CObject_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CObject_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Object", nullptr, m_WindowFlag);

	_float3 vPos = Find_PickingPos(nullptr);

	// 현재 피킹 위치 표시
	ImGui::Text("Picking Position");
	ImGui::Text("%.1f /", vPos.x);
	ImGui::SameLine();
	ImGui::Text("%.1f /", vPos.y);
	ImGui::SameLine();
	ImGui::Text("%.1f", vPos.z);
	ImGui::Text("----------------------------------------");

	ImGui::End();
}

HRESULT CObject_Window::Render()
{
	return S_OK;
}

_float3 CObject_Window::Find_PickingPos(CVIBuffer* pVIBuffer)
{
	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	BEGININSTANCE; pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir); 

	CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
		static_cast<CTerrain*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), 
			TEXT("GameObject_Terrain")))->Get_Buffer()); ENDINSTANCE;

	_float fDist = FLT_MAX;
	m_fDist = FLT_MAX;

	_uint ivtxcntX = pTerrain->Get_TerrainSizeX();
	_uint ivtxcntZ = pTerrain->Get_TerrainSizeZ();

	const _float3* pTerrainVtxPos = pTerrain->Get_PosArray();

	for (_ulong j = 0; j < ivtxcntZ - 1; ++j)
	{
		for (_ulong i = 0; i < ivtxcntX - 1; ++i)
		{
			_ulong dwIndex = i * ivtxcntX + j;

			_ulong		iIndices[4] = {
				dwIndex + ivtxcntX,
				dwIndex + ivtxcntX + 1,
				dwIndex + 1,
				dwIndex
			};

			if (TriangleTests::Intersects(vRayPos, vRayDir,
				pTerrainVtxPos[iIndices[0]], pTerrainVtxPos[iIndices[1]], pTerrainVtxPos[iIndices[2]],
				fDist))
			{
				if (m_fDist > fDist)
					m_fDist = fDist;
			}

			if (TriangleTests::Intersects(vRayPos, vRayDir,
				pTerrainVtxPos[iIndices[0]], pTerrainVtxPos[iIndices[2]], pTerrainVtxPos[iIndices[3]],
				fDist))
			{
				if (m_fDist > fDist)
					m_fDist = fDist;
			}
		}
	}

	if (FLT_MAX > m_fDist)
	{
		_float4 vFinalPos;

		vRayDir *= m_fDist;
		vFinalPos = vRayPos + vRayDir;

		return _float3(vFinalPos.x , vFinalPos.y, vFinalPos.z);
	}

	return _float3(-1.f, -1.f, -1.f);
}

CObject_Window* CObject_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CObject_Window* pInstance = New CObject_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CObject_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CObject_Window::Free(void)
{
	__super::Free();
}
