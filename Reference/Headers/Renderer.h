#pragma once
#include "Component.h"
#include "DOF.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CGameObject
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_DEPTH, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND,
					  RENDER_GLOW, RENDER_DISTORTION, RENDER_EDGEHIGHLIGHT, RENDER_RADIALBLUR, RENDER_SCREEN, RENDER_UI,
		// ETC
		RENDER_PICKING, RENDER_BRUSHING, RENDER_UITEXTURE, RENDER_END };

public:
	// Debug & Check
	_float Get_GlowPower() { return m_fGlowPower; }
	void Set_GlowPower(_float fPower) { m_fGlowPower = fPower; }
	_float Get_HDR() { return m_fHDR; }
	void Set_HDR(_float fPower) { m_fHDR = fPower; }
	class CDOF* Get_Dof() { return m_pDOF; }
	void Set_Fog(_bool isFog, _float4 vFogColor, _float3 vFogCenterPos, _float fFogRadius) {
		m_isCircleFog = isFog;
		m_vFogColor = vFogColor;
		m_vFogCenterPos = vFogCenterPos;
		m_fFogRadius = fFogRadius;
	}
	void Set_Fog(_bool isFog, _float4 vFogColor, _float fFogY) {
		m_isYFog = isFog;
		m_vFogColor = vFogColor;
		m_fFogY = fFogY;
	}
	void FadeOut(_float fFadeSpeed, _float4 vFadeColor = { 0.f, 0.f, 0.f, 1.f }) {
		m_vFadeColor = vFadeColor;
		m_isFade = true;
		m_fFadeSpeed = fFadeSpeed;
	}
	void FadeIn(_float fFadeSpeed, _float4 vFadeColor = { 0.f, 0.f, 0.f, 1.f }) {
		m_vFadeColor = vFadeColor;
		m_isFade = true;
		m_fFadeSpeed = fFadeSpeed * -1.f;
	}

public:
	// radial on / off , sample 최대개수 = 10
	_float Get_ScreenRadialBlurWidth() { return m_fRadialBlurWidth; }
	_bool Get_IsScreenRadial() { return m_isScreenRadial; }
	void Set_ScreenRadial(_bool isRadial, _float fRadialWidth) {
		m_isScreenRadial = isRadial;
		m_fRadialBlurWidth = fRadialWidth;
	}
	void Set_ScreenRadial(_bool isRadial, _float fTime, _float fRadialWidth) { 
		m_isScreenRadial = isRadial;
		m_fRadialBlurWidth = fRadialWidth;
		m_fRadialTimeAcc = 0.f;
		m_fRadialTime = fTime;
	}
	void Set_SSAO(_bool isSSAO) { m_isSSAO = isSSAO; }
	_bool Get_SSAO() { return m_isSSAO; }
	void Play_Rain() { m_isRaining = true; }
	void Stop_Rain() { m_isRaining = false; }
	void Set_Night();

public:
	void Defualt_Shading();

private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;

public:
	void	Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

#ifdef _DEBUG
public:
	HRESULT Add_DebugGroup(CComponent* pDebugCom);
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
#ifdef _DEBUG
	HRESULT Render_Picking();
	HRESULT Render_Brushing();
#endif // _DEBUG
	HRESULT Render_Lights();
	HRESULT Render_Depth();
	HRESULT Render_SSAO();
	HRESULT Render_Deferred();

	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_HDR();
	HRESULT Render_PostProcessing();
	HRESULT Render_Distortion();
	HRESULT Render_RadialBlur();
	HRESULT Render_EdgeHighLight();
	HRESULT Render_Screen();
	HRESULT Render_ScreenRadial();
	HRESULT Render_Rain();
	HRESULT Render_LightShaft();
	HRESULT Render_Fade();

	HRESULT Render_UI();

#ifdef _DEBUG
	HRESULT Render_UITexture();
#endif // _DEBUG

private:
	// 알파블렌딩 객체를 그릴때 깊이에 따라 순서 관리를 위한 함수.
	// 카메라 위치를 기준으로 먼것부터 그려 처리함.
	HRESULT Sort_Render(RENDERGROUP eGroup);
	HRESULT Sort_Z(RENDERGROUP eGroup);
	HRESULT Add_Components();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
	HRESULT Render_MRTs();
	void Is_DebugRender();
	_bool Is_MRTRender();
#endif // _DEBUG

private:	
	list<class CGameObject*>		m_RenderObjects[RENDER_END];
#ifdef _DEBUG
private:
	list<class CComponent*>			m_DebugObject;
	_bool							m_isDebugRender = { true };
	_bool							m_isMRTRender = { true };
	_bool							m_isDistortion = { false };
#endif // _DEBUG

private:
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };

private:
	class CVIBuffer_Rect*	m_pRectBuffer = { nullptr };
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	class CShader*	m_pDeferredShader = { nullptr };
	class CShader*	m_pLightShader = { nullptr };
	class CShader*	m_pPostProcessingShader = { nullptr };
	class CShader*	m_pSSAOShader = { nullptr };
	class CShader*	m_pDistortionShader = { nullptr };
	class CShader*	m_pRadialBlurShader = { nullptr };
	class CShader*	m_pFadeShader = { nullptr };
	class CShader*	m_pRainShader = { nullptr };
	class CShader*	m_pEdgeShader = { nullptr };
	class CShader*	m_pLightShaftShader = { nullptr };
	class CTexture*	m_pNoiseTexture = { nullptr };
	class CTexture* m_pFadeTexture = { nullptr };

private:
	_bool		m_isSSAO = { false };
	_bool		m_isScreenRadial = { false };
	_float		m_fRadialBlurWidth = { 0.f };
	_float		m_fRadialTime = { 0.f };
	_float		m_fRadialTimeAcc = { 0.f };
	_float		m_fRainTimeAcc = { 0.f };
	_bool		m_isCircleFog = { false };
	_bool		m_isFloorFog = { false };
	_bool		m_isYFog = { false };
	_float4		m_vFogColor;
	_float3		m_vFogCenterPos;
	_float		m_fFogRadius = { 0.f };
	_float		m_fFogY = { 0.f };

private:
	_bool		m_isFade = { false };
	_float		m_fFadeTime = { 1.f };
	_float		m_fFadeSpeed = { 0.f };
	_float4		m_vFadeColor = { 0.f, 0.f, 0.f, 1.f };
	_bool		m_isRaining = { false };
	_bool		m_isNight = { false };
	_float		m_fGlowPower = { 0.f };
	_float		m_fHDR = { 0.f };

private:
	class CBlur*		m_pBlur = { nullptr };
	class CBloom*		m_pBloom = { nullptr };
	class CShadow*		m_pShadow = { nullptr };
	class CGlow*		m_pGlow = { nullptr };
	class CDOF*			m_pDOF = { nullptr };
	class CFlowmap*		m_pFlowmap = { nullptr };

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END