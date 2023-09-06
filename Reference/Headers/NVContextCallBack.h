#pragma once
#include "Engine_Defines.h"
#include <mutex>

BEGIN(Engine)

class CNVContextCallBack : public cloth::DxContextManagerCallback
{
public:
	CNVContextCallBack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _bool isSynchronizeResources = false);
	virtual ~CNVContextCallBack();

public:
	virtual void acquireContext() override;
	virtual void releaseContext() override;
	virtual ID3D11Device* getDevice() const override;
	virtual ID3D11DeviceContext* getContext() const override;
	virtual bool synchronizeResources() const override;

private:
	recursive_mutex			m_Mutex;
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_bool					m_isSynchronizeResources = { false };
};

END