#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CMapDummy;

class CObject_Window final : public CImWindow
{
typedef struct QuadTerrainDesc {
	// 이 구조체는 지형을 4분할해서 구역을 나눠 마우스 위치를 기준으로
	// 해당하는 구간의 정점들만 순환하게 만드는 것이 목적
	_uint iOriginStart = { 0 }; // 원래 시작
	_uint iOriginEnd = { 0 };	// 원래 끝
	_uint iStart = { 0 };		// 변경된 시작
	_uint iEnd = { 0 };			// 변겅된 끝
	_uint iCenter = { 0 };		// 변경된 값들의 중앙값
}QUADTERRAIN;

private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float3 Find_PickingPos(); // 지형의 피킹 위치를 찾는 함수
	_float3 Find_PickingPosTest(); // 최적화 테스트 함수
	HRESULT Create_Dummy(); // 최초 1번 Dummy 생성
	void InitializeQuadTerrain(); // 최초 1번 QUADTERRAIN값 초기화하는 함수

private:
	_float m_fDist = { FLT_MAX }; // Find_PickingPos에서 사용하는 거리 비교를 위한 변수

	/* 지형 4분할 최적화 관련 변수*/
	_bool m_isInitQuadTerrain = { true }; // 최초 1번 QUADTERRAIN값 초기화
	QUADTERRAIN m_TerrainXDesc;
	QUADTERRAIN m_TerrainZDesc;

	CMapDummy* m_pDummy = { nullptr }; // 생성해둔 Dummy의 주소

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END