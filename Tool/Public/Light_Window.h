#pragma once
#include "ImWindow.h"
#include "Light.h"
#include"Collider.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
class CTransform;
END


BEGIN(Tool)
class CMapObject;
class CLightDot;
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
	_uint Get_LightIndex() { return m_iLightIndex; }


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
	void Set_LightInfo();//빛안의 정보들 수정할때 넣을값들
	void FloatToFloat4(_float* Input, _float4 Out); // float 배열에서 float4로변환
	void Float4ToFloat(_float* Input, _float4 Out);//이름반대로댐
	_float3 Find_PickPos();
	void Picking_Menu(CLight::LIGHTDESC LightDesc);

private:
	int m_iLightType = 0;
	_int m_iLightIndex;
	_int m_iCurrent_LightIndex;

	_bool m_isSetting = false;
	string m_szName;
	string StrInput;
	_uint m_iCurrent_Idx = 0;

	_bool m_isHold = true;
	_bool m_isCurrent_Idx;
	_bool m_isRender = true;
	_char AddLightName[MAX_PATH] = {};
	_bool m_isCheck = false;
	_bool m_isCheckPicking=false;
	_bool m_isCursorIn;
private:
	vector<CLight::LIGHTDESC> m_vecLightDesc;
	vector<string> m_vecLightList;
	_float vPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_float3 m_vPickPos = { 0.f,0.f,0.f };
	_float vDir[4] = { 1.f, -1.f, 1.f, 0.0f };
	_float fRange[1] = { 0.f };
	_float fSpotPower[1] = { 0.f };
	_float vDiffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_float vAmbient[4] = { 1.f, 1.f, 1.f, 1.0f };
	_float vSpecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	CLight::LIGHTDESC LightDesc;
	LIGHTINFO m_SaveDesc;
	

private://Engine
	CTransform* m_pTrnasform = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CCollider* m_pCollider = { nullptr };

private://tool

	CMapObject* m_pObject = { nullptr };
	CLightDot* m_pLightDot = { nullptr};


public:
	static CLight_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize);
	virtual void Free() override;
};

END