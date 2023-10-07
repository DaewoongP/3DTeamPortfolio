#pragma once
#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager() = default;
	virtual ~CLight_Manager() = default;

public:
	CLight* Add_Lights(const CLight::LIGHTDESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Clear_Lights();

private:
	list<class CLight*>		m_Lights;

public:
	virtual void Free() override;
};

END