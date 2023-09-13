#pragma once

/* =============================================== */
// 
//	정 : 박정환
//	부 : 박대웅
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_PICKING, RENDER_BRUSHING, RENDER_UI, RENDER_UITEXTURE, RENDER_END };

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
	HRESULT Render_Shadow();
	HRESULT Render_SoftShadow();
	HRESULT Render_SSAO();
	HRESULT Render_Deferred();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_Blur();
	HRESULT Render_PostProcessing();
	HRESULT Render_Distortion();
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
	_bool Is_DebugRender();
	_bool Is_MRTRender();
#endif // _DEBUG

private:	
	list<class CGameObject*>		m_RenderObjects[RENDER_END];
#ifdef _DEBUG
private:
	list<class CComponent*>			m_DebugObject;
	_bool							m_isDebugRender = { true };
	_bool							m_isMRTRender = { true };
#endif // _DEBUG


private:
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };

private: /* Deferred Shader */
	class CVIBuffer_Rect*			m_pDeferredBuffer = { nullptr };
	class CShader*					m_pDeferredShader = { nullptr };
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private: /* Post Processing */
	class CVIBuffer_Rect*			m_pPostProcessingBuffer = { nullptr };
	class CShader*					m_pPostProcessingShader = { nullptr };

private: /* Shader_Type */
	class CVIBuffer_Rect*			m_pShadeTypeBuffer = { nullptr };//각종 쉐이더처리 해줄때 쓸 것
	class CShader*					m_pShadeTypeShader = { nullptr };

private: /* Shader_Type */
	class CVIBuffer_Rect*			m_pSSAOBuffer = { nullptr };//각종 쉐이더처리 해줄때 쓸 것
	class CShader*					m_pSSAOShader = { nullptr };

private: /* AfterShader*/
	class CVIBuffer_Rect* m_pAfterShaderBuffer = { nullptr };//각종 쉐이더처리 해줄때 쓸 것
	class CShader* m_pAfterShader = { nullptr };
private:
	_float m_fFrameTime = 0.f;

	class CTexture*					 m_pTexture = { nullptr };
	class CTexture* m_pTexture2 = { nullptr };
	class CTexture* m_pTexture3 = { nullptr };

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END