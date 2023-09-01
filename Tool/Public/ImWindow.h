#pragma once
#include "Base.h"
#include "GameInstance.h"
#include "Tool_Defines.h"

BEGIN(Tool)

static bool VectorGetter(void* data, int idx, const char** out_str)
{
	auto& vector = *static_cast<std::vector<std::string>*>(data);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_str = vector.at(idx).c_str();
	return true;
}

static bool VectorGetterW(void* data, int idx, const char** out_str)
{
	auto& vector = *static_cast<std::vector<std::wstring>*>(data);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_str = vector.at(idx).c_str();
	return true;
}

class CImWindow abstract : public CBase
{
protected:
	explicit CImWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImWindow() = default;

public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	CGameInstance*			m_pGameInstance = { nullptr };
	ImGuiWindowFlags		m_WindowFlag;
	ImVec2                  m_vWindowPos;
	ImVec2                  m_vWindowSize;

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	virtual void Free(void) override;
};

END