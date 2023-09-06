#pragma once
/* =============================================== */
// 
//	�� : ����ȯ
//	�� : �ڴ��
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class CRenderTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CRenderTarget_Manager)
private:
	explicit CRenderTarget_Manager();
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);

	/* pMRTTag�� �߰��Ǿ��ִ� ����Ÿ�ٵ��� ��ġ�� ���ε��Ѵ�. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);
	// ����۸� �����ϰ� ����Ʈ ���μ����� ������ ����Ÿ���� ���ε���.
	HRESULT Begin_PostProcessingRenderTarget(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);

	/* �������·� �����Ѵ�. (0��°�� ����۰� ���ε� �� ���·� �����ش�.) */
	HRESULT End_MRT(ID3D11DeviceContext* pContext);
	HRESULT End_PostProcessingRenderTarget(ID3D11DeviceContext* pContext);

	HRESULT Bind_ShaderResourceView(const _tchar* pTargetTag, class CShader* pShader, const _char* pConstantName);

	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _tchar* pMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	unordered_map<const _tchar*, class CRenderTarget*>			m_RenderTargets;

	/* ��Ƽ����Ÿ��.(���ÿ� ��ġ�� ���ε��Ǿ���� ����Ÿ�ٵ��� ��Ƶд�.) */
	unordered_map<const _tchar*, list<class CRenderTarget*>>	m_MRTs;

private:
	ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
	// Post Processing�� ���� ����Ÿ��
	ID3D11RenderTargetView* m_pPostRenderTargetView = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };
	ID3D11DepthStencilView* m_pShadowView = { nullptr };

private:
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END