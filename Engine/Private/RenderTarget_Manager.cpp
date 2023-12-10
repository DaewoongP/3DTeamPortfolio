#include "..\Public\RenderTarget_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CRenderTarget_Manager)

CRenderTarget_Manager::CRenderTarget_Manager()
{

}

HRESULT CRenderTarget_Manager::Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor,_bool isShadow)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, eFormat, vClearColor,isShadow);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	MRTDESC* pMrtDesc = Find_MRT(pMRTTag);

	if (nullptr == pMrtDesc)
	{
		MRTDESC MrtDesc;
		MrtDesc.RenderTagets.push_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MrtDesc);
	}
	else
	{
		pMrtDesc->RenderTagets.push_back(pRenderTarget);
	}

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, _bool isShadow)
{
	std::lock_guard<std::mutex> lock(mtx);

	if (false == isShadow) /* 원래 렌더타겟 바인딩 */
	{
		MRTDESC* pMRTDesc = Find_MRT(pMRTTag);

		if (nullptr == pMRTDesc)
			return E_FAIL;

		pContext->OMGetRenderTargets(MAX_RENDERTARGET, pMRTDesc->pPrevRenderTargets, &pMRTDesc->pPrevDepthStencilView);

		ID3D11RenderTargetView* pRenderTargets[MAX_RENDERTARGET] = { nullptr };

		_uint		iNumViews = 0;

		for (auto& pRenderTarget : pMRTDesc->RenderTagets)
		{
			pRenderTarget->Clear(isShadow);
			pRenderTargets[iNumViews++] = pRenderTarget->Get_RTV();
		}

		pContext->OMSetRenderTargets(iNumViews, pRenderTargets, pMRTDesc->pPrevDepthStencilView);
	}
	else
	{
		// 뎁스스텐실뷰 자체가 한번에 무조건 한개만 바인딩 가능하기 때문에
		// for문을 여러번 돌아줄 필요가 없다.
		// 따라서 0번 iterator로 처리.

		MRTDESC* pMRTDesc = Find_MRT(pMRTTag);

		if (nullptr == pMRTDesc)
			return E_FAIL;

		pContext->OMGetRenderTargets(MAX_RENDERTARGET, pMRTDesc->pPrevRenderTargets, &pMRTDesc->pPrevDepthStencilView);
		
		ID3D11RenderTargetView* pRenderTargets[MAX_RENDERTARGET] = { nullptr };
		ID3D11DepthStencilView* pDepthStencilView = { nullptr };
		
		CRenderTarget* pRenderTarget = (pMRTDesc->RenderTagets).front();
		// 렌더타겟 및 뎁스스텐실 초기화
		pRenderTarget->Clear(isShadow);
		pRenderTargets[0] = pRenderTarget->Get_RTV();
		pDepthStencilView = pRenderTarget->Get_DSV();

		D3D11_VIEWPORT CurrentRenderViewPort = pRenderTarget->Get_ViewPort();

		_uint iViewports = 1;
		pContext->RSGetViewports(&iViewports, &m_OriginViewPortDesc);
		pContext->RSSetViewports(1, &CurrentRenderViewPort);

		// 뎁스스텐실만 적용할 것이므로 1개만 처리.
		pContext->OMSetRenderTargets(1, pRenderTargets, pDepthStencilView);
	}
	
	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, _bool isShadow)
{
	std::lock_guard<std::mutex> lock(mtx);

	MRTDESC* pMRTDesc = Find_MRT(pMRTTag);

	if (nullptr == pMRTDesc)
		return E_FAIL;

	if (false == isShadow)
	{
		pContext->OMSetRenderTargets(MAX_RENDERTARGET, pMRTDesc->pPrevRenderTargets, pMRTDesc->pPrevDepthStencilView);

		for (auto& pRenderTarget : pMRTDesc->pPrevRenderTargets)
		{
			Safe_Release(pRenderTarget);
		}
		Safe_Release(pMRTDesc->pPrevDepthStencilView);
	}
	else
	{
		// 원본값으로 다시 변경

		pContext->OMSetRenderTargets(MAX_RENDERTARGET, pMRTDesc->pPrevRenderTargets, pMRTDesc->pPrevDepthStencilView);
		pContext->RSSetViewports(1, &m_OriginViewPortDesc);

		for (auto& pRenderTarget : pMRTDesc->pPrevRenderTargets)
		{
			Safe_Release(pRenderTarget);
		}
		Safe_Release(pMRTDesc->pPrevDepthStencilView);
	}

	return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_ShaderResourceView(const _tchar* pTargetTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResourceView(pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT CRenderTarget_Manager::Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CRenderTarget_Manager::Render_Debug(const _tchar* pMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pTarget : Pair.second.RenderTagets)
		{
			if (nullptr != pTarget)
				pTarget->Render(pShader, pVIBuffer);
		}
	}

	return S_OK;
}
#endif // _DEBUG

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const _tchar* pTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

CRenderTarget_Manager::MRTDESC* CRenderTarget_Manager::Find_MRT(const _tchar* pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CRenderTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second.RenderTagets)
			Safe_Release(pRenderTarget);

		Pair.second.RenderTagets.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	
	m_RenderTargets.clear();
}
