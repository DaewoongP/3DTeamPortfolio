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
	HRESULT Render_Deferred();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_PostProcessing();
	HRESULT Render_UI();

private:
	// ���ĺ��� ��ü�� �׸��� ���̿� ���� ���� ������ ���� �Լ�.
	// ī�޶� ��ġ�� �������� �հͺ��� �׷� ó����.
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

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END