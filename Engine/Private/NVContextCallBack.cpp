#include "../Public/NVContextCallBack.h"

void CNVContextCallBack::acquireContext()
{
}

void CNVContextCallBack::releaseContext()
{
}

ID3D11Device* CNVContextCallBack::getDevice() const
{
	return nullptr;
}

ID3D11DeviceContext* CNVContextCallBack::getContext() const
{
	return nullptr;
}

bool CNVContextCallBack::synchronizeResources() const
{
	return false;
}
