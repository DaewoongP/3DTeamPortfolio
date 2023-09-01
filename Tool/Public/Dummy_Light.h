#pragma once
#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
END

BEGIN(Tool)

class CDummy_Light final : public CBase
{
public:
	enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOTLIGHT, TYPE_AREA, TYPE_END };
	
	typedef struct tagLightDesc
	{
		TYPE	eType;
		_float4	vDir;
		_float4	vPos;
		_float	fRange;
		_float	fSpotPower;

		_float4	vDiffuse;
		_float4	vAmbient;
		_float4	vSpecular;
	}LIGHTDESC;

private:
	explicit CLight() = default;
	virtual ~CLight() = default;

public:
	const LIGHTDESC* Get_LightDesc() const {return &m_LightDesc;}
	
	_tchar* Get_GameObject_Tag() { return m_pTag; }
	void	Set_GameObject_Tag(_tchar* wszTag) { lstrcpy(m_pTag, wszTag); }
	void	Set_Pos(_float3 vPos) { m_pTransform->Set_Position(vPos); }

	void Set_LightDesc(LIGHTDESC LightDesc) { m_LightDesc = LightDesc; }
	void Set_Position(_float4 vPosition) { m_LightDesc.vPos = vPosition; }

public:
	virtual HRESULT Initialize_Prototype();
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	LIGHTDESC m_LightDesc;


public:
	static CDummy_Light* Create(const LIGHTDESC& LightDesc);
	virtual void Free() override;
};
END
