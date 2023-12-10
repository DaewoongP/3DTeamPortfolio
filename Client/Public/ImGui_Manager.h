#pragma once
#ifdef _DEBUG
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
private:
	explicit CImGui_Manager() = default;
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Render();

public:
	HRESULT Add_Function(function<void()> Function);
	void Clear_Function();

	static void NextWindow_LeftBottom();
	static void MatrixImgui(_float4x4& pMatrix, const _char* pNodeName);

private:
	_float4					m_vImGuiClearColor;
	ID3D11Texture2D*		m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pImGuiRTV = { nullptr };
	list<function<void()>>	m_Functions;
	_bool					m_isRender = { false };

public:
	virtual void Free() override;
};

END

#define ADD_IMGUI(_func)			CImGui_Manager::GetInstance()->Add_Function(_func);
#define CLEAR_IMGUI_FUNCTION		CImGui_Manager::GetInstance()->Clear_Function();
#endif // _DEBUG