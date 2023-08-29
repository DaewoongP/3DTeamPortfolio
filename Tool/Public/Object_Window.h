#pragma once
#include "ImWindow.h"

BEGIN(Tool)

class CObject_Window final : public CImWindow
{
private:
	explicit CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CObject_Window() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float3 Find_PickingPos(class CVIBuffer* pVIBuffer); // 지형의 피킹 위치를 찾는 함수

	_float m_fDist = { FLT_MAX }; // Find_PickingPos에서 사용하는 거리 변수

public:
	static CObject_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free(void) override;
};

END