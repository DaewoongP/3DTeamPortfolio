#pragma once
#include "Light.h"
#define			MAX_SHADOW		2

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager() = default;
	virtual ~CLight_Manager() = default;

public:
	_uint Get_LightShadowNum() { return m_iBindedLightMatrices; }
	_float4x4* Get_LightViewMatrix(_uint iIndex) { return &m_LightViewMatrix[iIndex]; }
	_float4x4* Get_LightProjMatrix(_uint iIndex) { return &m_LightProjMatrix[iIndex]; }
	
public:
	HRESULT Reserve_Lights(_uint iNumReserve);
	CLight* Add_Lights(const CLight::LIGHTDESC& LightDesc, _bool isShadow = false, _uint iLightViewIndex = 0, _float fAspect = 1280.f / 720.f);
	HRESULT Clear_Lights();
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	CLight* Create_Light(const CLight::LIGHTDESC& LightDesc);

private:
	list<class CLight*>			m_Lights;
	queue<class CLight*>		m_LightPool;
	_float4x4					m_LightViewMatrix[MAX_SHADOW];
	_float4x4					m_LightProjMatrix[MAX_SHADOW];
	class CLight*				m_pShadowLights[MAX_SHADOW] = { nullptr };
	_uint						m_iBindedLightMatrices = { 0 };

public:
	virtual void Free() override;
};

END