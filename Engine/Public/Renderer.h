#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };

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
	HRESULT Render_Lights();
	HRESULT Render_Shadow();
	HRESULT Render_SSAO();
	HRESULT Render_Deferred();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_Blur();
	HRESULT Render_PostProcessing();
	HRESULT Render_UI();

private:
	// 알파블렌딩 객체를 그릴때 깊이에 따라 순서 관리를 위한 함수.
	// 카메라 위치를 기준으로 먼것부터 그려 처리함.
	HRESULT Sort_Blend();
	HRESULT Sort_UI();
	HRESULT Add_Components();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif // _DEBUG

private:	
	list<class CGameObject*>		m_RenderObjects[RENDER_END];
#ifdef _DEBUG
private:
	list<class CComponent*>			m_DebugObject;
	_bool							m_isDebugRender = { true };
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
public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END