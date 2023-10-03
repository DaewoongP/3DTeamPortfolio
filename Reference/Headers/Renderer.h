#pragma once

/* =============================================== */
// 
//	정 : 박정환
//	부 : 박대웅
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CGameObject
{
public:
	enum RENDERGROUP {RENDER_PRIORITY, RENDER_DEPTH, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND,
					  RENDER_GLOW, RENDER_PICKING, RENDER_BRUSHING, RENDER_UI, RENDER_UITEXTURE, RENDER_END };

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
	HRESULT Render_UI();

#ifdef _DEBUG
	HRESULT Render_UITexture();
#endif // _DEBUG

private:
	// 알파블렌딩 객체를 그릴때 깊이에 따라 순서 관리를 위한 함수.
	// 카메라 위치를 기준으로 먼것부터 그려 처리함.
	HRESULT Sort_Blend();
	HRESULT Sort_UI();
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
	class CVIBuffer_Rect*			m_pRectBuffer = { nullptr };
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	class CShader*					m_pDeferredShader = { nullptr };
	class CShader*					m_pLightShader = { nullptr };
	class CShader*					m_pPostProcessingShader = { nullptr };
	class CShader*					m_pShadeTypeShader = { nullptr };
	class CShader*					m_pSSAOShader = { nullptr };
	class CShader*					m_pAfterShader = { nullptr };

private:
	class CBlur*					m_pBlur = { nullptr };
	class CBloom*					m_pBloom = { nullptr };
	class CShadow*					m_pShadow = { nullptr };
	class CGlow*					m_pGlow = { nullptr };
	_float							m_fGlowPower = { 0.f };
	class CMotionBlurInstance*		m_pMotionBlurInstance = { nullptr };
	class CMotionBlur*				m_pMotionBlur = { nullptr };
	class CDOF*						m_pDOF = { nullptr };

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END