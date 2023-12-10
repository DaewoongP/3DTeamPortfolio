#pragma once
/* =============================================== */
// 
//	�� : ����ȯ
//	�� : �ڴ��
//
/* =============================================== */

#include "Base.h"
#define		MAX_RENDERTARGET		8
BEGIN(Engine)

class CRenderTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CRenderTarget_Manager)
private:
	typedef struct tagMRT
	{
		list<class CRenderTarget*>	RenderTagets;
		ID3D11RenderTargetView*		pPrevRenderTargets[MAX_RENDERTARGET] = { nullptr };
		ID3D11DepthStencilView*		pPrevDepthStencilView = { nullptr };
	}MRTDESC;

private:
	explicit CRenderTarget_Manager();
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor,_bool isShadow=false);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);

	/* pMRTTag�� �߰��Ǿ��ִ� ����Ÿ�ٵ��� ��ġ�� ���ε��Ѵ�. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, _bool isShadow = false);

	/* �������·� �����Ѵ�. (0��°�� ����۰� ���ε� �� ���·� �����ش�.) */
	HRESULT End_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, _bool isShadow = false);

	HRESULT Bind_ShaderResourceView(const _tchar* pTargetTag, class CShader* pShader, const _char* pConstantName);
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _tchar* pMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	_umap<const _tchar*, class CRenderTarget*>	m_RenderTargets;

	/* ��Ƽ����Ÿ��.(���ÿ� ��ġ�� ���ε��Ǿ���� ����Ÿ�ٵ��� ��Ƶд�.) */
	// Tag, pair<MRT's RenderTaget List, PrevRenderTargetView ptr >
	_umap<const _tchar*, MRTDESC>				m_MRTs;

private:
	D3D11_VIEWPORT				m_OriginViewPortDesc;

private:
	MRTDESC* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END