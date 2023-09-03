#pragma once
#include "ImWindow.h"
#include "Light.h"

BEGIN(Tool)

class CLight_Window final : public CImWindow
{
	typedef struct Lightinfo 
	{
		_float vPos[4]		;
		_float vDir[4] 		;
		_float fRange[1] 	;
		_float fSpotPower[1];
		_float vDiffuse[4] 	;
		_float vAmbient[4] 	;
		_float vSpecular[4] ;
	}LIGHTINFO;

private:
	CLight_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight_Window() = default;


public:
	virtual HRESULT Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	

	HRESULT Create_Light();
	HRESULT Save_Light();
	HRESULT Load_Light();
	void Set_Tool_Light(_uint iIndex, CLight::LIGHTDESC LightDesc);
	void Delete_Light(_uint iIndex,const _char* pName);
	void Clear_Light();
	void ResetValue();
	void Light_ComboBox();
	void FloatToFloat4(_float* Input, _float4 Out); // float 배열에서 float4로변환
private:
	int m_iLightType = 0;
	_int m_iLightIndex;
	_int m_iCurrent_LightIndex;

	_bool m_isSetting = false;
	string m_szName;
	string StrInput;
	_uint m_iCurrent_Idx = 0;

	_bool m_isCurrent_Idx;
	_bool m_isRender = true;
	_char AddLightName[MAX_PATH] = {};
	_bool m_isCheck = false;
private:
	vector<CLight::LIGHTDESC> m_vecLightDesc;
	vector<string> m_vecLightList;
	_float vPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_float vDir[4] = { 1.f, -1.f, 1.f, 0.0f };
	_float fRange[1] = { 0.f };
	_float fSpotPower[1] = { 0.f };
	_float vDiffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_float vAmbient[4] = { 1.f, 1.f, 1.f, 1.0f };
	_float vSpecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	CLight::LIGHTDESC LightDesc;
	LIGHTINFO m_SaveDesc;
	

public:
	static CLight_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free() override;
};

END