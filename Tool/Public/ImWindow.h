#pragma once
#include "Base.h"
#include "GameInstance.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CImWindow abstract : public CBase
{
protected:
	explicit CImWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImWindow() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

	void MatrixNode(_float4x4* pMatrix, const _char* pNodeName, const _char* pPosTag, const _char* pRotTag, const _char* pScaleTag, _float fSpeed = 0.01f);

protected:
	CGameInstance*			m_pGameInstance = { nullptr };
	ImGuiWindowFlags		m_WindowFlag;
	ImVec2                  m_vWindowPos;
	ImVec2                  m_vWindowSize;
	
protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	virtual void Free(void);
};

END