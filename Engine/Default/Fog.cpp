#include "Fog.h"

CFog::CFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent(m_pDevice,m_pContext)
{
}

HRESULT CFog::Initialize_Prototype(const _tchar* pTargetTag)
{
	return E_NOTIMPL;
}

HRESULT CFog::Render()
{
	return E_NOTIMPL;
}

HRESULT CFog::Add_Components()
{
	return E_NOTIMPL;
}

CFog* CFog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag)
{
	return nullptr;
}

void CFog::Free()
{
}
