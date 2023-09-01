#pragma once
#include "ImWindow.h"

BEGIN(Engine)
class CLight;
END

BEGIN(Tool)

class CLight_Window final : public CImWindow
{
public:
	

private:
	CLight_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight_Window() = default;


public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Save_Light();
	void Load_Light();

	HRESULT Create_Light();
	void Clear_Light();

	void Light_ComboBox();

private:
	int m_iLightType = 0;
	_int m_iLightIndex = { 0 };

	_bool m_isSetting = false;
	string m_szName;
	string StrInput;
	//char* pInput;
private:
	vector<CLight::LIGHTDESC> m_vecLightDesc;
	vector<string> m_vecLightList;
	
public:
	static CLight_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free() override;
};

END