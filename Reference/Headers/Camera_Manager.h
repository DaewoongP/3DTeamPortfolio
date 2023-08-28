#pragma once
#include "Camera.h"

BEGIN(Engine)

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Add_MainCamera();
	HRESULT Add_SubCamera(const _tchar* pCameraTag);

private:
	queue<class CCamera*>	m_Cameras;

public:
	virtual void Free() override;
};

END