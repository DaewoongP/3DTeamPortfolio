#include "../Public/NVContextCallBack.h"

CNVContextCallBack::CNVContextCallBack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _bool isSynchronizeResources)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_isSynchronizeResources(isSynchronizeResources)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CNVContextCallBack::~CNVContextCallBack()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

void CNVContextCallBack::acquireContext()
{
	m_Mutex.lock();
}

void CNVContextCallBack::releaseContext()
{
	m_Mutex.unlock();
}

ID3D11Device* CNVContextCallBack::getDevice() const
{
	return m_pDevice;
}

ID3D11DeviceContext* CNVContextCallBack::getContext() const
{
	return m_pContext;
}

bool CNVContextCallBack::synchronizeResources() const
{
	return m_isSynchronizeResources;
}
