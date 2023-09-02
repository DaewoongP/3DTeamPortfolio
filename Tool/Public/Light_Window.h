#pragma once
#include "ImWindow.h"

BEGIN(Engine)
class CLight;
END

BEGIN(Tool)

class CLight_Window final : public CImWindow
{
public:
	enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOTLIGHT, TYPE_END };

	typedef struct Value
	{
		CLight::TYPE	eType;
		_float4	vDir;
		_float4	vPos;
		_float	fRange;
		_float	fSpotPower;

		_float4	vDiffuse;
		_float4	vAmbient;
		_float4	vSpecular;
	}VALUE;

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
	void ResetValue();
	void Light_ComboBox();
	void FloatToFloat4(_float* Input, _float4 Out); // float 배열에서 float4로변환
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
	//list<string, CLight::LIGHTDESC> m_ListLight;
	VALUE LightInfo;
public:
	static CLight_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free() override;
};

END