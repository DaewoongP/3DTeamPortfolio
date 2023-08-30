#include "..\Public\Object_Window.h"

#include "Terrain.h"
#include "VIBuffer_Terrain.h"
#include "CDummy.h"

CObject_Window::CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CObject_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	if (FAILED(Create_Dummy()))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CObject_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// 한번만 QUADTERRAIN 값들 초기화
	if (true == m_isInitQuadTerrain)
	{
		InitializeQuadTerrain();
		m_isInitQuadTerrain = false;
	}

	ImGui::Begin("Object", nullptr, m_WindowFlag);

	_float3 vPos = Find_PickingPos();
	//_float3 vPos = Find_PickingPosTest();

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

_float3 CObject_Window::Find_PickingPos()
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

		return _float3(vFinalPos.x, vFinalPos.y, vFinalPos.z);
	}

	return _float3(-1.f, -1.f, -1.f);
}

_float3 CObject_Window::Find_PickingPosTest()
{
	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	BEGININSTANCE; pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

	CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
		static_cast<CTerrain*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
			TEXT("GameObject_Terrain")))->Get_Buffer()); ENDINSTANCE;

	_float fDist = FLT_MAX;
	m_fDist = FLT_MAX;

	_uint ivtxcntX = pTerrain->Get_TerrainSizeX() - 1;
	_uint ivtxcntZ = pTerrain->Get_TerrainSizeZ() - 1;

	const _float3* pTerrainVtxPos = pTerrain->Get_PosArray();

	for (_ulong j = m_TerrainZDesc.iStart; j < m_TerrainZDesc.iEnd; ++j)
	{
		for (_ulong i = m_TerrainXDesc.iStart; i < m_TerrainXDesc.iEnd; ++i)
		{
			_ulong dwIndex = i * (m_TerrainXDesc.iEnd + 1) + j;

			_ulong		iIndices[4] = {
				dwIndex + m_TerrainXDesc.iEnd + 1,
				dwIndex + m_TerrainXDesc.iEnd + 2,
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

		// 이전 결과에 따라 다음 순환할 범위를 정해줌
		// X : 0 ~ iCenter, Z : 0 ~ iCenter
		if (m_TerrainXDesc.iCenter >= vFinalPos.x && m_TerrainZDesc.iCenter >= vFinalPos.z)
		{
			m_TerrainXDesc.iStart = m_TerrainXDesc.iOriginStart;
			m_TerrainXDesc.iEnd = m_TerrainXDesc.iCenter;
			m_TerrainZDesc.iStart = m_TerrainZDesc.iOriginStart;
			m_TerrainZDesc.iEnd = m_TerrainZDesc.iCenter;
		}

		// X : iCenter ~ ivtxcnt, Z : 0 ~ iCenter
		else if (m_TerrainZDesc.iCenter < vFinalPos.z && m_TerrainZDesc.iCenter >= vFinalPos.z)
		{
			m_TerrainXDesc.iStart = m_TerrainXDesc.iCenter;
			m_TerrainXDesc.iEnd = m_TerrainXDesc.iEnd;
			m_TerrainZDesc.iStart = m_TerrainZDesc.iOriginStart;
			m_TerrainZDesc.iEnd = m_TerrainZDesc.iCenter;
		}

		// X : 0 ~ iCenter, Z : iCenter ~ ivtzcnt
		else if (m_TerrainXDesc.iCenter >= vFinalPos.x && m_TerrainZDesc.iCenter < vFinalPos.z)
		{
			m_TerrainXDesc.iStart = m_TerrainXDesc.iOriginStart;
			m_TerrainXDesc.iEnd = m_TerrainXDesc.iCenter;
			m_TerrainZDesc.iStart = m_TerrainZDesc.iCenter;
			m_TerrainZDesc.iEnd = m_TerrainZDesc.iEnd;;
		}

		// X : iCenter ~ ivtxcnt, Z : iCenter ~ ivtzcnt
		else if (m_TerrainXDesc.iCenter < vFinalPos.x && m_TerrainZDesc.iCenter < vFinalPos.z)
		{
			m_TerrainXDesc.iStart = m_TerrainXDesc.iCenter;
			m_TerrainXDesc.iEnd = m_TerrainXDesc.iEnd;
			m_TerrainZDesc.iStart = m_TerrainZDesc.iCenter;
			m_TerrainZDesc.iEnd = m_TerrainZDesc.iEnd;;
		}

		return _float3(vFinalPos.x, vFinalPos.y, vFinalPos.z);
	}

	return _float3(-1.f, -1.f, -1.f);
}

HRESULT CObject_Window::Create_Dummy()
{
	_float3 vPos = { 5.f, 0.f, 5.f };

	BEGININSTANCE; if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Prototype_GameObject_MapDummy"), 
		TEXT("Layer_Tool"), TEXT("Map_Dummy"), &vPos)))
	{
		MSG_BOX("Failed to GameObject Map_Dummy");
		return E_FAIL;
	}

	m_pDummy = static_cast<CDummy*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("Map_Dummy")));

	_float4x4 PivotMatrix = XMMatrixIdentity();

	if (FAILED(pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Tree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
			TEXT("../../Resources/Models/NonAnims/Tree/Tree.dat"), PivotMatrix))))
		return E_FAIL;

	m_pDummy->Add_Model_Component(TEXT("Prototype_Component_Model_Tree"));
	m_pDummy->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh")); ENDINSTANCE;

	return S_OK;
}

void CObject_Window::InitializeQuadTerrain()
{
	// 값 초기화
	BEGININSTANCE; CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
		static_cast<CTerrain*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
			TEXT("GameObject_Terrain")))->Get_Buffer()); ENDINSTANCE;

	_uint ivtxcntX = pTerrain->Get_TerrainSizeX() - 1;
	_uint ivtxcntZ = pTerrain->Get_TerrainSizeZ() - 1;

	m_TerrainXDesc.iOriginStart = 0;
	m_TerrainXDesc.iOriginEnd = ivtxcntX;
	m_TerrainXDesc.iStart = 0;
	m_TerrainXDesc.iEnd = ivtxcntX;
	m_TerrainXDesc.iCenter = (m_TerrainXDesc.iOriginStart + m_TerrainXDesc.iOriginEnd) >> 1;

	m_TerrainZDesc.iOriginStart = 0;
	m_TerrainZDesc.iOriginEnd = ivtxcntZ;
	m_TerrainZDesc.iStart = 0;
	m_TerrainZDesc.iEnd = ivtxcntZ;
	m_TerrainZDesc.iCenter = (m_TerrainZDesc.iOriginStart + m_TerrainZDesc.iOriginEnd) >> 1;
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
