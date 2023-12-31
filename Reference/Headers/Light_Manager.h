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
	/* 다중 그림자 처리용 함수들입니다. */
	_uint Get_CurrentLightShadowNum();
	const _bool* Get_IsShadowRender() const { return m_isShadowRender; }
	_float4x4* Get_LightViewMatrix(_uint iIndex) { return &m_LightViewMatrix[iIndex]; }
	_float4x4* Get_LightProjMatrix(_uint iIndex) { return &m_LightProjMatrix[iIndex]; }
	const CLight::LIGHTDESC* Get_ShadowLightDesc(_uint iIndex);
	void Set_IsShadowRender(_uint iShadowIndex, _bool isRender) { m_isShadowRender[iShadowIndex] = isRender; }
	///////////////////////////////

public:
	HRESULT Add_Light(const CLight::LIGHTDESC& LightDesc, _Inout_ class CLight** ppLight = nullptr, _bool isShadow = false, _uint iLightViewIndex = 0, _float fAspect = 1280.f / 720.f);
	// PointLight only
	HRESULT Add_InstanceLight(_float3 vPos, _float fStartRange, _float fTime, _float4 vColor, _bool isIncrease = false, _float fIncreasePower = 0.f);
	HRESULT Clear_Lights();

public:
	HRESULT Reserve_Lights(_uint iNumReserve);
	HRESULT Return_Light(class CLight* pLight);

public: /* 그림자 */
	HRESULT Update_ShadowMatrix(_uint iShadowIndex, CLight::LIGHTDESC LightDesc);

public:
	void Tick(_float fTimeDelta);

public: /* Deferred */
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	CLight* Create_Light(const CLight::LIGHTDESC& LightDesc);

private:
	list<class CLight*>			m_Lights;
	queue<class CLight*>		m_LightPool;

private:
	list<class CLight*>			m_InstanceLights;

private: /* 그림자 처리 변수 */
	_float4x4					m_LightViewMatrix[MAX_SHADOW];
	_float4x4					m_LightProjMatrix[MAX_SHADOW];
	class CLight*				m_pShadowLights[MAX_SHADOW] = { nullptr };
	_bool						m_isShadowRender[MAX_SHADOW] = { false };
	_float						m_fAspect = { 0.f };

public:
	virtual void Free() override;
};

END