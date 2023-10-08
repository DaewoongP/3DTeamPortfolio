#pragma once
#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const CLight::LIGHTDESC* Get_Light(_uint iIndex);
	_float4x4* Get_LightProj() { return &m_ProjLight; }
	_float4x4* Get_LightView()
	{
		return &m_ViewLight;
	}
	_float4* Get_LightPosition() { return &m_fLightPos; }

	void Set_Light(_uint iIndex, _float fWinSizeX, _float fWinSizeY, CLight::LIGHTDESC LightDesc);
	void Set_LightProj(_float4x4 ProjLight) { m_ProjLight = ProjLight; }
	void Set_LightView(_float4x4 ViewLight) { m_ViewLight = ViewLight; }
	_bool Light_NullCheck() { return m_Lights.empty(); }

public:
	CLight* Add_Lights(_float fWinSizeX, _float fWinSizeY, const CLight::LIGHTDESC& LightDesc);
	HRESULT Delete_Lights(_uint iIndex, const _char* Name);
	HRESULT Clear_Lights();
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);


private:
	list<class CLight*>		m_Lights;
	_float4x4				m_ViewLight;
	_float4x4				m_ProjLight;
	_float4					m_fLightPos;
public:
	virtual void Free() override;
};

END