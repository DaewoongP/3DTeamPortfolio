#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CNVContextCallBack : public cloth::DxContextManagerCallback
{
public:
	virtual ~CNVContextCallBack() = default;

public:
	virtual void acquireContext() override;
	virtual void releaseContext() override;
	virtual ID3D11Device* getDevice() const override;
	virtual ID3D11DeviceContext* getContext() const override;
	virtual bool synchronizeResources() const override;
};

END